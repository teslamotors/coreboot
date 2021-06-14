/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <gpio.h>
#include <soc/smi.h>
#include <variant/gpio.h>
#include <console/console.h>

void mainboard_smi_gpi(u32 gpi_sts)
{
}
void mainboard_smi_sleep(u8 slp_typ)
{
}
int mainboard_smi_apmc(u8 apmc)
{
	return 0;
}
