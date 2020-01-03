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

#include <types.h>
#include <delay.h>
#include <boot/coreboot_tables.h>
#include <console/uart.h>
#include <arch/io.h>
#include <console/console.h>	/* for __console definition */
#include <soc/clock.h>
#include <soc/common_definition.h>
#include <soc/cpu.h>
#include <soc/periph.h>
#include <soc/uart.h>

static struct trav_uart * base_port =
		(struct trav_uart *)TRAV_PERIC_UART0;

static void serial_setbrg_dev(void)
{
	struct trav_uart *uart = base_port;
	u32 baud,sclk;

	baud = CONFIG_TTYS0_BAUD;
	sclk = clock_get_uart(0);

	uart_set_baudrate(uart, sclk, baud);
}

/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 */
static void trav_init_dev(void)
{
	struct trav_uart *uart = base_port;

	/* enable FIFOs */
	writel(UART_UFCON, &uart->ufcon);
	writel(UART_UMCON, &uart->umcon);
	/* 8N1 */
	writel(UART_ULCON, &uart->ulcon);
	writel(UART_UCON, &uart->ucon);

	serial_setbrg_dev();
}

static int trav_uart_err_check(int op)
{
	struct trav_uart *uart = base_port;
	unsigned int mask;

	/*
	 * UERSTAT
	 * Break Detect	[3]
	 * Frame Err	[2] : receive operation
	 * Parity Err	[1] : receive operation
	 * Overrun Err	[0] : receive operation
	 */
	if (op)
		mask = 0x8;
	else
		mask = 0xf;

	return readl(&uart->uerstat) & mask;
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
static unsigned char trav_uart_rx_byte(void)
{
	struct trav_uart *uart = base_port;

	/* wait for character to arrive */
	while (!(readl(&uart->ufstat) & (RX_FIFO_COUNT_MASK |
					 RX_FIFO_FULL_MASK))) {
		if (trav_uart_err_check(0))
			return 0;
	}

	return readb(&uart->urxh) & 0xff;
}

static unsigned char trav_uart_rx_byte_timeout(void)
{
	struct trav_uart *uart = base_port;
	unsigned int val, count = 0;

	/* wait for 1.5 sec for character to arrive */
	do {
		val = readl(&uart->ufstat) & (RX_FIFO_COUNT_MASK | RX_FIFO_FULL_MASK);
		if (!val) {
			mdelay(1);
			count++;
		}
		else {
			break;
		}
	} while (count < 1500);

	if (trav_uart_err_check(0))
		return 0;

	return readb(&uart->urxh) & 0xff;
}

/*
 * Output a single byte to the serial port.
 */
static void trav_uart_tx_byte(unsigned char data)
{
	struct trav_uart *uart = base_port;

	/* wait for room in the tx FIFO */
	while ((readl(&uart->ufstat) & TX_FIFO_FULL_MASK)) {
		if (trav_uart_err_check(1))
			return;
	}

	writeb(data, &uart->utxh);
}

void uart_init(int idx)
{
	trav_init_dev();
}

unsigned char uart_rx_byte(int idx)
{
	return trav_uart_rx_byte();
}

unsigned char uart_rx_byte_timeout(int idx)
{
	return trav_uart_rx_byte_timeout();
}

void uart_tx_byte(int idx, unsigned char data)
{
	trav_uart_tx_byte(data);
}

void uart_tx_flush(int idx)
{
}

static void uart_set_baudrate(struct trav_uart *uart, u32 sclk, u32 baud)
{
	u32 div, frac;

	div = sclk / (baud * 16) - 1;
	frac = (sclk / baud) - 16 * (1 + div);

	__raw_writel(div, &uart->ubrdiv);
	__raw_writel(frac, &uart->ufracval);
}

#if !defined(__PRE_RAM__)

uintptr_t uart_platform_base(int idx)
{
	return ((uintptr_t) base_port);
}
void uart_fill_lb(void *data)
{
	struct lb_serial serial;
	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	serial.baud = default_baudrate();
	serial.regwidth = 4;
	lb_add_serial(&serial, data);
	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}
#endif
