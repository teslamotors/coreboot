/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/reset.h>
#include <arch/mmio.h>
#include <console/console.h>
#include <soc/amd/common/block/psp/psp_def.h>
#include <soc/rpmc.h>
#include <types.h>
#include <delay.h>
#include "cc6.h"

#define CAP_RPMC_SUPPORTED	BIT(1)

#define HSTI_STATE_RPMC_ENABLED	BIT(8)
#define HSTI_STATE_RPMC_SUCCESS	BIT(9)

int rpmc_supported(void)
{
	uint32_t caps;

	if (psp_get_caps_v2(&caps)) {
		printk(BIOS_ERR, "RPMC: Failed to get PSP capabilities\n");
		return -1;
	}

	printk(BIOS_DEBUG, "RPMC: Capabilities = %x\n", caps);

	if (!(caps & CAP_RPMC_SUPPORTED)) {
		printk(BIOS_ERR, "RPMC: SPI ROM missing support\n");
		return 0;
	}

	return 1;
}

int rpmc_provisioned(void)
{
	uint32_t state;

	if (psp_get_hsti_state(&state)) {
		printk(BIOS_ERR, "RPMC: Failed to get HSTI state\n");
		return -1;
	}

	printk(BIOS_DEBUG, "RPMC: HSTI = %x\n", state);

	if (state & HSTI_STATE_RPMC_ENABLED) {
		printk(BIOS_INFO, "RPMC: Already enabled\n");
		return 1;
	}

	if (state & HSTI_STATE_RPMC_SUCCESS) {
		printk(BIOS_NOTICE, "RPMC: Provisioning success\n");
		return 1;
	}

	return 0;
}

int rpmc_provision(bool lock)
{
	int cmd_status;
	u32 rpmc_status;
	struct mbox_cmd_set_rpmc_address_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		},
		.req = {
			.address = CONFIG_PSP_RPMC_ROOT_KEY_ADDRESS,
			.lock = lock ? 1 : 0,
		}
	};

	printk(BIOS_DEBUG, "RPMC: Enable%s... ", lock ? " and lock" : "");

	cc6_disable();
	/* Give cores in CC6 a chance to exit */
	mdelay(100);

	cmd_status = send_psp_command(MBOX_BIOS_CMD_SET_RPMC_ADDRESS, &buffer);

	cc6_enable();

	psp_print_cmd_status(cmd_status, &buffer.header);

	if (cmd_status) {
		printk(BIOS_ERR, "RPMC: Provisioning request failed: %d\n", cmd_status);
		return -1;
	}

	rpmc_status = read32(&buffer.header.status);
	if (rpmc_status) {
		printk(BIOS_ERR, "RPMC: error: %d\n", rpmc_status);
		return -1;
	}

	printk(BIOS_NOTICE, "RPMC: Rebooting\n");
	warm_reset();
	return -1;
}
