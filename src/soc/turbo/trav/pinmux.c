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

#include <console/console.h>
#include <assert.h>
#include <stdlib.h>
#include <soc/common_definition.h>
#include <soc/gpio.h>
#include <soc/pinmux.h>
#include <soc/periph.h>

static void trav_uart_config(int peripheral)
{
	struct trav_gpio_part2 *gpio_peric =
			(struct trav_gpio_part2 *)TRAV_GPIO_PERIC;
	struct trav_gpio_bank *bank = NULL;
	int i, start = 0, count = 0, func = 0;

	switch (peripheral) {
	case PERIPH_ID_UART0:
		bank = &gpio_peric->gpb7;
		start = 0;
		count = 4;
		func = 2;
		break;
	case PERIPH_ID_UART1:
		bank = &gpio_peric->gpb7;
		start = 4;
		count = 4;
		func = 2;
		break;
	}
	for (i = start; i < start + count; i++) {
		trav_gpio_set_pull(bank, i, TRAV_GPIO_PULL_NONE);
		trav_gpio_cfg_pin(bank, i, TRAV_GPIO_FUNC(func));
	}
}

void trav_pinmux_uart0(void)
{
	trav_uart_config(PERIPH_ID_UART0);
}

void trav_pinmux_uart1(void)
{
	trav_uart_config(PERIPH_ID_UART1);
}


static void trav_i2c_config(int peripheral)
{
	struct trav_gpio_part2 *gpio_peric =
			(struct trav_gpio_part2 *)TRAV_GPIO_PERIC;

	switch (peripheral) {
	case PERIPH_ID_I2C0:
		trav_gpio_cfg_pin(&gpio_peric->gpb0, 0, TRAV_GPIO_FUNC(0x2));
		trav_gpio_cfg_pin(&gpio_peric->gpb0, 1, TRAV_GPIO_FUNC(0x2));
		break;
	case PERIPH_ID_I2C1:
		trav_gpio_cfg_pin(&gpio_peric->gpb0, 2, TRAV_GPIO_FUNC(0x2));
		trav_gpio_cfg_pin(&gpio_peric->gpb0, 3, TRAV_GPIO_FUNC(0x2));
		break;
	case PERIPH_ID_I2C2:
		trav_gpio_cfg_pin(&gpio_peric->gpb0, 4, TRAV_GPIO_FUNC(0x2));
		trav_gpio_cfg_pin(&gpio_peric->gpb0, 5, TRAV_GPIO_FUNC(0x2));
		break;
	case PERIPH_ID_I2C3:
		trav_gpio_cfg_pin(&gpio_peric->gpb0, 6, TRAV_GPIO_FUNC(0x2));
		trav_gpio_cfg_pin(&gpio_peric->gpb0, 7, TRAV_GPIO_FUNC(0x2));
		break;
	case PERIPH_ID_I2C4:
		trav_gpio_cfg_pin(&gpio_peric->gpb1, 0, TRAV_GPIO_FUNC(0x2));
		trav_gpio_cfg_pin(&gpio_peric->gpb1, 1, TRAV_GPIO_FUNC(0x2));
		break;
	case PERIPH_ID_I2C5:
		trav_gpio_cfg_pin(&gpio_peric->gpb1, 2, TRAV_GPIO_FUNC(0x2));
		trav_gpio_cfg_pin(&gpio_peric->gpb1, 3, TRAV_GPIO_FUNC(0x2));
		break;
	case PERIPH_ID_I2C6:
		trav_gpio_cfg_pin(&gpio_peric->gpb1, 4, TRAV_GPIO_FUNC(0x2));
		trav_gpio_cfg_pin(&gpio_peric->gpb1, 5, TRAV_GPIO_FUNC(0x2));
		break;
	case PERIPH_ID_I2C7:
		trav_gpio_cfg_pin(&gpio_peric->gpb1, 6, TRAV_GPIO_FUNC(0x2));
		trav_gpio_cfg_pin(&gpio_peric->gpb1, 7, TRAV_GPIO_FUNC(0x2));
		break;
	}
}

void trav_pinmux_i2c0(void)
{
	trav_i2c_config(PERIPH_ID_I2C0);
}

void trav_pinmux_i2c1(void)
{
	trav_i2c_config(PERIPH_ID_I2C1);
}

void trav_pinmux_i2c2(void)
{
	trav_i2c_config(PERIPH_ID_I2C2);
}

void trav_pinmux_i2c3(void)
{
	trav_i2c_config(PERIPH_ID_I2C3);
}

void trav_pinmux_i2c4(void)
{
	trav_i2c_config(PERIPH_ID_I2C4);
}

void trav_pinmux_i2c5(void)
{
	trav_i2c_config(PERIPH_ID_I2C5);
}

void trav_pinmux_i2c6(void)
{
	trav_i2c_config(PERIPH_ID_I2C6);
}

void trav_pinmux_i2c7(void)
{
	trav_i2c_config(PERIPH_ID_I2C7);
}
