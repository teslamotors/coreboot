/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/cache.h>
#include <arch/clock.h>
#include <arch/exception.h>
#include <arch/io.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <delay.h>
#include <program_loading.h>
#include <symbols.h>
#include <timestamp.h>
#include <stdlib.h>

#include <soc/clock_init.h>
#include <soc/clock.h>
#include <soc/cpu.h>
#include <soc/power.h>
#include <soc/dmc_lpddr4phy_init.h>
#include <soc/gic.h>
#include <soc/mmu_operations.h>
#include <soc/pinmux.h>
#include <soc/trustzone.h>
#include <soc/common_definition.h>
#include <soc/netspeed.h>
#include <soc/netspeed_c.h>
#include <soc/scs_helpers.h>
#include <soc/sms_helpers.h>
#include <soc/boot_control.h>
#include <soc/alternate_cbfs.h>
#include <soc/wdt.h>

#define ACE_MODE		0x00000002
#define GPU_CONFIG_COH		0x14430320
#define GPU_RESET_MASK		0x00000003
#define GPU_RESET_DONE		0x3
#define ENABLE_REQUEST		0x1

DECLARE_OPTIONAL_REGION(timestamp);

__attribute__((weak)) void bootblock_mainboard_early_init(void) { /* no-op */ }
__attribute__((weak)) void bootblock_mainboard_init(void) { /* do nothing */ }

void bootblock_soc_early_init(void)
{
	/* Set Counter Frequency */
	set_cntfrq(TRAV_COUNTER_FREQ);
	clock_system_early_init();
	/* Set GPIOs for UART */
	trav_pinmux_uart0();
}

#if !IS_ENABLED(CONFIG_BOOTBLOCK_X)
static void set_gpu_coherency(void)
{
	write32((void *)GPU_CONFIG_COH, 0x0);
	/* Waiting for GPU Reset to complete */
	while((read32(((void *)(GPU_RESET_STATUS))) & GPU_RESET_MASK) != GPU_RESET_DONE);

	/* Set GPU Coherency to ACE mode */
	write32((void *)GPU_CONFIG_COH, ACE_MODE);

	/* Enable all GPU port for Coherency */
	write32((void *) GPU_M0_PORT, ENABLE_REQUEST);
	write32((void *) GPU_M1_PORT, ENABLE_REQUEST);
	write32((void *) GPU_M2_PORT, ENABLE_REQUEST);
	write32((void *) GPU_M3_PORT, ENABLE_REQUEST);
}

void bootblock_soc_init(void)
{
	unsigned int memclk;
	unsigned int cpucl_clk, gpuclk, tripclk;

	noc_address_stride_update();

	memclk = trav_bios_mem_clk_get(trav_board_id() > 1);
	cpucl_clk = trav_bios_cpucl_clk_get();
	gpuclk = trav_bios_gpu_clk_get(trav_board_id() > 1);
	tripclk = trav_bios_trip_clk_get();

	clock_system_init(memclk, cpucl_clk, gpuclk, tripclk, trav_cpu_rev());

	printk(BIOS_INFO,"Before mem init\n");
	mem_ctrl_init(memclk, trav_from_a72_warm_reset());
	printk(BIOS_INFO,"After mem init\n");

	trustzone_init();
	trav_mmu_init();
	printk(BIOS_INFO,"After mmu init\n");

	set_gpu_coherency();
}

static void bootblock_slog_log(void)
{
#if 0
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;
	char msg[27] = "from coreboot bootblock -";

	msg[25] = env->boot_from[0];
	msg[26] = env->boot_from[1];
	slog_log_message(msg, 27);
#endif
}

#else

static void bootblock_slog_log(void)
{
#if 0
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;
	char msg[15] = "from a72bl1 -";

	msg[13] = 'x';
	msg[14] = (char) (env->bootx_booted_bank + 'a');
	slog_log_message(msg, 15);
#endif
}

#endif

void asmlinkage bootblock_main_with_timestamp(uint64_t base_timestamp)
{
	/* Initialize timestamps if we have TIMESTAMP region in memlayout.ld. */
	if (IS_ENABLED(CONFIG_COLLECT_TIMESTAMPS) && _timestamp_size > 0)
		timestamp_init(base_timestamp);

	bootblock_soc_early_init();
	bootblock_mainboard_early_init();

	if (IS_ENABLED(CONFIG_BOOTBLOCK_CONSOLE)) {
		console_init();
		printk(BIOS_INFO,"TRAV console\n");
		exception_init();
	}

#if IS_ENABLED(CONFIG_BOOTBLOCK_X)
        scs_service_init();
	sms_service_init();

	if (is_boot_from_qspi())
		copy_bootblock();
	else {
		int status;

		clock_system_early_ufs_init(trav_cpu_rev());
		status = load_bootblock_from_ufs();

		/* may need to fallback to recovery */
		if (status == -2)
			copy_bootblock();
	}

	bootblock_slog_log();

	check_a72bl1_versions();
	run_bootblock();
#else
	a72wdt_gpio_conf();
	wdt_enable();

	scs_service_init();
	sms_service_init();

	/* pass to ram stage */
	check_turbo_a_b();
	check_scs_versions();
	check_sms_versions();
	check_a72bootblock_versions();
	check_a72_warm_reset();

	bootblock_slog_log();

	bootblock_soc_init();
	bootblock_mainboard_init();

	if (is_boot_from_qspi())
		copy_coreboot_rom();

	run_ramstage();
#endif
}

void main(void)
{
	uint64_t base_timestamp = 0;

	init_timer();

	if (IS_ENABLED(CONFIG_COLLECT_TIMESTAMPS))
		base_timestamp = timestamp_get();

	bootblock_main_with_timestamp(base_timestamp);
}
