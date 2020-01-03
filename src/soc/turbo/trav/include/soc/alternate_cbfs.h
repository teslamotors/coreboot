/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Samsung Electronics
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

#ifndef _CPU_TURBO_TRAV_ALTERNATE_CBFS_H_
#define _CPU_TURBO_TRAV_ALTERNATE_CBFS_H_

/* To Do : Define CBFS related definitions */

int load_bootblock_from_ufs(void);
void load_coreboot_from_ufs(void);

#endif /* _CPU_TURBO_TRAV_ALTERNATE_CBFS_H_ */
