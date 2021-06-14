/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <infoz/variants.h>
#include <soc/gpio.h>
#include <soc/smi.h>
#include <stdlib.h>
#include <boardid.h>
#include <infoz/gpio.h>
#include <console/console.h>

/*
 * Variant-specific GPIO quirks
 */

/* Rev C and D */
static const struct soc_amd_gpio coriander_rev_cd_override[] = {
	/* Pin AT15: SOC-nRST-P3V3-GPIO-EXP-3 */
	PAD_GPO(GPIO_3, HIGH),

	/* Pin AK10: SOC-nRST-P3V3-GPIO-EXP-2 */
	PAD_GPO(GPIO_16, HIGH),

	/* Pin BB16: SER1-REAR-TOUCH-nRESET_R */
	PAD_GPO(GPIO_144, HIGH),
};

/* Rev HG-A */
static const struct soc_amd_gpio coriander_hg_override[] = {
	/* Pin AT15: GPU-GTW-SOC-PWR-GD	*/
	PAD_GPI(GPIO_3, PULL_DOWN),

	/* Pin AK10: GPU-SOC-TEMP-NALERT */
	PAD_GPI(GPIO_16, PULL_UP),

	/* Pin BB14: SOC-DP-MUX-SW-SEL */
	PAD_GPO(GPIO_21, LOW),

	/* Pin BA13: SER1-REAR-TOUCH-NRESET */
	PAD_GPO(GPIO_22, HIGH),

	/* Pin BB16: GPU-SOC-CTEMP-FAULT */
	PAD_GPI(GPIO_144, PULL_DOWN),
};

/*
 * Get HW-appropriate array of GPIO definitions that override the
 * common InfoZ table
 */
const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	enum infoz_hw_rev hw_rev;

	hw_rev = variant_get_rev();

	switch (hw_rev) {
	case INFOZ_HW_REV_C:
	case INFOZ_HW_REV_D:
		*size = ARRAY_SIZE(coriander_rev_cd_override);
		return coriander_rev_cd_override;
	case INFOZ_HW_REV_HGA:
		*size = ARRAY_SIZE(coriander_hg_override);
		return coriander_hg_override;
	case INFOZ_HW_UNKNOWN:
	default:
		printk(BIOS_WARNING,
			"Unknown board revision, GPIO configuration may be broken\n");
		*size = 0;
		return NULL;
	}
}
