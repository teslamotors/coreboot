/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _QUALCOMM_CHIP_H_
#define _QUALCOMM_CHIP_H_

#define QCA_VENID		0x17CB
#define QCA_DEVID_6595	0x1102

struct drivers_qualcomm_config {
	const char *hid_wifi;	/* ACPI _HID */
	const char *desc_wifi;	/* Device Description */
	unsigned int uid_wifi;	/* ACPI _UID */
	struct acpi_gpio wlan_en_gpio;

	const char *hid_bt;		/* ACPI _HID */
	const char *desc_bt;	/* Device Description */
	unsigned int uid_bt;	/* ACPI _UID */
	struct acpi_gpio bt_en_gpio;
};

#endif /* _QUALCOMM_CHIP_H_ */
