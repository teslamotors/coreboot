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

#include <string.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c.h>
#include <cbmem.h>
#include <delay.h>
#include <vbe.h>
#include <boot/coreboot_tables.h>
#include <arch/transition.h>
#include <armv8/arch/cpu.h>
#include <stdlib.h>
#include <symbols.h>

#include <soc/common_definition.h>

#include <soc/scs_helpers.h>
#include <soc/sms_helpers.h>

static void mainboard_init(device_t dev)
{
	uintptr_t begin = (uintptr_t)0x80000000;
	size_t size = (uintptr_t)0x80000000;

	cbmem_initialize_empty();

	ram_resource(dev, 0, begin / KiB, size / KiB);
	printk(BIOS_INFO, "Mainboard Init \n");

#if 0
	scs_service_init();
	sms_service_init();

	slog_log_message("from coreboot ramstage", 22);
#endif

	return;
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name	= "Turbo Sentinel",
	.enable_dev = mainboard_enable,
};
