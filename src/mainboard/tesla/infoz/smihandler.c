/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <gpio.h>
#include <soc/smi.h>
#include <infoz/variants.h>
#include <variant/gpio.h>
#include <console/console.h>

void mainboard_smi_gpi(u32 gpi_sts)
{
}

void mainboard_smi_sleep(u8 slp_typ)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	printk(BIOS_DEBUG, "mainboard_smi_sleep: %x\n", slp_typ);

	gpios = variant_sleep_gpio_table(&num_gpios, slp_typ);
	program_gpios(gpios, num_gpios);
}

int mainboard_smi_apmc(u8 apmc)
{
	return 0;
}
