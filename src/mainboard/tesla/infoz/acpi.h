/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __INFOZ_ACPI_H__
#define __INFOZ_ACPI_H__

#include <types.h>

typedef struct acpi_oem_hsti_t {
	acpi_header_t header;
	u32 state;
} __packed acpi_oem_hsti_t;

typedef struct acpi_oem_psb_t {
	acpi_header_t header;
	u32 config;
	u16 bios_key_rev_id;
	u8 root_key_hash[32];
} __packed acpi_oem_psb_t;

typedef struct acpi_oem_state_t {
	acpi_header_t header;
	u8 state;
} __packed acpi_oem_state_t;

typedef struct acpi_psb_hsti1_t {
	acpi_header_t header;
	u32 state;
} __packed acpi_psb_hsti1_t;

#endif  /* __INFOZ_ACPI_H__ */
