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

#ifndef _CPU_TURBO_TRAV_DMC_H_
#define _CPU_TURBO_TRAV_DMC_H_

#include <stdint.h>
#include <soc/common_definition.h>
#include <soc/cpu.h>

#define LPDDR4PHY_OFFSET1		0x40000
#define LPDDR4PHY_OFFSET2		0x80000
#define LPDDR4PHY_OFFSET3		0x100000
#define LPDDR4PHY_OFFSET4		0x1c0044
#define LPDDR4PHY_OFFSET5		0x240000
#define LPDDR4PHY_OFFSET6		0x300010
#define LPDDR4PHY_OFFSET7		0x340000
#define LPDDR4PHY_OFFSET8		0x400200
#define LPDDR4PHY_OFFSET9		0x44000c
#define LPDDR4PHY_OFFSET10		0x480020
#define LPDDR4PHY_OFFSET11		0x500200
#define LPDDR4PHY_OFFSET12		0x640804
#define LPDDR4PHY_OFFSET13		0x800200
#define LPDDR4PHY_OFFSET14		0x84000c
#define LPDDR4PHY_OFFSET15		0x880020
#define LPDDR4PHY_OFFSET16		0x900200
#define LPDDR4PHY_OFFSET17		0xa40804
#define LPDDR4PHY_OFFSET18		0xc00200
#define LPDDR4PHY_OFFSET19		0xc4000c
#define LPDDR4PHY_OFFSET20		0xc80020
#define LPDDR4PHY_OFFSET21		0xd00200
#define LPDDR4PHY_OFFSET22		0xe40804
#define LPDDR4PHY_OFFSET23		0x140000	// IMEM (0x50000)
#define LPDDR4PHY_OFFSET24		0x150000	// DMEM (0x54000)
#define LPDDR4PHY_OFFSET25		0x240000	// PIE (0x90000)
#define LPDDR4PHY_OFFSET26		0x150800	// DMEM MSG (0x54200)

#define DRAM_VID_SAMSUNG		0x1
#define DRAM_VID_MICRON		0xFF
#define DRAM_VID_SKHYNIX		0x6

// RFSHTMG bitfield offset
#define T_RFC_NOM_X1_X32_OFFSET	16
#define T_RFC_MIN_OFFSET	0

// ZQCTL0 bitfield offset
#define T_ZQ_LONG_NOP_OFFSET	16
#define T_ZQ_SHORT_NOP_OFFSET	0

// ZQCTL1 bitfield offset
#define T_ZQ_RESET_NOP_OFFSET	20
#define T_ZQ_SHORT_INTERVAL_X1024_OFFSET 0

// INIT0 bitfield offset
#define SKIP_DRAM_INIT_OFFSET	30
#define POST_CKE_X1024_OFFSET	16
#define PRE_CKE_X1024_OFFSET	0

// INIT1 bitfield offset
#define DRAM_RSTN_X1024_OFFSET	16

// INIT3 bitfield offset
#define MR_OFFSET	16
#define EMR_OFFSET	0

// INIT4 bitfield offset
#define EMR2_OFFSET	16
#define EMR3_OFFSET	0

// INIT6 bitfield offset
#define MR4_OFFSET	16	
#define MR5_OFFSET	0

// INIT7 bitfield offset
#define MR22_OFFSET	16
#define MR6_OFFSET	0

// RANKCTL bitfield offset
#define DIFF_RANK_WR_GAP_OFFSET	8
#define DIFF_RANK_RD_GAP_OFFSET	4

// DMEM_OFFSET_0x0
#define MSGMISC_OFFSET	8

// DMEM_OFFSET_0x4
#define PMUREVISION_OFFSET	0

// DMEM_OFFSET_0x8
#define PSTATE_OFFSET	0
#define PLLBYPASSEN_OFFSET	8

// DMEM_OFFSET_0xC
#define DRAMFREQ_OFFSET	0

// DMEM_OFFSET_0x10
#define DFIFREQRATIO_OFFSET	0
#define BPZNRESVAL_OFFSET	8

// DMEM_OFFSET_0x14
#define PHYODTIMPEDANCE_OFFSET	0
#define PHYDRVIMPEDANCE_OFFSET	8

// DMEM_OFFSET_0x18
#define PHYVREF_OFFSET	0
#define LP4MISC_OFFSET	8 	// added

// DMEM_OFFSET_0x1C
#define CSTESTFAIL_OFFSET	8

// DMEM_OFFSET_0x20
#define SEQUENCECTRL_OFFSET	0

// DMEM_OFFSET_0x24
#define HDTCTRL_OFFSET	0

// DMEM_OFFSET_0x2C
#define DFIMRLMARGIN_OFFSET	0

// DMEM_OFFSET_0x30
#define USEBROADCASTMR_OFFSET	0
#define LP4QUICKBOOT_OFFSET	8	// added

// DMEM_OFFSET_0x34
#define CATRAINOPT_OFFSET	8

// DMEM_OFFSET_0x38
#define X8MODE_OFFSET	0
#define RX2D_TRAINOPT_OFFSET	8

// DMEM_OFFSET_0x3C
#define TX2D_TRAINOPT_OFFSET	0
#define SHARE2DVREFRESULT_OFFSET	8

// DMEM_OFFSET_0x40
#define DELAY_WEIGHT2D_OFFSET	0
#define VOLTAGE_WEIGHT2D_OFFSET	8

// DMEM_OFFSET_0x44
#define PHYCONFIGOVERRIDE_OFFSET	0	// added

// DMEM_OFFSET_0x48
#define ENABLEDDQSCHA_OFFSET	0
#define CSPRESENTCHA_OFFSET	8

// DMEM_OFFSET_0x4C
#define CDD_CHA_RR_1_0_OFFSET	0
#define CDD_CHA_RR_0_1_OFFSET	8

// DMEM_OFFSET_0x50
#define CDD_CHA_RW_1_1_OFFSET	0
#define CDD_CHA_RW_1_0_OFFSET	8

// DMEM_OFFSET_0x54
#define CDD_CHA_RW_0_1_OFFSET	0
#define CDD_CHA_RW_0_0_OFFSET	8

// DMEM_OFFSET_0x58
#define CDD_CHA_WR_1_1_OFFSET	0
#define CDD_CHA_WR_1_0_OFFSET	8

// DMEM_OFFSET_0x5C
#define CDD_CHA_WR_0_1_OFFSET	0
#define CDD_CHA_WR_0_0_OFFSET	8

// DMEM_OFFSET_0x60
#define CDD_CHA_WW_1_0_OFFSET	0
#define CDD_CHA_WW_0_1_OFFSET	8

// DMEM_OFFSET_0x64
#define MR1_A0_OFFSET	0
#define MR2_A0_OFFSET	8

// DMEM_OFFSET_0x68
#define MR3_A0_OFFSET	0
#define MR4_A0_OFFSET	8

// DMEM_OFFSET_0x6C
#define MR11_A0_OFFSET	0
#define MR12_A0_OFFSET	8

// DMEM_OFFSET_0x70
#define MR13_A0_OFFSET	0
#define MR14_A0_OFFSET	8

// DMEM_OFFSET_0x74
#define MR16_A0_OFFSET	0
#define MR17_A0_OFFSET	8

// DMEM_OFFSET_0x78
#define MR22_A0_OFFSET	0
#define MR24_A0_OFFSET	8

// DMEM_OFFSET_0x7C
#define MR1_A1_OFFSET	0
#define MR2_A1_OFFSET	8

// DMEM_OFFSET_0x80
#define MR3_A1_OFFSET	0
#define MR4_A1_OFFSET	8

// DMEM_OFFSET_0x84
#define MR11_A1_OFFSET	0
#define MR12_A1_OFFSET	8

// DMEM_OFFSET_0x88
#define MR13_A1_OFFSET	0
#define MR14_A1_OFFSET	8

// DMEM_OFFSET_0x8C
#define MR16_A1_OFFSET	0
#define MR17_A1_OFFSET	8

// DMEM_OFFSET_0x90
#define MR22_A1_OFFSET	0
#define MR24_A1_OFFSET	8

// DMEM_OFFSET_0x94
#define CATERMINATINGRANKCHA_OFFSET	0

// DMEM_OFFSET_0x98
#define TRAINEDVREFDQ_A0_2D_OFFSET	8

// DMEM_OFFSET_0x9C
#define TRAINEDVREFDQ_A1_2D_OFFSET	0
#define RXCLKDLY_MARGIN_A0_2D_OFFSET	8

// DMEM_OFFSET_0xA0
#define VREFDAC_MARGIN_A0_2D_OFFSET	0
#define TXDQDLY_MARGIN_A0_2D_OFFSET	8

// DMEM_OFFSET_0xA4
#define DEVICEVREF_MARGIN_A0_2D_OFFSET	0
#define RXCLKDLY_MARGIN_A1_2D_OFFSET	8

// DMEM_OFFSET_0xA8
#define VREFDAC_MARGIN_A1_2D_OFFSET	0
#define TXDQDLY_MARGIN_A1_2D_OFFSET	8

// DMEM_OFFSET_0xAC
#define DEVICEVREF_MARGIN_A1_2D_OFFSET	0

// DllGainCtl
#define DllGainTV_OFFSET		4


struct dmc_regs_t {
	uint32_t	UMCTL2_REGS_MSTR;
	uint32_t	UMCTL2_REGS_STAT;
	uint8_t		res0[0x8];
	uint32_t	UMCTL2_REGS_MRCTRL0;
	uint32_t	UMCTL2_REGS_MRCTRL1;
	uint32_t	UMCTL2_REGS_MRSTAT;
	uint8_t		res1[0x4];
	uint32_t	UMCTL2_REGS_DERATEEN;
	uint32_t	UMCTL2_REGS_DERATEINT;
	uint32_t	UMCTL2_REGS_MSTR2;
	uint8_t		res2[0x4];
	uint32_t	UMCTL2_REGS_PWRCTL;
	uint32_t	UMCTL2_REGS_PWRTMG;
	uint32_t	UMCTL2_REGS_HWLPCTL;
	uint8_t		res3[0x14];
	uint32_t	UMCTL2_REGS_RFSHCTL0;
	uint32_t	UMCTL2_REGS_RFSHCTL1;
	uint8_t		res4[0x8];
	uint32_t	UMCTL2_REGS_RFSHCTL3;
	uint32_t	UMCTL2_REGS_RFSHTMG;
	uint8_t		res5[0x8];
	uint32_t	UMCTL2_REGS_ECCCFG0;
	uint32_t	UMCTL2_REGS_ECCCFG1;
	uint32_t	UMCTL2_REGS_ECCSTAT;
	uint32_t	UMCTL2_REGS_ECCCTL;
	uint32_t	UMCTL2_REGS_ECCERRCNT;
	uint32_t	UMCTL2_REGS_ECCCADDR0;
	uint32_t	UMCTL2_REGS_ECCCADDR1;
	uint32_t	UMCTL2_REGS_ECCCSYN0;
	uint32_t	UMCTL2_REGS_ECCCSYN1;
	uint32_t	UMCTL2_REGS_ECCCSYN2;
	uint32_t	UMCTL2_REGS_ECCBITMASK0;
	uint32_t	UMCTL2_REGS_ECCBITMASK1;
	uint32_t	UMCTL2_REGS_ECCBITMASK2;
	uint32_t	UMCTL2_REGS_ECCUADDR0;
	uint32_t	UMCTL2_REGS_ECCUADDR1;
	uint32_t	UMCTL2_REGS_ECCUSYN0;
	uint32_t	UMCTL2_REGS_ECCUSYN1;
	uint32_t	UMCTL2_REGS_ECCUSYN2;
	uint32_t	UMCTL2_REGS_ECCPOISONADDR0;
	uint32_t	UMCTL2_REGS_ECCPOISONADDR1;
	uint32_t	UMCTL2_REGS_CRCPARCTL0;
	uint8_t		res6[0x8];
	uint32_t	UMCTL2_REGS_CRCPARSTAT;
	uint32_t	UMCTL2_REGS_INIT0;
	uint32_t	UMCTL2_REGS_INIT1;
	uint32_t	UMCTL2_REGS_INIT2;
	uint32_t	UMCTL2_REGS_INIT3;
	uint32_t	UMCTL2_REGS_INIT4;
	uint32_t	UMCTL2_REGS_INIT5;
	uint32_t	UMCTL2_REGS_INIT6;
	uint32_t	UMCTL2_REGS_INIT7;
	uint32_t	UMCTL2_REGS_DIMMCTL;
	uint32_t	UMCTL2_REGS_RANKCTL;
	uint8_t		res7[0x8];
	uint32_t	UMCTL2_REGS_DRAMTMG0;
	uint32_t	UMCTL2_REGS_DRAMTMG1;
	uint32_t	UMCTL2_REGS_DRAMTMG2;
	uint32_t	UMCTL2_REGS_DRAMTMG3;
	uint32_t	UMCTL2_REGS_DRAMTMG4;
	uint32_t	UMCTL2_REGS_DRAMTMG5;
	uint32_t	UMCTL2_REGS_DRAMTMG6;
	uint32_t	UMCTL2_REGS_DRAMTMG7;
	uint32_t	UMCTL2_REGS_DRAMTMG8;
	uint8_t		res8[0xc];
	uint32_t	UMCTL2_REGS_DRAMTMG12;
	uint32_t	UMCTL2_REGS_DRAMTMG13;
	uint32_t	UMCTL2_REGS_DRAMTMG14;
	uint8_t		res9[0x44];
	uint32_t	UMCTL2_REGS_ZQCTL0;
	uint32_t	UMCTL2_REGS_ZQCTL1;
	uint32_t	UMCTL2_REGS_ZQCTL2;
	uint32_t	UMCTL2_REGS_ZQSTAT;
	uint32_t	UMCTL2_REGS_DFITMG0;
	uint32_t	UMCTL2_REGS_DFITMG1;
	uint32_t	UMCTL2_REGS_DFILPCFG0;
	uint8_t		res10[0x4];
	uint32_t	UMCTL2_REGS_DFIUPD0;
	uint32_t	UMCTL2_REGS_DFIUPD1;
	uint32_t	UMCTL2_REGS_DFIUPD2;
	uint8_t		res11[0x4];
	uint32_t	UMCTL2_REGS_DFIMISC;
	uint32_t	UMCTL2_REGS_DFITMG2;
	uint8_t		res12[0x4];
	uint32_t	UMCTL2_REGS_DFISTAT;
	uint32_t	UMCTL2_REGS_DBICTL;
	uint32_t	UMCTL2_REGS_DFIPHYMSTR;
	uint8_t		res13[0x38];
	uint32_t	UMCTL2_REGS_ADDRMAP0;
	uint32_t	UMCTL2_REGS_ADDRMAP1;
	uint32_t	UMCTL2_REGS_ADDRMAP2;
	uint32_t	UMCTL2_REGS_ADDRMAP3;
	uint32_t	UMCTL2_REGS_ADDRMAP4;
	uint32_t	UMCTL2_REGS_ADDRMAP5;
	uint32_t	UMCTL2_REGS_ADDRMAP6;
	uint8_t		res14[0x8];
	uint32_t	UMCTL2_REGS_ADDRMAP9;
	uint32_t	UMCTL2_REGS_ADDRMAP10;
	uint32_t	UMCTL2_REGS_ADDRMAP11;
	uint8_t		res15[0x10];
	uint32_t	UMCTL2_REGS_ODTCFG;
	uint32_t	UMCTL2_REGS_ODTMAP;
	uint8_t		res16[0x8];
	uint32_t	UMCTL2_REGS_SCHED;
	uint32_t	UMCTL2_REGS_SCHED1;
	uint8_t		res17[0x4];
	uint32_t	UMCTL2_REGS_PERFHPR1;
	uint8_t		res18[0x4];
	uint32_t	UMCTL2_REGS_PERFLPR1;
	uint8_t		res19[0x4];
	uint32_t	UMCTL2_REGS_PERFWR1;
	uint8_t		res20[0x90];
	uint32_t	UMCTL2_REGS_DBG0;
	uint32_t	UMCTL2_REGS_DBG1;
	uint32_t	UMCTL2_REGS_DBGCAM;
	uint32_t	UMCTL2_REGS_DBGCMD;
	uint32_t	UMCTL2_REGS_DBGSTAT;
	uint8_t		res21[0xc];
	uint32_t	UMCTL2_REGS_SWCTL;
	uint32_t	UMCTL2_REGS_SWSTAT;
	uint8_t		res22[0x44];
	uint32_t	UMCTL2_REGS_POISONCFG;
	uint32_t	UMCTL2_REGS_POISONSTAT;
	uint32_t	UMCTL2_REGS_ADVECCINDEX;
	uint8_t		res23[0x4];
	uint32_t	UMCTL2_REGS_ECCPOISONPAT0;
	uint8_t		res24[0x4];
	uint32_t	UMCTL2_REGS_ECCPOISONPAT2;
	uint32_t	UMCTL2_REGS_ECCAPSTAT;
	uint8_t		res25[0x70];
	uint32_t	UMCTL2_MP_PSTAT;
	uint32_t	UMCTL2_MP_PCCFG;
	uint32_t	UMCTL2_MP_PCFGR_0;
	uint32_t	UMCTL2_MP_PCFGW_0;
	uint8_t		res26[0x84];
	uint32_t	UMCTL2_MP_PCTRL_0;
	uint32_t	UMCTL2_MP_PCFGQOS0_0;
	uint32_t	UMCTL2_MP_PCFGQOS1_0;
	uint32_t	UMCTL2_MP_PCFGWQOS0_0;
	uint32_t	UMCTL2_MP_PCFGWQOS1_0;
	uint8_t		res27[0xa60];
	uint32_t	UMCTL2_MP_SARBASE0;
	uint32_t	UMCTL2_MP_SARSIZE0;
	uint32_t	UMCTL2_MP_SARBASE1;
	uint32_t	UMCTL2_MP_SARSIZE1;
	uint32_t	UMCTL2_MP_SARBASE2;
	uint32_t	UMCTL2_MP_SARSIZE2;
	uint32_t	UMCTL2_MP_SARBASE3;
	uint32_t	UMCTL2_MP_SARSIZE3;
	uint32_t	UMCTL2_MP_SBRCTL;
	uint32_t	UMCTL2_MP_SBRSTAT;
	uint32_t	UMCTL2_MP_SBRWDATA0;
	uint8_t		res28[0x10f0];
	uint32_t	UMCTL2_REGS_FREQ1_DERATEEN;
	uint32_t	UMCTL2_REGS_FREQ1_DERATEINT;
	uint8_t		res29[0xc];
	uint32_t	UMCTL2_REGS_FREQ1_PWRTMG;
	uint8_t		res30[0x18];
	uint32_t	UMCTL2_REGS_FREQ1_RFSHCTL0;
	uint8_t		res31[0x10];
	uint32_t	UMCTL2_REGS_FREQ1_RFSHTMG;
	uint8_t		res32[0x74];
	uint32_t	UMCTL2_REGS_FREQ1_INIT3;
	uint32_t	UMCTL2_REGS_FREQ1_INIT4;
	uint8_t		res33[0x4];
	uint32_t	UMCTL2_REGS_FREQ1_INIT6;
	uint32_t	UMCTL2_REGS_FREQ1_INIT7;
	uint8_t		res34[0x4];
	uint32_t	UMCTL2_REGS_FREQ1_RANKCTL;
	uint8_t		res35[0x8];
	uint32_t	UMCTL2_REGS_FREQ1_DRAMTMG0;
	uint32_t	UMCTL2_REGS_FREQ1_DRAMTMG1;
	uint32_t	UMCTL2_REGS_FREQ1_DRAMTMG2;
	uint32_t	UMCTL2_REGS_FREQ1_DRAMTMG3;
	uint32_t	UMCTL2_REGS_FREQ1_DRAMTMG4;
	uint32_t	UMCTL2_REGS_FREQ1_DRAMTMG5;
	uint32_t	UMCTL2_REGS_FREQ1_DRAMTMG6;
	uint32_t	UMCTL2_REGS_FREQ1_DRAMTMG7;
	uint32_t	UMCTL2_REGS_FREQ1_DRAMTMG8;
	uint8_t		res36[0xc];
	uint32_t	UMCTL2_REGS_FREQ1_DRAMTMG12;
	uint32_t	UMCTL2_REGS_FREQ1_DRAMTMG13;
	uint32_t	UMCTL2_REGS_FREQ1_DRAMTMG14;
	uint8_t		res37[0x44];
	uint32_t	UMCTL2_REGS_FREQ1_ZQCTL0;
	uint8_t		res38[0xc];
	uint32_t	UMCTL2_REGS_FREQ1_DFITMG0;
	uint32_t	UMCTL2_REGS_FREQ1_DFITMG1;
	uint8_t		res39[0x1c];
	uint32_t	UMCTL2_REGS_FREQ1_DFITMG2;
	uint8_t		res40[0x88];
	uint32_t	UMCTL2_REGS_FREQ1_ODTCFG;
	uint8_t		res41[0xddc];
	uint32_t	UMCTL2_REGS_FREQ2_DERATEEN;
	uint32_t	UMCTL2_REGS_FREQ2_DERATEINT;
	uint8_t		res42[0xc];
	uint32_t	UMCTL2_REGS_FREQ2_PWRTMG;
	uint8_t		res43[0x18];
	uint32_t	UMCTL2_REGS_FREQ2_RFSHCTL0;
	uint8_t		res44[0x10];
	uint32_t	UMCTL2_REGS_FREQ2_RFSHTMG;
	uint8_t		res45[0x74];
	uint32_t	UMCTL2_REGS_FREQ2_INIT3;
	uint32_t	UMCTL2_REGS_FREQ2_INIT4;
	uint8_t		res46[0x4];
	uint32_t	UMCTL2_REGS_FREQ2_INIT6;
	uint32_t	UMCTL2_REGS_FREQ2_INIT7;
	uint8_t		res47[0x4];
	uint32_t	UMCTL2_REGS_FREQ2_RANKCTL;
	uint8_t		res48[0x8];
	uint32_t	UMCTL2_REGS_FREQ2_DRAMTMG0;
	uint32_t	UMCTL2_REGS_FREQ2_DRAMTMG1;
	uint32_t	UMCTL2_REGS_FREQ2_DRAMTMG2;
	uint32_t	UMCTL2_REGS_FREQ2_DRAMTMG3;
	uint32_t	UMCTL2_REGS_FREQ2_DRAMTMG4;
	uint32_t	UMCTL2_REGS_FREQ2_DRAMTMG5;
	uint32_t	UMCTL2_REGS_FREQ2_DRAMTMG6;
	uint32_t	UMCTL2_REGS_FREQ2_DRAMTMG7;
	uint32_t	UMCTL2_REGS_FREQ2_DRAMTMG8;
	uint8_t		res49[0xc];
	uint32_t	UMCTL2_REGS_FREQ2_DRAMTMG12;
	uint32_t	UMCTL2_REGS_FREQ2_DRAMTMG13;
	uint32_t	UMCTL2_REGS_FREQ2_DRAMTMG14;
	uint8_t		res50[0x44];
	uint32_t	UMCTL2_REGS_FREQ2_ZQCTL0;
	uint8_t		res51[0xc];
	uint32_t	UMCTL2_REGS_FREQ2_DFITMG0;
	uint32_t	UMCTL2_REGS_FREQ2_DFITMG1;
	uint8_t		res52[0x1c];
	uint32_t	UMCTL2_REGS_FREQ2_DFITMG2;
	uint8_t		res53[0x88];
	uint32_t	UMCTL2_REGS_FREQ2_ODTCFG;
	uint8_t		res54[0xddc];
	uint32_t	UMCTL2_REGS_FREQ3_DERATEEN;
	uint32_t	UMCTL2_REGS_FREQ3_DERATEINT;
	uint8_t		res55[0xc];
	uint32_t	UMCTL2_REGS_FREQ3_PWRTMG;
	uint8_t		res56[0x18];
	uint32_t	UMCTL2_REGS_FREQ3_RFSHCTL0;
	uint8_t		res57[0x10];
	uint32_t	UMCTL2_REGS_FREQ3_RFSHTMG;
	uint8_t		res58[0x74];
	uint32_t	UMCTL2_REGS_FREQ3_INIT3;
	uint32_t	UMCTL2_REGS_FREQ3_INIT4;
	uint8_t		res59[0x4];
	uint32_t	UMCTL2_REGS_FREQ3_INIT6;
	uint32_t	UMCTL2_REGS_FREQ3_INIT7;
	uint8_t		res60[0x4];
	uint32_t	UMCTL2_REGS_FREQ3_RANKCTL;
	uint8_t		res61[0x8];
	uint32_t	UMCTL2_REGS_FREQ3_DRAMTMG0;
	uint32_t	UMCTL2_REGS_FREQ3_DRAMTMG1;
	uint32_t	UMCTL2_REGS_FREQ3_DRAMTMG2;
	uint32_t	UMCTL2_REGS_FREQ3_DRAMTMG3;
	uint32_t	UMCTL2_REGS_FREQ3_DRAMTMG4;
	uint32_t	UMCTL2_REGS_FREQ3_DRAMTMG5;
	uint32_t	UMCTL2_REGS_FREQ3_DRAMTMG6;
	uint32_t	UMCTL2_REGS_FREQ3_DRAMTMG7;
	uint32_t	UMCTL2_REGS_FREQ3_DRAMTMG8;
	uint8_t		res62[0xc];
	uint32_t	UMCTL2_REGS_FREQ3_DRAMTMG12;
	uint32_t	UMCTL2_REGS_FREQ3_DRAMTMG13;
	uint32_t	UMCTL2_REGS_FREQ3_DRAMTMG14;
	uint8_t		res63[0x44];
	uint32_t	UMCTL2_REGS_FREQ3_ZQCTL0;
	uint8_t		res64[0xc];
	uint32_t	UMCTL2_REGS_FREQ3_DFITMG0;
	uint32_t	UMCTL2_REGS_FREQ3_DFITMG1;
	uint8_t		res65[0x1c];
	uint32_t	UMCTL2_REGS_FREQ3_DFITMG2;
	uint8_t		res66[0x88];
	uint32_t	UMCTL2_REGS_FREQ3_ODTCFG;
};

struct lpddr4phy_regs_t0 {
	uint8_t		res[0x68];
	uint32_t	DWC_DDRPHYA_ANIB0_MtestMuxSel;
	uint8_t		res0[0x34];
	uint32_t	DWC_DDRPHYA_ANIB0_AForceTriCont;
	uint8_t		res1[0x68];
	uint32_t	DWC_DDRPHYA_ANIB0_ATxImpedance;
	uint8_t		res2[0x3c];
	uint32_t	DWC_DDRPHYA_ANIB0_ATestPrbsErr;
	uint8_t		res3[0x4];
	uint32_t	DWC_DDRPHYA_ANIB0_ATxSlewRate;
	uint32_t	DWC_DDRPHYA_ANIB0_ATestPrbsErrCnt;
	uint8_t		res4[0xa4];
	uint32_t	DWC_DDRPHYA_ANIB0_ATxDly_p0;
	uint8_t		res5[0x3e64];
	uint32_t	DWC_DDRPHYA_ANIB1_MtestMuxSel;
	uint8_t		res6[0x34];
	uint32_t	DWC_DDRPHYA_ANIB1_AForceTriCont;
	uint8_t		res7[0x68];
	uint32_t	DWC_DDRPHYA_ANIB1_ATxImpedance;
	uint8_t		res8[0x3c];
	uint32_t	DWC_DDRPHYA_ANIB1_ATestPrbsErr;
	uint8_t		res9[0x4];
	uint32_t	DWC_DDRPHYA_ANIB1_ATxSlewRate;
	uint32_t	DWC_DDRPHYA_ANIB1_ATestPrbsErrCnt;
	uint8_t		res10[0xa4];
	uint32_t	DWC_DDRPHYA_ANIB1_ATxDly_p0;
	uint8_t		res11[0x3e64];
	uint32_t	DWC_DDRPHYA_ANIB2_MtestMuxSel;
	uint8_t		res12[0x34];
	uint32_t	DWC_DDRPHYA_ANIB2_AForceTriCont;
	uint8_t		res13[0x68];
	uint32_t	DWC_DDRPHYA_ANIB2_ATxImpedance;
	uint8_t		res14[0x3c];
	uint32_t	DWC_DDRPHYA_ANIB2_ATestPrbsErr;
	uint8_t		res15[0x4];
	uint32_t	DWC_DDRPHYA_ANIB2_ATxSlewRate;
	uint32_t	DWC_DDRPHYA_ANIB2_ATestPrbsErrCnt;
	uint8_t		res16[0xa4];
	uint32_t	DWC_DDRPHYA_ANIB2_ATxDly_p0;
};

struct lpddr4phy_regs_t1 {
	uint32_t	DWC_DDRPHYA_DBYTE0_DbyteMiscMode;
	uint32_t	DWC_DDRPHYA_DBYTE0_TsmByte0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingParam;
	uint32_t	DWC_DDRPHYA_DBYTE0_UseDqsEnReplica_p0;
	uint8_t		res0[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxTrainPatternEnable;
	uint32_t	DWC_DDRPHYA_DBYTE0_TsmByte1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TsmByte2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TsmByte3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TsmByte4;
	uint8_t		res1[0x8];
	uint32_t	DWC_DDRPHYA_DBYTE0_TestModeConfig;
	uint32_t	DWC_DDRPHYA_DBYTE0_TsmByte5;
	uint8_t		res2[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_MtestMuxSel;
	uint8_t		res3[0x10];
	uint32_t	DWC_DDRPHYA_DBYTE0_DtsmTrainModeCtrl;
	uint32_t	DWC_DDRPHYA_DBYTE0_DFIMRL_p0;
	uint8_t		res4[0xc];
	uint32_t	DWC_DDRPHYA_DBYTE0_AsyncDbyteMode;
	uint8_t		res5[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_AsyncDbyteTxEn;
	uint8_t		res6[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_AsyncDbyteTxData;
	uint8_t		res7[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_AsyncDbyteRxData;
	uint8_t		res8[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC1_r0;
	uint8_t		res9[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingCntr_r0;
	uint8_t		res10[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC0_r0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl0_b0_p0;
	uint8_t		res11[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b0_p0;
	uint8_t		res12[0x10];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxEqualizationMode_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl2_b0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl2_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b0_p0;
	uint8_t		res13[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxFifoCheckStatus;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxFifoCheckErrValues;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxFifoInfo;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxFifoVisibility;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxFifoContentsDQ3210;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxFifoContentsDQ7654;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxFifoContentsDBI;
	uint8_t		res14[0x8];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxSlewRate_b0_p0;
	uint8_t		res15[0x8];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingIncDecDtsmEn_r0;
	uint8_t		res16[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg0_r0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg1_r0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg2_r0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg3_r0;
	uint8_t		res17[0x50];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg0_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg1_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg2_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg3_u0_p0;
	uint8_t		res18[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg0_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg1_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg2_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg3_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg0_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg1_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg2_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg3_u0_p0;
	uint8_t		res19[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE0_Dq0LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE0_Dq1LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE0_Dq2LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE0_Dq3LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE0_Dq4LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE0_Dq5LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE0_Dq6LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE0_Dq7LnSel;
	uint8_t		res20[0x8];
	uint32_t	DWC_DDRPHYA_DBYTE0_PptCtlStatic;
	uint32_t	DWC_DDRPHYA_DBYTE0_PptCtlDyn;
	uint32_t	DWC_DDRPHYA_DBYTE0_PptInfo;
	uint32_t	DWC_DDRPHYA_DBYTE0_PptRxEnEvnt;
	uint32_t	DWC_DDRPHYA_DBYTE0_PptDqsCntInvTrnTg0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_PptDqsCntInvTrnTg1_p0;
	uint8_t		res21[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_DtsmBlankingCtrl;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm0_i0;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm1_i0;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm2_i0;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxChkDataSelects;
	uint32_t	DWC_DDRPHYA_DBYTE0_DtsmUpThldXingInd;
	uint32_t	DWC_DDRPHYA_DBYTE0_DtsmLoThldXingInd;
	uint32_t	DWC_DDRPHYA_DBYTE0_DbyteAllDtsmCtrl0;
	uint32_t	DWC_DDRPHYA_DBYTE0_DbyteAllDtsmCtrl1;
	uint32_t	DWC_DDRPHYA_DBYTE0_DbyteAllDtsmCtrl2;
	uint8_t		res22[0x10];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r0_p0;
	uint8_t		res23[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg0_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg1_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg2_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg3_u0_p0;
	uint8_t		res24[0x40];
	uint32_t	DWC_DDRPHYA_DBYTE0_DxLcdlStatus;
	uint8_t		res25[0x12c];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC1_r1;
	uint8_t		res26[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingCntr_r1;
	uint8_t		res27[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC0_r1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl0_b1_p0;
	uint8_t		res28[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b1_p0;
	uint8_t		res29[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b1_p0;
	uint8_t		res30[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl2_b1_p0;
	uint8_t		res31[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b1_p0;
	uint8_t		res32[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxSlewRate_b1_p0;
	uint8_t		res33[0x8];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingIncDecDtsmEn_r1;
	uint8_t		res34[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg0_r1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg1_r1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg2_r1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg3_r1;
	uint8_t		res35[0x50];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg0_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg1_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg2_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg3_u1_p0;
	uint8_t		res36[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg0_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg1_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg2_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg3_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg0_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg1_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg2_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg3_u1_p0;
	uint8_t		res37[0x78];
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm0_i1;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm1_i1;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm2_i1;
	uint8_t		res38[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r1_p0;
	uint8_t		res39[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg0_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg1_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg2_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg3_u1_p0;
	uint8_t		res40[0x170];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC1_r2;
	uint8_t		res41[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingCntr_r2;
	uint8_t		res42[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC0_r2;
	uint8_t		res43[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingIncDecDtsmEn_r2;
	uint8_t		res44[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg0_r2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg1_r2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg2_r2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg3_r2;
	uint8_t		res45[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm0_i2;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm1_i2;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm2_i2;
	uint8_t		res46[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r2_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r2_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r2_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r2_p0;
	uint8_t		res47[0x1b0];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC1_r3;
	uint8_t		res48[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingCntr_r3;
	uint8_t		res49[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC0_r3;
	uint8_t		res50[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingIncDecDtsmEn_r3;
	uint8_t		res51[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg0_r3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg1_r3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg2_r3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg3_r3;
	uint8_t		res52[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm0_i3;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm1_i3;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm2_i3;
	uint8_t		res53[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r3_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r3_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r3_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r3_p0;
	uint8_t		res54[0x1b0];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC1_r4;
	uint8_t		res55[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingCntr_r4;
	uint8_t		res56[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC0_r4;
	uint8_t		res57[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingIncDecDtsmEn_r4;
	uint8_t		res58[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg0_r4;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg1_r4;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg2_r4;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg3_r4;
	uint8_t		res59[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm0_i4;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm1_i4;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm2_i4;
	uint8_t		res60[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r4_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r4_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r4_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r4_p0;
	uint8_t		res61[0x1b0];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC1_r5;
	uint8_t		res62[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingCntr_r5;
	uint8_t		res63[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC0_r5;
	uint8_t		res64[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingIncDecDtsmEn_r5;
	uint8_t		res65[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg0_r5;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg1_r5;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg2_r5;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg3_r5;
	uint8_t		res66[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm0_i5;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm1_i5;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm2_i5;
	uint8_t		res67[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r5_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r5_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r5_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r5_p0;
	uint8_t		res68[0x1b0];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC1_r6;
	uint8_t		res69[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingCntr_r6;
	uint8_t		res70[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC0_r6;
	uint8_t		res71[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingIncDecDtsmEn_r6;
	uint8_t		res72[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg0_r6;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg1_r6;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg2_r6;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg3_r6;
	uint8_t		res73[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm0_i6;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm1_i6;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm2_i6;
	uint8_t		res74[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r6_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r6_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r6_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r6_p0;
	uint8_t		res75[0x1b0];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC1_r7;
	uint8_t		res76[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingCntr_r7;
	uint8_t		res77[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC0_r7;
	uint8_t		res78[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingIncDecDtsmEn_r7;
	uint8_t		res79[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg0_r7;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg1_r7;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg2_r7;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg3_r7;
	uint8_t		res80[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm0_i7;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm1_i7;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm2_i7;
	uint8_t		res81[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r7_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r7_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r7_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r7_p0;
	uint8_t		res82[0x1b0];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC1_r8;
	uint8_t		res83[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingCntr_r8;
	uint8_t		res84[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE0_VrefDAC0_r8;
	uint8_t		res85[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE0_TrainingIncDecDtsmEn_r8;
	uint8_t		res86[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg0_r8;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg1_r8;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg2_r8;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxPBDlyTg3_r8;
	uint8_t		res87[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm0_i8;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm1_i8;
	uint32_t	DWC_DDRPHYA_DBYTE0_Tsm2_i8;
	uint8_t		res88[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r8_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r8_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r8_p0;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r8_p0;
	uint8_t		res89[0x1cf0];
	uint32_t	DWC_DDRPHYA_DBYTE1_DbyteMiscMode;
	uint32_t	DWC_DDRPHYA_DBYTE1_TsmByte0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingParam;
	uint32_t	DWC_DDRPHYA_DBYTE1_UseDqsEnReplica_p0;
	uint8_t		res90[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxTrainPatternEnable;
	uint32_t	DWC_DDRPHYA_DBYTE1_TsmByte1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TsmByte2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TsmByte3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TsmByte4;
	uint8_t		res91[0x8];
	uint32_t	DWC_DDRPHYA_DBYTE1_TestModeConfig;
	uint32_t	DWC_DDRPHYA_DBYTE1_TsmByte5;
	uint8_t		res92[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_MtestMuxSel;
	uint8_t		res93[0x10];
	uint32_t	DWC_DDRPHYA_DBYTE1_DtsmTrainModeCtrl;
	uint32_t	DWC_DDRPHYA_DBYTE1_DFIMRL_p0;
	uint8_t		res94[0xc];
	uint32_t	DWC_DDRPHYA_DBYTE1_AsyncDbyteMode;
	uint8_t		res95[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_AsyncDbyteTxEn;
	uint8_t		res96[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_AsyncDbyteTxData;
	uint8_t		res97[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_AsyncDbyteRxData;
	uint8_t		res98[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC1_r0;
	uint8_t		res99[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingCntr_r0;
	uint8_t		res100[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC0_r0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl0_b0_p0;
	uint8_t		res101[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b0_p0;
	uint8_t		res102[0x10];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxEqualizationMode_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl2_b0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl2_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b0_p0;
	uint8_t		res103[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxFifoCheckStatus;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxFifoCheckErrValues;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxFifoInfo;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxFifoVisibility;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxFifoContentsDQ3210;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxFifoContentsDQ7654;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxFifoContentsDBI;
	uint8_t		res104[0x8];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxSlewRate_b0_p0;
	uint8_t		res105[0x8];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingIncDecDtsmEn_r0;
	uint8_t		res106[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg0_r0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg1_r0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg2_r0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg3_r0;
	uint8_t		res107[0x50];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg0_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg1_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg2_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg3_u0_p0;
	uint8_t		res108[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg0_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg1_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg2_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg3_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg0_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg1_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg2_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg3_u0_p0;
	uint8_t		res109[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE1_Dq0LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE1_Dq1LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE1_Dq2LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE1_Dq3LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE1_Dq4LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE1_Dq5LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE1_Dq6LnSel;
	uint32_t	DWC_DDRPHYA_DBYTE1_Dq7LnSel;
	uint8_t		res110[0x8];
	uint32_t	DWC_DDRPHYA_DBYTE1_PptCtlStatic;
	uint32_t	DWC_DDRPHYA_DBYTE1_PptCtlDyn;
	uint32_t	DWC_DDRPHYA_DBYTE1_PptInfo;
	uint32_t	DWC_DDRPHYA_DBYTE1_PptRxEnEvnt;
	uint32_t	DWC_DDRPHYA_DBYTE1_PptDqsCntInvTrnTg0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_PptDqsCntInvTrnTg1_p0;
	uint8_t		res111[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_DtsmBlankingCtrl;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm0_i0;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm1_i0;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm2_i0;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxChkDataSelects;
	uint32_t	DWC_DDRPHYA_DBYTE1_DtsmUpThldXingInd;
	uint32_t	DWC_DDRPHYA_DBYTE1_DtsmLoThldXingInd;
	uint32_t	DWC_DDRPHYA_DBYTE1_DbyteAllDtsmCtrl0;
	uint32_t	DWC_DDRPHYA_DBYTE1_DbyteAllDtsmCtrl1;
	uint32_t	DWC_DDRPHYA_DBYTE1_DbyteAllDtsmCtrl2;
	uint8_t		res112[0x10];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r0_p0;
	uint8_t		res113[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg0_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg1_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg2_u0_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg3_u0_p0;
	uint8_t		res114[0x40];
	uint32_t	DWC_DDRPHYA_DBYTE1_DxLcdlStatus;
	uint8_t		res115[0x12c];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC1_r1;
	uint8_t		res116[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingCntr_r1;
	uint8_t		res117[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC0_r1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl0_b1_p0;
	uint8_t		res118[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b1_p0;
	uint8_t		res119[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b1_p0;
	uint8_t		res120[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl2_b1_p0;
	uint8_t		res121[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b1_p0;
	uint8_t		res122[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxSlewRate_b1_p0;
	uint8_t		res123[0x8];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingIncDecDtsmEn_r1;
	uint8_t		res124[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg0_r1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg1_r1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg2_r1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg3_r1;
	uint8_t		res125[0x50];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg0_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg1_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg2_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg3_u1_p0;
	uint8_t		res126[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg0_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg1_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg2_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg3_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg0_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg1_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg2_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg3_u1_p0;
	uint8_t		res127[0x78];
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm0_i1;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm1_i1;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm2_i1;
	uint8_t		res128[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r1_p0;
	uint8_t		res129[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg0_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg1_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg2_u1_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg3_u1_p0;
	uint8_t		res130[0x170];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC1_r2;
	uint8_t		res131[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingCntr_r2;
	uint8_t		res132[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC0_r2;
	uint8_t		res133[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingIncDecDtsmEn_r2;
	uint8_t		res134[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg0_r2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg1_r2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg2_r2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg3_r2;
	uint8_t		res135[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm0_i2;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm1_i2;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm2_i2;
	uint8_t		res136[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r2_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r2_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r2_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r2_p0;
	uint8_t		res137[0x1b0];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC1_r3;
	uint8_t		res138[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingCntr_r3;
	uint8_t		res139[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC0_r3;
	uint8_t		res140[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingIncDecDtsmEn_r3;
	uint8_t		res141[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg0_r3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg1_r3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg2_r3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg3_r3;
	uint8_t		res142[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm0_i3;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm1_i3;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm2_i3;
	uint8_t		res143[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r3_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r3_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r3_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r3_p0;
	uint8_t		res144[0x1b0];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC1_r4;
	uint8_t		res145[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingCntr_r4;
	uint8_t		res146[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC0_r4;
	uint8_t		res147[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingIncDecDtsmEn_r4;
	uint8_t		res148[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg0_r4;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg1_r4;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg2_r4;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg3_r4;
	uint8_t		res149[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm0_i4;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm1_i4;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm2_i4;
	uint8_t		res150[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r4_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r4_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r4_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r4_p0;
	uint8_t		res151[0x1b0];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC1_r5;
	uint8_t		res152[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingCntr_r5;
	uint8_t		res153[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC0_r5;
	uint8_t		res154[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingIncDecDtsmEn_r5;
	uint8_t		res155[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg0_r5;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg1_r5;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg2_r5;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg3_r5;
	uint8_t		res156[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm0_i5;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm1_i5;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm2_i5;
	uint8_t		res157[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r5_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r5_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r5_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r5_p0;
	uint8_t		res158[0x1b0];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC1_r6;
	uint8_t		res159[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingCntr_r6;
	uint8_t		res160[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC0_r6;
	uint8_t		res161[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingIncDecDtsmEn_r6;
	uint8_t		res162[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg0_r6;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg1_r6;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg2_r6;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg3_r6;
	uint8_t		res163[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm0_i6;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm1_i6;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm2_i6;
	uint8_t		res164[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r6_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r6_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r6_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r6_p0;
	uint8_t		res165[0x1b0];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC1_r7;
	uint8_t		res166[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingCntr_r7;
	uint8_t		res167[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC0_r7;
	uint8_t		res168[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingIncDecDtsmEn_r7;
	uint8_t		res169[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg0_r7;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg1_r7;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg2_r7;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg3_r7;
	uint8_t		res170[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm0_i7;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm1_i7;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm2_i7;
	uint8_t		res171[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r7_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r7_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r7_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r7_p0;
	uint8_t		res172[0x1b0];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC1_r8;
	uint8_t		res173[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingCntr_r8;
	uint8_t		res174[0x34];
	uint32_t	DWC_DDRPHYA_DBYTE1_VrefDAC0_r8;
	uint8_t		res175[0x84];
	uint32_t	DWC_DDRPHYA_DBYTE1_TrainingIncDecDtsmEn_r8;
	uint8_t		res176[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg0_r8;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg1_r8;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg2_r8;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxPBDlyTg3_r8;
	uint8_t		res177[0x118];
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm0_i8;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm1_i8;
	uint32_t	DWC_DDRPHYA_DBYTE1_Tsm2_i8;
	uint8_t		res178[0x2c];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r8_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r8_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r8_p0;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r8_p0;
};

struct lpddr4phy_regs_t2 {
	uint32_t	DWC_DDRPHYA_MASTER0_RxFifoInit;
	uint32_t	DWC_DDRPHYA_MASTER0_ForceClkDisable;
	uint32_t	DWC_DDRPHYA_MASTER0_ClockingCtrl;
	uint32_t	DWC_DDRPHYA_MASTER0_ForceInternalUpdate;
	uint32_t	DWC_DDRPHYA_MASTER0_PhyConfig;
	uint32_t	DWC_DDRPHYA_MASTER0_PGCR;
	uint8_t		res0[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_TestBumpCntrl1;
	uint32_t	DWC_DDRPHYA_MASTER0_CalUclkInfo_p0;
	uint8_t		res1[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_TestBumpCntrl;
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY0_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY1_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY2_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY3_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_PhyAlertStatus;
	uint32_t	DWC_DDRPHYA_MASTER0_PPTTrainSetup_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_PPTTrainSetup2_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_ATestMode;
	uint8_t		res2[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_TxCalBinP;
	uint32_t	DWC_DDRPHYA_MASTER0_TxCalBinN;
	uint32_t	DWC_DDRPHYA_MASTER0_TxCalPOvr;
	uint32_t	DWC_DDRPHYA_MASTER0_TxCalNOvr;
	uint32_t	DWC_DDRPHYA_MASTER0_DfiMode;
	uint32_t	DWC_DDRPHYA_MASTER0_TristateModeCA_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_MtestMuxSel;
	uint32_t	DWC_DDRPHYA_MASTER0_MtestPgmInfo;
	uint32_t	DWC_DDRPHYA_MASTER0_DynPwrDnUp;
	uint32_t	DWC_DDRPHYA_MASTER0_PMIEnable;
	uint32_t	DWC_DDRPHYA_MASTER0_PhyTID;
	uint8_t		res3[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_HwtMRL_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_DFIPHYUPD;
	uint32_t	DWC_DDRPHYA_MASTER0_PdaMrsWriteMode;
	uint32_t	DWC_DDRPHYA_MASTER0_DFIGEARDOWNCTL;
	uint32_t	DWC_DDRPHYA_MASTER0_DqsPreambleControl_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_MasterX4Config;
	uint32_t	DWC_DDRPHYA_MASTER0_WrLevBits;
	uint32_t	DWC_DDRPHYA_MASTER0_EnableCsMulticast;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtLpCsMultiCast;
	uint8_t		res4[0xc];
	uint32_t	DWC_DDRPHYA_MASTER0_Acx4AnibDis;
	uint32_t	DWC_DDRPHYA_MASTER0_DMIPinPresent_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p0;
	uint8_t		res5[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_Db0LcdlCalPhDetOut;
	uint32_t	DWC_DDRPHYA_MASTER0_Db1LcdlCalPhDetOut;
	uint32_t	DWC_DDRPHYA_MASTER0_Db2LcdlCalPhDetOut;
	uint32_t	DWC_DDRPHYA_MASTER0_Db3LcdlCalPhDetOut;
	uint32_t	DWC_DDRPHYA_MASTER0_Db4LcdlCalPhDetOut;
	uint32_t	DWC_DDRPHYA_MASTER0_Db5LcdlCalPhDetOut;
	uint32_t	DWC_DDRPHYA_MASTER0_Db6LcdlCalPhDetOut;
	uint32_t	DWC_DDRPHYA_MASTER0_Db7LcdlCalPhDetOut;
	uint32_t	DWC_DDRPHYA_MASTER0_Db8LcdlCalPhDetOut;
	uint32_t	DWC_DDRPHYA_MASTER0_Db9LcdlCalPhDetOut;
	uint32_t	DWC_DDRPHYA_MASTER0_DbyteDllModeCntrl;
	uint32_t	DWC_DDRPHYA_MASTER0_DbyteRxEnTrain;
	uint8_t		res6[0xc];
	uint32_t	DWC_DDRPHYA_MASTER0_AnLcdlCalPhDetOut;
	uint8_t		res7[0x14];
	uint32_t	DWC_DDRPHYA_MASTER0_CalOffsets;
	uint8_t		res8[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_SarInitVals;
	uint8_t		res9[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_CalPExtOvr;
	uint32_t	DWC_DDRPHYA_MASTER0_CalCmpr5Ovr;
	uint32_t	DWC_DDRPHYA_MASTER0_CalNIntOvr;
	uint8_t		res10[0x10];
	uint32_t	DWC_DDRPHYA_MASTER0_CalDrvStr0;
	uint8_t		res11[0x10];
	uint32_t	DWC_DDRPHYA_MASTER0_ProcOdtCtl;
	uint32_t	DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p0;
	uint8_t		res12[0x10];
	uint32_t	DWC_DDRPHYA_MASTER0_MemAlertControl;
	uint32_t	DWC_DDRPHYA_MASTER0_MemAlertControl2;
	uint8_t		res13[0xc];
	uint32_t	DWC_DDRPHYA_MASTER0_MemResetL;
	uint8_t		res14[0x34];
	uint32_t	DWC_DDRPHYA_MASTER0_PUBMODE;
	uint32_t	DWC_DDRPHYA_MASTER0_MiscPhyStatus;
	uint32_t	DWC_DDRPHYA_MASTER0_CoreLoopbackSel;
	uint32_t	DWC_DDRPHYA_MASTER0_DllTrainParam;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtLpCsEnA;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtLpCsEnB;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtLpCsEnBypass;
	uint32_t	DWC_DDRPHYA_MASTER0_DfiCAMode;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtCACtl;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtCAMode;
	uint32_t	DWC_DDRPHYA_MASTER0_DllControl;
	uint32_t	DWC_DDRPHYA_MASTER0_PulseDllUpdatePhase;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtControlOvr0;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtControlOvr1;
	uint32_t	DWC_DDRPHYA_MASTER0_DllGainCtl_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_DllLockParam_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtControlVal0;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtControlVal1;
	uint8_t		res15[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_AcsmGlblStart;
	uint32_t	DWC_DDRPHYA_MASTER0_AcsmGlblSglStpCtrl;
	uint8_t		res16[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_LcdlCalPhase;
	uint32_t	DWC_DDRPHYA_MASTER0_LcdlCalCtrl;
	uint8_t		res17[0x8];
	uint32_t	DWC_DDRPHYA_MASTER0_CalRate;
	uint32_t	DWC_DDRPHYA_MASTER0_CalZap;
	uint8_t		res18[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_PState;
	uint32_t	DWC_DDRPHYA_MASTER0_CalPreDriverOverride;
	uint32_t	DWC_DDRPHYA_MASTER0_PllOutGateControl;
	uint8_t		res19[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_UcMemResetControl;
	uint32_t	DWC_DDRPHYA_MASTER0_PorControl;
	uint8_t		res20[0x18];
	uint32_t	DWC_DDRPHYA_MASTER0_CalBusy;
	uint32_t	DWC_DDRPHYA_MASTER0_CalMisc2;
	uint8_t		res21[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_CalMisc;
	uint8_t		res22[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_CalCmpr5;
	uint32_t	DWC_DDRPHYA_MASTER0_CalNInt;
	uint32_t	DWC_DDRPHYA_MASTER0_CalPExt;
	uint8_t		res23[0x24];
	uint32_t	DWC_DDRPHYA_MASTER0_CalCmpInvert;
	uint8_t		res24[0x14];
	uint32_t	DWC_DDRPHYA_MASTER0_CalCmpanaCntrl;
	uint8_t		res25[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_DfiRdDataCsDestMap_p0;
	uint8_t		res26[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_VrefInGlobal_p0;
	uint8_t		res27[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_DfiWrDataCsDestMap_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_MasUpdGoodCtr;
	uint32_t	DWC_DDRPHYA_MASTER0_PhyUpd0GoodCtr;
	uint32_t	DWC_DDRPHYA_MASTER0_PhyUpd1GoodCtr;
	uint32_t	DWC_DDRPHYA_MASTER0_CtlUpd0GoodCtr;
	uint32_t	DWC_DDRPHYA_MASTER0_CtlUpd1GoodCtr;
	uint32_t	DWC_DDRPHYA_MASTER0_MasUpdFailCtr;
	uint32_t	DWC_DDRPHYA_MASTER0_PhyUpd0FailCtr;
	uint32_t	DWC_DDRPHYA_MASTER0_PhyUpd1FailCtr;
	uint32_t	DWC_DDRPHYA_MASTER0_PhyPerfCtrEnable;
	uint8_t		res28[0x14];
	uint32_t	DWC_DDRPHYA_MASTER0_PllPwrDn;
	uint32_t	DWC_DDRPHYA_MASTER0_PllReset;
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl2_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl0;
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl1_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_PllTst;
	uint32_t	DWC_DDRPHYA_MASTER0_PllLockStatus;
	uint32_t	DWC_DDRPHYA_MASTER0_PllTestMode_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl3;
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl4_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_PllEndofCal;
	uint32_t	DWC_DDRPHYA_MASTER0_PllStandbyEff;
	uint32_t	DWC_DDRPHYA_MASTER0_PllDacValOut;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestSeq;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestRingSelDb;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestRingSelAc;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestCntDfiClkIV;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestCntDfiClk;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestCntRingOscDb0;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestCntRingOscDb1;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestCntRingOscDb2;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestCntRingOscDb3;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestCntRingOscDb4;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestCntRingOscDb5;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestCntRingOscDb6;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestCntRingOscDb7;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestCntRingOscDb8;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestCntRingOscDb9;
	uint32_t	DWC_DDRPHYA_MASTER0_DlyTestCntRingOscAc;
	uint32_t	DWC_DDRPHYA_MASTER0_MstLcdlDbgCntl;
	uint32_t	DWC_DDRPHYA_MASTER0_MstLcdl0DbgRes;
	uint32_t	DWC_DDRPHYA_MASTER0_MstLcdl1DbgRes;
	uint32_t	DWC_DDRPHYA_MASTER0_LcdlDbgCntl;
	uint32_t	DWC_DDRPHYA_MASTER0_AcLcdlStatus;
	uint8_t		res29[0x20];
	uint32_t	DWC_DDRPHYA_MASTER0_CUSTPHYREV;
	uint32_t	DWC_DDRPHYA_MASTER0_PHYREV;
	uint32_t	DWC_DDRPHYA_MASTER0_LP3ExitSeq0BStartVector;
	uint32_t	DWC_DDRPHYA_MASTER0_DfiFreqXlat0;
	uint32_t	DWC_DDRPHYA_MASTER0_DfiFreqXlat1;
	uint32_t	DWC_DDRPHYA_MASTER0_DfiFreqXlat2;
	uint32_t	DWC_DDRPHYA_MASTER0_DfiFreqXlat3;
	uint32_t	DWC_DDRPHYA_MASTER0_DfiFreqXlat4;
	uint32_t	DWC_DDRPHYA_MASTER0_DfiFreqXlat5;
	uint32_t	DWC_DDRPHYA_MASTER0_DfiFreqXlat6;
	uint32_t	DWC_DDRPHYA_MASTER0_DfiFreqXlat7;
	uint32_t	DWC_DDRPHYA_MASTER0_TxRdPtrInit;
	uint32_t	DWC_DDRPHYA_MASTER0_DfiInitComplete;
	uint32_t	DWC_DDRPHYA_MASTER0_DfiFreqRatio_p0;
	uint32_t	DWC_DDRPHYA_MASTER0_RxFifoChecks;
	uint8_t		res30[0x10];
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAA0toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAA1toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAA2toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAA3toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAA4toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAA5toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAA6toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAA7toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAA8toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAA9toDfi;
	uint8_t		res31[0x18];
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAB0toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAB1toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAB2toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAB3toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAB4toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAB5toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAB6toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAB7toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAB8toDfi;
	uint32_t	DWC_DDRPHYA_MASTER0_MapCAB9toDfi;
	uint8_t		res32[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_PhyInterruptEnable;
	uint32_t	DWC_DDRPHYA_MASTER0_PhyInterruptFWControl;
	uint32_t	DWC_DDRPHYA_MASTER0_PhyInterruptMask;
	uint32_t	DWC_DDRPHYA_MASTER0_PhyInterruptClear;
	uint32_t	DWC_DDRPHYA_MASTER0_PhyInterruptStatus;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress0;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress1;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress2;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress3;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress4;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress5;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress6;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress7;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress8;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress9;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress10;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress11;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress12;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress13;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress14;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress15;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtAddress17;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtActN;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtBank0;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtBank1;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtBank2;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtBg0;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtBg1;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtCasN;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtRasN;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtWeN;
	uint32_t	DWC_DDRPHYA_MASTER0_HwtSwizzleHwtParityIn;
};

struct lpddr4phy_regs_t3 {
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x4;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x5;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x6;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x7;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x8;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x9;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x10;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x11;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x12;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x13;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x14;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x15;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x16;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x17;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x18;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x19;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x20;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x21;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x22;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x23;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x24;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x25;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x26;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x27;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x28;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x29;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x30;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq0x31;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x4;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x5;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x6;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x7;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x8;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x9;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x10;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x11;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x12;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x13;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x14;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x15;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x16;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x17;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x18;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x19;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x20;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x21;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x22;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x23;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x24;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x25;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x26;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x27;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x28;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x29;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x30;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq1x31;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x4;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x5;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x6;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x7;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x8;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x9;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x10;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x11;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x12;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x13;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x14;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x15;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x16;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x17;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x18;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x19;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x20;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x21;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x22;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x23;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x24;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x25;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x26;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x27;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x28;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x29;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x30;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq2x31;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x4;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x5;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x6;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x7;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x8;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x9;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x10;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x11;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x12;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x13;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x14;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x15;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x16;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x17;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x18;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x19;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x20;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x21;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x22;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x23;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x24;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x25;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x26;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x27;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x28;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x29;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x30;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmSeq3x31;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x0_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x0_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x1_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x1_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x2_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x2_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x3_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x3_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x4_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x4_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x5_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x5_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x6_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x6_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x7_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x7_p0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPStateOvrEn;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPStateOvrVal;
	uint8_t		res0[0xb8];
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl23;
	uint8_t		res1[0x4];
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCkeVal;
	uint8_t		res2[0x14];
	uint32_t	DWC_DDRPHYA_ACSM0_LowSpeedClockDivider;
	uint8_t		res3[0x1c];
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl4;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl5;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl6;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl7;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl8;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl9;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl10;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl11;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl12;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl13;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl14;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl15;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmOdtCtrl0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmOdtCtrl1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmOdtCtrl2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmOdtCtrl3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmOdtCtrl4;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmOdtCtrl5;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmOdtCtrl6;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmOdtCtrl7;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmOdtCtrl8;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl16;
	uint32_t	DWC_DDRPHYA_ACSM0_LowSpeedClockStopVal;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl18;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl19;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl20;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl21;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl22;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl0;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl4;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl5;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl6;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl7;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl8;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl9;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl10;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl11;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl12;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl13;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl14;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmCtrl15;
};

struct lpddr4phy_regs_t4 {
	uint32_t	DWC_DDRPHYA_PPGC0_PPGCCtrl1;
	uint8_t		res0[0xc];
	uint32_t	DWC_DDRPHYA_PPGC0_PpgcLane2CrcInMap0;
	uint32_t	DWC_DDRPHYA_PPGC0_PpgcLane2CrcInMap1;
	uint8_t		res1[0x34];
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly0_i0;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly1_i0;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly2_i0;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly3_i0;
	uint8_t		res2[0x20];
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte0;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte1;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte2;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte3;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte4;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte5;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte6;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte7;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte8;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte9;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte10;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte11;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte12;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte13;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte14;
	uint32_t	DWC_DDRPHYA_PPGC0_GenPrbsByte15;
	uint8_t		res3[0x80];
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsGenCtl;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsGenStateLo;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsGenStateHi;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsChkStateLo;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsChkStateHi;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsGenCtl1;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsGenCtl2;
	uint8_t		res4[0x2f4];
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly0_i1;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly1_i1;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly2_i1;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly3_i1;
	uint8_t		res5[0x3f0];
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly0_i2;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly1_i2;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly2_i2;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly3_i2;
	uint8_t		res6[0x3f0];
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly0_i3;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly1_i3;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly2_i3;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly3_i3;
	uint8_t		res7[0x3f0];
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly0_i4;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly1_i4;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly2_i4;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly3_i4;
	uint8_t		res8[0x3f0];
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly0_i5;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly1_i5;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly2_i5;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly3_i5;
	uint8_t		res9[0x3f0];
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly0_i6;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly1_i6;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly2_i6;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly3_i6;
	uint8_t		res10[0x3f0];
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly0_i7;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly1_i7;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly2_i7;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly3_i7;
	uint8_t		res11[0x3f0];
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly0_i8;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly1_i8;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly2_i8;
	uint32_t	DWC_DDRPHYA_PPGC0_PrbsTapDly3_i8;
};

struct lpddr4phy_regs_t5 {
	uint32_t	DWC_DDRPHYA_INITENG0_PreSequenceReg0b0s0;
	uint32_t	DWC_DDRPHYA_INITENG0_PreSequenceReg0b0s1;
	uint32_t	DWC_DDRPHYA_INITENG0_PreSequenceReg0b0s2;
	uint32_t	DWC_DDRPHYA_INITENG0_PreSequenceReg0b1s0;
	uint32_t	DWC_DDRPHYA_INITENG0_PreSequenceReg0b1s1;
	uint32_t	DWC_DDRPHYA_INITENG0_PreSequenceReg0b1s2;
	uint32_t	DWC_DDRPHYA_INITENG0_PostSequenceReg0b0s0;
	uint32_t	DWC_DDRPHYA_INITENG0_PostSequenceReg0b0s1;
	uint32_t	DWC_DDRPHYA_INITENG0_PostSequenceReg0b0s2;
	uint32_t	DWC_DDRPHYA_INITENG0_PostSequenceReg0b1s0;
	uint32_t	DWC_DDRPHYA_INITENG0_PostSequenceReg0b1s1;
	uint32_t	DWC_DDRPHYA_INITENG0_PostSequenceReg0b1s2;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BDisableFlag0;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BDisableFlag1;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BDisableFlag2;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BDisableFlag3;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BDisableFlag4;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BDisableFlag5;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BDisableFlag6;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BDisableFlag7;
	uint8_t		res0[0xc];
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b0;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b1;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b2;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b3;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b4;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b5;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b6;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b7;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b8;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b9;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b10;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b11;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b12;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b13;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b14;
	uint32_t	DWC_DDRPHYA_INITENG0_StartVector0b15;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0bWaitCondSel;
	uint32_t	DWC_DDRPHYA_INITENG0_PhyInLP3;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b0s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b0s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b0s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b1s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b1s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b1s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b2s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b2s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b2s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b3s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b3s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b3s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b4s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b4s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b4s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b5s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b5s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b5s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b6s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b6s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b6s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b7s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b7s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b7s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b8s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b8s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b8s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b9s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b9s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b9s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b10s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b10s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b10s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b11s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b11s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b11s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b12s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b12s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b12s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b13s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b13s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b13s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b14s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b14s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b14s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b15s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b15s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b15s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b16s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b16s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b16s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b17s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b17s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b17s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b18s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b18s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b18s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b19s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b19s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b19s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b20s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b20s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b20s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b21s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b21s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b21s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b22s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b22s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b22s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b23s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b23s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b23s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b24s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b24s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b24s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b25s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b25s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b25s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b26s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b26s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b26s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b27s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b27s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b27s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b28s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b28s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b28s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b29s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b29s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b29s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b30s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b30s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b30s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b31s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b31s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b31s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b32s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b32s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b32s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b33s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b33s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b33s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b34s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b34s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b34s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b35s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b35s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b35s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b36s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b36s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b36s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b37s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b37s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b37s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b38s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b38s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b38s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b39s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b39s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b39s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b40s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b40s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b40s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b41s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b41s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b41s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b42s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b42s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b42s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b43s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b43s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b43s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b44s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b44s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b44s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b45s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b45s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b45s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b46s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b46s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b46s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b47s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b47s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b47s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b48s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b48s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b48s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b49s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b49s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b49s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b50s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b50s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b50s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b51s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b51s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b51s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b52s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b52s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b52s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b53s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b53s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b53s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b54s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b54s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b54s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b55s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b55s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b55s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b56s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b56s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b56s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b57s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b57s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b57s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b58s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b58s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b58s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b59s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b59s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b59s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b60s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b60s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b60s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b61s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b61s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b61s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b62s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b62s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b62s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b63s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b63s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b63s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b64s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b64s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b64s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b65s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b65s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b65s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b66s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b66s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b66s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b67s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b67s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b67s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b68s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b68s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b68s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b69s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b69s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b69s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b70s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b70s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b70s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b71s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b71s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b71s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b72s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b72s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b72s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b73s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b73s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b73s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b74s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b74s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b74s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b75s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b75s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b75s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b76s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b76s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b76s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b77s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b77s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b77s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b78s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b78s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b78s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b79s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b79s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b79s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b80s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b80s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b80s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b81s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b81s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b81s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b82s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b82s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b82s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b83s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b83s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b83s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b84s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b84s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b84s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b85s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b85s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b85s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b86s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b86s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b86s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b87s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b87s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b87s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b88s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b88s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b88s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b89s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b89s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b89s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b90s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b90s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b90s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b91s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b91s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b91s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b92s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b92s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b92s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b93s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b93s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b93s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b94s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b94s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b94s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b95s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b95s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b95s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b96s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b96s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b96s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b97s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b97s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b97s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b98s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b98s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b98s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b99s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b99s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b99s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b100s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b100s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b100s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b101s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b101s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b101s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b102s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b102s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b102s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b103s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b103s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b103s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b104s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b104s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b104s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b105s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b105s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b105s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b106s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b106s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b106s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b107s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b107s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b107s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b108s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b108s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b108s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b109s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b109s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b109s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b110s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b110s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b110s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b111s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b111s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b111s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b112s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b112s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b112s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b113s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b113s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b113s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b114s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b114s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b114s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b115s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b115s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b115s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b116s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b116s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b116s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b117s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b117s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b117s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b118s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b118s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b118s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b119s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b119s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b119s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b120s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b120s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b120s2;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b121s0;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b121s1;
	uint32_t	DWC_DDRPHYA_INITENG0_SequenceReg0b121s2;
	uint8_t		res1[0x1a8];
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR1_p0;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR2_p0;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR3_p0;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR4_p0;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR5_p0;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR6_p0;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR7_p0;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR8_p0;
	uint8_t		res2[0x3d8];
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BFixedAddrBits;
};

struct lpddr4phy_regs_t6 {
	uint32_t	DWC_DDRPHYA_DRTUB0_DctShadowRegs;
	uint8_t		res0[0xac];
	uint32_t	DWC_DDRPHYA_DRTUB0_DctWriteOnlyShadow;
	uint8_t		res1[0x4];
	uint32_t	DWC_DDRPHYA_DRTUB0_UctWriteOnly;
	uint32_t	DWC_DDRPHYA_DRTUB0_UctWriteProt;
	uint32_t	DWC_DDRPHYA_DRTUB0_UctDatWriteOnly;
	uint32_t	DWC_DDRPHYA_DRTUB0_UctDatWriteProt;
	uint32_t	DWC_DDRPHYA_DRTUB0_UctlErr;
	uint8_t		res2[0x124];
	uint32_t	DWC_DDRPHYA_DRTUB0_UcclkHclkEnables;
	uint32_t	DWC_DDRPHYA_DRTUB0_CurPstate0b;
	uint8_t		res3[0x4c];
	uint32_t	DWC_DDRPHYA_DRTUB0_ClrWakeupSticky;
	uint32_t	DWC_DDRPHYA_DRTUB0_WakeupMask;
	uint8_t		res4[0x158];
	uint32_t	DWC_DDRPHYA_DRTUB0_CUSTPUBREV;
	uint32_t	DWC_DDRPHYA_DRTUB0_PUBREV;
};

struct lpddr4phy_regs_t7 {
	uint32_t	DWC_DDRPHYA_APBONLY0_MicroContMuxSel;
	uint8_t		res0[0xc];
	uint32_t	DWC_DDRPHYA_APBONLY0_UctShadowRegs;
	uint8_t		res1[0xac];
	uint32_t	DWC_DDRPHYA_APBONLY0_DctWriteOnly;
	uint32_t	DWC_DDRPHYA_APBONLY0_DctWriteProt;
	uint32_t	DWC_DDRPHYA_APBONLY0_UctWriteOnlyShadow;
	uint8_t		res2[0x4];
	uint32_t	DWC_DDRPHYA_APBONLY0_UctDatWriteOnlyShadow;
	uint32_t	DWC_DDRPHYA_APBONLY0_NeverGateCsrClock;
	uint8_t		res3[0x4];
	uint32_t	DWC_DDRPHYA_APBONLY0_DfiCfgRdDataValidTicks;
	uint8_t		res4[0x184];
	uint32_t	DWC_DDRPHYA_APBONLY0_MicroReset;
	uint8_t		res5[0x134];
	uint32_t	DWC_DDRPHYA_APBONLY0_SequencerOverride;
	uint8_t		res6[0x48];
	uint32_t	DWC_DDRPHYA_APBONLY0_DfiInitCompleteShadow;
};

struct lpddr4phy_regs_t8 {
	uint32_t	DWC_DDRPHYA_ANIB0_ATxDly_p1;
	uint8_t		res0[0x3ffc];
	uint32_t	DWC_DDRPHYA_ANIB1_ATxDly_p1;
	uint8_t		res1[0x3ffc];
	uint32_t	DWC_DDRPHYA_ANIB2_ATxDly_p1;
};

struct lpddr4phy_regs_t9 {
	uint32_t	DWC_DDRPHYA_DBYTE0_UseDqsEnReplica_p1;
	uint8_t		res0[0x70];
	uint32_t	DWC_DDRPHYA_DBYTE0_DFIMRL_p1;
	uint8_t		res1[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl0_b0_p1;
	uint8_t		res2[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b0_p1;
	uint8_t		res3[0x10];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxEqualizationMode_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b0_p1;
	uint8_t		res4[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl2_b0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl2_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b0_p1;
	uint8_t		res5[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxSlewRate_b0_p1;
	uint8_t		res6[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg0_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg1_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg2_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg3_u0_p1;
	uint8_t		res7[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg0_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg1_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg2_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg3_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg0_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg1_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg2_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg3_u0_p1;
	uint8_t		res8[0x68];
	uint32_t	DWC_DDRPHYA_DBYTE0_PptDqsCntInvTrnTg0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_PptDqsCntInvTrnTg1_p1;
	uint8_t		res9[0x40];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r0_p1;
	uint8_t		res10[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg0_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg1_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg2_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg3_u0_p1;
	uint8_t		res11[0x1b4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl0_b1_p1;
	uint8_t		res12[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b1_p1;
	uint8_t		res13[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b1_p1;
	uint8_t		res14[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl2_b1_p1;
	uint8_t		res15[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b1_p1;
	uint8_t		res16[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxSlewRate_b1_p1;
	uint8_t		res17[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg0_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg1_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg2_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg3_u1_p1;
	uint8_t		res18[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg0_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg1_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg2_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg3_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg0_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg1_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg2_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg3_u1_p1;
	uint8_t		res19[0xb0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r1_p1;
	uint8_t		res20[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg0_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg1_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg2_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg3_u1_p1;
	uint8_t		res21[0x3b0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r2_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r2_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r2_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r2_p1;
	uint8_t		res22[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r3_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r3_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r3_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r3_p1;
	uint8_t		res23[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r4_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r4_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r4_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r4_p1;
	uint8_t		res24[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r5_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r5_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r5_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r5_p1;
	uint8_t		res25[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r6_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r6_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r6_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r6_p1;
	uint8_t		res26[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r7_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r7_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r7_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r7_p1;
	uint8_t		res27[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r8_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r8_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r8_p1;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r8_p1;
	uint8_t		res28[0x1cfc];
	uint32_t	DWC_DDRPHYA_DBYTE1_UseDqsEnReplica_p1;
	uint8_t		res29[0x70];
	uint32_t	DWC_DDRPHYA_DBYTE1_DFIMRL_p1;
	uint8_t		res30[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl0_b0_p1;
	uint8_t		res31[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b0_p1;
	uint8_t		res32[0x10];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxEqualizationMode_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b0_p1;
	uint8_t		res33[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl2_b0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl2_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b0_p1;
	uint8_t		res34[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxSlewRate_b0_p1;
	uint8_t		res35[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg0_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg1_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg2_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg3_u0_p1;
	uint8_t		res36[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg0_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg1_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg2_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg3_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg0_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg1_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg2_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg3_u0_p1;
	uint8_t		res37[0x68];
	uint32_t	DWC_DDRPHYA_DBYTE1_PptDqsCntInvTrnTg0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_PptDqsCntInvTrnTg1_p1;
	uint8_t		res38[0x40];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r0_p1;
	uint8_t		res39[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg0_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg1_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg2_u0_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg3_u0_p1;
	uint8_t		res40[0x1b4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl0_b1_p1;
	uint8_t		res41[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b1_p1;
	uint8_t		res42[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b1_p1;
	uint8_t		res43[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl2_b1_p1;
	uint8_t		res44[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b1_p1;
	uint8_t		res45[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxSlewRate_b1_p1;
	uint8_t		res46[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg0_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg1_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg2_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg3_u1_p1;
	uint8_t		res47[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg0_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg1_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg2_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg3_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg0_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg1_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg2_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg3_u1_p1;
	uint8_t		res48[0xb0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r1_p1;
	uint8_t		res49[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg0_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg1_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg2_u1_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg3_u1_p1;
	uint8_t		res50[0x3b0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r2_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r2_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r2_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r2_p1;
	uint8_t		res51[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r3_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r3_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r3_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r3_p1;
	uint8_t		res52[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r4_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r4_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r4_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r4_p1;
	uint8_t		res53[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r5_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r5_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r5_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r5_p1;
	uint8_t		res54[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r6_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r6_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r6_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r6_p1;
	uint8_t		res55[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r7_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r7_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r7_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r7_p1;
	uint8_t		res56[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r8_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r8_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r8_p1;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r8_p1;
};

struct lpddr4phy_regs_t10 {
	uint32_t	DWC_DDRPHYA_MASTER0_CalUclkInfo_p1;
	uint8_t		res0[0x8];
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY0_p1;
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY1_p1;
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY2_p1;
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY3_p1;
	uint8_t		res1[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_PPTTrainSetup_p1;
	uint32_t	DWC_DDRPHYA_MASTER0_PPTTrainSetup2_p1;
	uint8_t		res2[0x1c];
	uint32_t	DWC_DDRPHYA_MASTER0_TristateModeCA_p1;
	uint8_t		res3[0x18];
	uint32_t	DWC_DDRPHYA_MASTER0_HwtMRL_p1;
	uint8_t		res4[0xc];
	uint32_t	DWC_DDRPHYA_MASTER0_DqsPreambleControl_p1;
	uint8_t		res5[0x20];
	uint32_t	DWC_DDRPHYA_MASTER0_DMIPinPresent_p1;
	uint32_t	DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p1;
	uint8_t		res6[0x9c];
	uint32_t	DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p1;
	uint8_t		res7[0x94];
	uint32_t	DWC_DDRPHYA_MASTER0_DllGainCtl_p1;
	uint32_t	DWC_DDRPHYA_MASTER0_DllLockParam_p1;
	uint8_t		res8[0xc8];
	uint32_t	DWC_DDRPHYA_MASTER0_DfiRdDataCsDestMap_p1;
	uint8_t		res9[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_VrefInGlobal_p1;
	uint8_t		res10[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_DfiWrDataCsDestMap_p1;
	uint8_t		res11[0x40];
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl2_p1;
	uint8_t		res12[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl1_p1;
	uint8_t		res13[0x8];
	uint32_t	DWC_DDRPHYA_MASTER0_PllTestMode_p1;
	uint8_t		res14[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl4_p1;
	uint8_t		res15[0xb4];
	uint32_t	DWC_DDRPHYA_MASTER0_DfiFreqRatio_p1;
};

struct lpddr4phy_regs_t11 {
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x0_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x0_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x1_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x1_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x2_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x2_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x3_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x3_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x4_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x4_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x5_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x5_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x6_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x6_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x7_p1;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x7_p1;
};

struct lpddr4phy_regs_t12 {
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR1_p1;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR2_p1;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR3_p1;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR4_p1;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR5_p1;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR6_p1;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR7_p1;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR8_p1;
};

struct lpddr4phy_regs_t13 {
	uint32_t	DWC_DDRPHYA_ANIB0_ATxDly_p2;
	uint8_t		res0[0x3ffc];
	uint32_t	DWC_DDRPHYA_ANIB1_ATxDly_p2;
	uint8_t		res1[0x3ffc];
	uint32_t	DWC_DDRPHYA_ANIB2_ATxDly_p2;
};

struct lpddr4phy_regs_t14 {
	uint32_t	DWC_DDRPHYA_DBYTE0_UseDqsEnReplica_p2;
	uint8_t		res0[0x70];
	uint32_t	DWC_DDRPHYA_DBYTE0_DFIMRL_p2;
	uint8_t		res1[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl0_b0_p2;
	uint8_t		res2[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b0_p2;
	uint8_t		res3[0x10];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxEqualizationMode_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b0_p2;
	uint8_t		res4[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl2_b0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl2_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b0_p2;
	uint8_t		res5[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxSlewRate_b0_p2;
	uint8_t		res6[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg0_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg1_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg2_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg3_u0_p2;
	uint8_t		res7[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg0_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg1_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg2_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg3_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg0_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg1_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg2_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg3_u0_p2;
	uint8_t		res8[0x68];
	uint32_t	DWC_DDRPHYA_DBYTE0_PptDqsCntInvTrnTg0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_PptDqsCntInvTrnTg1_p2;
	uint8_t		res9[0x40];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r0_p2;
	uint8_t		res10[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg0_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg1_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg2_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg3_u0_p2;
	uint8_t		res11[0x1b4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl0_b1_p2;
	uint8_t		res12[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b1_p2;
	uint8_t		res13[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b1_p2;
	uint8_t		res14[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl2_b1_p2;
	uint8_t		res15[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b1_p2;
	uint8_t		res16[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxSlewRate_b1_p2;
	uint8_t		res17[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg0_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg1_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg2_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg3_u1_p2;
	uint8_t		res18[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg0_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg1_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg2_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg3_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg0_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg1_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg2_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg3_u1_p2;
	uint8_t		res19[0xb0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r1_p2;
	uint8_t		res20[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg0_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg1_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg2_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg3_u1_p2;
	uint8_t		res21[0x3b0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r2_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r2_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r2_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r2_p2;
	uint8_t		res22[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r3_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r3_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r3_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r3_p2;
	uint8_t		res23[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r4_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r4_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r4_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r4_p2;
	uint8_t		res24[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r5_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r5_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r5_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r5_p2;
	uint8_t		res25[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r6_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r6_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r6_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r6_p2;
	uint8_t		res26[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r7_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r7_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r7_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r7_p2;
	uint8_t		res27[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r8_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r8_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r8_p2;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r8_p2;
	uint8_t		res28[0x1cfc];
	uint32_t	DWC_DDRPHYA_DBYTE1_UseDqsEnReplica_p2;
	uint8_t		res29[0x70];
	uint32_t	DWC_DDRPHYA_DBYTE1_DFIMRL_p2;
	uint8_t		res30[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl0_b0_p2;
	uint8_t		res31[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b0_p2;
	uint8_t		res32[0x10];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxEqualizationMode_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b0_p2;
	uint8_t		res33[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl2_b0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl2_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b0_p2;
	uint8_t		res34[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxSlewRate_b0_p2;
	uint8_t		res35[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg0_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg1_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg2_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg3_u0_p2;
	uint8_t		res36[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg0_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg1_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg2_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg3_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg0_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg1_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg2_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg3_u0_p2;
	uint8_t		res37[0x68];
	uint32_t	DWC_DDRPHYA_DBYTE1_PptDqsCntInvTrnTg0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_PptDqsCntInvTrnTg1_p2;
	uint8_t		res38[0x40];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r0_p2;
	uint8_t		res39[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg0_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg1_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg2_u0_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg3_u0_p2;
	uint8_t		res40[0x1b4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl0_b1_p2;
	uint8_t		res41[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b1_p2;
	uint8_t		res42[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b1_p2;
	uint8_t		res43[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl2_b1_p2;
	uint8_t		res44[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b1_p2;
	uint8_t		res45[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxSlewRate_b1_p2;
	uint8_t		res46[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg0_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg1_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg2_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg3_u1_p2;
	uint8_t		res47[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg0_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg1_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg2_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg3_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg0_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg1_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg2_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg3_u1_p2;
	uint8_t		res48[0xb0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r1_p2;
	uint8_t		res49[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg0_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg1_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg2_u1_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg3_u1_p2;
	uint8_t		res50[0x3b0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r2_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r2_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r2_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r2_p2;
	uint8_t		res51[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r3_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r3_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r3_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r3_p2;
	uint8_t		res52[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r4_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r4_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r4_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r4_p2;
	uint8_t		res53[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r5_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r5_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r5_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r5_p2;
	uint8_t		res54[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r6_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r6_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r6_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r6_p2;
	uint8_t		res55[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r7_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r7_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r7_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r7_p2;
	uint8_t		res56[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r8_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r8_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r8_p2;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r8_p2;
};

struct lpddr4phy_regs_t15 {
	uint32_t	DWC_DDRPHYA_MASTER0_CalUclkInfo_p2;
	uint8_t		res0[0x8];
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY0_p2;
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY1_p2;
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY2_p2;
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY3_p2;
	uint8_t		res1[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_PPTTrainSetup_p2;
	uint32_t	DWC_DDRPHYA_MASTER0_PPTTrainSetup2_p2;
	uint8_t		res2[0x1c];
	uint32_t	DWC_DDRPHYA_MASTER0_TristateModeCA_p2;
	uint8_t		res3[0x18];
	uint32_t	DWC_DDRPHYA_MASTER0_HwtMRL_p2;
	uint8_t		res4[0xc];
	uint32_t	DWC_DDRPHYA_MASTER0_DqsPreambleControl_p2;
	uint8_t		res5[0x20];
	uint32_t	DWC_DDRPHYA_MASTER0_DMIPinPresent_p2;
	uint32_t	DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p2;
	uint8_t		res6[0x9c];
	uint32_t	DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p2;
	uint8_t		res7[0x94];
	uint32_t	DWC_DDRPHYA_MASTER0_DllGainCtl_p2;
	uint32_t	DWC_DDRPHYA_MASTER0_DllLockParam_p2;
	uint8_t		res8[0xc8];
	uint32_t	DWC_DDRPHYA_MASTER0_DfiRdDataCsDestMap_p2;
	uint8_t		res9[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_VrefInGlobal_p2;
	uint8_t		res10[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_DfiWrDataCsDestMap_p2;
	uint8_t		res11[0x40];
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl2_p2;
	uint8_t		res12[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl1_p2;
	uint8_t		res13[0x8];
	uint32_t	DWC_DDRPHYA_MASTER0_PllTestMode_p2;
	uint8_t		res14[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl4_p2;
	uint8_t		res15[0xb4];
	uint32_t	DWC_DDRPHYA_MASTER0_DfiFreqRatio_p2;
};

struct lpddr4phy_regs_t16 {
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x0_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x0_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x1_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x1_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x2_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x2_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x3_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x3_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x4_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x4_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x5_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x5_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x6_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x6_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x7_p2;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x7_p2;
};

struct lpddr4phy_regs_t17 {
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR1_p2;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR2_p2;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR3_p2;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR4_p2;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR5_p2;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR6_p2;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR7_p2;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR8_p2;
};

struct lpddr4phy_regs_t18 {
	uint32_t	DWC_DDRPHYA_ANIB0_ATxDly_p3;
	uint8_t		res0[0x3ffc];
	uint32_t	DWC_DDRPHYA_ANIB1_ATxDly_p3;
	uint8_t		res1[0x3ffc];
	uint32_t	DWC_DDRPHYA_ANIB2_ATxDly_p3;
};

struct lpddr4phy_regs_t19 {
	uint32_t	DWC_DDRPHYA_DBYTE0_UseDqsEnReplica_p3;
	uint8_t		res0[0x70];
	uint32_t	DWC_DDRPHYA_DBYTE0_DFIMRL_p3;
	uint8_t		res1[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl0_b0_p3;
	uint8_t		res2[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b0_p3;
	uint8_t		res3[0x10];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxEqualizationMode_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b0_p3;
	uint8_t		res4[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl2_b0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl2_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b0_p3;
	uint8_t		res5[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxSlewRate_b0_p3;
	uint8_t		res6[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg0_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg1_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg2_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg3_u0_p3;
	uint8_t		res7[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg0_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg1_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg2_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg3_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg0_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg1_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg2_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg3_u0_p3;
	uint8_t		res8[0x68];
	uint32_t	DWC_DDRPHYA_DBYTE0_PptDqsCntInvTrnTg0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_PptDqsCntInvTrnTg1_p3;
	uint8_t		res9[0x40];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r0_p3;
	uint8_t		res10[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg0_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg1_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg2_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg3_u0_p3;
	uint8_t		res11[0x1b4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl0_b1_p3;
	uint8_t		res12[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b1_p3;
	uint8_t		res13[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b1_p3;
	uint8_t		res14[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl2_b1_p3;
	uint8_t		res15[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b1_p3;
	uint8_t		res16[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxSlewRate_b1_p3;
	uint8_t		res17[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg0_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg1_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg2_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxEnDlyTg3_u1_p3;
	uint8_t		res18[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg0_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg1_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg2_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkDlyTg3_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg0_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg1_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg2_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_RxClkcDlyTg3_u1_p3;
	uint8_t		res19[0xb0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r1_p3;
	uint8_t		res20[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg0_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg1_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg2_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqsDlyTg3_u1_p3;
	uint8_t		res21[0x3b0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r2_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r2_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r2_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r2_p3;
	uint8_t		res22[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r3_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r3_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r3_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r3_p3;
	uint8_t		res23[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r4_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r4_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r4_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r4_p3;
	uint8_t		res24[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r5_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r5_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r5_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r5_p3;
	uint8_t		res25[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r6_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r6_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r6_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r6_p3;
	uint8_t		res26[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r7_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r7_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r7_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r7_p3;
	uint8_t		res27[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg0_r8_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg1_r8_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg2_r8_p3;
	uint32_t	DWC_DDRPHYA_DBYTE0_TxDqDlyTg3_r8_p3;
	uint8_t		res28[0x1cfc];
	uint32_t	DWC_DDRPHYA_DBYTE1_UseDqsEnReplica_p3;
	uint8_t		res29[0x70];
	uint32_t	DWC_DDRPHYA_DBYTE1_DFIMRL_p3;
	uint8_t		res30[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl0_b0_p3;
	uint8_t		res31[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b0_p3;
	uint8_t		res32[0x10];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxEqualizationMode_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b0_p3;
	uint8_t		res33[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl2_b0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl2_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b0_p3;
	uint8_t		res34[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxSlewRate_b0_p3;
	uint8_t		res35[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg0_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg1_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg2_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg3_u0_p3;
	uint8_t		res36[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg0_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg1_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg2_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg3_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg0_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg1_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg2_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg3_u0_p3;
	uint8_t		res37[0x68];
	uint32_t	DWC_DDRPHYA_DBYTE1_PptDqsCntInvTrnTg0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_PptDqsCntInvTrnTg1_p3;
	uint8_t		res38[0x40];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r0_p3;
	uint8_t		res39[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg0_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg1_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg2_u0_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg3_u0_p3;
	uint8_t		res40[0x1b4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl0_b1_p3;
	uint8_t		res41[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b1_p3;
	uint8_t		res42[0x14];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b1_p3;
	uint8_t		res43[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl2_b1_p3;
	uint8_t		res44[0x4];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b1_p3;
	uint8_t		res45[0x44];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxSlewRate_b1_p3;
	uint8_t		res46[0x80];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg0_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg1_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg2_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxEnDlyTg3_u1_p3;
	uint8_t		res47[0x20];
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg0_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg1_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg2_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkDlyTg3_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg0_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg1_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg2_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_RxClkcDlyTg3_u1_p3;
	uint8_t		res48[0xb0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r1_p3;
	uint8_t		res49[0x30];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg0_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg1_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg2_u1_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqsDlyTg3_u1_p3;
	uint8_t		res50[0x3b0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r2_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r2_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r2_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r2_p3;
	uint8_t		res51[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r3_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r3_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r3_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r3_p3;
	uint8_t		res52[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r4_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r4_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r4_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r4_p3;
	uint8_t		res53[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r5_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r5_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r5_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r5_p3;
	uint8_t		res54[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r6_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r6_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r6_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r6_p3;
	uint8_t		res55[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r7_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r7_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r7_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r7_p3;
	uint8_t		res56[0x3f0];
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg0_r8_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg1_r8_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg2_r8_p3;
	uint32_t	DWC_DDRPHYA_DBYTE1_TxDqDlyTg3_r8_p3;
};

struct lpddr4phy_regs_t20 {
	uint32_t	DWC_DDRPHYA_MASTER0_CalUclkInfo_p3;
	uint8_t		res0[0x8];
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY0_p3;
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY1_p3;
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY2_p3;
	uint32_t	DWC_DDRPHYA_MASTER0_Seq0BDLY3_p3;
	uint8_t		res1[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_PPTTrainSetup_p3;
	uint32_t	DWC_DDRPHYA_MASTER0_PPTTrainSetup2_p3;
	uint8_t		res2[0x1c];
	uint32_t	DWC_DDRPHYA_MASTER0_TristateModeCA_p3;
	uint8_t		res3[0x18];
	uint32_t	DWC_DDRPHYA_MASTER0_HwtMRL_p3;
	uint8_t		res4[0xc];
	uint32_t	DWC_DDRPHYA_MASTER0_DqsPreambleControl_p3;
	uint8_t		res5[0x20];
	uint32_t	DWC_DDRPHYA_MASTER0_DMIPinPresent_p3;
	uint32_t	DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p3;
	uint8_t		res6[0x9c];
	uint32_t	DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p3;
	uint8_t		res7[0x94];
	uint32_t	DWC_DDRPHYA_MASTER0_DllGainCtl_p3;
	uint32_t	DWC_DDRPHYA_MASTER0_DllLockParam_p3;
	uint8_t		res8[0xc8];
	uint32_t	DWC_DDRPHYA_MASTER0_DfiRdDataCsDestMap_p3;
	uint8_t		res9[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_VrefInGlobal_p3;
	uint8_t		res10[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_DfiWrDataCsDestMap_p3;
	uint8_t		res11[0x40];
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl2_p3;
	uint8_t		res12[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl1_p3;
	uint8_t		res13[0x8];
	uint32_t	DWC_DDRPHYA_MASTER0_PllTestMode_p3;
	uint8_t		res14[0x4];
	uint32_t	DWC_DDRPHYA_MASTER0_PllCtrl4_p3;
	uint8_t		res15[0xb4];
	uint32_t	DWC_DDRPHYA_MASTER0_DfiFreqRatio_p3;
};

struct lpddr4phy_regs_t21 {
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x0_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x0_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x1_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x1_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x2_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x2_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x3_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x3_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x4_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x4_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x5_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x5_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x6_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x6_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback0x7_p3;
	uint32_t	DWC_DDRPHYA_ACSM0_AcsmPlayback1x7_p3;
};

struct lpddr4phy_regs_t22 {
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR1_p3;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR2_p3;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR3_p3;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR4_p3;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR5_p3;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR6_p3;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR7_p3;
	uint32_t	DWC_DDRPHYA_INITENG0_Seq0BGPR8_p3;
};

struct lpddr4phy_training_firmware_dmem_t {
	uint32_t	DMEM_0x0;
	uint32_t	DMEM_0x4;
	uint32_t	DMEM_0x8;
	uint32_t	DMEM_0xC;
	uint32_t	DMEM_0x10;
	uint32_t	DMEM_0x14;
	uint32_t	DMEM_0x18;
	uint32_t	DMEM_0x1C;
	uint32_t	DMEM_0x20;
	uint32_t	DMEM_0x24;
	uint32_t	DMEM_0x28;
	uint32_t	DMEM_0x2C;
	uint32_t	DMEM_0x30;
	uint32_t	DMEM_0x34;
	uint32_t	DMEM_0x38;
	uint32_t	DMEM_0x3C;
	uint32_t	DMEM_0x40;
	uint32_t	DMEM_0x44;
	uint32_t	DMEM_0x48;
	uint32_t	DMEM_0x4C;
	uint32_t	DMEM_0x50;
	uint32_t	DMEM_0x54;
	uint32_t	DMEM_0x58;
	uint32_t	DMEM_0x5C;
	uint32_t	DMEM_0x60;
	uint32_t	DMEM_0x64;
	uint32_t	DMEM_0x68;
	uint32_t	DMEM_0x6C;
	uint32_t	DMEM_0x70;
	uint32_t	DMEM_0x74;
	uint32_t	DMEM_0x78;
	uint32_t	DMEM_0x7C;
	uint32_t	DMEM_0x80;
	uint32_t	DMEM_0x84;
	uint32_t	DMEM_0x88;
	uint32_t	DMEM_0x8C;
	uint32_t	DMEM_0x90;
	uint32_t	DMEM_0x94;
	uint32_t	DMEM_0x98;
	uint32_t	DMEM_0x9C;
	uint32_t	DMEM_0xA0;
	uint32_t	DMEM_0xA4;
	uint32_t	DMEM_0xA8;
	uint32_t	DMEM_0xAC;
};

struct lpddr4phy_diag_firmware_dmem_msg_t {
	uint32_t	DMEM_MSG_0x0;
	uint32_t	DMEM_MSG_0x4;
	uint32_t	DMEM_MSG_0x8;
	uint32_t	DMEM_MSG_0xC;
	uint32_t	DMEM_MSG_0x10;
	uint32_t	DMEM_MSG_0x14;
	uint32_t	DMEM_MSG_0x18;
	uint32_t	DMEM_MSG_0x1C;
	uint32_t	DMEM_MSG_0x20;
	uint32_t	DMEM_MSG_0x24;
};

/*******************************************
	SNPS PHY training firmware
********************************************/
typedef enum {
	END_OF_INITIALIZATION             =0x00, 
	END_OF_FINE_WRITE_LEVELING        =0x01, 
	END_OF_READ_ENABLE                =0x02, 
	END_OF_READ_DELAY                 =0x03, 
	END_OF_WRITE_DELAY                =0x04, 
	END_OF_2D_READ_DELAY              =0x05, 
	END_OF_2D_WRITE_DELAY             =0x06, 
	TRAINING_HAS_RUN_SUCCESSFULLY     =0x07, 
	START_STREAMING_MESSAGE_MODE      =0x08, 
	END_OF_MAX_READ_LATENCY           =0x09, 
	END_OF_READ_DQ_DESKEW             =0x0A, 
	END_OF_LRDIMM_SPECIFIC_TRAINING   =0x0C, 
	END_OF_CA_TRAIN                   =0x0D, 
	END_OF_MPR_READ_DELAY             =0xFD, 
	END_OF_WRITE_LEVELING_COARSE      =0xFE, 
	TRAINING_HAS_FAILED               =0xFF 
} Train_UctWriteOnlyShadow_Status_t;

// Training Firmware SequenceCtrl code
#define DevInit	(1<<0)
#define WrLvl	(1<<1)
#define RxEn		(1<<2)
#define RdDQS1D	(1<<3)
#define WrDQ1D	(1<<4)
#define RdDeskew	(1<<8)
#define MxRdLat	(1<<9)
#define LPCA		(1<<12)
#define rd2D		(1<<5)
#define wr2D		(1<<6)

typedef enum {
	MSG_16BIT_MODE	=0x00, 
	MSG_32BIT_MODE	=0x01, 
} Train_MSG_Mode_t;

struct dmc_config_t {
	unsigned int numRank;
	unsigned int deratingEn;
};

struct phy_config_t {
	unsigned int retraining;
};

struct sdram_config_t {
	unsigned int b;
};

struct phy_diag_param_t {
	unsigned int byte;
	unsigned int bit;
	unsigned int testItem;
	unsigned int socDrvStren; 
	unsigned int dramDqOdt;
	unsigned int socCaDrvStren;
	unsigned int dramCaOdt;
	unsigned int dramPuStren;
	unsigned int dramPdStren;
	unsigned int socOdt;
	unsigned int testPrbs;	
	unsigned int testPattern;
};

/*
#define PRELIMINA_1D_IMEM_SIZ	14840
#define PRELIMINA_1D_DMEM_SIZ	804
#define PRELIMINA_2D_IMEM_SIZ	13654
#define PRELIMINA_2D_DMEM_SIZ	636

#define V1240_1D_IMEM_SIZ		15634
#define V1240_1D_DMEM_SIZ		804
#define V1240_2D_IMEM_SIZ		11248
#define V1240_2D_DMEM_SIZ		658

#define V1260_1D_IMEM_SIZ		15500
#define V1260_1D_DMEM_SIZ		812
#define V1260_2D_IMEM_SIZ		11546
#define V1260_2D_DMEM_SIZ		666

#define TRAIN_FW_1D_IMEM_SIZ	V1260_1D_IMEM_SIZ
#define TRAIN_FW_1D_DMEM_SIZ	V1260_1D_DMEM_SIZ
#define TRAIN_FW_2D_IMEM_SIZ	V1260_2D_IMEM_SIZ
#define TRAIN_FW_2D_DMEM_SIZ	V1260_2D_DMEM_SIZ
*/

void umctl2_init_nLPDDR4_Xm_all_ch(unsigned int mif_init_freq);
void umctl2_init_nLPDDR4_Xm(unsigned int mif_init_freq, int i);
void umctl2_init_nLPDDR4_Xm_set_default_MR_all_ch(void);
void umctl2_init_nLPDDR4_Xm_set_default_MR(int i);
void umctl2_init2_nLPDDR4_Xm_all_ch(void);
void umctl2_init2_nLPDDR4_Xm(int i);
void load_dmem_msg_block(int ch, unsigned int mif_init_freq, uint32_t train_2d);
uint32_t get_mail_training_firmware (int ch, uint32_t mode);
uint32_t get_mail_training_firmware_2d (int ch, uint32_t mode);
void decode_streaming_msg(int ch);
void decode_major_msg(uint32_t mail);
void check_init_nLPDDR4_Xm_all_ch(void);
void check_init_nLPDDR4_Xm(int i);
void mem_ctrl_init(unsigned int memclk, unsigned char warm_reset);

void pub_init_nLPDDR4_Xm_all_ch(unsigned int mif_init_freq);
void pub_init_nLPDDR4_Xm(unsigned int mif_init_freq, int i);
void pub_1d_training_all_ch(unsigned int mif_init_freq);
void pub_1d_training(int stage, unsigned int mif_init_freq, int i);
void pub_2d_training_all_ch(unsigned int mif_init_freq);
void pub_2d_training_load(unsigned int mif_init_freq, int i);
void pub_2d_training_run(int i);
uint32_t pub_2d_training_handshake(int i);
void pub_init2_nLPDDR4_Xm_all_ch(unsigned int mif_init_freq);
void pub_init2_nLPDDR4_Xm(unsigned int mif_init_freq, int i);

#define apb_wr(a,v)	writel(v,a)
void load_dmem_1d_binary(int ch);
void load_imem_1d_binary(int ch);

void post_init_wdqs_all_ch(unsigned int mif_init_freq);
void post_init_wdqs(unsigned int mif_init_freq, int i);
void load_dmem_2d_binary(int ch);
void load_imem_2d_binary(int ch);
void load_pie_binary(int ch);

#endif /* _CPU_TURBO_TRAV_DMC_H_ */

