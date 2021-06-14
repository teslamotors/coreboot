/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __DRIVERS_AMD_DGPU_H__
#define __DRIVERS_AMD_DGPU_H__

#include <acpi/acpi_device.h>

struct drivers_amd_dgpu_config {
	/* Does the device have a power resource? */
	bool has_power_resource;

	/* GPIO used to take device out of reset or to put it into reset. */
	struct acpi_gpio reset_gpio;
	/* GPIO used to enable device. */
	struct acpi_gpio enable_gpio;
	/* GPIO indicating dGPU power is good */
	struct acpi_gpio pgood_gpio;

	/* Delay to be inserted after device is taken out of reset. */
	unsigned int reset_delay_ms;
	/* Delay to be inserted after device is put into reset. */
	unsigned int reset_off_delay_ms;
	/* Delay to be inserted after device is enabled. */
	unsigned int enable_delay_ms;
	/* Delay to be inserted after device is disabled. */
	unsigned int enable_off_delay_ms;
};

#endif /* ___DRIVERS_AMD_DGPU_H__ */
