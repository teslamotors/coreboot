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

/* Register map for TRAV PMU */

#ifndef _CPU_TURBO_TRAV_TIMER_H_
#define _CPU_TURBO_TRAV_TIMER_H_

#include <stddef.h>
#include <soc/cpu.h>

#define MCT_HZ    24000000

struct trav_mct {
	uint32_t	mct_cfg;
	uint8_t		reserved0[0xfc];
	uint32_t	g_cnt_l;
	uint32_t	g_cnt_u;
	uint8_t		reserved1[0x8];
	uint32_t	g_cnt_wstat;
	uint8_t		reserved2[0xec];
	uint32_t	g_comp0_l;
	uint32_t	g_comp0_u;
	uint32_t	g_comp0_addr_incr;
	uint8_t		reserved3[0x4];
	uint32_t	g_comp1_l;
	uint32_t	g_comp1_u;
	uint32_t	g_comp1_addr_incr;
	uint8_t		reserved4[0x4];
	uint32_t	g_comp2_l;
	uint32_t	g_comp2_u;
	uint32_t	g_comp2_addr_incr;
	uint8_t		reserved5[0x4];
	uint32_t	g_comp3_l;
	uint32_t	g_comp3_u;
	uint32_t	g_comp3_addr_incr;
	uint8_t		reserved6[0x4];
	uint32_t	g_tcon;
	uint32_t	g_int_cstat;
	uint32_t	g_int_enb;
	uint32_t	g_wstat;
	uint8_t		reserved7[0xb0];
	uint32_t	l0_tcntb;
	uint32_t	l0_tcnto;
	uint32_t	l0_icntb;
	uint32_t	l0_icnto;
	uint32_t	l0_frcntb;
	uint32_t	l0_frcnto;
	uint8_t		reserved8[0x8];
	uint32_t	l0_tcon;
	uint8_t		reserved9[0xc];
	uint32_t	l0_int_cstat;
	uint32_t	l0_int_enb;
	uint8_t		reserved10[0x8];
	uint32_t	l0_wstat;
	uint8_t		reserved11[0xbc];
	uint32_t	l1_tcntb;
	uint32_t	l1_tcnto;
	uint32_t	l1_icntb;
	uint32_t	l1_icnto;
	uint32_t	l1_frcntb;
	uint32_t	l1_frcnto;
	uint8_t		reserved12[0x8];
	uint32_t	l1_tcon;
	uint8_t		reserved13[0xc];
	uint32_t	l1_int_cstat;
	uint32_t	l1_int_enb;
	uint8_t		reserved14[0x8];
	uint32_t	l1_wstat;
};
check_member(trav_mct, l1_wstat,(size_t)0x440);

static struct trav_mct * const trav_mct =
		(void *)TRAV_MCT;

#endif /* _CPU_TURBO_TRAV_TIMER_H_ */
