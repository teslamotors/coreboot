/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Tesla Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <types.h>

#ifndef _SOC_TURBO_TRAV_SMS_HELPERS_H_
#define _SOC_TURBO_TRAV_SMS_HELPERS_H_

#define RESET_MODE_WARM   1
#define RESET_MODE_COLD   0

void sms_service_init(void);
void check_sms_versions(void);
void sms_reset(int reset_mode);

#endif /* _SOC_TURBO_TRAV_SMS_HELPERS_H_ */
