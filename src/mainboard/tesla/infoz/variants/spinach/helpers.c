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

#define INFOZ_REV_B_TEST_EN_PIN		(7)
#define INFOZ_REV_B_TEST_STS_PIN	(75)

#define INFOZ_REV_G_TEST_PIN		(23)

enum infoz_hw_rev variant_get_rev(void)
{
	static bool cached = false;
	static enum infoz_hw_rev rev = INFOZ_HW_UNKNOWN;
	uint32_t gpio_reg1, gpio_reg2;
	int gpio_level;

	if (cached)
		return rev;

	/* Save gpio control setting */
	gpio_reg1 = gpio_read32(INFOZ_REV_B_TEST_EN_PIN);
	gpio_reg2 = gpio_read32(INFOZ_REV_B_TEST_STS_PIN);

	/* Enable serializer 2 which is only populate on Rev A */
	gpio_output(INFOZ_REV_B_TEST_EN_PIN, 1);
	mdelay(10);

	/* Serializer 2 will pullup the pin on Rev A, low on newer rev */
	gpio_input_pulldown(INFOZ_REV_B_TEST_STS_PIN);
	udelay(1);
	gpio_level = gpio_get(INFOZ_REV_B_TEST_STS_PIN);

	/* Restore gpio control setting */
	gpio_write32(INFOZ_REV_B_TEST_STS_PIN, gpio_reg2);
	gpio_write32(INFOZ_REV_B_TEST_EN_PIN, gpio_reg1);

	if (gpio_level == 1)
		rev = INFOZ_HW_REV_A;
	else {
		/* Save gpio control setting */
		gpio_reg1 = gpio_read32(INFOZ_REV_G_TEST_PIN);

		/* Distinguish Rev B vs G. Pin is pullup on Rev G, floats on Rev B. */
		gpio_input_pulldown(INFOZ_REV_G_TEST_PIN);
		udelay(1);
		gpio_level = gpio_get(INFOZ_REV_G_TEST_PIN);

		/* Restore gpio control setting */
		gpio_write32(INFOZ_REV_G_TEST_PIN, gpio_reg1);

		if (gpio_level == 0)
			rev = INFOZ_HW_REV_B;
		else
			rev = INFOZ_HW_REV_G;
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

	if (cached)
		return flags;

	flags |= INFOZ_HAS_DGPU;

	switch (variant_get_rev()) {
	case INFOZ_HW_REV_G:
		flags |= INFOZ_QCA_WIFI_BT;
		/* Fall through - Rev G is same as Rev B except with Qualcomm WiFi/BT module */
	case INFOZ_HW_REV_B:
		flags |= INFOZ_BOARD_ID_6BIT;
		break;
	case INFOZ_HW_REV_A:
		flags |= INFOZ_NEEDS_ESPI_RESET;
		flags |= INFOZ_HAS_DGPU_PWREN_GPIO_108;
		/* Fall through */
	case INFOZ_HW_UNKNOWN:
	default:
		break;
	}

	cached = true;

	return flags;
}

const char *variant_board_sku(void)
{
	return "1960100";
}

#if !ENV_BOOTBLOCK
void variant_devtree_update(void)
{
	infoz_common_devtree_update();
}
#endif	/* !ENV_BOOTBLOCK */
