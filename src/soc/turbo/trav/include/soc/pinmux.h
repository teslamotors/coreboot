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

#ifndef _CPU_TURBO_TRAV_PINMUX_H_
#define _CPU_TURBO_TRAV_PINMUX_H_

/*
 * Flags for setting specific configarations of peripherals.
 * List will grow with support for more devices getting added.
 */
enum {
	PINMUX_FLAG_NONE	= 0x00000000,


	/* Flags for SROM controller */
	PINMUX_FLAG_BANK	= 3 << 0,       /* bank number (0-3) */
	PINMUX_FLAG_16BIT	= 1 << 2,       /* 16-bit width */
};

void trav_pinmux_uart0(void);
void trav_pinmux_uart1(void);

void trav_pinmux_i2c0(void);
void trav_pinmux_i2c1(void);
void trav_pinmux_i2c2(void);
void trav_pinmux_i2c3(void);
void trav_pinmux_i2c4(void);
void trav_pinmux_i2c5(void);
void trav_pinmux_i2c6(void);
void trav_pinmux_i2c7(void);

#endif /* _CPU_TURBO_TRAV_PINMUX_H_ */
