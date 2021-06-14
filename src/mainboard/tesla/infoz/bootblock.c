/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <infoz/variants.h>
#include <soc/southbridge.h>
#include <console/console.h>
#include <amdblocks/biosram.h>

void bootblock_mainboard_early_init(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	gpios = variant_early_gpio_table(&num_gpios);
	program_gpios(gpios, num_gpios);

	/* Set BLINK LED to solid ON during early boot */
	fch_blink_rate(FCH_BLINK_ON);

	if (debug_console_requested())
		debug_console_set_enabled();
}

void bootblock_mainboard_init(void)
{
	if (panic_detected()) {
		printk(BIOS_ERR, "Kernel panic from last boot: ");
		panic_msg_print(BIOS_ERR);
		printk(BIOS_ERR, "\n");
		panic_msg_clear();
	}
}
