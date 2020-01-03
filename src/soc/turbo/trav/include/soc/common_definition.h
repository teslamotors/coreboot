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

#ifndef _CPU_TURBO_TRAV_COMMON_DEFINITION_H_
#define _CPU_TURBO_TRAV_COMMON_DEFINITION_H_

#include <arch/io.h>

#define writell(_v, _a)				write64((void *)(_a), (uint64_t)(_v))
#define readll(a)				read64((void *)(a))
#define writel(a,b)					write32((void *)(b), (unsigned long)(a))
#define readl(a)					read32((void *)(a))
#define writeb(_v, _a)				(*(volatile unsigned char *) (_a) = (_v))
#define readb(_a)					(*(volatile const unsigned char *) (_a))
#define __raw_writel(v, a)			(*(volatile unsigned int *)(uintptr_t)(a) = (v))
#define __raw_readl(a)				(*(volatile unsigned int *)(uintptr_t)(a))
#define setbit(addr, bit)			__raw_writel(__raw_readl(addr) | (1 << bit), addr)
#define clearbit(addr, bit)			__raw_writel(__raw_readl(addr) & ~(1 << bit), addr)
/* Read a 32 bit pointer */
#define READ32(x)			*((uint32_t *) (uintptr_t) x)
/* Typecast to a 32 bit value */
#define TYPECAST32(x)			((uint32_t) (uintptr_t) x)

#endif	/* _CPU_TURBO_TRAV_COMMON_DEFINITION_H_ */
