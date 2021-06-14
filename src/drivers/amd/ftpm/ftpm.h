/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This driver implements AMD fTPM functionality that differs from the standard
 * TPM CRB specification.
 */
#include <types.h>

/*
 * ftpm_active
 *
 * Checks if the TPM_SUPPORTED Bit is set in the PSP capabilities mailbox response.
 * This is needed in order to use the fTPM, because we have to check prior using
 * the interface that this bit is set correctly - otherwise the interface identification
 * and control area registers will not be populated.
 */
bool ftpm_active(void);
