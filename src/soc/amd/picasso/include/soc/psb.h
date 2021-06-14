/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PICASSO_PSB_H__
#define __PICASSO_PSB_H__

#include <types.h>

int psb_get_hsti_status(uint32_t *status);

int psb_enable(void);

int psb_update_anti_rollback(void);

#endif /* __PICASSO_PSB_H__ */
