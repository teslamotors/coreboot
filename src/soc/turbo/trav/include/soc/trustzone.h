/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef CPU_TURBO_TRAV_TRUSTZONE_H
#define CPU_TURBO_TRAV_TRUSTZONE_H

#include <stdint.h>

#define TRUSTZONE_DISABLE	0xffffffff
#define TRUSTZONE_DISABLE_EXCEPT_FMP 0xffffffef
#define TZPC_PROT0SET	0x204
#define TZPC_PROT1SET	0x214
#define SECURITY0	0x08
#define SECURITY1	0x0c
#define SECURITY2	0x10
#define SECURITY3	0x14
#define SECURITY4	0x18
#define SECURITY5	0x1c
#define TZPC_PERIC_0	0x2000
#define TZPC_PERIC_1	0x2004
#define TZPC_PERIC_2	0x2008
#define TZPC_PERIC_3	0x200C
#define TZPC_PERIC_4	0x2010
#define TZPC_PERIC_5	0x2014
#define TZPC_FSYS0_0	0x2000
#define TZPC_FSYS0_1	0x2004
#define TZPC_PMU_0	0x2000
void trustzone_init(void);

#endif	/* CPU_TURBO_TRAV_TRUSTZONE_H */
