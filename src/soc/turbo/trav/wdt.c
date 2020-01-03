/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

/* ported from Linux Kernel drivers/watchdog/s3c2410_wdt.c */

#include <stdlib.h>
#include <string.h>
#include <arch/io.h>
#include <types.h>

#include <console/console.h>

#include <soc/wdt.h>
#include <soc/cpu.h>
#include <soc/common_definition.h>

#define S3C2410_WTCON           0x00
#define S3C2410_WTDAT           0x04
#define S3C2410_WTCNT           0x08
#define S3C2410_WTCLRINT        0x0c

#define S3C2410_WTCON_RSTEN     (1 << 0)
#define S3C2410_WTCON_INTEN     (1 << 2)
#define S3C2410_WTCON_ENABLE    (1 << 5)

#define S3C2410_WTCON_DIV16     (0 << 3)
#define S3C2410_WTCON_DIV32     (1 << 3)
#define S3C2410_WTCON_DIV64     (2 << 3)
#define S3C2410_WTCON_DIV128    (3 << 3)

#define S3C2410_WTCON_MAXDIV    0x80

#define S3C2410_WTCON_PRESCALE(x)       ((x) << 8)
#define S3C2410_WTCON_PRESCALE_MASK     (0xff << 8)
#define S3C2410_WTCON_PRESCALE_MAX      0xff

#define TRAV_WDT_DISABLE_REG_OFFSET          0x040c

#define TRAV_WDT_MASK_BIT       23
#define EXYNOS5_WDT_FREQ 24000000

#define WDT_NUM 3

#define DIV_ROUND_UP(x, y)  (((x) + (y) - 1) / (y))

static int s3c2410wdt_set_heartbeat(unsigned long reg_base, unsigned int timeout, uint32_t *p_count)
{
	unsigned long freq = EXYNOS5_WDT_FREQ;
	unsigned int count;
	unsigned int divisor = 1;
	unsigned long wtcon;

	if (timeout < 1)
		return -1;

	freq = DIV_ROUND_UP(freq, 128);
	count = timeout * freq;

	printk(BIOS_DEBUG, "Heartbeat: count=%d, timeout=%d, freq=%lu\n",
		count, timeout, freq);

	/* if the count is bigger than the watchdog register,
	   then work out what we need to do (and if) we can
	   actually make this value
	*/

	if (count >= 0x10000) {
		divisor = DIV_ROUND_UP(count, 0xffff);

		if (divisor > 0x100) {
			printk(BIOS_ERR, "timeout %d too big\n", timeout);
			return -1;
		}
	}

	printk(BIOS_DEBUG, "Heartbeat: timeout=%d, divisor=%d, count=%d (%08x)\n",
		timeout, divisor, count, DIV_ROUND_UP(count, divisor));

	count = DIV_ROUND_UP(count, divisor);
	*p_count = count;

	/* update the pre-scaler */
	wtcon = readl(reg_base + S3C2410_WTCON);
	wtcon &= ~S3C2410_WTCON_PRESCALE_MASK;
	wtcon |= S3C2410_WTCON_PRESCALE(divisor-1);

	writel(count, reg_base + S3C2410_WTDAT);
	writel(wtcon, reg_base + S3C2410_WTCON);

	return 0;
}

static void update_reg_bits(unsigned long base, unsigned long offset, uint32_t mask, uint32_t val)
{
	uint32_t v;

	v = readl(base + offset);
	v &= ~mask;
	v |= val & mask;
	writel(v, base + offset);
}

static void s3c2410wdt_mask_and_disable_reset(int i, int use_mask)
{
	uint32_t mask_val;
	uint32_t val;

	mask_val = 1 << (TRAV_WDT_MASK_BIT + i);
	val = use_mask ? mask_val : 0;
	update_reg_bits(TRAV_CENTRAL_PMU, TRAV_WDT_DISABLE_REG_OFFSET, mask_val, val);
}

static void s3c2410wdt_stop(unsigned long reg_base)
{
	uint32_t wtcon;

	wtcon = readl(reg_base + S3C2410_WTCON);
	wtcon &= ~(S3C2410_WTCON_ENABLE | S3C2410_WTCON_RSTEN);
	writel(wtcon, reg_base + S3C2410_WTCON);
}

static void s3c2410wdt_start(unsigned long reg_base, uint32_t count)
{
	uint32_t wtcon;

	s3c2410wdt_stop(reg_base);

	wtcon = readl(reg_base + S3C2410_WTCON);
	wtcon |= S3C2410_WTCON_ENABLE | S3C2410_WTCON_DIV128;

	wtcon |= S3C2410_WTCON_INTEN;
	wtcon |= S3C2410_WTCON_RSTEN;

	printk(BIOS_DEBUG, "Starting watchdog: count=0x%0x, wtcon=%08x\n",
		count, wtcon);

	writel(count, reg_base + S3C2410_WTDAT);
	writel(count, reg_base + S3C2410_WTCNT);
	writel(wtcon, reg_base + S3C2410_WTCON);
}

#define WDT0_TIMEOUT 15
#define WDT1_TIMEOUT 15
#define WDT2_TIMEOUT 15

void wdt_enable(void)
{
	unsigned long base_addr[WDT_NUM] = { TRAV_WDT0, TRAV_WDT1, TRAV_WDT2 };
	unsigned int timeout[WDT_NUM] = { WDT0_TIMEOUT, WDT1_TIMEOUT, WDT2_TIMEOUT };
	uint32_t count[WDT_NUM];
	int i;

	for (i = 0; i < WDT_NUM; i++) {
		s3c2410wdt_set_heartbeat(base_addr[i], timeout[i], &count[i]);

		s3c2410wdt_mask_and_disable_reset(i, 0);

		s3c2410wdt_start(base_addr[i], count[i]);
		printk(BIOS_INFO, "wdt%d enabled with %ds timeout\n", i, timeout[i]);
	}
}
