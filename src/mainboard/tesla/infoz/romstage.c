/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/gpio_banks.h>
#include <infoz/variants.h>
#include <console/console.h>
#include <soc/romstage.h>

/* mainboard romstage entry point before FSP-M */
void mainboard_romstage_entry_s3(int s3_resume)
{
	printk(BIOS_SPEW, "infoz romstage entry: s3_resume=%d\n", s3_resume);

	if (!s3_resume) {
		/* Ensure GPIO expanders in consistent state for warm boots */
		variant_gpio_exp_reset();
	}
}
