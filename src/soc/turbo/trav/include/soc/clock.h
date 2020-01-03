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

#ifndef _CPU_TURBO_TRAV_CLOCK_H_
#define _CPU_TURBO_TRAV_CLOCK_H_

#include <stdint.h>

#include <soc/cpu.h>

//#define MEMCLK	2133
//#define MEMCLK	1866
//#define MEMCLK	1066
#define MEMCLK		 533

//#define CPUCL_CLK	2400
//#define CPUCL_CLK	2200
//#define CPUCL_CLK	2000
//#define CPUCL_CLK	1500
#define CPUCL_CLK	1000

//#define GPU_CLK	1000
//#define GPU_CLK	 700
#define GPU_CLK		 250

//#define TRIP_CLK	2400
//#define TRIP_CLK	2000
//#define TRIP_CLK	1600
//#define TRIP_CLK	 800
#define TRIP_CLK	 400

uint32_t clock_get_arm(void);
uint32_t clock_get_i2c(void);
uint32_t clock_get_uart(int dev_index);
unsigned int trav_bios_mem_clk_get(int twin);
unsigned int trav_bios_cpucl_clk_get(void);
unsigned int trav_bios_gpu_clk_get(int twin);
unsigned int trav_bios_trip_clk_get(void);

#endif /* _CPU_TURBO_TRAV_CLOCK_H_ */
