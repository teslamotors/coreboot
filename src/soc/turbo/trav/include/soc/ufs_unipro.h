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
#ifndef _UFSUNIPRO_H_
#define _UFSUNIPRO_H_

#define H_UIC_DME_GET	0x00000001
#define H_UIC_DME_SET	0x00000002
#define H_UIC_DME_LINKSTARTUP	0x00000016
#define H_UIC_DBG_AUTOMODE_THLD	0x9536

#define N_DeviceID	0x3000
#define N_DeviceID_valid	0x3001
#define T_ConnectionState	0x4020
#define T_PeerDeviceID	0x4021
#define T_CPortMode	0x402B
#define PA_Local_TxLccEnable	0x155E
#define PA_TxTermination	0x1569
#define PA_HSSeries	0x156A
#define PA_Hibern8Time	0x15A7
#define PA_TActivate	0x15A8
#define PA_PWRModeUserData0	0x15B0
#define PA_PWRModeUserData1	0x15B1
#define PA_PWRModeUserData2	0x15B2
#define PA_AvailTxDataLanes	0x1520
#define PA_AvailRxDataLanes	0x1540
#define PA_ActiveTxDataLanes	0x1560
#define PA_TxGear	0x1568
#define PA_TxTrailingClocks	0x1564
#define PA_PWRMode	0x1571
#define PA_ActiveRxDataLanes	0x1580
#define PA_RxGear	0x1583
#define PA_RxTermination	0x1584

#define DL_FC0ProtTimeOutVal	0x2041
#define DL_TC0ReplayTimeOutVal	0x2042
#define DL_AFC0ReqTimeOutVal	0x2043

typedef enum {
	LANE_0 = 0,
	LANE_1 = 1,
	UNIPRO_CMD = 0,
	MPHY_INT = 2,
	MPHY_CMN = 0,

	TX_LANE_0 = 0,
	TX_LANE_1 = 1,
	TX_LANE_2 = 2,
	TX_LANE_3 = 3,
	RX_LANE_0 = 4,
	RX_LANE_1 = 5,
	RX_LANE_2 = 6,
	RX_LANE_3 = 7,
} UFS_LANE;

#endif  /* _UFSUNIPRO_H_ */
