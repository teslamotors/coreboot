/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PICASSO_RPMC_H__
#define __PICASSO_RPMC_H__

#include <types.h>

/*
 * Query PSP for SPI RPMC support.
 *
 * Returns -1 on error, 0 if RPMC not supported, and 1 if RPMC supported.
 */
int rpmc_supported(void);

/*
 * Query HSTI for SPI RPMC provisioned state.
 *
 * Returns -1 on error, 0 if RPMC is not provisioned, and 1 if RPMC
 * provisioning is locked or provisioning was successful.
 */
int rpmc_provisioned(void);

/*
 * Provision RPMC root key for primary SPI device and optionally lock
 * RPMC root key provisioning.
 *
 * Returns -1 on error, otherwise performs warm reset.
 */
int rpmc_provision(bool lock);

#endif /* __PICASSO_RPMC_H__ */
