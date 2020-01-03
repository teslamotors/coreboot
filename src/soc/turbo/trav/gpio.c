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
#include <string.h>
#include <delay.h>
#include <assert.h>
#include <soc/common_definition.h>
#include <soc/gpio.h>
#include <soc/cpu.h>

#define CON_MASK(x)		(0xf << ((x) << 2))
#define CON_SFR(x, v)		((v) << ((x) << 2))

#define DAT_MASK(x)		(0x1 << (x))
#define DAT_SET(x)		(0x1 << (x))

#define PULL_MASK(x)		(0xf << ((x) << 2))
#define PULL_MODE(x, v)		((v) << ((x) << 2))

#define DRV_MASK(x)		(0x3 << ((x) << 2))
#define DRV_SET(x, m)		((m) << ((x) << 2))
#define RATE_MASK(x)		(0x1 << (((x) << 2) + 3))
#define RATE_SET(x)		(0x1 << (((x) << 2) + 3))

void trav_gpio_cfg_pin(struct trav_gpio_bank *bank, int gpio, int cfg)
{
	unsigned int value;

	value = readl(&bank->con);
	value &= ~CON_MASK(gpio);
	value |= CON_SFR(gpio, cfg);
	writel(value, &bank->con);
}

void trav_gpio_direction_output(struct trav_gpio_bank *bank, int gpio, int en)
{
	trav_gpio_set_value(bank, gpio, en);
	trav_gpio_cfg_pin(bank, gpio, TRAV_GPIO_OUTPUT);
}

void trav_gpio_direction_input(struct trav_gpio_bank *bank, int gpio)
{
	trav_gpio_cfg_pin(bank, gpio, TRAV_GPIO_INPUT);
}

void trav_gpio_set_value(struct trav_gpio_bank *bank, int gpio, int en)
{
	unsigned int value;

	value = readl(&bank->dat);
	value &= ~DAT_MASK(gpio);
	if (en)
		value |= DAT_SET(gpio);
	writel(value, &bank->dat);
}

unsigned int trav_gpio_get_value(struct trav_gpio_bank *bank, int gpio)
{
	unsigned int value;

	value = readl(&bank->dat);
	return !!(value & DAT_MASK(gpio));
}

void trav_gpio_set_pull(struct trav_gpio_bank *bank, int gpio, int mode)
{
	unsigned int value;

	value = readl(&bank->pud);
	value &= ~PULL_MASK(gpio);

	switch (mode) {
	case TRAV_GPIO_PULL_DOWN:
	case TRAV_GPIO_PULL_UP:
		value |= PULL_MODE(gpio, mode);
		break;
	default:
		break;
	}

	writel(value, &bank->pud);
}

void trav_gpio_set_drv(struct trav_gpio_bank *bank, int gpio, int mode)
{
	unsigned int value;

	value = readl(&bank->drv);
	value &= ~DRV_MASK(gpio);

	switch (mode) {
	case TRAV_GPIO_DRV_1X:
	case TRAV_GPIO_DRV_2X:
	case TRAV_GPIO_DRV_3X:
	case TRAV_GPIO_DRV_4X:
		value |= DRV_SET(gpio, mode);
		break;
	default:
		return;
	}

	writel(value, &bank->drv);
}

void trav_gpio_set_rate(struct trav_gpio_bank *bank, int gpio, int mode)
{
	unsigned int value;

	value = readl(&bank->drv);
	value &= ~RATE_MASK(gpio);

	switch (mode) {
	case TRAV_GPIO_DRV_FAST:
	case TRAV_GPIO_DRV_SLOW:
		value |= RATE_SET(gpio);
		break;
	default:
		return;
	}

	writel(value, &bank->drv);
}

static struct trav_gpio_bank *trav_gpio_get_bank(unsigned int gpio)
{
	const struct gpio_info *data;
	unsigned int upto;
	int i, count;

	data = get_gpio_data();
	count = get_bank_num();
	for (i = upto = 0; i < count;
	     i++, upto = data->max_gpio, data++) {
		printk(BIOS_INFO, "i=%d, upto=%d\n", i, upto);
		if (gpio < data->max_gpio) {
			struct trav_gpio_bank *bank;
			bank = (struct trav_gpio_bank *)data->reg_addr;
			bank += (gpio - upto) / GPIO_PER_BANK;
			printk(BIOS_INFO, "gpio=%d, bank=%p\n", gpio, bank);
			return bank;
		}
	}
	return NULL;
}

int trav_gpio_get_pin(unsigned gpio)
{
	return gpio % GPIO_PER_BANK;
}

/* Common GPIO API */
int gpio_request(unsigned gpio, const char *label)
{
	return 0;
}

int gpio_free(unsigned gpio)
{
	return 0;
}

int gpio_direction_input(unsigned gpio)
{
	trav_gpio_direction_input(trav_gpio_get_bank(gpio),
				 trav_gpio_get_pin(gpio));
	return 0;
}

int gpio_direction_output(unsigned gpio, int value)
{
	trav_gpio_direction_output(trav_gpio_get_bank(gpio),
				  trav_gpio_get_pin(gpio), value);
	return 0;
}

int gpio_get_value(unsigned gpio)
{
	return (int) trav_gpio_get_value(trav_gpio_get_bank(gpio),
					trav_gpio_get_pin(gpio));
}

int gpio_set_value(unsigned gpio, int value)
{
	trav_gpio_set_value(trav_gpio_get_bank(gpio),
			   trav_gpio_get_pin(gpio), value);

	return 0;
}

void gpio_set_pull(int gpio, int value)
{
	trav_gpio_set_pull(trav_gpio_get_bank(gpio),
			  trav_gpio_get_pin(gpio), value);
}

void gpio_cfg_pin(int gpio, int cfg)
{
	trav_gpio_cfg_pin(trav_gpio_get_bank(gpio),
			 trav_gpio_get_pin(gpio), cfg);
}

void gpio_set_drv(int gpio, int drv)
{
	trav_gpio_set_drv(trav_gpio_get_bank(gpio),
			 trav_gpio_get_pin(gpio), drv);
}
