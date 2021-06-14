/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __INFOZ_BOARD_UTILS_H__
#define __INFOZ_BOARD_UTILS_H__

#include <stddef.h>
#include <boardid.h>
#include <soc/platform_descriptors.h>
#include <drivers/i2c/generic/chip.h>
#include "chip.h"

/*
 * Nested device path to an I2C device on the MP2 I2C controller
 * Used with find_dev_nested_path()
 */
#define mp2_i2c_dev_path(bus_num, slave_addr) \
	{ \
		{ .type = DEVICE_PATH_PCI, .pci.devfn = PCIE_GPP_A_DEVFN }, \
		{ .type = DEVICE_PATH_PCI, .pci.devfn = FUSION_HUB_DEVFN }, \
		{ \
			.type = DEVICE_PATH_GENERIC, \
			.generic.id = bus_num , \
			.generic.subid = 0 , \
		}, \
		{ .type = DEVICE_PATH_I2C, .i2c.device = slave_addr } \
	}

struct drivers_i2c_generic_config *
get_mmio_i2c_device_cfg(uintptr_t mmio_base, unsigned int slave_addr);

struct drivers_i2c_generic_config *
get_i2c_device_cfg(const struct device_path path[], size_t path_len,
		   const char *dev_name);

void update_i2c_reset_gpio(const struct device_path path[],
			   size_t path_len, const char *dev_name,
			   int gpio_num);

void update_i2c_int_prop(struct drivers_i2c_generic_config *i2c_cfg,
			 int idx, int val);

struct device * infoz_find_dgpu_dev(void);

void find_and_disable_dev(const struct device_path path[], size_t path_len);

int infoz_dgpu_power_good(void);

void infoz_disable_dgpu_devs(void);

void infoz_common_devtree_update(void);

#endif /* __INFOZ_BOARD_UTILS_H__ */
