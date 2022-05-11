/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <stddef.h>
#include <boardid.h>
#include <soc/platform_descriptors.h>
#include "chip.h"

enum infoz_hw_rev {
	INFOZ_HW_UNKNOWN = 0,
	INFOZ_HW_REV_A,
	INFOZ_HW_REV_B,
	INFOZ_HW_REV_C,
	INFOZ_HW_REV_D,
	INFOZ_HW_REV_G,
	INFOZ_HW_REV_HGA,
	INFOZ_HW_REV_HGD,
};

enum infoz_hw_flags {
	INFOZ_HAS_DGPU = BIT(0),
	INFOZ_FIXUP_GPIO_EXP_RESET = BIT(1),
	INFOZ_OLD_REAR_TOUCH_RESET = BIT(2),
	INFOZ_BOARD_ID_6BIT = BIT(3),
	INFOZ_HAS_DGPU_PWREN_GPIO_108 = BIT(4),
	INFOZ_NEEDS_ESPI_RESET = BIT(5),
	INFOZ_QCA_WIFI_BT = BIT(6),
};

enum infoz_boot_bank {
	INFOZ_BOOT_BANK_UNKNOWN = 0,
	INFOZ_BOOT_BANK_A,
	INFOZ_BOOT_BANK_B,
};

/* PCIe peripherals - arbitrarily assigned flag bits for internal usage */
#define INFOZ_PERIPH_COUNT				5
#define INFOZ_PERIPH_NVME				(BIT(0))
#define INFOZ_PERIPH_WIFI				(INFOZ_PERIPH_WIFI_BCM | INFOZ_PERIPH_WIFI_QCA)
#define INFOZ_PERIPH_WIFI_BCM			(BIT(1))
#define INFOZ_PERIPH_WIFI_QCA			(BIT(2))
#define INFOZ_PERIPH_GPU				(INFOZ_PERIPH_GPU_PWREN_GPIO_108 | INFOZ_PERIPH_GPU_PWREN_GPIO_4)
#define INFOZ_PERIPH_GPU_PWREN_GPIO_108	(BIT(3))
#define INFOZ_PERIPH_GPU_PWREN_GPIO_4	(BIT(4))

#define INFOZ_PERIPH_ALL	((1 << INFOZ_PERIPH_COUNT) - 1)

const struct sci_source *get_gpe_table(size_t *num);
const struct soc_amd_gpio *variant_early_gpio_table(size_t *size);
const struct soc_amd_gpio *variant_romstage_gpio_table(size_t *size);
const struct soc_amd_gpio *variant_wifi_romstage_gpio_table(size_t *size);
/*
 * This function provides base GPIO configuration table. It is typically provided by
 * baseboard using a weak implementation. If GPIO configuration for a variant differs
 * significantly from the baseboard, then the variant can also provide a strong implementation
 * of this function.
 */
const struct soc_amd_gpio *variant_base_gpio_table(size_t *size);
/*
 * This function allows variant to override any GPIOs that are different than the base GPIO
 * configuration provided by variant_base_gpio_table().
 */
const struct soc_amd_gpio *variant_override_gpio_table(size_t *size);

/*
 * This function provides GPIO table for the pads that need to be configured when entering
 * sleep.
 */
const struct soc_amd_gpio *variant_sleep_gpio_table(size_t *size, int slp_typ);

void variant_romstage_entry(void);
/* Modify devictree settings during ramstage. */
void variant_devtree_update(void);

/* Per variant FSP-S initialization, default implementation in baseboard and
 * overrideable by the variant. */
void variant_get_dxio_ddi_descriptors(const fsp_dxio_descriptor **dxio_descs,
				      size_t *dxio_num,
				      const fsp_ddi_descriptor **ddi_descs,
				      size_t *ddi_num);

const fsp_dxio_descriptor *variant_get_dxio_descriptors(size_t *num);
const fsp_ddi_descriptor *variant_get_ddi_descriptors(size_t *num);

/* Retrieve attributes from FW_CONFIG in CBI. */
/* Return 1 if FW_CONFIG expected to be valid, else 0. */
int variant_fw_config_valid(void);
/* Return 0 if non-existent, 1 if present. */
int variant_has_emmc(void);
/* Return 0 if non-existent, 1 if present. */
int variant_has_nvme(void);

void variant_gpio_exp_reset(void);
void variant_pcie_devs_power(int on);

int debug_console_requested(void);
int debug_console_is_enabled(void);
void debug_console_set_enabled(void);

/* Determine if booting in factory by using CROS_SKU_UNPROVISIONED. */
int boot_is_factory_unprovisioned(void);

enum infoz_hw_rev variant_get_rev(void);
int variant_get_flags(void);
enum infoz_boot_bank variant_get_boot_bank(void);
bool variant_recovery_forced(void);
const char *variant_board_sku(void);

#endif /* __BASEBOARD_VARIANTS_H__ */
