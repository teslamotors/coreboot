/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include "chip.h"

/*
 * ACPI NAME = "WF" ("BT") + hex value of last 8 bits of dev_path_encode + '\0'
 * The above representation returns unique and consistent name every time
 * wifi_qualcomm_acpi_name (bt_qualcomm_acpi_name) is invoked. The last 8 bits
 * chosen since it contains the bus address of the device.
 */
#define QCA_ACPI_NAME_MAX_LEN 5

#if CONFIG(HAVE_ACPI_TABLES)
static const char *wifi_qualcomm_acpi_name(const struct device *dev)
{
	static char wifi_acpi_name[QCA_ACPI_NAME_MAX_LEN];

	/* ACPI 6.3, ASL 20.2.2: (Name Objects Encoding). */
	snprintf(wifi_acpi_name, sizeof(wifi_acpi_name), "WF%02X",
		 (dev_path_encode(dev) & 0xff));
	return wifi_acpi_name;
}

static const char *bt_qualcomm_acpi_name(const struct device *dev)
{
	static char bt_acpi_name[QCA_ACPI_NAME_MAX_LEN];

	/* ACPI 6.3, ASL 20.2.2: (Name Objects Encoding). */
	snprintf(bt_acpi_name, sizeof(bt_acpi_name), "BT%02X",
		 (dev_path_encode(dev) & 0xff));
	return bt_acpi_name;
}

static int is_supported_device(const struct device *dev)
{
	if ((dev->vendor == QCA_VENID) && (dev->device == QCA_DEVID_6595))
		return 1;

	return 0;
}

static void wifi_qualcomm_fill_ssdt(const struct device *dev)
{
	const struct drivers_qualcomm_config *config = dev->chip_info;
	struct acpi_dp *dsd = NULL;
	char path[DEVICE_PATH_MAX] = {};

	if (!dev->enabled)
		return;

	if (!is_supported_device(dev))
		return;

	if (!acpi_device_path(dev->bus->dev))
		return;

	/* Scope */
	acpigen_write_scope("\\_SB");

	/* Wifi Device */
	acpigen_write_device(acpi_device_name(dev));

	acpigen_write_name_string("_HID", config->hid_wifi);
	acpigen_write_name_integer("_UID", config->uid_wifi);
	acpigen_write_name_string("_DDN", config->desc_wifi);
	acpigen_write_STA(acpi_device_status(dev));

	/* Generate device path in \_SB scope */
	snprintf(path, sizeof(path), "%s.%s", "\\_SB", acpi_device_name(dev));

	if (config->wlan_en_gpio.pin_count == 1) {
		/* Resources */
		acpigen_write_name("_CRS");
		acpigen_write_resourcetemplate_header();
		acpi_device_write_gpio(&config->wlan_en_gpio);
		acpigen_write_resourcetemplate_footer();

		/* DSD */
		dsd = acpi_dp_new_table("_DSD");
		acpi_dp_add_gpio(dsd, "wlan-en-gpio", path,
					0, 0,
					config->wlan_en_gpio.active_low);
		acpi_dp_write(dsd);
	}

	acpigen_pop_len(); /* WiFi Device */

	printk(BIOS_INFO, "%s: %s %s\n", path,
	       dev->chip_ops ? dev->chip_ops->name : "", dev_path(dev));

	/* BT Device */
	acpigen_write_device(bt_qualcomm_acpi_name(dev));

	acpigen_write_name_string("_HID", config->hid_bt);
	acpigen_write_name_integer("_UID", config->uid_bt);
	acpigen_write_name_string("_DDN", config->desc_bt);
	acpigen_write_STA(acpi_device_status(dev));

	/* Generate device path in \_SB scope */
	snprintf(path, sizeof(path), "%s.%s", "\\_SB", bt_qualcomm_acpi_name(dev));

	if (config->bt_en_gpio.pin_count == 1) {
		/* Resources */
		acpigen_write_name("_CRS");
		acpigen_write_resourcetemplate_header();
		acpi_device_write_gpio(&config->bt_en_gpio);
		acpigen_write_resourcetemplate_footer();

		/* DSD */
		dsd = acpi_dp_new_table("_DSD");
		acpi_dp_add_gpio(dsd, "bt-en-gpio", path,
					0, 0,
					config->bt_en_gpio.active_low);
		acpi_dp_write(dsd);
	}

	acpigen_pop_len(); /* BT Device */

	acpigen_pop_len(); /* Scope */
}
#endif

struct device_operations wifi_qualcomm_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= wifi_qualcomm_acpi_name,
	.acpi_fill_ssdt		= wifi_qualcomm_fill_ssdt,
#endif
};

static void wifi_qualcomm_enable(struct device *dev)
{
	struct drivers_qualcomm_config *config = dev ? dev->chip_info : NULL;

	if (!config)
		return;

	dev->ops = &wifi_qualcomm_ops;
}

struct chip_operations drivers_qualcomm_ops = {
	CHIP_NAME("WiFi/BT Qualcomm")
	.enable_dev = wifi_qualcomm_enable
};

static const unsigned short qualcomm_pci_device_ids[] = {
	QCA_DEVID_6595,
	0
};

static const struct pci_driver qualcomm_wifi_pci_driver __pci_driver = {
	.ops		= &wifi_qualcomm_ops,
	.vendor		= QCA_VENID,
	.devices	= qualcomm_pci_device_ids,
};
