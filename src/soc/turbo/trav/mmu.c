/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <arch/mmu.h>
#include <symbols.h>

#include <soc/mmu_operations.h>

void trav_mmu_init(void)
{
	mmu_init();

	/* IRAM 320KB */
	mmu_config_range((void *)0x0, 0x50000, CACHED_MEM);

	/* SFRs */
	mmu_config_range((void *)0x10000000, 0x10000000, DEV_MEM);

	/* LPDDR4 PHY control registers */
	mmu_config_range((void *)0x20000000, 0x10000000, DEV_MEM);

	/* DRAM area */
	mmu_config_range((void *)0x80000000, 0x80000000, CACHED_MEM);

	/* set ttb as secure */
	mmu_config_range(_ttb, _ttb_size, SECURE_MEM);
	mmu_enable();
}
