/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static const char *mp2_acpi_name(const struct device *dev)
{
	return "MP2C";
}

static struct device_operations mp2_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_static_bus,
	.acpi_name		= mp2_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
};

static const struct pci_driver mp2_driver __pci_driver = {
	.ops	= &mp2_ops,
	.vendor	= PCI_VENDOR_ID_AMD,
	.device	= PCI_DEVICE_ID_AMD_FAM17H_MP2,
};
