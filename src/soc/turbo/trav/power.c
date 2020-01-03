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

/* Power setup code for TRAV */

#include <arch/io.h>
#include <arch/hlt.h>
#include <console/console.h>
#include <soc/power.h>

static void ps_hold_setup(void)
{
	/* Set PS-Hold high */
	setbits_le32(&trav_power->ps_hold_control,
		     POWER_PS_HOLD_CONTROL_DATA_HIGH);
}

/* Enable HW thermal trip */
void power_enable_hw_thermal_trip(void)
{
	setbits_le32(&trav_power->ps_hold_control, POWER_ENABLE_HW_TRIP);
}

void power_exit_wakeup(void)
{
	uint64_t addr = (uint64_t)trav_power->inform0;
	typedef void (*resume_func)(void);

	trav_power->inform1 = 0;
	((resume_func)addr)();
}

void power_init(void)
{
	ps_hold_setup();
}

uint32_t power_read_reset_status(void)
{
	return trav_power->inform1;
}

void power_reset(void)
{
	/* Clear inform1 so there's no change we think we've got a wake reset */
	trav_power->inform1 = 0;

	setbits_le32(&trav_power->swreset, 1);
}

/* This function never returns */
void power_shutdown(void)
{
	clrbits_le32(&trav_power->ps_hold_control,
		     POWER_PS_HOLD_CONTROL_DATA_HIGH);

	hlt();
}
