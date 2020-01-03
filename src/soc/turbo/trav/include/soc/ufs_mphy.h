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

#ifndef _UFSMPHY_H_
#define _UFSMPHY_H_

#define TX_PWMGEAR_CAPABILITY	0x04
#define TX_HIBERN8TIME_CAPABILITY	0x0F
#define TX_EQUALIZER_SETTING_CAPABILITY	0x12
#define TX_MODE	0x21
#define TX_HSRATE_SERIES	0x22
#define TX_ADVANCED_GRANULARITY_STEP	0x35
#define TX_FSM_STATE	0x41
#define RX_PWMGEAR_CAPABILITY	0x84
#define RX_HS_G1_PREPARE_LENGTH_CAPABILITY	0x8C
#define RX_MIN_ACTIVATETIME_CAPABILITY	0x8F

#define MPHY_PMA_CMN_ADDR(offset)	(offset)
#define MPHY_PMA_TRSV_ADDR(offset, lane)	((0x140)*lane+(offset))

#endif  /* _UFSMPHY_H_ */
