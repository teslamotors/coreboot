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

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <delay.h>
#include <console/console.h>
#include <device/device.h>
#include <cbmem.h>
#include <arch/cache.h>
#include <soc/cpu.h>

static struct device_operations cpu_ops = {
	.read_resources   = DEVICE_NOOP,
	.set_resources    = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init             = DEVICE_NOOP,
	.scan_bus         = DEVICE_NOOP,
};

static void enable_trav_dev(device_t dev)
{
	/* DUMMY: Add stuff when required */
	dev->ops = &cpu_ops;
}

struct chip_operations soc_turbo_trav_ops = {
	CHIP_NAME("Turbo TRAV")
	.enable_dev = enable_trav_dev,
};
