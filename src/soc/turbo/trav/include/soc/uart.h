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

#ifndef _CPU_TURBO_TRAV_UART_H_
#define _CPU_TURBO_TRAV_UART_H_

#include <stddef.h>

#define UART_UFCON	0x111
#define UART_UMCON	0
#define UART_ULCON	0x3
#define UART_UCON	0x3c5


#define RX_FIFO_COUNT_MASK	0xff
#define RX_FIFO_FULL_MASK	(1 << 8)
#define TX_FIFO_FULL_MASK	(1 << 24)

struct trav_uart {
	uint32_t	ulcon;
	uint32_t	ucon;
	uint32_t	ufcon;
	uint32_t	umcon;
	uint32_t	utrstat;
	uint32_t	uerstat;
	uint32_t	ufstat;
	uint32_t	umstat;
	uint8_t		utxh;
	uint8_t		res1[3];
	uint8_t		urxh;
	uint8_t		res2[3];
	uint32_t	ubrdiv;
	uint32_t	ufracval;
	uint8_t		res3[0xffcc];
};
check_member(trav_uart, ufracval, 0x2C);

static void uart_set_baudrate(struct trav_uart *uart, u32 sclk, u32 baud);
unsigned char uart_rx_byte_timeout(int idx);
#endif /* _CPU_TURBO_TRAV_UART_H_ */
