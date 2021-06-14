/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>

#include "tpm.h"

void crb_tpm_dsm_query(void *arg)
{
	acpigen_write_return_singleton_buffer(0x03);
}

void __weak crb_tpm_dsm_start(void *arg)
{
	acpigen_write_return_integer(0);
}
