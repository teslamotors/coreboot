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

#ifndef _SOC_TURBO_TRAV_SCS_HELPERS_H_
#define _SOC_TURBO_TRAV_SCS_HELPERS_H_

void copy_coreboot_rom(void);
void check_turbo_a_b(void);
int get_turbo_cpu_rev(void);
void check_scs_versions(void);
void copy_bootblock(void);
void run_bootblock(void);
void check_a72_warm_reset(void);

int get_otp_ddr_info(uint32_t *info);
int set_otp_ddr_info(uint32_t info);

int slog_log_message(const char *data, size_t size);

#define	VERIFY_BOOTBLOCK_A	1
#define	VERIFY_BOOTBLOCK_B	((VERIFY_BOOTBLOCK_A) + 1)
#define	VERIFY_COREBOOT_A	10
#define	VERIFY_COREBOOT_B	((VERIFY_COREBOOT_A) + 1)

int verify_coreboot(int index, size_t buffer, size_t in_size, size_t buffer_crt, size_t in_size_crt);

void scs_service_init(void);

#endif /* _SOC_TURBO_TRAV_SCS_HELPERS_H_ */
