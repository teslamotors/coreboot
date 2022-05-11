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

/* Rev A */
static const struct soc_amd_gpio spinach_rev_a_gpio_override[] = {
	/* Pin AT15: GPU-GTW-SOC-PWR-GD	*/
	PAD_GPI(GPIO_3, PULL_DOWN),

	/* Pin AP9: SOC-DISP-3-PWR-EN - Center Display */
	PAD_GPO(GPIO_5, HIGH),

	/* Pin AK10: GPU-SOC-TEMP-NALERT */
	PAD_GPI(GPIO_16, PULL_UP),

	/* Pin BB14: BRD-ID-CONFIG-5 */
	PAD_GPI(GPIO_21, PULL_NONE),

	/* Pin BA13: SER1-REAR-TOUCH-NRESET */
	PAD_GPO(GPIO_22, HIGH),

	/* Pin BA11: SOC-SER-1-PWR-EN - Center Display */
	PAD_GPO(GPIO_68, HIGH),

	/* Pin BB16: GPU-SOC-CTEMP-FAULT */
	PAD_GPI(GPIO_144, PULL_DOWN),
};

/* Rev B (Broadcom WiFi/BT) and Rev G (Qualcomm WiFi/BT) */
static const struct soc_amd_gpio spinach_rev_b_gpio_override[] = {
	/* Pin AT15: GPU-GTW-SOC-PWR-GD	*/
	PAD_GPI(GPIO_3, PULL_DOWN),

	/* Pin AP9: SOC-DISP-3-PWR-EN - Center Display */
	PAD_GPO(GPIO_5, HIGH),

	/* Pin AU10: BRD-ID-CONFIG-4 */
	PAD_GPI(GPIO_6, PULL_NONE),

	/* Pin AK10: GPU-SOC-TEMP-NALERT */
	PAD_GPI(GPIO_16, PULL_UP),

	/* Pin BB14: BRD-ID-CONFIG-5 */
	PAD_GPI(GPIO_21, PULL_NONE),

	/* Pin BA13: SER1-REAR-TOUCH-NRESET */
	PAD_GPO(GPIO_22, HIGH),

	/* Pin BA11: SOC-SER-1-PWR-EN - Center Display */
	PAD_GPO(GPIO_68, HIGH),

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
	case INFOZ_HW_REV_A:
		*size = ARRAY_SIZE(spinach_rev_a_gpio_override);
		return spinach_rev_a_gpio_override;
	case INFOZ_HW_REV_B:
	case INFOZ_HW_REV_G:
		*size = ARRAY_SIZE(spinach_rev_b_gpio_override);
		return spinach_rev_b_gpio_override;
	case INFOZ_HW_UNKNOWN:
	default:
		printk(BIOS_WARNING,
			"Unknown board revision, GPIO configuration may be broken\n");
		*size = 0;
		return NULL;
	}
}

static const struct soc_amd_gpio spinach_sleep_gpio_table[] = {
	/*
	 * Pin BA13: SER1-REAR-TOUCH-NRESET
	 * Drive low to avoid leakage to serializer during S3.
	 */
	PAD_GPO(GPIO_22, LOW),
};


const struct soc_amd_gpio *variant_sleep_gpio_table(size_t *size, int slp_typ)
{
	*size = ARRAY_SIZE(spinach_sleep_gpio_table);
	return spinach_sleep_gpio_table;
}

