/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>

struct drivers_i2c_mp2_config {
	const char *name;	/* ACPI Device Name */
	const char *desc;	/* Device Description */
	unsigned int uid;	/* ACPI _UID */
};
