/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include "chip.h"

#define MP2_ACPI_HID "AMDI0011"

static void mp2_fill_ssdt(const struct device *dev)
{
	struct drivers_i2c_mp2_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	const char *path = acpi_device_path(dev);

	if (!dev || !dev->enabled || !scope || !path)
		return;

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", MP2_ACPI_HID);
	acpigen_write_name_integer("_UID", config->uid);
	acpigen_write_name_integer("_ADR", config->uid);
	acpigen_write_name_string("_DDN", config->desc);
	acpigen_write_STA(acpi_device_status(dev));

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", path, dev->chip_ops->name, dev_path(dev));
}

static const char *mp2_acpi_name(const struct device *dev)
{
	struct drivers_i2c_mp2_config *config = dev->chip_info;
	static char name[5];

	if (config->name)
		return config->name;

	snprintf(name, sizeof(name), "D%03.3X", dev->path.i2c.device);
	return name;
}

static struct device_operations mp2_ops = {
	.read_resources	= noop_read_resources,
	.set_resources	= noop_set_resources,
	.scan_bus	= scan_static_bus,
	.acpi_name	= mp2_acpi_name,
	.acpi_fill_ssdt	= mp2_fill_ssdt,
};

static void mp2_enable(struct device *dev)
{
	struct drivers_i2c_mp2_config *config = dev->chip_info;

	if (!config)
		return;

	dev->ops = &mp2_ops;

	/* Name the device as per description provided in devicetree */
	if (config->desc)
		dev->name = config->desc;
}

struct chip_operations drivers_i2c_mp2_ops = {
	CHIP_NAME("AMD MP2 I2C Bus Device")
	.enable_dev = mp2_enable
};
