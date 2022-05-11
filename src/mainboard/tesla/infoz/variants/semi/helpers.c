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

#define INFOZ_REV_B_TEST_PIN	(23)

enum infoz_hw_rev variant_get_rev(void)
{
	return INFOZ_HW_REV_A;
}

/*
 * Use of flags for variant/config quirks preferred over directly
 * checking board models / revisions
 */
int variant_get_flags(void)
{
	return (INFOZ_HAS_DGPU | INFOZ_BOARD_ID_6BIT);
}

const char *variant_board_sku(void)
{
	return "1960200";
}

#if !ENV_BOOTBLOCK
void variant_devtree_update(void)
{
	infoz_common_devtree_update();
}
#endif	/* !ENV_BOOTBLOCK */
