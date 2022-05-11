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

static const struct soc_amd_gpio semi_override[] = {
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
	*size = ARRAY_SIZE(semi_override);
	return semi_override;
}

static const struct soc_amd_gpio semi_sleep_gpio_table[] = {
	/*
	 * Pin BA13: SER1-REAR-TOUCH-NRESET
	 * Drive low to avoid leakage to serializer during S3.
	 */
	PAD_GPO(GPIO_22, LOW),
};


const struct soc_amd_gpio *variant_sleep_gpio_table(size_t *size, int slp_typ)
{
	*size = ARRAY_SIZE(semi_sleep_gpio_table);
	return semi_sleep_gpio_table;
}

