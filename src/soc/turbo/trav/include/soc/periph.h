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

#ifndef CPU_TURBO_TRAV_PERIPH_H
#define CPU_TURBO_TRAV_PERIPH_H

/*
 * Peripherals requiring clock/pinmux configuration. List will
 * grow with support for more devices getting added.
 *
 * At present the order is arbitrary - we may be able to take advantage
 * of some orthogonality later.
 */
enum periph_id {
	PERIPH_ID_UART0,
	PERIPH_ID_UART1,
	PERIPH_ID_I2C0,
	PERIPH_ID_I2C1,
	PERIPH_ID_I2C2,
	PERIPH_ID_I2C3,
	PERIPH_ID_I2C4,
	PERIPH_ID_I2C5,
	PERIPH_ID_I2C6,
	PERIPH_ID_I2C7,
};

#endif
