/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Samsung Electronics
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

#include <memlayout.h>

#include <arch/header.ld>

SECTIONS
{
	SRAM_START(0x00000)
#if IS_ENABLED(CONFIG_BOOTBLOCK_X)
	/* 0x00000 ~ for -x size 0x8000 32K */
	BOOTBLOCK(CONFIG_BOOTBLOCK_X_START, CONFIG_BOOTBLOCK_X_SIZE)
#else
	/* 0x8000 ~ for -r -a or -b size 0x20000 128K */
	BOOTBLOCK(CONFIG_BOOTBLOCK_START, CONFIG_BOOTBLOCK_SIZE)
#endif
	/* 0x28000 for oops buf */
	BOOT_FILE_CRT_MEM(CONFIG_BOOT_FILE_CRT_MEM_BASE, CONFIG_BOOT_FILE_CRT_MEM_SIZE)
	TIMESTAMP(0x2b000, 4K)
	TTB(0x2c000, 12K)
	PSCI_SHARED_MEM(CONFIG_PSCI_SHARED_MEM_BASE, 4K)
	ATF_BL31(CONFIG_ATF_BL31_BASE, CONFIG_ATF_BL31_SIZE)
	/* 0x48000 oops buf end */
	BOOT_FILE_LAYOUT_MEM(CONFIG_BOOT_FILE_LAYOUT_MEM_BASE, 4K)
	BOOT_ENV_MEM(CONFIG_BOOT_ENV_MEM_BASE, 4K)
	STACK(0x4c000, 16K)
	SRAM_END(0x50000)
	DRAM_START(0x80000000)
	RAMSTAGE(0x80000000, 256K)
	CBFS_CACHE(0xf0000000, CONFIG_ROM_SIZE)
}
