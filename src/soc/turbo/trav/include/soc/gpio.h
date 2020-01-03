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

#ifndef _CPU_TURBO_TRAV_GPIO_H_
#define _CPU_TURBO_TRAV_GPIO_H_

#include <soc/cpu.h>

struct trav_gpio_bank {
	unsigned int	con;
	unsigned int	dat;
	unsigned int	pud;
	unsigned int	drv;
	unsigned int	pdn_con;
	unsigned int	pdn_pud;
	unsigned char	res1[8];
};

struct trav_gpio_part1 {	/* 0x15020000 - FSYS0 */
	struct trav_gpio_bank gpf0;
	struct trav_gpio_bank gpf1;
	struct trav_gpio_bank gpf6;
	struct trav_gpio_bank gpf4;
	struct trav_gpio_bank gpf5;
};

struct trav_gpio_part2 {	/* 0x141F0000 -PERIC */
	struct trav_gpio_bank gpc8;
	struct trav_gpio_bank gpf2;
	struct trav_gpio_bank gpf3;
	struct trav_gpio_bank gpd0;
	struct trav_gpio_bank gpb0;
	struct trav_gpio_bank gpb1;
	struct trav_gpio_bank gpb4;
	struct trav_gpio_bank gpb5;
	struct trav_gpio_bank gpb6;
	struct trav_gpio_bank gpb7;
	struct trav_gpio_bank gpd1;
	struct trav_gpio_bank gpd2;
	struct trav_gpio_bank gpd3;
	struct trav_gpio_bank gpg[8];
};
struct trav_gpio_part3 {	/* 0x114F0000 - PMU */
	struct trav_gpio_bank gpq0;
};

/* functions */
void trav_gpio_cfg_pin(struct trav_gpio_bank *bank, int gpio, int cfg);
void trav_gpio_direction_output(struct trav_gpio_bank *bank, int gpio, int en);
void trav_gpio_direction_input(struct trav_gpio_bank *bank, int gpio);
void trav_gpio_set_value(struct trav_gpio_bank *bank, int gpio, int en);
unsigned int trav_gpio_get_value(struct trav_gpio_bank *bank, int gpio);
void trav_gpio_set_pull(struct trav_gpio_bank *bank, int gpio, int mode);
void trav_gpio_set_drv(struct trav_gpio_bank *bank, int gpio, int mode);
void trav_gpio_set_rate(struct trav_gpio_bank *bank, int gpio, int mode);
int trav_gpio_get_pin(unsigned gpio);

/* GPIO pins per bank  */
#define GPIO_PER_BANK 3

/* A list of valid GPIO numbers for the asm-generic/gpio.h interface */
enum trav_gpio_pin {
	/* GPIO_PART1_STARTS */
	TRAV_GPIO_F00,	/* 0 */
	TRAV_GPIO_F01,
	TRAV_GPIO_F02,
	TRAV_GPIO_F03,
	TRAV_GPIO_F04,
	TRAV_GPIO_F05,
	TRAV_GPIO_F06,
	TRAV_GPIO_F10,	/* 7  0x20 */
	TRAV_GPIO_F11,
	TRAV_GPIO_F12,
	TRAV_GPIO_F13,
	TRAV_GPIO_F14,
	TRAV_GPIO_F15,
	TRAV_GPIO_F16,
	TRAV_GPIO_F17,
	TRAV_GPIO_F60,	/* 15   0x40 */
	TRAV_GPIO_F61,
	TRAV_GPIO_F62,
	TRAV_GPIO_F40,	/* 18   0x40 */
	TRAV_GPIO_F41,
	TRAV_GPIO_F50,	/* 20   0x40 */
	TRAV_GPIO_F51,
	TRAV_GPIO_F52,
	TRAV_GPIO_F53,
	TRAV_GPIO_F54,
	TRAV_GPIO_F55,

	/* GPIO_PART2_STARTS */
	TRAV_GPIO_MAX_PORT_PART_1,
	TRAV_GPIO_C80 = TRAV_GPIO_MAX_PORT_PART_1,
	TRAV_GPIO_C81,
	TRAV_GPIO_C82,
	TRAV_GPIO_C83,
	TRAV_GPIO_F21,
	TRAV_GPIO_F22,
	TRAV_GPIO_F23,
	TRAV_GPIO_F24,
	TRAV_GPIO_F25,
	TRAV_GPIO_F26,
	TRAV_GPIO_F30,
	TRAV_GPIO_F31,
	TRAV_GPIO_F32,
	TRAV_GPIO_F33,
	TRAV_GPIO_F34,
	TRAV_GPIO_F35,
	TRAV_GPIO_F36,
	TRAV_GPIO_F37,
	TRAV_GPIO_D00,
	TRAV_GPIO_D01,
	TRAV_GPIO_D02,
	TRAV_GPIO_D03,
	TRAV_GPIO_D04,
	TRAV_GPIO_D05,
	TRAV_GPIO_D06,
	TRAV_GPIO_D07,
	TRAV_GPIO_B00,
	TRAV_GPIO_B01,
	TRAV_GPIO_B02,
	TRAV_GPIO_B03,
	TRAV_GPIO_B04,
	TRAV_GPIO_B05,
	TRAV_GPIO_B06,
	TRAV_GPIO_B07,
	TRAV_GPIO_B10,
	TRAV_GPIO_B11,
	TRAV_GPIO_B12,
	TRAV_GPIO_B13,
	TRAV_GPIO_B14,
	TRAV_GPIO_B15,
	TRAV_GPIO_B16,
	TRAV_GPIO_B17,
	TRAV_GPIO_B40,
	TRAV_GPIO_B41,
	TRAV_GPIO_B42,
	TRAV_GPIO_B43,
	TRAV_GPIO_B44,
	TRAV_GPIO_B45,
	TRAV_GPIO_B46,
	TRAV_GPIO_B47,
	TRAV_GPIO_B50,
	TRAV_GPIO_B51,
	TRAV_GPIO_B52,
	TRAV_GPIO_B53,
	TRAV_GPIO_B60,
	TRAV_GPIO_B61,
	TRAV_GPIO_B62,
	TRAV_GPIO_B63,
	TRAV_GPIO_B64,
	TRAV_GPIO_B65,
	TRAV_GPIO_B66,
	TRAV_GPIO_B67,
	TRAV_GPIO_B70,
	TRAV_GPIO_B71,
	TRAV_GPIO_B72,
	TRAV_GPIO_B73,
	TRAV_GPIO_B74,
	TRAV_GPIO_B75,
	TRAV_GPIO_B76,
	TRAV_GPIO_B77,
	TRAV_GPIO_D10,
	TRAV_GPIO_D11,
	TRAV_GPIO_D12,
	TRAV_GPIO_D13,
	TRAV_GPIO_D14,
	TRAV_GPIO_D20,
	TRAV_GPIO_D21,
	TRAV_GPIO_D22,
	TRAV_GPIO_D23,
	TRAV_GPIO_D24,
	TRAV_GPIO_D30,
	TRAV_GPIO_D31,
	TRAV_GPIO_D32,
	TRAV_GPIO_D33,
	TRAV_GPIO_D34,
	TRAV_GPIO_D35,
	TRAV_GPIO_D36,
	TRAV_GPIO_G00,
	TRAV_GPIO_G01,
	TRAV_GPIO_G02,
	TRAV_GPIO_G03,
	TRAV_GPIO_G04,
	TRAV_GPIO_G05,
	TRAV_GPIO_G06,
	TRAV_GPIO_G07,
	TRAV_GPIO_G10,
	TRAV_GPIO_G11,
	TRAV_GPIO_G12,
	TRAV_GPIO_G13,
	TRAV_GPIO_G14,
	TRAV_GPIO_G15,
	TRAV_GPIO_G16,
	TRAV_GPIO_G17,
	TRAV_GPIO_G20,
	TRAV_GPIO_G21,
	TRAV_GPIO_G22,
	TRAV_GPIO_G23,
	TRAV_GPIO_G24,
	TRAV_GPIO_G25,
	TRAV_GPIO_G26,
	TRAV_GPIO_G27,
	TRAV_GPIO_G30,
	TRAV_GPIO_G31,
	TRAV_GPIO_G32,
	TRAV_GPIO_G33,
	TRAV_GPIO_G34,
	TRAV_GPIO_G35,
	TRAV_GPIO_G36,
	TRAV_GPIO_G37,
	TRAV_GPIO_G40,
	TRAV_GPIO_G41,
	TRAV_GPIO_G42,
	TRAV_GPIO_G43,
	TRAV_GPIO_G44,
	TRAV_GPIO_G45,
	TRAV_GPIO_G46,
	TRAV_GPIO_G47,
	TRAV_GPIO_G50,
	TRAV_GPIO_G51,
	TRAV_GPIO_G52,
	TRAV_GPIO_G53,
	TRAV_GPIO_G54,
	TRAV_GPIO_G55,
	TRAV_GPIO_G56,
	TRAV_GPIO_G57,
	TRAV_GPIO_G60,
	TRAV_GPIO_G61,
	TRAV_GPIO_G62,
	TRAV_GPIO_G63,
	TRAV_GPIO_G64,
	TRAV_GPIO_G65,
	TRAV_GPIO_G66,
	TRAV_GPIO_G67,
	TRAV_GPIO_G70,
	TRAV_GPIO_G71,
	TRAV_GPIO_G72,
	TRAV_GPIO_G73,
	TRAV_GPIO_G74,
	TRAV_GPIO_G75,
	TRAV_GPIO_G76,
	TRAV_GPIO_G77,

	/* GPIO_PART3_STARTS */
	TRAV_GPIO_MAX_PORT_PART_2,
	TRAV_GPIO_Q00 = TRAV_GPIO_MAX_PORT_PART_2,
	TRAV_GPIO_Q01,
	TRAV_GPIO_Q02,

	TRAV_GPIO_MAX_PORT
};

struct gpio_info {
	u64 reg_addr;	/* Address of register for this part */
	u32 max_gpio;	/* Maximum GPIO in this part */
};

#define TRAV_GPIO_NUM_PARTS 3
static struct gpio_info trav_gpio_data[TRAV_GPIO_NUM_PARTS] = {
	{ TRAV_GPIO_FSYS0, TRAV_GPIO_MAX_PORT_PART_1 },
	{ TRAV_GPIO_PERIC, TRAV_GPIO_MAX_PORT_PART_2 },
	{ TRAV_GPIO_PMU, TRAV_GPIO_MAX_PORT },
};

static inline struct gpio_info *get_gpio_data(void)
{
	return trav_gpio_data;
}

static inline unsigned int get_bank_num(void)
{
	return TRAV_GPIO_NUM_PARTS;
}

#define name_to_gpio(n) trav_name_to_gpio(n)
#define gpio_describe(str) trav_gpio_describe(str)

void gpio_cfg_pin(int gpio, int cfg);
void gpio_set_drv(int gpio, int drv);
int gpio_set_value(unsigned gpio, int value);
int gpio_get_value(unsigned gpio);
int gpio_request(unsigned gpio, const char *label);
int gpio_free(unsigned gpio);
int gpio_direction_input(unsigned gpio);
int gpio_direction_output(unsigned gpio, int value);

/* Pin configurations */
#define TRAV_GPIO_INPUT		0x0
#define TRAV_GPIO_OUTPUT		0x1
#define TRAV_GPIO_IRQ			0xf
#define TRAV_GPIO_FUNC(x)		(x)

/* Pull mode */
#define TRAV_GPIO_PULL_NONE		0x0
#define TRAV_GPIO_PULL_DOWN		0x1
#define TRAV_GPIO_PULL_UP		0x3

/* Drive Strength level */
#define TRAV_GPIO_DRV_1X		0x0
#define TRAV_GPIO_DRV_3X		0x1
#define TRAV_GPIO_DRV_2X		0x2
#define TRAV_GPIO_DRV_4X		0x3
#define TRAV_GPIO_DRV_FAST		0x0
#define TRAV_GPIO_DRV_SLOW		0x1

/**
 * Set GPIO pull mode.
 *
 * @param gpio	GPIO pin
 * @param mode	Either GPIO_PULL_DOWN or GPIO_PULL_UP
 */
void gpio_set_pull(int gpio, int mode);

#endif /* _CPU_TURBO_TRAV_GPIO_H_ */
