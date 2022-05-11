/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <inttypes.h>
#include <infoz/variants.h>
#include <gpio.h>
#include <board_utils.h>
#include <soc/pci_devs.h>
#include <amdblocks/acpimmio.h>
#include <drivers/i2c/generic/chip.h>
#include <device/device.h>
#include <delay.h>

int variant_has_emmc(void)
{
	return 0;
}

int variant_has_nvme(void)
{
	return 1;
}

#define INFOZ_HG_TEST_PIN	(21)
#define INFOZ_CD_TEST_PIN	(4)
#define INFOZ_HGCD_TEST_PIN	(4)

enum infoz_hw_rev variant_get_rev(void)
{
	static bool cached = false;
	static enum infoz_hw_rev rev = INFOZ_HW_UNKNOWN;
	uint32_t gpio_reg;
	int gpio_level;

	if (cached)
		return rev;

	/* Save gpio control setting */
	gpio_reg = gpio_read32(INFOZ_HG_TEST_PIN);

	/* Distinguish non-HG vs HG.  Pin floats on non-HG.  */
	gpio_input_pullup(INFOZ_HG_TEST_PIN);
	udelay(1);
	gpio_level = gpio_get(INFOZ_HG_TEST_PIN);

	/* Restore gpio control setting */
	gpio_write32(INFOZ_HG_TEST_PIN, gpio_reg);

	if (gpio_level == 0) {
		gpio_reg = gpio_read32(INFOZ_HGCD_TEST_PIN);

		/* Distinguish Rev HG-C vs Rev HG-D. Pin is pulled low on Rev HG-D */
		gpio_input(INFOZ_HGCD_TEST_PIN);
		udelay(1);
		gpio_level = gpio_get(INFOZ_HGCD_TEST_PIN);

		if (gpio_level == 0)
			rev = INFOZ_HW_REV_HGD;
		else
			rev = INFOZ_HW_REV_HGA;

		/* Restore gpio control setting */
		gpio_write32(INFOZ_HGCD_TEST_PIN, gpio_reg);
	} else {
		/* Save gpio control setting */
		gpio_reg = gpio_read32(INFOZ_CD_TEST_PIN);

		/* Distinguish Rev C vs Rev D.  Pin floats on Rev D */
		gpio_input_pullup(INFOZ_CD_TEST_PIN);
		udelay(1);
		gpio_level = gpio_get(INFOZ_CD_TEST_PIN);

		/* Restore gpio control setting */
		gpio_write32(INFOZ_CD_TEST_PIN, gpio_reg);

		if (gpio_level == 0)
			rev = INFOZ_HW_REV_C;
		else
			rev = INFOZ_HW_REV_D;
	}

	cached = true;
	return rev;
}

/*
 * Use of flags for variant/config quirks preferred over directly
 * checking board models / revisions
 */
int variant_get_flags(void)
{
	static bool cached = false;
	static int flags = 0;
	enum infoz_hw_rev rev;

	if (cached)
		return flags;

	rev = variant_get_rev();
	switch (rev) {
	case INFOZ_HW_REV_C:
	case INFOZ_HW_REV_D:
		flags |= INFOZ_FIXUP_GPIO_EXP_RESET;
		flags |= INFOZ_OLD_REAR_TOUCH_RESET;
		break;
	case INFOZ_HW_REV_HGA:
		flags |= INFOZ_NEEDS_ESPI_RESET;
		flags |= INFOZ_HAS_DGPU_PWREN_GPIO_108;
		/* Fall through */
	case INFOZ_HW_REV_HGD:
	case INFOZ_HW_UNKNOWN:
	default:
		flags |= INFOZ_HAS_DGPU;
		break;
	}

	cached = true;
	return flags;
}

const char *variant_board_sku(void)
{
	enum infoz_hw_rev rev;

	rev = variant_get_rev();

	switch (rev) {
	case INFOZ_HW_REV_C:
	case INFOZ_HW_REV_D:
		return "1955000";
	case INFOZ_HW_REV_HGA:
	case INFOZ_HW_REV_HGD:
	default:
		return "1960000";
	}
}

#if !ENV_BOOTBLOCK
void variant_devtree_update(void)
{
	infoz_common_devtree_update();
}
#endif	/* !ENV_BOOTBLOCK */
