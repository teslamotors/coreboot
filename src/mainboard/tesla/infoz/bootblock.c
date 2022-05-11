/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <bootmode.h>
#include <delay.h>
#include <infoz/variants.h>
#include <soc/southbridge.h>
#include <console/console.h>
#include <amdblocks/biosram.h>
#include <amdblocks/acpimmio.h>
#include <acpi/acpi.h>

static void reset_spi_interfaces(void);

static void reset_spi_interfaces(void)
{
	printk(BIOS_NOTICE, "Begin ESPI reset\n");

	power_off_aoac_device(FCH_AOAC_DEV_LPC);
	power_off_aoac_device(FCH_AOAC_DEV_ESPI);

	// Wait for clean off
	wait_for_aoac_disabled(FCH_AOAC_DEV_LPC);
	wait_for_aoac_disabled(FCH_AOAC_DEV_ESPI);

	// Power on devices
	power_on_aoac_device(FCH_AOAC_DEV_LPC);
	power_on_aoac_device(FCH_AOAC_DEV_ESPI);

	// wait for devices to turn back on.
	wait_for_aoac_enabled(FCH_AOAC_DEV_LPC);
	wait_for_aoac_enabled(FCH_AOAC_DEV_ESPI);

	printk(BIOS_NOTICE, "End ESPI reset\n");
}

void bootblock_mainboard_early_init(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	/* Let PSP finish SPI reads to avoid hang from bus contention */
	mdelay(500);

	gpios = variant_early_gpio_table(&num_gpios);
	program_gpios(gpios, num_gpios);

	/* Set BLINK LED to solid ON during early boot */
	fch_blink_rate(FCH_BLINK_ON);

	if (debug_console_requested())
		debug_console_set_enabled();
}

void bootblock_mainboard_init(void)
{
	int flags = 0;

	/* Enable WDT */
	fch_wdt_kick(5);

	if (panic_detected()) {
		printk(BIOS_ERR, "Kernel panic from last boot: ");
		panic_msg_print(BIOS_ERR);
		printk(BIOS_ERR, "\n");
		panic_msg_clear();
	}

	flags = variant_get_flags();
	if (platform_is_resuming() && (flags & INFOZ_NEEDS_ESPI_RESET)) {
		// Workaround E/SPI interfaces being disabled on S3 wake.
		reset_spi_interfaces();
	}
}
