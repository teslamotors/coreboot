/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <inttypes.h>
#include <soc/pci_devs.h>
#include <drivers/i2c/generic/chip.h>
#include <drivers/amd/dgpu/chip.h>
#include <device/device.h>
#include <infoz/gpio.h>
#include <infoz/variants.h>
#include <gpio.h>
#include "board_utils.h"

/* Find generic i2c config for chip under given MMIO-based I2C controller */
struct drivers_i2c_generic_config *
get_mmio_i2c_device_cfg(uintptr_t mmio_base, unsigned int slave_addr)
{
	struct device *mmio_dev = NULL;
	struct device *dev = NULL;

	mmio_dev = dev_find_path(NULL, DEVICE_PATH_MMIO);
	while (mmio_dev != NULL) {
		if (mmio_dev->path.mmio.addr == mmio_base)
			break;
		mmio_dev = dev_find_path(mmio_dev, DEVICE_PATH_MMIO);
	}

	if (mmio_dev == NULL) {
		printk(BIOS_ERR, "Could not find MMIO base 0x%" PRIxPTR " in device tree\n", mmio_base);
		return NULL;
	}

	while ((dev = dev_bus_each_child(mmio_dev->link_list, dev)) != NULL) {
		if (dev->path.type != DEVICE_PATH_I2C)
			continue;
		if (dev->path.i2c.device == slave_addr)
			return config_of(dev);
	}

	printk(BIOS_ERR, "Could not find I2C slave addr 0x%02x in device tree\n", slave_addr);

	return NULL;
}

/* Find generic i2c device in device tree */
struct drivers_i2c_generic_config *
get_i2c_device_cfg(const struct device_path path[], size_t path_len,
		   const char *dev_name)
{
	const struct device *dev;

	dev = find_dev_nested_path(pci_root_bus(), path, path_len);
	if (!dev) {
		printk(BIOS_ERR, "%s: %s not found\n", __func__, dev_name);
		return NULL;
	}

	return (struct drivers_i2c_generic_config *) config_of(dev);
}

/* Update i2c generic device config's reset_gpio value */
void update_i2c_reset_gpio(const struct device_path path[],
				       size_t path_len, const char *dev_name,
				       int gpio_num)
{
	struct drivers_i2c_generic_config *i2c_cfg;

	i2c_cfg = get_i2c_device_cfg(path, path_len, dev_name);
	if (!i2c_cfg)
		return;

	i2c_cfg->reset_gpio.pin_count = 1;
	i2c_cfg->reset_gpio.pins[0] = gpio_num;

	printk(BIOS_INFO, "%s: uid %d: Fixing up reset_gpio pin to %d\n",
	       i2c_cfg->hid, i2c_cfg->uid, gpio_num);
}

/* Update i2c generic device's property list int value */
void update_i2c_int_prop(struct drivers_i2c_generic_config *i2c_cfg,
				       int idx, int val)
{
	if (!i2c_cfg)
		return;

	i2c_cfg->property_list[idx].integer = val;
	printk(BIOS_INFO, "%s: uid %d: Fixing up integer property %d to %d\n",
	       i2c_cfg->hid, i2c_cfg->uid, idx, val);
}

struct device * infoz_find_dgpu_dev(void)
{
	struct device *dev;
	/* Define how to find dGPU in devicetree */
	static const struct device_path dgpu_path[] = {
		{ .type = DEVICE_PATH_PCI, .pci.devfn = PCIE_GPP_0_DEVFN },
		{ .type = DEVICE_PATH_PCI, .pci.devfn = PCI_DEVFN(0,0) },
	};

	dev = find_dev_nested_path(pci_root_bus(), dgpu_path, ARRAY_SIZE(dgpu_path));

	return dev;
}

/*
 * Return 1 if dGPU power rails are OK.  Only meaningful when dGPU input power
 * is enabled.
 */
int infoz_dgpu_power_good(void)
{
	struct device *dgpu_dev;
	struct drivers_amd_dgpu_config *dgpu_cfg;
	int flags;

	flags = variant_get_flags();

	if (!(flags & INFOZ_HAS_DGPU))
		return 0;

	dgpu_dev = infoz_find_dgpu_dev();
	if (!dgpu_dev)
		return 0;

	dgpu_cfg = (struct drivers_amd_dgpu_config *) config_of(dgpu_dev);
	if (dgpu_cfg->pgood_gpio.pin_count != 1)
		return 0;

	return gpio_get(dgpu_cfg->pgood_gpio.pins[0]);
}

void find_and_disable_dev(const struct device_path path[], size_t path_len)
{
	struct device *dev;

	dev = find_dev_nested_path(pci_root_bus(), path, path_len);
	if (!dev)
		return;

	dev->enabled = 0;
}

void infoz_disable_dgpu_devs(void)
{
	static const struct device_path temp_diff[] = mp2_i2c_dev_path(0, 0x4c);
	static const struct device_path temp_a[] = mp2_i2c_dev_path(0, 0x4d);
	static const struct device_path temp_b[] = mp2_i2c_dev_path(0, 0x4f);
	struct device *dgpu_dev;

	printk(BIOS_NOTICE, "Disabling dGPU devices\n");
	find_and_disable_dev(temp_diff, ARRAY_SIZE(temp_diff));
	find_and_disable_dev(temp_a, ARRAY_SIZE(temp_a));
	find_and_disable_dev(temp_b, ARRAY_SIZE(temp_b));

	dgpu_dev = infoz_find_dgpu_dev();
	if (dgpu_dev)
		dgpu_dev->enabled = 0;
}

/* Update gpio pins for dgpu based on variant. */
void infoz_dgpu_update_gpio(void)
{
	struct device *dgpu_dev;
	struct drivers_amd_dgpu_config *dgpu_cfg;
	int flags;

	flags = variant_get_flags();

	if (!(flags & INFOZ_HAS_DGPU))
		return;

	dgpu_dev = infoz_find_dgpu_dev();
	if (!dgpu_dev)
		return;

	dgpu_cfg = (struct drivers_amd_dgpu_config *) config_of(dgpu_dev);

	/* Check for older hardware rev with dgpu power gpio pin 108. */
	if (flags & INFOZ_HAS_DGPU_PWREN_GPIO_108)
	{
		dgpu_cfg->enable_gpio.pin_count = 1;
		dgpu_cfg->enable_gpio.pins[0] = GPIO_108;
	}
}

#define INFOZ_OLD_REAR_TOUCH_RESET_GPIO	144
#define INFOZ_LINUX_GPIO_BASE		256

/* ramstage devicetree fixups applicable for all InfoZ variants */
void infoz_common_devtree_update(void)
{
	/* U5011 I2C GPIO Expander */
	static const struct device_path u5011_path[] = mp2_i2c_dev_path(1, 0x20);
	/* U5013 I2C GPIO Expander */
	static const struct device_path u5013_path[] = mp2_i2c_dev_path(0, 0x21);
	/* U5014 I2C GPIO Expander */
	static const struct device_path u5014_path[] = mp2_i2c_dev_path(0, 0x20);
	int flags;

	flags = variant_get_flags();

	if (flags & INFOZ_FIXUP_GPIO_EXP_RESET) {
		/*
		 * This variant uses GPIO lines for resetting the GPIO
		 * expanders that differ from the devicetree
		 * reset_gpio values.  Fix these up dynamically.
		 */
		update_i2c_reset_gpio(u5011_path, ARRAY_SIZE(u5011_path),
				      "GPIO Expander U5011", GPIO_13);
		update_i2c_reset_gpio(u5013_path, ARRAY_SIZE(u5013_path),
				      "GPIO Expander U5013", GPIO_16);
		update_i2c_reset_gpio(u5014_path, ARRAY_SIZE(u5014_path),
				      "GPIO Expander U5014", GPIO_3);
	}

	if (flags & INFOZ_OLD_REAR_TOUCH_RESET) {
		struct drivers_i2c_generic_config *i2c_cfg;

		/* SIW17700 rear touch device */
		i2c_cfg = get_mmio_i2c_device_cfg(APU_I2C3_BASE, 0x28);
		update_i2c_int_prop(i2c_cfg, 1,
				    INFOZ_OLD_REAR_TOUCH_RESET_GPIO +
				    INFOZ_LINUX_GPIO_BASE);
	}

	/* Update gpio pins for dgpu based on variant. */
	infoz_dgpu_update_gpio();

	/*
	 * dGPU should be powered at this point.  If power isn't up,
	 * we either don't have a dGPU installed or it's malfunctioning.
	 * Disable the associated devices in this case.
	 */
	if (!infoz_dgpu_power_good())
		infoz_disable_dgpu_devs();
}
