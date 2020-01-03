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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <assert.h>
#include <stdlib.h>
#include <timer.h>
#include <arch/io.h>
#include <console/console.h>
#include <soc/clock.h>
#include <soc/clock_init.h>
#include <soc/periph.h>

extern unsigned char uart_rx_byte_timeout(int idx);

/* Return I2C clock frequency */
uint32_t clock_get_i2c(void)
{
	return 0;

}

/* Return uart clock frequency */
uint32_t clock_get_uart(int dev_index)
{
	return 24000000;

}

/* Return ARM clock frequency */
uint32_t clock_get_arm(void)
{
	return 0;
}

unsigned int trav_bios_mem_clk_get(int twin)
{
	unsigned int mem_clk = MEMCLK;

#if IS_ENABLED(CONFIG_TURBO_TRAV_MEMCLK_INPUT)
	unsigned char char1 = 'z';

	printk(BIOS_INFO, "Memory clock setting selection:\n");
	printk(BIOS_INFO, "Enter - 0 for 2133MHz, 1 for 1866MHz, 2 for 1066MHz, 3 for 533MHz, (Default: %dMHz)\n",
			 mem_clk);
	printk(BIOS_INFO, "Waiting for 1.5 seconds..\n");
	char1 = uart_rx_byte_timeout(0);
	switch (char1) {
	case '0':
		mem_clk = 2133;
		break;
	case '1':
		mem_clk = 1866;
		break;
	case '2':
		mem_clk = 1066;
		break;
	case '3':
		mem_clk = 533;
		break;
	default:
		char1 = 'z';
	}
	if (char1 != 'z')
		printk(BIOS_INFO,"Selected %c\n", char1);

	printk(BIOS_INFO, "Memory clock is %dMHz\n", mem_clk);
#endif

#if CONFIG_TURBO_TRAV_MEMCLK > 0
	int conf_mem_clk = CONFIG_TURBO_TRAV_MEMCLK;

	if (twin)
		conf_mem_clk = CONFIG_TURBO_TRAV_TWIN_MEMCLK;

	if (conf_mem_clk == 2133 || conf_mem_clk == 1866 ||
	    conf_mem_clk == 1066 || conf_mem_clk == 533)
		mem_clk = conf_mem_clk;

	printk(BIOS_INFO, "Memory clock is %dMhz\n", mem_clk);
#endif

	return mem_clk;
}

unsigned int trav_bios_cpucl_clk_get(void)
{
	unsigned int cpucl_clk = CPUCL_CLK;

#if IS_ENABLED(CONFIG_TURBO_TRAV_CPUCL_CLK_INPUT)
	unsigned char char1 = 'a';

	printk(BIOS_INFO, "CPUCL clock setting selection:\n");
	printk(BIOS_INFO, "Enter - 0 for 2400MHz, 1 for 2200MHz, 2 for 2000MHz, 3 for 1600MHz, 4 for 1500MHz, 5 for 1000MHz, (Default: %dMHz)\n",
			cpucl_clk);
	printk(BIOS_INFO, "Waiting for 1.5 seconds...\n");
	char1 = uart_rx_byte_timeout(0);
	switch (char1) {
	case '0':
		cpucl_clk = 2400;
		break;
	case '1':
		cpucl_clk = 2200;
		break;
	case '2':
		cpucl_clk = 2000;
		break;
	case '3':
		cpucl_clk = 1600;
		break;
	case '4':
		cpucl_clk = 1500;
		break;
	case '5':
		cpucl_clk = 1000;
		break;
	default:
		char1 = 'a';
	}
	if (char1 != 'a')
		printk(BIOS_INFO,"Selected %c\n", char1);

	printk(BIOS_INFO, "CPUCL clock is %dMHz\n", cpucl_clk);
#endif

#if CONFIG_TURBO_TRAV_CPUCL_CLK > 0
	if (CONFIG_TURBO_TRAV_CPUCL_CLK == 2400 || CONFIG_TURBO_TRAV_CPUCL_CLK == 2200 ||
	    CONFIG_TURBO_TRAV_CPUCL_CLK == 2000 || CONFIG_TURBO_TRAV_CPUCL_CLK == 1600 ||
	    CONFIG_TURBO_TRAV_CPUCL_CLK == 1500 || CONFIG_TURBO_TRAV_CPUCL_CLK == 1000)
		cpucl_clk = CONFIG_TURBO_TRAV_CPUCL_CLK;

	printk(BIOS_INFO, "CPUCL clock is %dMhz\n", cpucl_clk);
#endif

	return cpucl_clk;
}

unsigned int trav_bios_gpu_clk_get(int twin)
{
        unsigned int gpu_clk = GPU_CLK;

#if IS_ENABLED(CONFIG_TURBO_TRAV_GPUCLK_INPUT)
        unsigned char char1 = 'a';

        printk(BIOS_INFO, "GPU clock setting selection:\n");
        printk(BIOS_INFO, "Enter - 0 for 1000MHz, 1 for 700MHz, 2 for 250MHz, (Default: %dMHz)\n",
                        gpu_clk);
        printk(BIOS_INFO, "Waiting for 1.5 seconds...\n");
        char1 = uart_rx_byte_timeout(0);
        switch (char1) {
        case '0':
                gpu_clk = 1000;
                break;
        case '1':
                gpu_clk = 700;
                break;
        case '2':
                gpu_clk = 250;
                break;
        default:
                char1 = 'a';
        }
        if (char1 != 'a')
                printk(BIOS_INFO,"Selected %c\n", char1);

        printk(BIOS_INFO, "GPU clock is %dMhz\n", gpu_clk);
#endif

#if (CONFIG_TURBO_TRAV_GPUCLK > 0)
	int conf_gpu_clk = CONFIG_TURBO_TRAV_GPUCLK;

	if (twin)
		conf_gpu_clk = CONFIG_TURBO_TRAV_TWIN_GPUCLK;

        if (conf_gpu_clk == 1000 || conf_gpu_clk == 700 || conf_gpu_clk == 250)
		gpu_clk = conf_gpu_clk;

        printk(BIOS_INFO, "GPU clock is %dMHz\n", gpu_clk);
#endif

        return gpu_clk;
}

unsigned int trav_bios_trip_clk_get(void)
{
	unsigned int trip_clk = TRIP_CLK;

#if IS_ENABLED(CONFIG_TURBO_TRAV_TRIPCLK_INPUT)
	unsigned char char1 = 'a';

	printk(BIOS_INFO, "TRIP clock setting selection:\n");
	printk(BIOS_INFO, "Enter - 0 for 2400MHz, 1 for 2000MHz, 2 for 1600MHz, 3 for 1200MHz, 4 for 800MHz, 5 for 400MHz, (Default: %dMHz)\n",
			trip_clk);
	printk(BIOS_INFO, "Waiting for 1.5 seconds...\n");
	char1 = uart_rx_byte_timeout(0);
	switch (char1) {
	case '0':
		trip_clk = 2400;
		break;
	case '1':
		trip_clk = 2000;
		break;
	case '2':
		trip_clk = 1600;
		break;
	case '3':
		trip_clk = 1200;
		break;
	case '4':
		trip_clk = 800;
		break;
	case '5':
		trip_clk = 400;
		break;
	default:
		char1 = 'a';
	}
	if (char1 != 'a')
		printk(BIOS_INFO,"Selected %c\n", char1);

	printk(BIOS_INFO, "TRIP clock is %dMhz\n", trip_clk);
#endif

#if (CONFIG_TURBO_TRAV_TRIPCLK > 0)
	if (CONFIG_TURBO_TRAV_TRIPCLK == 2400 || CONFIG_TURBO_TRAV_TRIPCLK == 2000 ||
	    CONFIG_TURBO_TRAV_TRIPCLK == 1600 || CONFIG_TURBO_TRAV_TRIPCLK == 1200 ||
	    CONFIG_TURBO_TRAV_TRIPCLK == 800  || CONFIG_TURBO_TRAV_TRIPCLK == 400)
		trip_clk = CONFIG_TURBO_TRAV_TRIPCLK;

	printk(BIOS_INFO, "TRIP clock is %dMHz\n", trip_clk);
#endif

	return trip_clk;
}
