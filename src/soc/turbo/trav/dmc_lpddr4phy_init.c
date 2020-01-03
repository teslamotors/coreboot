/*
 *
 * Copyright (C) 2017 Samsung Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <stdlib.h>
#include <string.h>
#include <delay.h>
#include <soc/dmc_lpddr4phy_init.h>
#include <soc/dmc_train_fw_imem_1d_bin.h>
#include <soc/dmc_train_fw_imem_2d_bin.h>
#include <soc/dmc_train_fw_dmem_1d_bin.h>
#include <soc/dmc_train_fw_dmem_2d_bin.h>
#include <soc/power.h>
#include <soc/clock.h>
#include <soc/clock_init.h>
#include <soc/boot_control.h>
#include <soc/mailbox_A72_SCS_interface.h>

#include <soc/scs_helpers.h>
#include <soc/sms_helpers.h>

/* use 8 bits now */
#define OTP_DDR_INFO_MASK    0xff

#define dmc_printf(...)	

//#define TRAINING_LOG
#define ECC_ENABLED
#define REDUCED_PHY_DFI_UPDATE		// (For PHY) Periodic DFI update of 7.5us
#define PERIODIC_RETRAIN		// (For DRAM) Periodic Re-training of 32ms
#define PERIODIC_ZQCAL_PHY		// (For PHY) Periodic ZQ cal of 20ms
#define PERIODIC_ZQCAL_DRAM		// (For DRAM) Periodic ZQ cal of 32ms
#define DFE_ON              // to enable SNPS Rx DFE to improve Read Signal Quality
#define RD_DBI_OFF					// For improving DMI signal quality

/* Selection of ECC Protected Region */
#define	ECC_PROT_REGION0	1	/* 0x80000000 - 0xBFFFFFFF */
#define	ECC_PROT_REGION1	1	/* 0xC0000000 - 0xFFFFFFFF */
#define	ECC_PROT_REGION2	1	/* 0x100000000 - 0x13FFFFFFF */
#define	ECC_PROT_REGION3	1	/* 0x140000000 - 0x17FFFFFFF */
#define	ECC_PROT_REGION4	1	/* 0x180000000 - 0x1BFFFFFFF */
#define	ECC_PROT_REGION5	1	/* 0x1C0000000 - 0x1FFFFFFFF */
#define	ECC_PROT_REGION6	1	/* 0x200000000 - 0x23FFFFFFF */

static int redo_initial_training = 0;

volatile struct dmc_regs_t * const __DMC_MIF[8] = {
	(struct dmc_regs_t *)TRAV_DMC_MIF0,
	(struct dmc_regs_t *)TRAV_DMC_MIF1,
	(struct dmc_regs_t *)TRAV_DMC_MIF2,
	(struct dmc_regs_t *)TRAV_DMC_MIF3,
	(struct dmc_regs_t *)TRAV_DMC_MIF4,
	(struct dmc_regs_t *)TRAV_DMC_MIF5,
	(struct dmc_regs_t *)TRAV_DMC_MIF6,
	(struct dmc_regs_t *)TRAV_DMC_MIF7,
};

volatile struct lpddr4phy_regs_t0 * const __LPDDR4PHY_MIF_REGS_t0[8] = {
	(struct lpddr4phy_regs_t0 *)TRAV_LPDDR4PHY_MIF0,
	(struct lpddr4phy_regs_t0 *)TRAV_LPDDR4PHY_MIF1,
	(struct lpddr4phy_regs_t0 *)TRAV_LPDDR4PHY_MIF2,
	(struct lpddr4phy_regs_t0 *)TRAV_LPDDR4PHY_MIF3,
	(struct lpddr4phy_regs_t0 *)TRAV_LPDDR4PHY_MIF4,
	(struct lpddr4phy_regs_t0 *)TRAV_LPDDR4PHY_MIF5,
	(struct lpddr4phy_regs_t0 *)TRAV_LPDDR4PHY_MIF6,
	(struct lpddr4phy_regs_t0 *)TRAV_LPDDR4PHY_MIF7,
};

volatile struct lpddr4phy_regs_t1 * const __LPDDR4PHY_MIF_REGS_t1[8] = {
	(struct lpddr4phy_regs_t1 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET1),
	(struct lpddr4phy_regs_t1 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET1),
	(struct lpddr4phy_regs_t1 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET1),
	(struct lpddr4phy_regs_t1 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET1),
	(struct lpddr4phy_regs_t1 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET1),
	(struct lpddr4phy_regs_t1 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET1),
	(struct lpddr4phy_regs_t1 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET1),
	(struct lpddr4phy_regs_t1 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET1),
};

volatile struct lpddr4phy_regs_t2 * const __LPDDR4PHY_MIF_REGS_t2[8] = {
	(struct lpddr4phy_regs_t2 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET2),
	(struct lpddr4phy_regs_t2 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET2),
	(struct lpddr4phy_regs_t2 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET2),
	(struct lpddr4phy_regs_t2 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET2),
	(struct lpddr4phy_regs_t2 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET2),
	(struct lpddr4phy_regs_t2 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET2),
	(struct lpddr4phy_regs_t2 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET2),
	(struct lpddr4phy_regs_t2 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET2),
};

volatile struct lpddr4phy_regs_t3 * const __LPDDR4PHY_MIF_REGS_t3[8] = {
	(struct lpddr4phy_regs_t3 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET3),
	(struct lpddr4phy_regs_t3 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET3),
	(struct lpddr4phy_regs_t3 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET3),
	(struct lpddr4phy_regs_t3 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET3),
	(struct lpddr4phy_regs_t3 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET3),
	(struct lpddr4phy_regs_t3 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET3),
	(struct lpddr4phy_regs_t3 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET3),
	(struct lpddr4phy_regs_t3 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET3),
};

volatile struct lpddr4phy_regs_t4 * const __LPDDR4PHY_MIF_REGS_t4[8] = {
	(struct lpddr4phy_regs_t4 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET4),
	(struct lpddr4phy_regs_t4 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET4),
	(struct lpddr4phy_regs_t4 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET4),
	(struct lpddr4phy_regs_t4 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET4),
	(struct lpddr4phy_regs_t4 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET4),
	(struct lpddr4phy_regs_t4 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET4),
	(struct lpddr4phy_regs_t4 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET4),
	(struct lpddr4phy_regs_t4 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET4),
};

volatile struct lpddr4phy_regs_t5 * const __LPDDR4PHY_MIF_REGS_t5[8] = {
	(struct lpddr4phy_regs_t5 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET5),
	(struct lpddr4phy_regs_t5 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET5),
	(struct lpddr4phy_regs_t5 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET5),
	(struct lpddr4phy_regs_t5 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET5),
	(struct lpddr4phy_regs_t5 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET5),
	(struct lpddr4phy_regs_t5 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET5),
	(struct lpddr4phy_regs_t5 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET5),
	(struct lpddr4phy_regs_t5 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET5),
};

volatile struct lpddr4phy_regs_t6 * const __LPDDR4PHY_MIF_REGS_t6[8] = {
	(struct lpddr4phy_regs_t6 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET6),
	(struct lpddr4phy_regs_t6 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET6),
	(struct lpddr4phy_regs_t6 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET6),
	(struct lpddr4phy_regs_t6 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET6),
	(struct lpddr4phy_regs_t6 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET6),
	(struct lpddr4phy_regs_t6 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET6),
	(struct lpddr4phy_regs_t6 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET6),
	(struct lpddr4phy_regs_t6 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET6),
};

volatile struct lpddr4phy_regs_t7 * const __LPDDR4PHY_MIF_REGS_t7[8] = {
	(struct lpddr4phy_regs_t7 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET7),
	(struct lpddr4phy_regs_t7 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET7),
	(struct lpddr4phy_regs_t7 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET7),
	(struct lpddr4phy_regs_t7 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET7),
	(struct lpddr4phy_regs_t7 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET7),
	(struct lpddr4phy_regs_t7 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET7),
	(struct lpddr4phy_regs_t7 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET7),
	(struct lpddr4phy_regs_t7 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET7),
};

volatile struct lpddr4phy_regs_t8 * const __LPDDR4PHY_MIF_REGS_t8[8] = {
	(struct lpddr4phy_regs_t8 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET8),
	(struct lpddr4phy_regs_t8 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET8),
	(struct lpddr4phy_regs_t8 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET8),
	(struct lpddr4phy_regs_t8 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET8),
	(struct lpddr4phy_regs_t8 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET8),
	(struct lpddr4phy_regs_t8 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET8),
	(struct lpddr4phy_regs_t8 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET8),
	(struct lpddr4phy_regs_t8 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET8),
};

volatile struct lpddr4phy_regs_t9 * const __LPDDR4PHY_MIF_REGS_t9[8] = {
	(struct lpddr4phy_regs_t9 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET9),
	(struct lpddr4phy_regs_t9 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET9),
	(struct lpddr4phy_regs_t9 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET9),
	(struct lpddr4phy_regs_t9 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET9),
	(struct lpddr4phy_regs_t9 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET9),
	(struct lpddr4phy_regs_t9 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET9),
	(struct lpddr4phy_regs_t9 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET9),
	(struct lpddr4phy_regs_t9 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET9),
};

volatile struct lpddr4phy_regs_t10 * const __LPDDR4PHY_MIF_REGS_t10[8] = {
	(struct lpddr4phy_regs_t10 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET10),
	(struct lpddr4phy_regs_t10 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET10),
	(struct lpddr4phy_regs_t10 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET10),
	(struct lpddr4phy_regs_t10 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET10),
	(struct lpddr4phy_regs_t10 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET10),
	(struct lpddr4phy_regs_t10 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET10),
	(struct lpddr4phy_regs_t10 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET10),
	(struct lpddr4phy_regs_t10 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET10),
};

volatile struct lpddr4phy_regs_t11 * const __LPDDR4PHY_MIF_REGS_t11[8] = {
	(struct lpddr4phy_regs_t11 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET11),
	(struct lpddr4phy_regs_t11 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET11),
	(struct lpddr4phy_regs_t11 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET11),
	(struct lpddr4phy_regs_t11 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET11),
	(struct lpddr4phy_regs_t11 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET11),
	(struct lpddr4phy_regs_t11 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET11),
	(struct lpddr4phy_regs_t11 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET11),
	(struct lpddr4phy_regs_t11 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET11),
};

volatile struct lpddr4phy_regs_t12 * const __LPDDR4PHY_MIF_REGS_t12[8] = {
	(struct lpddr4phy_regs_t12 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET12),
	(struct lpddr4phy_regs_t12 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET12),
	(struct lpddr4phy_regs_t12 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET12),
	(struct lpddr4phy_regs_t12 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET12),
	(struct lpddr4phy_regs_t12 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET12),
	(struct lpddr4phy_regs_t12 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET12),
	(struct lpddr4phy_regs_t12 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET12),
	(struct lpddr4phy_regs_t12 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET12),
};

volatile struct lpddr4phy_regs_t13 * const __LPDDR4PHY_MIF_REGS_t13[8] = {
	(struct lpddr4phy_regs_t13 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET13),
	(struct lpddr4phy_regs_t13 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET13),
	(struct lpddr4phy_regs_t13 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET13),
	(struct lpddr4phy_regs_t13 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET13),
	(struct lpddr4phy_regs_t13 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET13),
	(struct lpddr4phy_regs_t13 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET13),
	(struct lpddr4phy_regs_t13 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET13),
	(struct lpddr4phy_regs_t13 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET13),
};

volatile struct lpddr4phy_regs_t14 * const __LPDDR4PHY_MIF_REGS_t14[8] = {
	(struct lpddr4phy_regs_t14 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET14),
	(struct lpddr4phy_regs_t14 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET14),
	(struct lpddr4phy_regs_t14 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET14),
	(struct lpddr4phy_regs_t14 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET14),
	(struct lpddr4phy_regs_t14 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET14),
	(struct lpddr4phy_regs_t14 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET14),
	(struct lpddr4phy_regs_t14 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET14),
	(struct lpddr4phy_regs_t14 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET14),
};

volatile struct lpddr4phy_regs_t15 * const __LPDDR4PHY_MIF_REGS_t15[8] = {
	(struct lpddr4phy_regs_t15 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET15),
	(struct lpddr4phy_regs_t15 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET15),
	(struct lpddr4phy_regs_t15 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET15),
	(struct lpddr4phy_regs_t15 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET15),
	(struct lpddr4phy_regs_t15 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET15),
	(struct lpddr4phy_regs_t15 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET15),
	(struct lpddr4phy_regs_t15 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET15),
	(struct lpddr4phy_regs_t15 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET15),
};

volatile struct lpddr4phy_regs_t16 * const __LPDDR4PHY_MIF_REGS_t16[8] = {
	(struct lpddr4phy_regs_t16 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET16),
	(struct lpddr4phy_regs_t16 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET16),
	(struct lpddr4phy_regs_t16 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET16),
	(struct lpddr4phy_regs_t16 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET16),
	(struct lpddr4phy_regs_t16 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET16),
	(struct lpddr4phy_regs_t16 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET16),
	(struct lpddr4phy_regs_t16 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET16),
	(struct lpddr4phy_regs_t16 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET16),
};

volatile struct lpddr4phy_regs_t17 * const __LPDDR4PHY_MIF_REGS_t17[8] = {
	(struct lpddr4phy_regs_t17 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET17),
	(struct lpddr4phy_regs_t17 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET17),
	(struct lpddr4phy_regs_t17 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET17),
	(struct lpddr4phy_regs_t17 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET17),
	(struct lpddr4phy_regs_t17 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET17),
	(struct lpddr4phy_regs_t17 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET17),
	(struct lpddr4phy_regs_t17 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET17),
	(struct lpddr4phy_regs_t17 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET17),
};

volatile struct lpddr4phy_regs_t18 * const __LPDDR4PHY_MIF_REGS_t18[8] = {
	(struct lpddr4phy_regs_t18 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET18),
	(struct lpddr4phy_regs_t18 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET18),
	(struct lpddr4phy_regs_t18 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET18),
	(struct lpddr4phy_regs_t18 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET18),
	(struct lpddr4phy_regs_t18 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET18),
	(struct lpddr4phy_regs_t18 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET18),
	(struct lpddr4phy_regs_t18 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET18),
	(struct lpddr4phy_regs_t18 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET18),
};

volatile struct lpddr4phy_regs_t19 * const __LPDDR4PHY_MIF_REGS_t19[8] = {
	(struct lpddr4phy_regs_t19 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET19),
	(struct lpddr4phy_regs_t19 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET19),
	(struct lpddr4phy_regs_t19 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET19),
	(struct lpddr4phy_regs_t19 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET19),
	(struct lpddr4phy_regs_t19 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET19),
	(struct lpddr4phy_regs_t19 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET19),
	(struct lpddr4phy_regs_t19 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET19),
	(struct lpddr4phy_regs_t19 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET19),
};

volatile struct lpddr4phy_regs_t20 * const __LPDDR4PHY_MIF_REGS_t20[8] = {
	(struct lpddr4phy_regs_t20 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET20),
	(struct lpddr4phy_regs_t20 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET20),
	(struct lpddr4phy_regs_t20 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET20),
	(struct lpddr4phy_regs_t20 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET20),
	(struct lpddr4phy_regs_t20 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET20),
	(struct lpddr4phy_regs_t20 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET20),
	(struct lpddr4phy_regs_t20 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET20),
	(struct lpddr4phy_regs_t20 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET20),
};

volatile struct lpddr4phy_regs_t21 * const __LPDDR4PHY_MIF_REGS_t21[8] = {
	(struct lpddr4phy_regs_t21 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET21),
	(struct lpddr4phy_regs_t21 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET21),
	(struct lpddr4phy_regs_t21 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET21),
	(struct lpddr4phy_regs_t21 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET21),
	(struct lpddr4phy_regs_t21 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET21),
	(struct lpddr4phy_regs_t21 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET21),
	(struct lpddr4phy_regs_t21 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET21),
	(struct lpddr4phy_regs_t21 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET21),
};

volatile struct lpddr4phy_regs_t22 * const __LPDDR4PHY_MIF_REGS_t22[8] = {
	(struct lpddr4phy_regs_t22 *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET22),
	(struct lpddr4phy_regs_t22 *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET22),
	(struct lpddr4phy_regs_t22 *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET22),
	(struct lpddr4phy_regs_t22 *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET22),
	(struct lpddr4phy_regs_t22 *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET22),
	(struct lpddr4phy_regs_t22 *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET22),
	(struct lpddr4phy_regs_t22 *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET22),
	(struct lpddr4phy_regs_t22 *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET22),
};

volatile unsigned int const __LPDDR4PHY_IMEM_BASE[8] = {
	(unsigned int)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET23),
	(unsigned int)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET23),
	(unsigned int)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET23),
	(unsigned int)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET23),
	(unsigned int)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET23),
	(unsigned int)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET23),
	(unsigned int)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET23),
	(unsigned int)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET23),
};

volatile unsigned int const __LPDDR4PHY_DMEM_BASE[8] = {
	(unsigned int)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET24),
	(unsigned int)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET24),
	(unsigned int)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET24),
	(unsigned int)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET24),
	(unsigned int)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET24),
	(unsigned int)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET24),
	(unsigned int)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET24),
	(unsigned int)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET24),
};

volatile struct lpddr4phy_training_firmware_dmem_t * const __LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[8] = {
	(struct lpddr4phy_training_firmware_dmem_t *)(TRAV_LPDDR4PHY_MIF0 + LPDDR4PHY_OFFSET24),
	(struct lpddr4phy_training_firmware_dmem_t *)(TRAV_LPDDR4PHY_MIF1 + LPDDR4PHY_OFFSET24),
	(struct lpddr4phy_training_firmware_dmem_t *)(TRAV_LPDDR4PHY_MIF2 + LPDDR4PHY_OFFSET24),
	(struct lpddr4phy_training_firmware_dmem_t *)(TRAV_LPDDR4PHY_MIF3 + LPDDR4PHY_OFFSET24),
	(struct lpddr4phy_training_firmware_dmem_t *)(TRAV_LPDDR4PHY_MIF4 + LPDDR4PHY_OFFSET24),
	(struct lpddr4phy_training_firmware_dmem_t *)(TRAV_LPDDR4PHY_MIF5 + LPDDR4PHY_OFFSET24),
	(struct lpddr4phy_training_firmware_dmem_t *)(TRAV_LPDDR4PHY_MIF6 + LPDDR4PHY_OFFSET24),
	(struct lpddr4phy_training_firmware_dmem_t *)(TRAV_LPDDR4PHY_MIF7 + LPDDR4PHY_OFFSET24),
};

static struct dmc_config_t dmc_config = {
	.numRank = 1,	// 2rank
	.deratingEn = 0,	// automatic MR4 read 
};

static struct phy_config_t phy_config = {
#ifdef PERIODIC_RETRAIN
	.retraining = 1,
#else
	.retraining = 0,
#endif
};

/*
static struct sdram_config_t sdram_config = {
	.b = 0,
};
*/

void umctl2_init_nLPDDR4_Xm_all_ch(unsigned int mif_init_freq)
{
	int ch;

	for (ch = 0; ch < 4; ch++)
		umctl2_init_nLPDDR4_Xm(mif_init_freq, ch);

	for (ch = 7; ch > 3; ch--)
		umctl2_init_nLPDDR4_Xm(mif_init_freq, ch);
}

// mif_init_freq - 2133 / 1866 / 1600 / 1333 / 1066
void umctl2_init_nLPDDR4_Xm(unsigned int mif_init_freq, int i)
{
	unsigned int mr4_read_interval;
	unsigned int dram_rstn_x1024;
	unsigned char mr1, mr2, mr3, mr13, mr11, mr12, mr22, mr14;	
	unsigned int dramtmg0, dramtmg1, dramtmg2, dramtmg3, dramtmg4, dramtmg5, dramtmg6;
	unsigned int dramtmg7, dramtmg8, dramtmg12, dramtmg13, dramtmg14;
	unsigned int t_zq_long_nop, t_zq_short_nop, t_zq_reset_nop, t_zq_short_interval_x1024;
	unsigned int skip_dram_init, post_cke_x1024, pre_cke_x1024;
	unsigned int t_refi, t_rfc;
	unsigned int diff_rank_wr_gap, diff_rank_rd_gap;
	unsigned int active_ranks, mr_rank;
	unsigned int derate_enable;
	unsigned int dfitmg0, dfitmg1, dfitmg2; 

	//////////////////////////////////////////////////////////////////////////////
	//
	//	Configuration by freqs
	//
	// MR4 read interval = 100ms
	// MR1 - RD-PRE=static, RD-POST=0.5tCK, WR-PRE=2tCK
	// MR2 - RL, WL
	// MR3 - WR-POST=0.5tCK, DBI-RD/DBI-WR=1,PDDS=RZQ/6,VDDQ/3
	// MR13 - To SNPS - (???) any reason to DM OFF (MR13 = 0x20) ?	
	// MR11 - DRAM ODT = 60
	// MR12 - VREF_CA = 27.2%
	// MR14 - VREF DQ = 19.2% (MR14) (default is 0x4D 27.2%)
	// MR22 - ODT-CA=disable, SoC-ODT=RZQ/6
	//
	//////////////////////////////////////////////////////////////////////////////	
	if (mif_init_freq == 2133) {
		mr4_read_interval = 0x065B5990;
		dram_rstn_x1024 = 0x105;
		mr1 = 0x74;	mr2 = 0x3f;	mr3 = 0xf1;	mr13 = 0x20; 
		mr11 = 0x04; mr12 = 0x4d; mr22 = 0x26; mr14 = 0x17;
		dramtmg0 = 0x2121042D; dramtmg1 = 0x00090941; 
#ifdef RD_DBI_OFF
		dramtmg2 = 0x09121719;	// rl = 36
#else
		dramtmg2 = 0x09141719;	// rl = 40
#endif	
		dramtmg3 = 0x00F0F000;
		dramtmg4 = 0x14040914; dramtmg5 = 0x02061111; dramtmg6 = 0x0202000A; dramtmg7 = 0x00000602;
		dramtmg8 = 0x0; dramtmg12 = 0x00020010; dramtmg13 = 0x16100002; dramtmg14 = 0x00000134;
		t_zq_long_nop = 0x42b;
		t_zq_short_nop = 0x20;
		t_zq_reset_nop = 0x36;
#ifdef PERIODIC_ZQCAL_DRAM
		t_zq_short_interval_x1024 = 0x878C; // 32ms //0x196a6; // 100ms
#else
		t_zq_short_interval_x1024 = 0xd1;	// 200us
#endif
		post_cke_x1024 = 3;
		pre_cke_x1024 = 0x824;
		t_refi = 0x208;
		t_rfc = 0x96;
		diff_rank_wr_gap = 7;
		diff_rank_rd_gap = 6;
#ifdef RD_DBI_OFF
		dfitmg0=0x039F820E;
#else
		dfitmg0=0x03A3820E;
#endif	
		dfitmg1=0x00000404;
		dfitmg2=0x0000230E;
	} else if (mif_init_freq == 1866) {
		mr4_read_interval = 0x058FA520;
		dram_rstn_x1024 = 0xe4;		
		mr1 = 0x64;	mr2 = 0x36;	mr3 = 0xf1;	mr13 = 0x20; 
		mr11 = 0x04; mr12 = 0x4d; mr22 = 0x26; mr14 = 0x17;		
		dramtmg0 = 0x25263F28; dramtmg1 = 0x00070738; dramtmg2 = 0x0F120C1D; dramtmg3 = 0x00E0E000;
		dramtmg4 = 0x11040A11; dramtmg5 = 0x02050E0E; dramtmg6 = 0x02020008; dramtmg7 = 0x00000502;
		dramtmg8 = 0x0; dramtmg12 = 0x00020010; dramtmg13 = 0x14100002; dramtmg14 = 0x0000010D;
		t_zq_long_nop = 0x3a5;
		t_zq_short_nop = 0x1c;
		t_zq_reset_nop = 0x2f;
		t_zq_short_interval_x1024 = 0xb7;		
		post_cke_x1024 = 0x3; //2;
		pre_cke_x1024 = 0x3; //0x71f;
		t_refi = 0x1C7;
		t_rfc = 0x83;		
		diff_rank_wr_gap = 7;		
		diff_rank_rd_gap = 6;	
		dfitmg0=0x039F821A;
		dfitmg1=0x00000404;
		dfitmg2=0x00001F1A;		
	} else if (mif_init_freq == 1600) {
		mr4_read_interval = 0x04C4B400;
		dram_rstn_x1024 = 0xc4;
		mr1 = 0x54;	mr2 = 0x2d;	mr3 = 0xf1;	mr13 = 0x20; 
		mr11 = 0x04; mr12 = 0x4d; mr22 = 0x26; mr14 = 0x17;			
		dramtmg0 = 0x15201A21; dramtmg1 = 0x00060730; dramtmg2 = 0x07101414; dramtmg3 = 0x00C0C000;
		dramtmg4 = 0x0F04080F; dramtmg5 = 0x02040C0C; dramtmg6 = 0x00000007; dramtmg7 = 0x00000402;
		dramtmg8 = 0x0; dramtmg12 = 0x00020000; dramtmg13 = 0x0C100002; dramtmg14 = 0x000000E6;
		t_zq_long_nop = 0x320;
		t_zq_short_nop = 0x18;
		t_zq_reset_nop = 0x28;
		t_zq_short_interval_x1024 = 0x9d;				
		post_cke_x1024 = 2;
		pre_cke_x1024 = 0x61b;
		t_refi = 0x186;
		t_rfc = 0x70;		
		diff_rank_wr_gap = 6;		
		diff_rank_rd_gap = 6;		
		dfitmg0=0x03A3821E;
		dfitmg1=0x00000404;
		dfitmg2=0x0000231E;		
	} else if (mif_init_freq == 1333) {
		mr4_read_interval =0x03F8FF90;	
		dram_rstn_x1024 = 0xa3;		
		mr1 = 0x44;	mr2 = 0x24;	mr3 = 0xf1;	mr13 = 0x20; 
		mr11 = 0x04; mr12 = 0x4d; mr22 = 0x26; mr14 = 0x17;	
		dramtmg0 = 0x141B161B; dramtmg1 = 0x00050628; dramtmg2 = 0x060E1312; dramtmg3 = 0x00A0A000;
		dramtmg4 = 0x0C04070D; dramtmg5 = 0x02040A0A; dramtmg6 = 0x00000006; dramtmg7 = 0x00000402;
		dramtmg8 = 0x0; dramtmg12 = 0x00020000; dramtmg13 = 0x0B100002; dramtmg14 = 0x000000C0;
		t_zq_long_nop = 0x29b;
		t_zq_short_nop = 0x14;
		t_zq_reset_nop = 0x22;
		t_zq_short_interval_x1024 = 0x83;						
		post_cke_x1024 = 2;
		pre_cke_x1024 = 0x516;
		t_refi = 0x145;
		t_rfc = 0x5E;		
		diff_rank_wr_gap = 6;		
		diff_rank_rd_gap = 6;		
		dfitmg0=0x03A3821E;
		dfitmg1=0x00000404;
		dfitmg2=0x0000231E;
	} else if (mif_init_freq == 1066) {
		mr4_read_interval = 0x032D4B20;	
		dram_rstn_x1024 = 0x83;		
		mr1 = 0x34;	mr2 = 0x1b;	mr3 = 0xf1;	mr13 = 0x20; 
		mr11 = 0x04; mr12 = 0x4d; mr22 = 0x26; mr14 = 0x17;
		dramtmg0 = 0x13161116; dramtmg1 = 0x00040520; dramtmg2 = 0x050B0F0F; dramtmg3 = 0x00808000;
		dramtmg4 = 0x0A04060B; dramtmg5 = 0x02030808; dramtmg6 = 0x00000005; dramtmg7 = 0x00000302;
		dramtmg8 = 0x0; dramtmg12 = 0x00020000; dramtmg13 = 0x0A100002; dramtmg14 = 0x0000009A;
		t_zq_long_nop = 0x215;
		t_zq_short_nop = 0x10;
		t_zq_reset_nop = 0x1b;
		t_zq_short_interval_x1024 = 0x69;						
		post_cke_x1024 = 2;
		pre_cke_x1024 = 0x412;
		t_refi = 0x104;
		t_rfc = 0x4B;		
		diff_rank_wr_gap = 5;		
		diff_rank_rd_gap = 6;		
		dfitmg0=0x03A3821E;
		dfitmg1=0x00000404;
		dfitmg2=0x0000231E;		
	}  else if (mif_init_freq == 533) {
		mr4_read_interval = 0x0196A590;	
		dram_rstn_x1024 = 0x42;		
		mr1 = 0x14;	mr2 = 0x49;	mr3 = 0xf1;	mr13 = 0x28; 
		mr11 = 0x44; mr12 = 0x4d; mr22 = 0x26; mr14 = 0x17;
		dramtmg0 = 0x0e0b110c; dramtmg1 = 0x00030410; 
#ifdef RD_DBI_OFF
		dramtmg2 = 0x04050c0e; 
#else
		dramtmg2 = 0x04060c0e; 
#endif
		dramtmg3 = 0x00505006;
		dramtmg4 = 0x05040305; dramtmg5 = 0x02090404; dramtmg6 = 0x020f0004; dramtmg7 = 0x0000040b;
		dramtmg8 = 0x00000101; dramtmg12 = 0x00020000; dramtmg13 = 0x00100002; dramtmg14 = 0x00000fc4;
		t_zq_long_nop = 0x10b;
		t_zq_short_nop = 0x8;
		t_zq_reset_nop = 0xe;
		t_zq_short_interval_x1024 = 0x35;						
		post_cke_x1024 = 0x3; //2;
		pre_cke_x1024 = 0x3; //0x412;
		t_refi = 0x82; //0x104;
		t_rfc = 0x53; //0x4B;		
		diff_rank_wr_gap = 0x5; //5;		
		diff_rank_rd_gap = 0x5; //6;	
#ifdef RD_DBI_OFF		
		dfitmg0=0x02858204;
#else
		dfitmg0=0x02878204;
#endif
		dfitmg1=0x00080101;
		dfitmg2=0x00000704;		
	} else { // other than the listed freq, the default is 1600Mhz
		mr4_read_interval = 0x04C4B400;
		dram_rstn_x1024 = 0xc4;		
		mr1 = 0x54;	mr2 = 0x2d;	mr3 = 0xf1;	mr13 = 0x20; 
		mr11 = 0x04; mr12 = 0x4d; mr22 = 0x26; mr14 = 0x17;			
		dramtmg0 = 0x15201A21; dramtmg1 = 0x00060730; dramtmg2 = 0x07101414; dramtmg3 = 0x00C0C000;
		dramtmg4 = 0x0F04080F; dramtmg5 = 0x02040C0C; dramtmg6 = 0x00000007; dramtmg7 = 0x00000402;
		dramtmg8 = 0x0; dramtmg12 = 0x00020000; dramtmg13 = 0x0C100002; dramtmg14 = 0x000000E6;
		t_zq_long_nop = 0x320;
		t_zq_short_nop = 0x18;
		t_zq_reset_nop = 0x28;
		t_zq_short_interval_x1024 = 0x9d;				
		post_cke_x1024 = 2;
		pre_cke_x1024 = 0x61b;
		t_refi = 0x186;
		t_rfc = 0x70;		
		diff_rank_wr_gap = 6;	
		diff_rank_rd_gap = 6;		
		dfitmg0=0x03A3821E;
		dfitmg1=0x00000404;
		dfitmg2=0x0000231E;		
	}

	// dis_dq = 1 : prevent transactions
	__DMC_MIF[i]->UMCTL2_REGS_DBG1 = 0x00000001;		
	// selfref_en = 1 : put SDRAM into sefl-refresh
	__DMC_MIF[i]->UMCTL2_REGS_PWRCTL = 0x00000001;		

	// device_config = 2 : x16 device		
	// frequency_mode = 0 : original register is used		
	//>		// active_ranks = 3 : rank 1 | rank 0
	// frequency_ratio = 0 : 1:2 ratio of DDRC to MEMCLK
	// burst_rdwr = 8 : BL16 (LPDDR4)
	// dll_off_mode = 0 : to put uMCTL2 and DRAM in DLL-on mode for normal frequency operation
	// data_bus_width = 0 : full DQ width to SDRAM
	// lpddr4 = 1 : SDRAM is LPDDR4
	active_ranks = (dmc_config.numRank == 2) ? 0x3 : 0x1;
	__DMC_MIF[i]->UMCTL2_REGS_MSTR = (0x80080020 | (active_ranks << 24));

	// mr_wr = 0 : No assert MR
	// mr_addr = 13 : MA13 (No use in LPDDR4) - but let's write as 13 for set the default value !
	//>		// mr_rank = 3 : rank 1 | rank 0 - only meaningful in this CSR setting
	// mr_type = 0 : 1:WR, 0:RD
	mr_rank = (dmc_config.numRank == 2) ? 0x3 : 0x1;
	__DMC_MIF[i]->UMCTL2_REGS_MRCTRL0 = (0x0000d000 | (mr_rank << 4));

	// mr_data [15:8]  = MA (MR address) - No meaning at this point !
	// mr_data [7:0]] = MRW data - No meaning at this point !
	__DMC_MIF[i]->UMCTL2_REGS_MRCTRL1 = 0x00001e30;

	// rc_derate_value = 2 : tRC derating is '+3' (3.75us / ddrc_core_clk) - set by freq
	// derate_value = 1 : derating is '+2' (1.875us / ddrc_core_clk) - set by freq
	// derate_enable = 0 : Enable MR4 read feature
	derate_enable = dmc_config.deratingEn;
	__DMC_MIF[i]->UMCTL2_REGS_DERATEEN = (0x00000202 | (derate_enable << 0));

	// mr4_read_interval = 0x8739e371 : MR4 read interval (DFI clock cycle) set by freq
	__DMC_MIF[i]->UMCTL2_REGS_DERATEINT = mr4_read_interval; //0x8739e371;

	// target_frequency = 0 : 0:FREQ0, 1:FREQ1, 2:FREQ2, 3:FREQ3 - only valid when MSTR.frequency_mode = 1
	__DMC_MIF[i]->UMCTL2_REGS_MSTR2 = 0x00000000;

	// dis_cam_drain_selfref = 1 : skip CAM drain when enter SR
	// selfref_en = 0 : exit SDRAM from sefl-refresh
	__DMC_MIF[i]->UMCTL2_REGS_PWRCTL = 0x000000AA;		

	// selfref_to_x32 = 0xd (in 32 DFI clocks) set by freq
	// t_dpd_x4096 = 0x11 (in 4096 DFI clocks) set by freq - No meaning at this point !
	// powerdown_to_x32 = 0x10 (in 32 DFI clocks) set by freq
	__DMC_MIF[i]->UMCTL2_REGS_PWRTMG = 0x000d1110;

	// hw_lp_idle_x32 = 0xe6 (in 32 DFI clocks) set by freq
	// hw_lp_exit_idle_en = 1
	// hw_lp_en = 1
	__DMC_MIF[i]->UMCTL2_REGS_HWLPCTL = 0x00ea0001;

	// refresh_margin = 0x2 (32 DFI clock cycles) set by freq - guided to use the default value 2
	// refresh_to_x32 = 0x15 (32 DFI clock cycles) set by freq
	// refresh_burst = 0 : single refresh - for the safety
	// per_bank_refresh = 1
	__DMC_MIF[i]->UMCTL2_REGS_RFSHCTL0 = 0x00215004;

	// refresh_timer1_start_value_x32 = 0 (32 DFI clock cycles) set by freq - don't need to set at this time - FOR PERF only
	// refresh_timer0_start_value_x32 = 1 (32 DFI clock cycles) set by freq - don't need to set at this time - FOR PERF only
	__DMC_MIF[i]->UMCTL2_REGS_RFSHCTL1 = 0x00000000;

	// dis_auto_refresh = 0
	__DMC_MIF[i]->UMCTL2_REGS_RFSHCTL3 = 0x00000000;

	// t_rfc_nom_x1_sel = 1 : 1: x1, 0: x32
	// t_rfc_nom_x1_x32 = 0x186 : tREFIpb 488ns (x1 clock if t_rfc_nom_x1_sel = 1) set by freq
	// t_rfc_min = 0x70 : tRFCpb 140ns (Clocks) set by freq
	__DMC_MIF[i]->UMCTL2_REGS_RFSHTMG = (0x80000000 |(t_refi << T_RFC_NOM_X1_X32_OFFSET) |(t_rfc << T_RFC_MIN_OFFSET));

	#ifdef ECC_ENABLED
	__DMC_MIF[i]->UMCTL2_REGS_ECCCFG0 = 0x003f0004 |
										(ECC_PROT_REGION0 << 8) |
										(ECC_PROT_REGION1 << 9) |
										(ECC_PROT_REGION2 << 10) |
										(ECC_PROT_REGION3 << 11) |
										(ECC_PROT_REGION4 << 12) |
										(ECC_PROT_REGION5 << 13) |
										(ECC_PROT_REGION6 << 14);
	#endif

	__DMC_MIF[i]->UMCTL2_REGS_CRCPARCTL0 = 0x00000000;

	// skip_dram_init = 3 : SDRAM Initialization routine is skipped after powerup. Controller starts up in Self-refresh Mode
	// post_cke_x1024 = 3 : CKE high to start the SDRAM initialization - 2us for LP4 (1024 DFI clock cycles) set by freq
	// pre_cke_x1024 = 2 : tINIT3 2ms in 1024 DFI clk - set by freq
	skip_dram_init = 0x3;	
	__DMC_MIF[i]->UMCTL2_REGS_INIT0 = (0x0 |(skip_dram_init << SKIP_DRAM_INIT_OFFSET) | (post_cke_x1024 << POST_CKE_X1024_OFFSET) | (pre_cke_x1024 << PRE_CKE_X1024_OFFSET)); // 0x00030002;

	// dram_rstn_x1024 = 3 : Number of cycles to assert SDRAM reset signal during init sequence. (1024 DFI clock)
	// pre_ocd_x32 = 2 : Counts of a global timer that pulses every 32 DFI clock cycles. There is no known specific requirement for this; it may be set to zero.
	__DMC_MIF[i]->UMCTL2_REGS_INIT1 = (0x00000000 | (dram_rstn_x1024 << DRAM_RSTN_X1024_OFFSET) | (0x1 << 0)); //0x00030002;

	// idle_after_reset_x32 = 0xd (32 DFI clock cycles) - for LP2 only
	// min_stable_clock_x1 = 5 (DFI clock cycles.) - for LP2/LP3 only 
	__DMC_MIF[i]->UMCTL2_REGS_INIT2 = 0x00000d05;

	// mr = 0x54 : MR1 value
	// emr = 0x2d : MR2 value
	__DMC_MIF[i]->UMCTL2_REGS_INIT3 = ((mr1 << MR_OFFSET) | (mr2 << EMR_OFFSET)); //0x0054002d;

	// emr2 = 0xc1 : MR3 value
	// emr3 = 0x20 : MR13 value
	__DMC_MIF[i]->UMCTL2_REGS_INIT4 = ((mr3 << EMR2_OFFSET) | (mr13 << EMR3_OFFSET));  //0x00c10020;

	// dev_zqinit_x32 = 0x10 : (32 DFI clock cycles.) for LP2/LP3
	// max_auto_init_x1024 = 4 : (1024 DFI clock cycles.) for LP2/LP3
	__DMC_MIF[i]->UMCTL2_REGS_INIT5 = 0x00100004;

	// mr4 = 0 : MR11 value
	// mr5 = 0x4d : MR12 value
	__DMC_MIF[i]->UMCTL2_REGS_INIT6 = ((mr11 << MR4_OFFSET) | (mr12 << MR5_OFFSET));  //0x0000004d;

	// mr22 = 0 : MR22 value
	// mr6 = 4d : MR14 value
	__DMC_MIF[i]->UMCTL2_REGS_INIT7 = ((mr22 << MR22_OFFSET) | (mr14 << MR6_OFFSET));  //0x0000004d;

	__DMC_MIF[i]->UMCTL2_REGS_DIMMCTL = 0x00000000;

	// diff_rank_wr_gap = 2 :  LPDDR4 = ODTLoff - ODTLon - BL/2 + 1 (+ 1, if WR postamble is 1.5tCK) - set by freq
	// diff_rank_rd_gap = 8 : the ODT switch off timing requirement when switching ranks during reads. - set by freq
	// max_rank_rd = 0 : Disable feature - FOR PERF ONLY
	__DMC_MIF[i]->UMCTL2_REGS_RANKCTL = (0x00000000 | (diff_rank_wr_gap << DIFF_RANK_WR_GAP_OFFSET) |(diff_rank_rd_gap << DIFF_RANK_RD_GAP_OFFSET));

	// AC timing param - set by freq
	__DMC_MIF[i]->UMCTL2_REGS_DRAMTMG0 = dramtmg0; 	//0x1a203522;
	__DMC_MIF[i]->UMCTL2_REGS_DRAMTMG1 = dramtmg1; 	//0x00060630;
	__DMC_MIF[i]->UMCTL2_REGS_DRAMTMG2 = dramtmg2; 	//0x07101114;
	__DMC_MIF[i]->UMCTL2_REGS_DRAMTMG3 = dramtmg3; 	//0x00b0c000;
	__DMC_MIF[i]->UMCTL2_REGS_DRAMTMG4 = dramtmg4; 	//0x1104080f;
	__DMC_MIF[i]->UMCTL2_REGS_DRAMTMG5 = dramtmg5; 	//0x070b0c0c;
	__DMC_MIF[i]->UMCTL2_REGS_DRAMTMG6 = dramtmg6; 	//0x0e070007;
	__DMC_MIF[i]->UMCTL2_REGS_DRAMTMG7 = dramtmg7; 	//0x0000070b;
	__DMC_MIF[i]->UMCTL2_REGS_DRAMTMG8 = dramtmg8; 	//0x00000101;
	__DMC_MIF[i]->UMCTL2_REGS_DRAMTMG12 = dramtmg12; 	//0x00020000;
	__DMC_MIF[i]->UMCTL2_REGS_DRAMTMG13 = dramtmg13; 	//0x72100002;
	__DMC_MIF[i]->UMCTL2_REGS_DRAMTMG14 = dramtmg14; 	//0x00000588;

	// dis_auto_zq = 0 : generate ZQCS/MPC(ZQ calibration) commands based on ZQCTL1.t_zq_short_interval_x1024
	// dis_srx_zqcl = 0 : Enable/Disable issuing of ZQCL/MPC(ZQ calibration) command at Self-Refresh/SR-Powerdown exit
	// zq_resistor_shared = 0 : ZQ resistor is not shared between ranks.
	// t_zq_long_nop = 0x320 : tZQCAL for LPDDR4 (DFI clock) - set by freq
	// t_zq_short_nop = 0x18 : DFI clock cycles of NOP required after a ZQCS (ZQ calibration short)/MPC(ZQ Latch) command is issued - set by freq
	__DMC_MIF[i]->UMCTL2_REGS_ZQCTL0 = (0x0 | (t_zq_long_nop << T_ZQ_LONG_NOP_OFFSET) | (t_zq_short_nop << T_ZQ_SHORT_NOP_OFFSET)); // 0x03200018;

	// t_zq_reset_nop = 0x28 : (DFI clock) NOP after a ZQReset (ZQ calibration Reset) command is issued to SDRAM. - set by freq
	// t_zq_short_interval_x1024 = 0x70 : (1024 DFI clock) interval between ZQCS (ZQ calibration short)/MPC(ZQ calibration) - set by freq
	__DMC_MIF[i]->UMCTL2_REGS_ZQCTL1 = ((t_zq_short_interval_x1024 << T_ZQ_SHORT_INTERVAL_X1024_OFFSET) | (t_zq_reset_nop << T_ZQ_RESET_NOP_OFFSET)); // 0x02800070;

	// zq_reset = 0 : no need set at init
	__DMC_MIF[i]->UMCTL2_REGS_ZQCTL2 = 0x00000000;

	// DFI - (???)
	__DMC_MIF[i]->UMCTL2_REGS_DFITMG0 = dfitmg0; //0x049b820a;
	__DMC_MIF[i]->UMCTL2_REGS_DFITMG1 = dfitmg1; //0x00120400;
	__DMC_MIF[i]->UMCTL2_REGS_DFILPCFG0 = 0x07804130; //0x07708100;
	__DMC_MIF[i]->UMCTL2_REGS_DFIUPD0 = 0x40400018; //0x20400018;
	__DMC_MIF[i]->UMCTL2_REGS_DFIUPD1 = 0x00c800dc; //0x004b006e;

	// dfi_phyupd_en = DFI phy-initiated mode
	__DMC_MIF[i]->UMCTL2_REGS_DFIUPD2 = 0x80000000;

	// dfi_init_complete_en = 1 : PHY initialization complete enable signal. When asserted the dfi_init_complete signal can be used to trigger SDRAM initialisation
	__DMC_MIF[i]->UMCTL2_REGS_DFIMISC = 0x00000001;

	// DFI - (???)
	// dfi_tphy_rdcslat = 0x1b
	// dfi_tphy_wrcslat = 0xa
	__DMC_MIF[i]->UMCTL2_REGS_DFITMG2 = dfitmg2; //0x00001b0a;

	// rd_dbi_en = 1 : set same as MR3[6]
	// wr_dbi_en = 1 : set same as MR3[7]
	// dm_en = 0 : set opposite as MR13[5]
#ifdef ECC_ENABLED
#ifdef RD_DBI_OFF
	__DMC_MIF[i]->UMCTL2_REGS_DBICTL = 0x00000001;
#else
	__DMC_MIF[i]->UMCTL2_REGS_DBICTL = 0x00000007;
#endif
#else
	__DMC_MIF[i]->UMCTL2_REGS_DBICTL = 0x00000006;
#endif

	// dfi_phymstr_en : Enable PHY master i/f
	__DMC_MIF[i]->UMCTL2_REGS_DFIPHYMSTR = 0x00000001;

	// DRAM row/col/bank address mapping
	if (dmc_config.numRank == 2) {
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP0 = 0x00000007;	// cs bit is at bit[17]
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP1 = 0x00080808;	// bank
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP2 = 0x00000000;	// column
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP3 = 0x00000000;	// column
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP4 = 0x00001f1f;	// column_11,10
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP5 = 0x08080808;	// row
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP6 = 0x08080808;	// row

		//__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP9 = 0x0207020a;	// Not used
		//__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP10 = 0x000b0a07;	// Not used
		//__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP11 = 0x00000006;	// Not used
	} else {
		#ifdef ECC_ENABLED
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP0 = 0x0000001F;    // cs bit is not used. No rank interleave
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP1 = 0x00050505;    // bank (3 bits shifted)
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP2 = 0x00000000;    // column
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP3 = 0x13131300;    // column (upper 3 bits goes to MSB)
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP4 = 0x00001f1f;    // column_11,10
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP5 = 0x04040404;    // row (3 bits shifted)
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP6 = 0x04040404;    // row (3 bits shifted)
		#else
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP0 = 0x001f1f1f;	// cs bit is not used. No rank interleave
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP1 = 0x00080808;	// bank
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP2 = 0x00000000;	// column
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP3 = 0x00000000;	// column
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP4 = 0x00001f1f;	// column_11,10
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP5 = 0x07070707;	// row
		__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP6 = 0x07070707;	// row

		//__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP9 = 0x0207020a;	// Not used
		//__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP10 = 0x000b0a07;	// Not used
		//__DMC_MIF[i]->UMCTL2_REGS_ADDRMAP11 = 0x00000006;	// Not used
		#endif
	}

	// ODT (???)
	__DMC_MIF[i]->UMCTL2_REGS_ODTCFG = 0x0e1b0658; //0x031f0b3c;
	// Set as 0x0. No need to set
	__DMC_MIF[i]->UMCTL2_REGS_ODTMAP = 0x00000000;

	//SCHED - for PERF only
	__DMC_MIF[i]->UMCTL2_REGS_SCHED = 0x003b1304; //LPR entries = 20/32, force LPR, page-close with timer
	__DMC_MIF[i]->UMCTL2_REGS_SCHED1 = 0x00000010; //Page close timer
	__DMC_MIF[i]->UMCTL2_REGS_PERFHPR1 = 0x020000a0; // starve - 160, RL - 2
	__DMC_MIF[i]->UMCTL2_REGS_PERFLPR1 = 0x08000080; // starve - 128, RL - 8
	__DMC_MIF[i]->UMCTL2_REGS_PERFWR1 =  0x10000300; // starve - 768, RL - 16

	// FOR DEBUG only
	__DMC_MIF[i]->UMCTL2_REGS_DBG0 = 0x00000000;
	// dis_dq = 0 : transaction is possible
	__DMC_MIF[i]->UMCTL2_REGS_DBG1 = 0x00000000;
	// FOR DEBUG only
	__DMC_MIF[i]->UMCTL2_REGS_DBGCMD = 0x00000000;
	// sw_done = 1 : quasi-dynamic programming is done
	__DMC_MIF[i]->UMCTL2_REGS_SWCTL = 0x00000001;
	// AXI poison config
	__DMC_MIF[i]->UMCTL2_REGS_POISONCFG = 0x00000000;

	// PORT
	__DMC_MIF[i]->UMCTL2_MP_PCCFG = 0x00000010; // page-match limit enabled
	// rd_port_aging_en = 1 : aging function on why (???)
	// rd_port_priority = 0x1d4 : initial load value of read aging counters (need ???)
	__DMC_MIF[i]->UMCTL2_MP_PCFGR_0 = 0x00005010;
	// wr_port_pagematch_en = 1 (need ???)
	// wr_port_aging_en = 0 (need ???)
	// wr_port_priority = 0x106 : initial load value of write aging counters (need ???)
	__DMC_MIF[i]->UMCTL2_MP_PCFGW_0 = 0x00005020;
	// port_en = 1 : Enable AXI port 0
	__DMC_MIF[i]->UMCTL2_MP_PCTRL_0 = 0x00000001;

	__DMC_MIF[i]->UMCTL2_REGS_RFSHCTL3 = 0x00000000;
	__DMC_MIF[i]->UMCTL2_REGS_PWRCTL = 0x000000aa;
	__DMC_MIF[i]->UMCTL2_REGS_PWRCTL = 0x000000a2;		

	// QOS (???)
	__DMC_MIF[i]->UMCTL2_MP_PCFGQOS0_0  = 0x00020000;
	__DMC_MIF[i]->UMCTL2_MP_PCFGQOS1_0  = 0x056f0041;
	__DMC_MIF[i]->UMCTL2_MP_PCFGWQOS0_0 = 0x00100302;
	__DMC_MIF[i]->UMCTL2_MP_PCFGWQOS1_0 = 0x00000038;

	// SAR
	// use 4 regions and each is 256MB
	if (dmc_config.numRank == 2) {
		__DMC_MIF[i]->UMCTL2_MP_SARBASE0 = 0x00000001;	//bit[28]
		__DMC_MIF[i]->UMCTL2_MP_SARSIZE0 = 0x00000001;	// n=3, size=n+1, in unit 256MB, 
		__DMC_MIF[i]->UMCTL2_MP_SARBASE1 = 0x00000003;	// larger than sarbase0
		__DMC_MIF[i]->UMCTL2_MP_SARSIZE1 = 0x00000001;
		__DMC_MIF[i]->UMCTL2_MP_SARBASE2 = 0x00000005;
		__DMC_MIF[i]->UMCTL2_MP_SARSIZE2 = 0x00000001;
		__DMC_MIF[i]->UMCTL2_MP_SARBASE3 = 0x00000007;
		__DMC_MIF[i]->UMCTL2_MP_SARSIZE3 = 0x00000001;			
	} else {			
		__DMC_MIF[i]->UMCTL2_MP_SARBASE0 = 0x00000001;	//bit[28]
		__DMC_MIF[i]->UMCTL2_MP_SARSIZE0 = 0x00000000;	// n=3, size=n+1, in unit 256MB, 
		__DMC_MIF[i]->UMCTL2_MP_SARBASE1 = 0x00000002;	// larger than sarbase0
		__DMC_MIF[i]->UMCTL2_MP_SARSIZE1 = 0x00000000;
		__DMC_MIF[i]->UMCTL2_MP_SARBASE2 = 0x00000003;
		__DMC_MIF[i]->UMCTL2_MP_SARSIZE2 = 0x00000000;
		__DMC_MIF[i]->UMCTL2_MP_SARBASE3 = 0x00000004;
		__DMC_MIF[i]->UMCTL2_MP_SARSIZE3 = 0x00000000;
	}
}

void umctl2_init_nLPDDR4_Xm_set_default_MR_all_ch(void)
{
	int ch;

	for (ch = 0; ch < 4; ch++)
		umctl2_init_nLPDDR4_Xm_set_default_MR(ch);
	
	for (ch = 7; ch > 3; ch--)
		umctl2_init_nLPDDR4_Xm_set_default_MR(ch);
}

void umctl2_init_nLPDDR4_Xm_set_default_MR(int i)
{
	// mr_wr = 1 : to propagate the default value 3'b101 to the mrctrl logic
	// [14:12] = 3'b101 : the value for normal MR command
	// LPDDR4, we do not touch MRCTRL0 except for mr_wr
	__DMC_MIF[i]->UMCTL2_REGS_MRCTRL0 = 0x8000d030;
}

void umctl2_init2_nLPDDR4_Xm_all_ch(void)
{
	int ch;

	for(ch = 0; ch < 4; ch++)
		umctl2_init2_nLPDDR4_Xm(ch);

	for(ch = 7; ch > 3; ch--)
		umctl2_init2_nLPDDR4_Xm(ch);
}		

void umctl2_init2_nLPDDR4_Xm(int i)
{
//	unsigned int i;

//	for (i = 0; i < 8; i++) {
		// dis_dq = 0 : Clear dis_dq so now transaction is possible
		__DMC_MIF[i]->UMCTL2_REGS_DBG1 = 0x0;		
		// dis_cam_drain_selfref = 1 : skip CAM drain when enter SR
		// selfref_en = 0 : exit SDRAM from sefl-refresh		
		//__DMC_MIF[i]->UMCTL2_REGS_PWRCTL = 0x80;		
		//__DMC_MIF[i]->UMCTL2_REGS_PWRCTL = 0x80;
		// sw_done = 0 : '0' enables quasi-dynamic register programming
		__DMC_MIF[i]->UMCTL2_REGS_SWCTL = 0x0;
		// dfi_init_complete_en = 0 : dfi_init_complete is not 
		__DMC_MIF[i]->UMCTL2_REGS_DFIMISC = 0x0;
		__DMC_MIF[i]->UMCTL2_REGS_DFIMISC = 0x0;
//	}
}

void load_imem_1d_binary(int ch)
{
	int i;
	
	for(i=0;i<TRAIN_FW_IMEM_1D_WORD_SIZE;i++)
	{
		//apb_wr(__LPDDR4PHY_IMEM_BASE[ch] + (i*0x4), (uint32_t)train_fw_imem_1d_bin[i]);	
		writel(train_fw_imem_1d_bin[i], (unsigned long)(0x20000000 + ch*0x2000000 + 0x140000 + i*4)); 
	}
}

void load_imem_2d_binary(int ch)
{
	int i;
	
	for(i=0;i<TRAIN_FW_IMEM_2D_WORD_SIZE;i++)
	{
		//apb_wr(__LPDDR4PHY_IMEM_BASE[ch] + (i*0x4), (uint32_t)train_fw_imem_2d_bin[i]);	
		writel(train_fw_imem_2d_bin[i], (unsigned long)(0x20000000 + ch*0x2000000 + 0x140000 + i*4)); 
	}
}

void load_dmem_1d_binary(int ch)
{
	int i;

	for(i=0;i<TRAIN_FW_DMEM_1D_WORD_SIZE;i++)
	{
		//apb_wr(__LPDDR4PHY_DMEM_BASE[ch] + (i*0x4), (uint32_t)train_fw_dmem_1d_bin[i]);	
		writel(train_fw_dmem_1d_bin[i], (unsigned long)(0x20000000 + ch*0x2000000 + 0x150000 + i*4));
	}
}

void load_dmem_2d_binary(int ch)
{
	int i;

	for(i=0;i<TRAIN_FW_DMEM_2D_WORD_SIZE;i++)
	{
		//apb_wr(__LPDDR4PHY_DMEM_BASE[ch] + (i*0x4), (uint32_t)train_fw_dmem_2d_bin[i]);	
		writel(train_fw_dmem_2d_bin[i], (unsigned long)(0x20000000 + ch*0x2000000 + 0x150000 + i*4));
	}
}

void load_dmem_msg_block(int ch, unsigned int mif_init_freq, uint32_t train_2d)
{
	uint8_t    MsgMisc = 1;
	uint8_t    Pstate = 0;
	uint8_t    PllBypassEn = 0;
	uint16_t	DRAMFreq = (mif_init_freq << 1);
	uint8_t    DfiFreqRatio = 0x2;	// DFI ratio 1:2
	uint8_t    BPZNResVal = 0xf0;	// ZQ resistance, 0: 240ohm
	
	uint8_t    PhyOdtImpedance = 60;	// in ohms
	uint8_t    PhyDrvImpedance = 40;	// in ohms
	uint8_t    PhyVref = 22; // in VDDQ*(PhyVref/128), 22 = 17.18%
	// Added in A-2017.5 version
	uint8_t    Lp4Misc = 0;	// 0: recommended

      //    SequenceCtrl[0] = Run DevInit - Device/phy initialization. Should always be set.
      //    SequenceCtrl[1] = Run WrLvl - Write leveling
      //    SequenceCtrl[2] = Run RxEn - Read gate training
      //    SequenceCtrl[3] = Run RdDQS1D - 1d read dqs training
      //    SequenceCtrl[4] = Run WrDQ1D - 1d write dq training
      //    SequenceCtrl[5] = (2D only) Run rd2D - 2d read dqs training
      //    SequenceCtrl[6] = (2D only) Run wr2D - 2d write dq training
      //    SequenceCtrl[7] = RFU, must be zero
      //    SequenceCtrl[8] = Run RdDeskew - Per lane read dq deskew training
      //    SequenceCtrl[9] = Run MxRdLat - Max read latency training
      //    SequenceCtrl[11-10] = RFU, must be zero
      //    SequenceCtrl[12]      = Run LPCA - CA Training
      //    SequenceCtrl[15-13] = RFU, must be zero	
	uint16_t    SequenceCtrl;

      //    0x05 = Detailed debug messages (e.g. Eye delays)
      //    0x0A = Coarse debug messages (e.g. rank information)
      //    0xC8 = Stage completion
      //    0xC9 = Assertion messages
      //    0xFF = Firmware completion messages only	  
	uint8_t    HdtCtrl;
	#ifdef TRAINING_LOG
	HdtCtrl = 0x04;
	#else
	HdtCtrl = 0xFF;
	#endif	  

	// Margin added to smallest passing trained DFI Max Read Latency value, in units of DFI clocks. Recommended to be >= 1. See the Training App Note for more details on the training process and the use of this value.
	// This margin must include the maximum positive drift expected in tDQSCK over the target temperature and voltage range of the users system.	  
	uint8_t    DFIMRLMargin = 2;	// 1 or 2 is good

	// Training firmware can optionally set per rank mode register values for DRAM partial array self-refresh features if desired.
	// 
	//    0x0 = Use MR<0:17>_A0 for rank 0 channel A
	//              Use MR<0:17>_B0 for rank 0 channel B
	//              Use MR<0:17>_A1 for rank 1 channel A
	//              Use MR<0:17>_B1 for rank 1 channel B
	// 
	//    0x1 = Use MR<0:17>_A0 setting for all channels/ranks
	// 
	// It is recommended in most LPDDR4 system configurations to set this to 1.
	// 
	// Note: When set to 0, only mode registers associated with Vref CA, Vref DQ, and DRAM partial array self-refresh may differ between ranks and channels.
	// 	
	uint8_t    UseBroadcastMR = 1;
	// Added in A-2017.5 version
	uint8_t    Lp4Quickboot = 0;	// 0: recommended
	uint8_t    CATrainOpt = 1;	// CA Vref Training
	uint8_t    X8Mode = 0;	// x16 mode

	// Added in A-2017.5 version
	uint16_t    PhyConfigOverride = 0;	// 0: recommended

	// Total number of DQ bits enabled in PHY Channel A	
	uint8_t    EnabledDQsChA = 16;
	// Indicates presence of DRAM at each chip select for PHY channel A.
	// 
	//  0x1 = CS0 is populated with DRAM
	//  0x3 = CS0 and CS1 are populated with DRAM
	// 
	// All other encodings are illegal
	uint8_t    CsPresentChA; // = 1;
	
	uint8_t    MR1;
	uint8_t    MR2;
	uint8_t    MR3;	
	uint8_t    MR4 = 0x00;	// Temperature, thermal offset = default = 0
	uint8_t    MR11;	
	uint8_t    MR12;
	uint8_t    MR13;		
	uint8_t    MR14;
	uint8_t    MR16 = 0x00;	// PASR bank
	uint8_t    MR17 = 0x00;	// PASR segment
	uint8_t    MR22;	
	uint8_t    MR24 = 0x00;	//TRR
	
	uint8_t    CATerminatingRankChA = 0x0;	 //Usually Rank0 is terminating rank

	// Bit fields, if 2D read training enabled, then use these additional options:
	// [0] DFE
	//      1 = Run rx2D with DFE
	//      0 = Run rx2D with DFE off
	// [1-2] Voltage Step Size (2^n)
	//      3 = 8 DAC settings between checked values
	//      2 = 4 DAC settings between checked values
	//      1 = 2 DAC settings between checked values
	//      0 = 1 DAC settings between checked values
	// [3-4] Delay Step Size (2^n)
	//      3 = 8 LCDL delays between checked values
	//      2 = 4 LCDL delays between checked values
	//      1 = 2 LCDL delays between checked values
	//      0 = 1 LCDL delays between checked values
	// [5-7] RFU, must be zero	
	uint8_t    RX2D_TrainOpt = 0;
	uint8_t    TX2D_TrainOpt = 0;
#ifdef DFE_ON
    RX2D_TrainOpt = 1;
#endif

	// Bitmap that designates the phy's vref source for every pstate
	//      If Share2DVrefResult[x] = 0, then pstate x will use the device voltage passed in MR6 as a global phy vref setting.
	//      If Share2DVrefResult[x] = 1, then pstate x will use the per-lane vref values found by the last 2D training run.	
	uint8_t    Share2DVrefResult = 1;
	// During 2D training, the ideal eye center changes depending on how valuable delay margin is compared to voltage margin. delay_weight2D sets the value, or weight, of one step of delay margin. The ratio of voltage_weight2D to delay_weight2D will be used by 2D training to choose your preferred center point. There are 32 delay steps in a perfect eye.
	uint8_t    Delay_Weight2D = 255; 
	// During 2D training, the ideal eye center changes depending on how valuable voltage margin is compared to delay margin. voltage_weight2D sets the value, or weight, of one step of voltage margin. The ratio of voltage_weight2D to delay_weight2D will be used by 2D training to choose your preferred center point. There are 128 voltage steps in a perfect eye. 	
	uint8_t    Voltage_Weight2D = 1; 

	uint8_t    MR13_DMD;

	#ifdef ECC_ENABLED
	MR13_DMD = (0x0<<5);
	#else
	MR13_DMD = (0x1<<5);
	#endif

#ifdef RD_DBI_OFF
	MR3 = 0x31;
#else
	MR3 = 0xf1;
#endif
	MR13 = 0x08|MR13_DMD;
	MR11 = 0x64; MR12 = 0x4d; MR22 = 0x0E; MR14 = 0x4d;

	if (mif_init_freq > 1866) {
		MR1 = 0x74;	MR2 = 0x3f;
		MR11 = 0x66;	MR14 = 0x22;
		PhyDrvImpedance = 40;	// in ohms //DS/ODT = 40/40
	} else if (mif_init_freq > 1600) {
		MR1 = 0x64;	MR2 = 0x36|0x40;
	} else if (mif_init_freq > 1333) {
		MR1 = 0x54;	MR2 = 0x2d|0x40;
	} else if (mif_init_freq > 1066) {
		MR1 = 0x44;	MR2 = 0x24|0x40;
	} else if (mif_init_freq > 800) {
		MR1 = 0x34;	MR2 = 0x1b|0x40;
	} else if (mif_init_freq > 533) {
		MR1 = 0x24;	MR2 = 0x12|0x40;
	} else if (mif_init_freq > 266) {
		MR1 = 0x14;	MR2 = 0x09|0x40;
		MR14 = 0x4a;    /* WR VREF = 26% */
		PhyVref = 26;		/* RD VREF = 20.31% */
	} else {
		MR1 = 0x04;	MR2 = 0x00|0x40;
	}

	if (train_2d == 0) {
		SequenceCtrl = (0x0|(0x1<<0)|(0x1<<1)|(0x1<<2)|(0x1<<3)|(0x1<<4)|(0x1<<8)|(0x1<<9)|(0x1<<12));		// (DevInit | WrLvl | RxEn) is mandatory for 1D
	} else {
		SequenceCtrl = (0x0 | DevInit | rd2D | wr2D);
	}

	if (dmc_config.numRank == 2) {
		CsPresentChA = 0x3;
	} else {
		CsPresentChA = 0x1;	
	}

	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x0 = (MsgMisc << MSGMISC_OFFSET);		// | (0x20<<0);
	
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x8 = (Pstate << PSTATE_OFFSET) | (PllBypassEn << PLLBYPASSEN_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0xC = (DRAMFreq << DRAMFREQ_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x10 = (DfiFreqRatio << DFIFREQRATIO_OFFSET) | (BPZNResVal << BPZNRESVAL_OFFSET);
	
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x14 = (PhyOdtImpedance << PHYODTIMPEDANCE_OFFSET) | (PhyDrvImpedance << PHYDRVIMPEDANCE_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x18 = (PhyVref << PHYVREF_OFFSET) | (Lp4Misc << LP4MISC_OFFSET);
	
	if (train_2d != 0)
		__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x1C = (0x13 << 0);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x20 = (SequenceCtrl << SEQUENCECTRL_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x24 = (HdtCtrl << HDTCTRL_OFFSET);

	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x2C = (DFIMRLMargin << DFIMRLMARGIN_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x30 = (UseBroadcastMR << USEBROADCASTMR_OFFSET) | (Lp4Quickboot << LP4QUICKBOOT_OFFSET);

	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x34 = (CATrainOpt << CATRAINOPT_OFFSET);

	if (train_2d == 0) {
		__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x38 = (X8Mode << X8MODE_OFFSET);
		if (redo_initial_training != 0)
			__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x3C = (Share2DVrefResult << SHARE2DVREFRESULT_OFFSET);
	} else {
		__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x38 = (X8Mode << X8MODE_OFFSET) | (RX2D_TrainOpt << RX2D_TRAINOPT_OFFSET);
		__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x3C = (TX2D_TrainOpt << TX2D_TRAINOPT_OFFSET) | (Share2DVrefResult << SHARE2DVREFRESULT_OFFSET);
		__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x40 = (Delay_Weight2D << DELAY_WEIGHT2D_OFFSET) | (Voltage_Weight2D << VOLTAGE_WEIGHT2D_OFFSET);
	}
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x44 = (PhyConfigOverride << PHYCONFIGOVERRIDE_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x48 = (EnabledDQsChA << ENABLEDDQSCHA_OFFSET) | (CsPresentChA << CSPRESENTCHA_OFFSET);

	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x64 = (MR1 << MR1_A0_OFFSET) | (MR2 << MR2_A0_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x68 = (MR3 << MR3_A0_OFFSET) | (MR4 << MR4_A0_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x6C = (MR11 << MR11_A0_OFFSET) | (MR12 << MR12_A0_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x70 = (MR13 << MR13_A0_OFFSET) | (MR14 << MR14_A0_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x74 = (MR16 << MR16_A0_OFFSET) | (MR17 << MR17_A0_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x78 = (MR22 << MR22_A0_OFFSET) | (MR24 << MR24_A0_OFFSET);

	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x7C = (MR1 << MR1_A1_OFFSET) | (MR2 << MR2_A1_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x80 = (MR3 << MR3_A1_OFFSET) | (MR4 << MR4_A1_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x84 = (MR11 << MR11_A1_OFFSET) | (MR12 << MR12_A1_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x88 = (MR13 << MR13_A1_OFFSET) | (MR14 << MR14_A1_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x8C = (MR16 << MR16_A1_OFFSET) | (MR17 << MR17_A1_OFFSET);
	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x90 = (MR22 << MR22_A1_OFFSET) | (MR24 << MR24_A1_OFFSET);

	__LPDDR4PHY_TRAINING_FIRMWARE_DMEM_t[ch]->DMEM_0x94 = CATerminatingRankChA	 << CATERMINATINGRANKCHA_OFFSET;
}

uint32_t get_mail_training_firmware (int ch, uint32_t mode)
{
	uint32_t mail;
	
	// Wait UctWriteProtShadow to be 0
	while ((__LPDDR4PHY_MIF_REGS_t7[ch]->DWC_DDRPHYA_APBONLY0_UctShadowRegs & 0x1) != 0);
	// Read out the mailbox
	mail = __LPDDR4PHY_MIF_REGS_t7[ch]->DWC_DDRPHYA_APBONLY0_UctWriteOnlyShadow;
	// if necessary, read out the upper word
	if (mode == MSG_32BIT_MODE) { 
		mail |= (__LPDDR4PHY_MIF_REGS_t7[ch]->DWC_DDRPHYA_APBONLY0_UctDatWriteOnlyShadow << 16);
	}
	// ACK to PMU
	__LPDDR4PHY_MIF_REGS_t7[ch]->DWC_DDRPHYA_APBONLY0_DctWriteProt = 0;
	// Wait PMU's receipt to ACK
	while ((__LPDDR4PHY_MIF_REGS_t7[ch]->DWC_DDRPHYA_APBONLY0_UctShadowRegs & 0x1) != 0x1);
	// Complete the mailbox protocol
	__LPDDR4PHY_MIF_REGS_t7[ch]->DWC_DDRPHYA_APBONLY0_DctWriteProt = 1;

	return mail;
}

uint32_t get_mail_training_firmware_2d (int ch, uint32_t mode)
{
	uint32_t mail;
	
	// Wait UctWriteProtShadow to be 0
	while ((__LPDDR4PHY_MIF_REGS_t7[ch]->DWC_DDRPHYA_APBONLY0_UctShadowRegs & 0x1) != 0);
	// Read out the mailbox
	mail = __LPDDR4PHY_MIF_REGS_t7[ch]->DWC_DDRPHYA_APBONLY0_UctWriteOnlyShadow;
	// if necessary, read out the upper word
	if (mode == MSG_32BIT_MODE) { 
		mail |= (__LPDDR4PHY_MIF_REGS_t7[ch]->DWC_DDRPHYA_APBONLY0_UctDatWriteOnlyShadow << 16);
	}
	// ACK to PMU
	__LPDDR4PHY_MIF_REGS_t7[ch]->DWC_DDRPHYA_APBONLY0_DctWriteProt = 0;
	// Wait PMU's receipt to ACK
	while ((__LPDDR4PHY_MIF_REGS_t7[ch]->DWC_DDRPHYA_APBONLY0_UctShadowRegs & 0x1) != 0x1);
	// Complete the mailbox protocol
	__LPDDR4PHY_MIF_REGS_t7[ch]->DWC_DDRPHYA_APBONLY0_DctWriteProt = 1;	

	return mail;
}

void decode_streaming_msg(int ch) 
{
	uint32_t i = 0;
	uint32_t string_index;
	uint32_t msg_32b;
	
	string_index = get_mail_training_firmware(ch, MSG_32BIT_MODE);
	//debug_string = lookup_string_index(string_index)
	//dmc_printf(string_index);
	#ifdef TRAINING_LOG
	printk(BIOS_INFO,"string_index=0x%x\n", string_index);
	#endif	
	
	//	The lower 16 bits of the string index are particularly useful as they say exactly how many additional 32-bit
	//	pieces of mail need to be read before the streaming message can be fully formatted.		
	while (i < (string_index & 0xffff)) {
		msg_32b = get_mail_training_firmware(ch, MSG_32BIT_MODE);
		#ifdef TRAINING_LOG
		printk(BIOS_INFO,"0x%x\n", msg_32b);
		#endif
		dmc_printf(msg_32b);
		i++;
	}
	//printf(debug_string, arg[0], arg[1], <A1><A6> , arg[i-1]);	
}

void decode_major_msg(uint32_t mail) 
{
	if (mail == TRAINING_HAS_FAILED) ; //asm("b ."); 
	if (mail == END_OF_INITIALIZATION) ;
	if (mail == END_OF_FINE_WRITE_LEVELING) ;
	if (mail == END_OF_READ_ENABLE) ;
	if (mail == END_OF_READ_DELAY) ;              
	if (mail == END_OF_WRITE_DELAY) ;             
	if (mail == END_OF_2D_READ_DELAY) ;
	if (mail == END_OF_2D_WRITE_DELAY) ;          
	if (mail == END_OF_MAX_READ_LATENCY) ;        
	if (mail == END_OF_READ_DQ_DESKEW) ;          
	if (mail == END_OF_LRDIMM_SPECIFIC_TRAINING) ;
	if (mail == END_OF_CA_TRAIN) ;
	if (mail == END_OF_MPR_READ_DELAY) ;
	if (mail == END_OF_WRITE_LEVELING_COARSE) ;
	if (mail == TRAINING_HAS_RUN_SUCCESSFULLY) ;
	if (mail == START_STREAMING_MESSAGE_MODE) ;
}

void pub_init_nLPDDR4_Xm_all_ch(unsigned int mif_init_freq)
{
	int ch;

	for(ch = 0; ch < 4; ch++)
		pub_init_nLPDDR4_Xm(mif_init_freq, ch);

	for(ch = 7; ch > 3; ch--)
		pub_init_nLPDDR4_Xm(mif_init_freq, ch);
}

void pub_init_nLPDDR4_Xm(unsigned int mif_init_freq, int i)
{
	uint32_t TxPreP, TxPreN, PreDrv;	
	uint32_t TxSlewRate;
	uint32_t ATxSlewRate;	
	uint32_t PllFreqSel;
	uint32_t ARdPtrInitVal;
	uint32_t ProcOdtTimeCtl;
	uint32_t CalUClkTicksPer1uS;
//	uint32_t DllGainIV, DllGainTV;
//	uint32_t Seq0BDLY0, Seq0BDLY1, Seq0BDLY2, Seq0BDLY3, dllLock;
	uint32_t DrvStrenFSDqP, DrvStrenFSDqN;
	uint32_t ADrvStrenP, ADrvStrenN;
	uint32_t GlobalVrefInDAC;

	// 1. TxSlewRate pstate / bytes / lane
	// TxPreDrvMode = 0x1;	// LPDDR4
	// TxPreP = 0xf
	// TxPreN = 0xf
	// TxSlewRate[pstate] = (TxPreDrvMode << csr_TxPreDrvMode_LSB) | (TxPreN[pstate] << csr_TxPreN_LSB) | (TxPreP[pstate] << csr_TxPreP_LSB);
	PreDrv = 1;
	TxPreN = 0xf;
	TxPreP = 0xf;
	TxSlewRate = (0x0 | (PreDrv << 8) | (TxPreN << 4) | (TxPreP << 0));
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_TxSlewRate_b0_p0 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_TxSlewRate_b1_p0 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_TxSlewRate_b0_p0 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_TxSlewRate_b1_p0 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE0_TxSlewRate_b0_p1 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE0_TxSlewRate_b1_p1 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE1_TxSlewRate_b0_p1 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE1_TxSlewRate_b1_p1 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE0_TxSlewRate_b0_p2 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE0_TxSlewRate_b1_p2 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE1_TxSlewRate_b0_p2 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE1_TxSlewRate_b1_p2 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE0_TxSlewRate_b0_p3 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE0_TxSlewRate_b1_p3 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE1_TxSlewRate_b0_p3 = TxSlewRate;
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE1_TxSlewRate_b1_p3 = TxSlewRate;

	// ATxSlewRate  anib
	// ATxPreDrvMode = 0x1;	
	// TxSlewRiseAC = 0xf
	// TxSlewFallAC = 0xf		
	PreDrv = 1;
	TxPreN = 0xf;
	TxPreP = 0xf;		
	ATxSlewRate = (0x0 | (PreDrv << 8) | (TxPreN << 4) | (TxPreP << 0));		
	__LPDDR4PHY_MIF_REGS_t0[i]->DWC_DDRPHYA_ANIB0_ATxSlewRate = ATxSlewRate;
	__LPDDR4PHY_MIF_REGS_t0[i]->DWC_DDRPHYA_ANIB1_ATxSlewRate = ATxSlewRate;
	__LPDDR4PHY_MIF_REGS_t0[i]->DWC_DDRPHYA_ANIB2_ATxSlewRate = ATxSlewRate;

	if ((mif_init_freq >> 1) < 235) { 
		PllFreqSel = 0x7; 
	} else if((mif_init_freq >> 1) < 313) { 
		PllFreqSel = 0x6;
	} else if((mif_init_freq >> 1) < 469) { 
		PllFreqSel = 0xb; 
	} else if((mif_init_freq >> 1) < 625) { 
		PllFreqSel = 0xa; 
	} else if((mif_init_freq >> 1) < 938) { 
		PllFreqSel = 0x19; 
	} else if((mif_init_freq >> 1) < 1067) { 
		PllFreqSel = 0x18; 
	} else { 
		PllFreqSel = 0x18;
	}		
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_PllCtrl2_p0 = PllFreqSel;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_PllCtrl2_p1 = PllFreqSel;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_PllCtrl2_p2 = PllFreqSel;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_PllCtrl2_p3 = PllFreqSel;

	if (mif_init_freq >= 933) {
		ARdPtrInitVal = 0x2;
	} else {
		ARdPtrInitVal = 0x1;
	}		
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p0 = ARdPtrInitVal;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p1 = ARdPtrInitVal;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p2 = ARdPtrInitVal;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p3 = ARdPtrInitVal;

	// ODT always on control
	// PIE will be used to write seq0bGPR4 value in ProcOdtCtl
	__LPDDR4PHY_MIF_REGS_t5[i]->DWC_DDRPHYA_INITENG0_Seq0BGPR4_p0 = 0x00000000;
	__LPDDR4PHY_MIF_REGS_t12[i]->DWC_DDRPHYA_INITENG0_Seq0BGPR4_p1 = 0x00000000;
	__LPDDR4PHY_MIF_REGS_t17[i]->DWC_DDRPHYA_INITENG0_Seq0BGPR4_p2 = 0x00000000;
	__LPDDR4PHY_MIF_REGS_t22[i]->DWC_DDRPHYA_INITENG0_Seq0BGPR4_p3 = 0x00000000;
	
	// WR : Pre-amble 2tCK, Post-ample 0.5tCK
	// RD : Pre-ample Static, Post-ample 0.5tCK
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DqsPreambleControl_p0 = 0x000000ab;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DbyteDllModeCntrl = 0x00000000;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_DqsPreambleControl_p1 = 0x000000ab;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DbyteDllModeCntrl = 0x00000000;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_DqsPreambleControl_p2 = 0x000000ab;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DbyteDllModeCntrl = 0x00000000;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_DqsPreambleControl_p3 = 0x000000ab;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DbyteDllModeCntrl = 0x00000000;

	if (mif_init_freq <= 933) {
		ProcOdtTimeCtl = 0xa;
	} else if (mif_init_freq <= 1200) {
		ProcOdtTimeCtl = 0x2;
	} else {
		ProcOdtTimeCtl = 0x3;
	}		
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p0 = ProcOdtTimeCtl;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p1 = ProcOdtTimeCtl;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p2 = ProcOdtTimeCtl;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p3 = ProcOdtTimeCtl;

	// ODT PU = 0 (high-Z)
	// ODT PD = 60ohm
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b0_p0 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b1_p0 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b0_p0 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b1_p0 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b0_p1 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b1_p1 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b0_p1 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b1_p1 = 0x00000600;		
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b0_p2 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b1_p2 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b0_p2 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b1_p2 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b0_p3 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b1_p3 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b0_p3 = 0x00000600;
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b1_p3 = 0x00000600;

	// DrvStren PU = 60ohm
	// DrvStren PD = 60ohm		
	DrvStrenFSDqP = 0x38;
	DrvStrenFSDqN = 0x38;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b0_p0 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b1_p0 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b0_p0 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b1_p0 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b0_p1 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b1_p1 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b0_p1 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b1_p1 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b0_p2 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b1_p2 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b0_p2 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b1_p2 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b0_p3 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b1_p3 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b0_p3 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b1_p3 = (DrvStrenFSDqN << 6)|(DrvStrenFSDqP << 0);

	// ATxImpedance
	//  ADrvStrenP    = dwc_ddrphy_phyinit_mapDrvStren ((float) userInputAdvanced.ATxImpedance, "ADrvStrenP" , userInputBasic.HardMacroVer); 
	//  ADrvStrenN    = dwc_ddrphy_phyinit_mapDrvStren ((float) userInputAdvanced.ATxImpedance, "ADrvStrenN" , userInputBasic.HardMacroVer);
	//  TxImpedance  = (ADrvStrenN << csr_ADrvStrenN_LSB) | (ADrvStrenP << csr_ADrvStrenP_LSB);	
	ADrvStrenP = 0x3;
	ADrvStrenN = 0x3;
	__LPDDR4PHY_MIF_REGS_t0[i]->DWC_DDRPHYA_ANIB0_ATxImpedance = (ADrvStrenN << 5)|(ADrvStrenP << 0); //(0x0 | (0x1 << 5) | (0x1 << 0));
	__LPDDR4PHY_MIF_REGS_t0[i]->DWC_DDRPHYA_ANIB1_ATxImpedance = (ADrvStrenN << 5)|(ADrvStrenP << 0); //(0x0 | (0x1 << 5) | (0x1 << 0)); 
	__LPDDR4PHY_MIF_REGS_t0[i]->DWC_DDRPHYA_ANIB2_ATxImpedance = (ADrvStrenN << 5)|(ADrvStrenP << 0); //(0x0 | (0x1 << 5) | (0x1 << 0));

	// 1 :  DFI1 does not physically exists
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DfiMode = 0x00000001;		
	// 4 : DFI for LPDDR4		
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DfiCAMode = 0x00000004;
	// 0 : ZQ resistor = 240ohm
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_CalDrvStr0 = 0x00000000;

	// DFI clk
	CalUClkTicksPer1uS = (mif_init_freq >> 1);
	if ((mif_init_freq & 0x1) != 0) {	// ceiling
		CalUClkTicksPer1uS++;
	}
	if (CalUClkTicksPer1uS < 24) {
		CalUClkTicksPer1uS = 24;
	}
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_CalUclkInfo_p0 = CalUClkTicksPer1uS;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_CalUclkInfo_p1 = CalUClkTicksPer1uS;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_CalUclkInfo_p2 = CalUClkTicksPer1uS;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_CalUclkInfo_p3 = CalUClkTicksPer1uS;

	// the interval between successive calibrations
	// CalOnce = 0
	// CalInterval = 9 : 20ms
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_CalRate = 0x00000009;		
#ifdef PERIODIC_ZQCAL_PHY
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_CalRate = 0x00000019;
#endif

	// 0x204 = VREF is 0.315 * VDDQ and LPDDR4
	// SetAnalogVref = 1 : 1 will force the local per-bit VREF generator to pass the global VREFA to the samplers.
	GlobalVrefInDAC = 32;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_VrefInGlobal_p0 = (GlobalVrefInDAC << 3)|0x00000004;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b0_p0 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b1_p0 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b0_p0 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b1_p0 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_VrefInGlobal_p1 = (GlobalVrefInDAC << 3)|0x00000004;		
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b0_p1 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b1_p1 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b0_p1 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t9[i]->DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b1_p1 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_VrefInGlobal_p2 = (GlobalVrefInDAC << 3)|0x00000004;
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b0_p2 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b1_p2 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b0_p2 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t14[i]->DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b1_p2 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_VrefInGlobal_p3 = (GlobalVrefInDAC << 3)|0x00000004;
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b0_p3 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b1_p3 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b0_p3 = 0x000005a1;
	__LPDDR4PHY_MIF_REGS_t19[i]->DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b1_p3 = 0x000005a1;

	// DfiFreqRatio = 1 : DFI ratio is 1:2
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DfiFreqRatio_p0 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_DfiFreqRatio_p1 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_DfiFreqRatio_p2 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_DfiFreqRatio_p3 = 0x00000001;

	// DisDynAdrTri = 1 : Dynamic Tristating is disabled for LPDDR4
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_TristateModeCA_p0 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_TristateModeCA_p1 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_TristateModeCA_p2 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_TristateModeCA_p3 = 0x00000001;

	// Just following the ref code, not for user's configuration
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DfiFreqXlat0 = 0x00000000;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DfiFreqXlat1 = 0x00000000;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DfiFreqXlat2 = 0x00004444;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DfiFreqXlat3 = 0x00008888;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DfiFreqXlat4 = 0x00005555;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DfiFreqXlat5 = 0x00000000;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DfiFreqXlat6 = 0x00000000;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DfiFreqXlat7 = 0x0000f000;

	// 11. DbyteMiscMode
	//    uint16_t regData;
	//    regData = 0x1 << csr_DByteDisable_LSB;
	//    unsigned int regData1; 
	//    regData1 = (0x1ff << csr_PowerDownRcvr_LSB | 0x1 << csr_PowerDownRcvrDqs_LSB | 0x1 << csr_RxPadStandbyEn_LSB) ;
	//    // Implements Section 1.3 of Pub Databook
	//     dwc_ddrphy_phyinit_userCustom_io_write16((c_addr | tDBYTE | csr_DbyteMiscMode_ADDR), regData);
	//     dwc_ddrphy_phyinit_userCustom_io_write16((c_addr | tDBYTE | csr_DqDqsRcvCntrl1_ADDR), regData1);
	//__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_DbyteMiscMode = (0x0 | (1 << 1));
	//__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl1 = (0x0 | (1 << 10) | (1 << 9) | (0x1ff));
	//__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_DbyteMiscMode = (0x0 | (1 << 1));
	//__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl1 = (0x0 | (1 << 10) | (1 << 9) | (0x1ff)); 		

	// X4TG = 0 : Not x4
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_MasterX4Config = 0x00000000;

	// RdDbiEnabled = 1 : DRAM's RD-DBI is ON
#ifdef RD_DBI_OFF
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DMIPinPresent_p0 = 0x00000000;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_DMIPinPresent_p1 = 0x00000000;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_DMIPinPresent_p2 = 0x00000000;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_DMIPinPresent_p3 = 0x00000000;
#else
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DMIPinPresent_p0 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_DMIPinPresent_p1 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_DMIPinPresent_p2 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_DMIPinPresent_p3 = 0x00000001;
#endif

	// SNPS WA for the DLL lock failure at the lower voltage
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DllLockParam_p0 = 0x00000002;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_DllLockParam_p1 = 0x00000002;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_DllLockParam_p2 = 0x00000002;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_DllLockParam_p3 = 0x00000002;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DllGainCtl_p0 = 0x00000055;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_DllGainCtl_p1 = 0x00000055;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_DllGainCtl_p2 = 0x00000055;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_DllGainCtl_p3 = 0x00000055;
#ifdef REDUCED_PHY_DFI_UPDATE
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DFIPHYUPD &= ~(0x0000000F);
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DFIPHYUPD |= (0x00000001);	// 16k
#endif
}

void pub_1d_training_all_ch(unsigned int mif_init_freq)
{
	int ch;
	int stage;

	for (stage = 0; stage < 2; stage++) {
		for(ch = 0; ch < 4; ch++)
			pub_1d_training(stage, mif_init_freq, ch);

		for(ch = 7; ch > 3; ch--)
			pub_1d_training(stage, mif_init_freq, ch);
	}
}

void pub_1d_training(int stage, unsigned int mif_init_freq, int i)
{
	uint32_t mail;

	switch (stage) {
	case 0:
		// load the fw
		__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroContMuxSel = 0x00000000;
		load_imem_1d_binary(i);
		__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroContMuxSel = 0x00000001;

		__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroContMuxSel = 0x00000000;
		load_dmem_1d_binary(i);
		load_dmem_msg_block(i, mif_init_freq, 0);
		// The following two SFR are set by Anwar
		//__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_TxCalPOvr = 0x2f;
		//__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_TxCalNOvr = 0x2f;
		__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroContMuxSel = 0x00000001;

		// execute the fw
		__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroReset = 9;	// ResetToMicro | StallToMicro
		__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroReset = 1;	// StallToMicro
		__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroReset = 0;	// - (Clear all)
		break;

	case 1:
		// read msg block
		do {
			mail = get_mail_training_firmware(i, MSG_16BIT_MODE);
			decode_major_msg(mail);
			// printk(BIOS_INFO,"train result = 0x%x\n", mail);
#ifdef TRAINING_LOG
			printk(BIOS_INFO,"mail=0x%x\n", mail);
#else
			printk(BIOS_INFO,"ch%d train result = 0x%x\n", i, mail);
#endif
			if (mail == START_STREAMING_MESSAGE_MODE) {
				decode_streaming_msg(i);
			}
		} while ((mail != TRAINING_HAS_RUN_SUCCESSFULLY) && (mail != TRAINING_HAS_FAILED));

		__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroReset = 1;	// StallToMicro - to halt the uCtrl after the training
		break;
	}
}

static uint32_t pub_2d_training(int stage, unsigned int mif_init_freq, int ch)
{
	uint32_t status = 0;

	switch (stage) {
	case 0:
		/* load */
		pub_2d_training_load(mif_init_freq, ch);

		/* run */
		pub_2d_training_run(ch);

		break;

	case 1:
		/* handshake */
		status = pub_2d_training_handshake(ch);
		break;
	}

	return status;
}


void pub_2d_training_all_ch(unsigned int mif_init_freq)
{
	int ch;
	int stage;
	char message[64];
	uint32_t status_0 = 0, status_1 = 0;

	for (stage = 0; stage < 2; stage++) {
		for (ch = 0; ch < 4; ch++)
			status_0 |= (pub_2d_training(stage, mif_init_freq, ch) & 0xff) << (ch * 8);

		for (ch = 7; ch > 3; ch--)
			status_1 |= (pub_2d_training(stage, mif_init_freq, ch) & 0xff) << ((ch - 4) * 8);
	}

	/* can not go on, issue cold reset */
	if (status_0 != 0x07070707 || status_1 != 0x07070707) {
		snprintf(message, 64, "dram training failed: %08x%08x", status_1, status_0);
		slog_log_message(message, 38);
		sms_reset(RESET_MODE_COLD);
	}
}

void pub_2d_training_load(unsigned int mif_init_freq, int i)
{
	// load the fw		
	__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroContMuxSel = 0x00000000;
	load_imem_2d_binary(i);
	__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroContMuxSel = 0x00000001;

	__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroContMuxSel = 0x00000000;				
	load_dmem_2d_binary(i);
	load_dmem_msg_block(i, mif_init_freq, 1);
	__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroContMuxSel = 0x00000001;					
}

void pub_2d_training_run(int i)
{
	// execute the fw
	__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroReset = 9;	// ResetToMicro | StallToMicro
	__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroReset = 1;	// StallToMicro
	__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroReset = 0;	// - (Clear all)				
}

uint32_t pub_2d_training_handshake(int i)
{
	uint32_t mail;
	
	// read msg block
	do {
		mail = get_mail_training_firmware_2d(i, MSG_16BIT_MODE);
		decode_major_msg(mail);
		//printk(BIOS_INFO,"2D train result = 0x%x\n", mail);
		#ifdef TRAINING_LOG
		printk(BIOS_INFO,"mail=0x%x\n", mail);
		#else
		printk(BIOS_INFO,"ch%d 2D train result = 0x%x\n", i, mail);
		#endif
		if (mail == START_STREAMING_MESSAGE_MODE) {
			decode_streaming_msg(i);
		}
	} while ((mail != TRAINING_HAS_RUN_SUCCESSFULLY) && (mail != TRAINING_HAS_FAILED));

	__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroReset = 1;	// StallToMicro - to halt the uCtrl after the training

	return mail;
}

void pub_init2_nLPDDR4_Xm_all_ch(unsigned int mif_init_freq)
{
	int ch;

	for(ch = 0; ch < 4; ch++)
		pub_init2_nLPDDR4_Xm(mif_init_freq, ch);

	for(ch = 7; ch > 3; ch--)
		pub_init2_nLPDDR4_Xm(mif_init_freq, ch);
}

void pub_init2_nLPDDR4_Xm(unsigned int mif_init_freq, int i)
{
	uint32_t Seq0BDLY0, Seq0BDLY1, Seq0BDLY2, Seq0BDLY3, dllLock;

	// PIE programming
	__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroContMuxSel = 0x00000000;

	load_pie_binary(i);

	// This is moved from C-phy init at A-2017.07
	// Seq0BDLY0 = 0.5us
	// Seq0BDLY1 = 1.0us
	// Seq0BDLY2 = 10.0us
	// Seq0BDLY3 = by equation
	Seq0BDLY0 = (mif_init_freq >> 4);
	Seq0BDLY1 = (mif_init_freq >> 3);
	Seq0BDLY2 = 10 * (mif_init_freq >> 3);
	if ((mif_init_freq % 8) != 0) {
		Seq0BDLY0++;
		Seq0BDLY1++;
		Seq0BDLY2++;
	}		

	if (mif_init_freq > 533) {
		dllLock = 176;
	} else if (mif_init_freq > 400) {
		dllLock = 132;
	} else {
		dllLock = 64;
	}
	Seq0BDLY3 = (dllLock >> 2);
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY0_p0 = Seq0BDLY0;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY1_p0 = Seq0BDLY1;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY2_p0 = Seq0BDLY2;
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY3_p0 = Seq0BDLY3;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY0_p1 = Seq0BDLY0;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY1_p1 = Seq0BDLY1;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY2_p1 = Seq0BDLY2;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY3_p1 = Seq0BDLY3;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY0_p2 = Seq0BDLY0;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY1_p2 = Seq0BDLY1;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY2_p2 = Seq0BDLY2;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY3_p2 = Seq0BDLY3;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY0_p3 = Seq0BDLY0;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY1_p3 = Seq0BDLY1;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY2_p3 = Seq0BDLY2;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_Seq0BDLY3_p3 = Seq0BDLY3;		

	// New in PHYINIT_VER_A_2017_5
	__LPDDR4PHY_MIF_REGS_t5[i]->DWC_DDRPHYA_INITENG0_Seq0BDisableFlag0 = 0x0000;
	__LPDDR4PHY_MIF_REGS_t5[i]->DWC_DDRPHYA_INITENG0_Seq0BDisableFlag1 = 0x0173;
	__LPDDR4PHY_MIF_REGS_t5[i]->DWC_DDRPHYA_INITENG0_Seq0BDisableFlag2 = 0x0060;
	__LPDDR4PHY_MIF_REGS_t5[i]->DWC_DDRPHYA_INITENG0_Seq0BDisableFlag3 = 0x6110;
	__LPDDR4PHY_MIF_REGS_t5[i]->DWC_DDRPHYA_INITENG0_Seq0BDisableFlag4 = 0x2152;
	__LPDDR4PHY_MIF_REGS_t5[i]->DWC_DDRPHYA_INITENG0_Seq0BDisableFlag5 = 0xDFBD;
	if (phy_config.retraining == 0) {
		__LPDDR4PHY_MIF_REGS_t5[i]->DWC_DDRPHYA_INITENG0_Seq0BDisableFlag6 = 0xffff;
	} else {
		__LPDDR4PHY_MIF_REGS_t5[i]->DWC_DDRPHYA_INITENG0_Seq0BDisableFlag6 = 0x0060;		
	}			
	__LPDDR4PHY_MIF_REGS_t5[i]->DWC_DDRPHYA_INITENG0_Seq0BDisableFlag7 = 0x6152;

	// Periodic training, but will not perform
	// PPTTrainSetup_ADDR = PhyMstrTrainInterval | PhyMstrMaxReqToAck;
	// PhyMstrTrainInterval = 0
	// PhyMstrMaxReqToAck = 0
	if (phy_config.retraining == 0) {
		__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_PPTTrainSetup_p0 = 0x0;
		__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_PPTTrainSetup_p1 = 0x0;
		__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_PPTTrainSetup_p2 = 0x0;
		__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_PPTTrainSetup_p3 = 0x0;
	} else {		
		__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_PPTTrainSetup_p0 = 0x48;
		__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_PPTTrainSetup_p1 = 0x48;
		__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_PPTTrainSetup_p2 = 0x48;
		__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_PPTTrainSetup_p3 = 0x48;		
	}
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_PPTTrainSetup2_p0 = 0x0003;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_PPTTrainSetup2_p1 = 0x0003;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_PPTTrainSetup2_p2 = 0x0003;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_PPTTrainSetup2_p3 = 0x0003;
	
	// New in PHYINIT_VER_A_2017_5
	__LPDDR4PHY_MIF_REGS_t3[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback0x0_p0 = ((0x3 << 6) | 0x20); 
	__LPDDR4PHY_MIF_REGS_t3[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback1x0_p0 = ((0x0 << 6) | 0x12); 
	__LPDDR4PHY_MIF_REGS_t3[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback0x1_p0 = ((0x3 << 6) | 0x20); 
	__LPDDR4PHY_MIF_REGS_t3[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback1x1_p0 = ((0x0 << 6) | 0x12); 
	__LPDDR4PHY_MIF_REGS_t3[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback0x2_p0 = ((0x3 << 6) | 0x20); 
	__LPDDR4PHY_MIF_REGS_t3[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback1x2_p0 = ((0x0 << 6) | 0x12); 
	__LPDDR4PHY_MIF_REGS_t11[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback0x0_p1 = ((0x3 << 6) | 0x20); 
	__LPDDR4PHY_MIF_REGS_t11[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback1x0_p1 = ((0x0 << 6) | 0x12); 
	__LPDDR4PHY_MIF_REGS_t11[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback0x1_p1 = ((0x3 << 6) | 0x20); 
	__LPDDR4PHY_MIF_REGS_t11[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback1x1_p1 = ((0x0 << 6) | 0x12); 
	__LPDDR4PHY_MIF_REGS_t11[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback0x2_p1 = ((0x3 << 6) | 0x20); 
	__LPDDR4PHY_MIF_REGS_t11[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback1x2_p1 = ((0x0 << 6) | 0x12);                                                                         
	__LPDDR4PHY_MIF_REGS_t16[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback0x0_p2 = ((0x3 << 6) | 0x20); 
	__LPDDR4PHY_MIF_REGS_t16[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback1x0_p2 = ((0x0 << 6) | 0x12); 
	__LPDDR4PHY_MIF_REGS_t16[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback0x1_p2 = ((0x3 << 6) | 0x20); 
	__LPDDR4PHY_MIF_REGS_t16[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback1x1_p2 = ((0x0 << 6) | 0x12); 
	__LPDDR4PHY_MIF_REGS_t16[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback0x2_p2 = ((0x3 << 6) | 0x20); 
	__LPDDR4PHY_MIF_REGS_t16[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback1x2_p2 = ((0x0 << 6) | 0x12);                                                                         
	__LPDDR4PHY_MIF_REGS_t21[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback0x0_p3 = ((0x3 << 6) | 0x20); 
	__LPDDR4PHY_MIF_REGS_t21[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback1x0_p3 = ((0x0 << 6) | 0x12); 
	__LPDDR4PHY_MIF_REGS_t21[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback0x1_p3 = ((0x3 << 6) | 0x20); 
	__LPDDR4PHY_MIF_REGS_t21[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback1x1_p3 = ((0x0 << 6) | 0x12); 
	__LPDDR4PHY_MIF_REGS_t21[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback0x2_p3 = ((0x3 << 6) | 0x20); 
	__LPDDR4PHY_MIF_REGS_t21[i]->DWC_DDRPHYA_ACSM0_AcsmPlayback1x2_p3 = ((0x0 << 6) | 0x12); 

	// For DDC
	// Address Command
	__LPDDR4PHY_MIF_REGS_t3[i]->DWC_DDRPHYA_ACSM0_AcsmCtrl13 = 0x0000000f;
	// Byte 0
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_TsmByte1 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_TsmByte2 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_TsmByte3 = 0x00000180;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_TsmByte5 = 0x00000001;
	if (phy_config.retraining == 0) {
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_TrainingParam = 0x00006201;
	} else {
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_TrainingParam = 0x00006209;
	}
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_Tsm0_i0 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_Tsm2_i1 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_Tsm2_i2 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_Tsm2_i3 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_Tsm2_i4 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_Tsm2_i5 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_Tsm2_i6 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_Tsm2_i7 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE0_Tsm2_i8 = 0x00000001;
	// Byte 1		
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_TsmByte1 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_TsmByte2 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_TsmByte3 = 0x00000180;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_TsmByte5 = 0x00000001;
	if (phy_config.retraining == 0) {
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_TrainingParam = 0x00006201;
	} else {
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_TrainingParam = 0x00006209;
	}
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_Tsm0_i0 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_Tsm2_i1 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_Tsm2_i2 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_Tsm2_i3 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_Tsm2_i4 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_Tsm2_i5 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_Tsm2_i6 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_Tsm2_i7 = 0x00000001;
	__LPDDR4PHY_MIF_REGS_t1[i]->DWC_DDRPHYA_DBYTE1_Tsm2_i8 = 0x00000001;

	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DbyteDllModeCntrl = 0x00000002;	// FIXME : toggle ? static ?
	__LPDDR4PHY_MIF_REGS_t6[i]->DWC_DDRPHYA_DRTUB0_UcclkHclkEnables = 0x00000002;	// Ucclk gating only for LPDDR4		
	__LPDDR4PHY_MIF_REGS_t7[i]->DWC_DDRPHYA_APBONLY0_MicroContMuxSel = 0x00000001;		
	// PIE programming END
}

void check_init_nLPDDR4_Xm_all_ch(void)
{
	int ch;

	for (ch = 0; ch < 4; ch++)
		check_init_nLPDDR4_Xm(ch);

	for (ch = 7; ch > 3; ch--)
		check_init_nLPDDR4_Xm(ch);		
}

void check_init_nLPDDR4_Xm(int i)
{
	unsigned int rdata;

	__DMC_MIF[i]->UMCTL2_REGS_SWCTL = 0x00000000;
	// dfi_init_start = 1 : it triggers the PHY init start request
	__DMC_MIF[i]->UMCTL2_REGS_DFIMISC = 0x00000020;
	__DMC_MIF[i]->UMCTL2_REGS_SWCTL = 0x00000001;		

	// polling dfi_init_complete set
	rdata = __DMC_MIF[i]->UMCTL2_REGS_DFISTAT;
	while ((rdata & 0x1) != 0x1) {
		rdata = __DMC_MIF[i]->UMCTL2_REGS_DFISTAT;
	}

	__DMC_MIF[i]->UMCTL2_REGS_SWCTL = 0x00000000;		
	// dfi_init_start = 0 : 
	__DMC_MIF[i]->UMCTL2_REGS_DFIMISC = 0x00000000;
	// dfi_init_complete_en = 1 : Indicates to controller that PHY has completed initialization
	__DMC_MIF[i]->UMCTL2_REGS_DFIMISC = 0x00000001;		
	// exit sref
	__DMC_MIF[i]->UMCTL2_REGS_PWRCTL = 0x00000080;
	// sw_done = 1 : quasi-dynamic register setting is done
	__DMC_MIF[i]->UMCTL2_REGS_SWCTL = 0x00000001;

	// wait sref_type to be 0
	rdata = __DMC_MIF[i]->UMCTL2_REGS_STAT;
	while ((rdata & (0x3 << 4)) != (0x0 << 4)) {
		rdata = __DMC_MIF[i]->UMCTL2_REGS_STAT;
	}

	// wait operating_mode to be 'normal'
	rdata = __DMC_MIF[i]->UMCTL2_REGS_STAT;
	while ((rdata & 0x7) != 0x1) {
		rdata = __DMC_MIF[i]->UMCTL2_REGS_STAT;
	}

	__DMC_MIF[i]->UMCTL2_REGS_SWCTL = 0x00000000;
	__DMC_MIF[i]->UMCTL2_REGS_PWRCTL = 0x00000088;		
	// restore before the port enable
	__DMC_MIF[i]->UMCTL2_REGS_PWRCTL = 0x0000008a;
	__DMC_MIF[i]->UMCTL2_REGS_RFSHCTL3 = 0x00000000;
	__DMC_MIF[i]->UMCTL2_REGS_SWCTL = 0x00000001;
	
	__DMC_MIF[i]->UMCTL2_MP_PCTRL_0 = 0x00000001;

	// en_dfi_dram_clk_disable = 1
	//__DMC_MIF[i]->UMCTL2_REGS_PWRCTL = 0x00000008;		
	// en_dfi_dram_clk_disable = 0
	//__DMC_MIF[i]->UMCTL2_REGS_PWRCTL = 0x00000000;		
	// port_en = 1 : Enable AXI port 0
	//__DMC_MIF[i]->UMCTL2_MP_PCTRL_0 = 0x00000001;
}

void post_init_wdqs_all_ch(unsigned int mif_init_freq)
{
	int ch;

	for (ch = 0; ch < 4; ch++)
		post_init_wdqs(mif_init_freq, ch);

	for (ch = 7; ch > 3; ch--)
		post_init_wdqs(mif_init_freq, ch);
}

// mif_init_freq - 2133 / 1866 / 1600 / 1333 / 1066
void post_init_wdqs(unsigned int mif_init_freq, int i)
{
	unsigned int rdata;

	rdata = __DMC_MIF[i]->UMCTL2_REGS_DRAMTMG2;

	switch(mif_init_freq)
	{
	case 2133:		
	case 1866:
		rdata += (10 << 8);
		break;
	case 1600:		
	case 1333:
		rdata += (8 << 8);
		break;
	case 1066:		
	case 533:
		rdata += (6 << 8);
		break;
	default: // other than the listed freq, the default is 1600Mhz
		rdata += (8 << 8);
	}
	__DMC_MIF[i]->UMCTL2_REGS_DRAMTMG2 = rdata;

	// WR : Pre-amble 2tCK, Post-ample 0.5tCK
	// RD : Pre-ample Static, Post-ample 0.5tCK
	__LPDDR4PHY_MIF_REGS_t2[i]->DWC_DDRPHYA_MASTER0_DqsPreambleControl_p0 = 0x000001ab;
	__LPDDR4PHY_MIF_REGS_t10[i]->DWC_DDRPHYA_MASTER0_DqsPreambleControl_p1 = 0x000001ab;
	__LPDDR4PHY_MIF_REGS_t15[i]->DWC_DDRPHYA_MASTER0_DqsPreambleControl_p2 = 0x000001ab;
	__LPDDR4PHY_MIF_REGS_t20[i]->DWC_DDRPHYA_MASTER0_DqsPreambleControl_p3 = 0x000001ab;
}

static void initialization_writes_for_parity(int stage, int i)
{
	switch (stage) {
	case 0:
		// Ensure that PCTRL_n.port_en are set to 0, block the AXI ports from taking the transaction
		__DMC_MIF[i]->UMCTL2_MP_PCTRL_0 = 0x00000000;

		// Set SBRCTL.scrub_mode = 1
		// Set SBRCTL.scrub_interval=0
		__DMC_MIF[i]->UMCTL2_MP_SBRCTL |= 0x00000004;
		__DMC_MIF[i]->UMCTL2_MP_SBRCTL &= ~(0x001FFF00);

		// Set the desired pattern via SBRWDATA0 and SBRWDATA1 registers
		__DMC_MIF[i]->UMCTL2_MP_SBRWDATA0 = 0x00000000;

		// Enable the SBR by programming SBRCTL.scrub_en=1
		__DMC_MIF[i]->UMCTL2_MP_SBRCTL |= 0x00000001;

		break;
	case 1:
		// Poll SBRSTAT.scrub_done=1 (all scrub writes commands have been sent)
		while ((__DMC_MIF[i]->UMCTL2_MP_SBRSTAT & 0x00000002) != 0x00000002);

		break;
	case 2:
		// Poll SBRSTAT.scrub_busy=0 (all scrub writes data have been sent)
		while ((__DMC_MIF[i]->UMCTL2_MP_SBRSTAT & 0x00000001) == 0x00000001);

		break;
	case 3:
		// Disable SBR by programming SBRCTL.scrub_en=0
		__DMC_MIF[i]->UMCTL2_MP_SBRCTL &= ~(0x00000001);

		// Enable AXI ports by programming PCTRL_n.port_en = 1
		__DMC_MIF[i]->UMCTL2_MP_PCTRL_0 = 0x00000001;

		break;
	}
}

static void initialization_writes_for_parity_all_ch(void)
{
	int stage;

	for (stage = 0; stage < 4; stage++) {
		int ch;

		for (ch = 0; ch < 4; ch++)
			initialization_writes_for_parity(stage, ch);

		for (ch = 7; ch > 3; ch--)
			initialization_writes_for_parity(stage, ch);
	}
}

static void check_otp_ddr_info(unsigned int mrr_value)
{
	int rv;
	uint32_t info;

	if (mrr_value != DRAM_VID_SAMSUNG && mrr_value != DRAM_VID_MICRON && mrr_value != DRAM_VID_SKHYNIX)
		return;

	rv = get_otp_ddr_info(&info);

	if (rv != 0) {
		printk(BIOS_INFO, "can not read otp ddr info");
		return;
	}

	if ((info & OTP_DDR_INFO_MASK) == mrr_value) {
		printk(BIOS_INFO, "OTP ddr info consistent\n");
		return;
	}

	if ((info & OTP_DDR_INFO_MASK) != 0) {
		printk(BIOS_INFO, "wrong otp ddr info : %x\n", info & OTP_DDR_INFO_MASK);
		return;
	}

	/* program it */
	info &= ~OTP_DDR_INFO_MASK;
	info |= mrr_value & OTP_DDR_INFO_MASK;

	rv = set_otp_ddr_info(info);

	printk(BIOS_INFO, "OTP ddr info programed: %x with status=%d\n", info & OTP_DDR_INFO_MASK, rv);

	/* need a cold reset */
	sms_reset(RESET_MODE_COLD);
}

static unsigned int get_dram_vendor(int ch)
{
	unsigned int mrr_value;

	/* Issue MR5 read */
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 = 0x0000d011;
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL1 = 0x00000500;
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= 0x80000000;

	while ((__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 & (0x1 << 31)) != (0x0 << 31));
	udelay(10);

	/* Read the return value - DMC_REG1.hif_mrr_data */
	mrr_value = readl((0x10830114 + ((unsigned long)ch << 20)));
	mrr_value &= 0xff;

	return mrr_value;
}

static unsigned int get_dram_vendor_info(void)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;
	unsigned int mrr_value_r, mrr_value_l, mrr_value;
	char message[64];

	mrr_value_r = get_dram_vendor(0);

	mrr_value_l = get_dram_vendor(7);

	if (mrr_value_r != mrr_value_l) {
		printk(BIOS_INFO,"DRAM R/L vendor not matched. \n");
		snprintf(message, 64, "dram failed: vendor: %02x %02x", mrr_value_l, mrr_value_r);
		slog_log_message(message, 26);
		sms_reset(RESET_MODE_COLD);
	}

	mrr_value = mrr_value_r;

	if (mrr_value == DRAM_VID_SAMSUNG) {	// Samsung
		printk(BIOS_INFO,"DRAM vendor is 0x1, Samsung. \n");
		strncpy(env->ddr_vendor, "Samsung", 8);
	} else if (mrr_value == DRAM_VID_MICRON) {	// Micron
		printk(BIOS_INFO,"DRAM vendor is 0xFF, Micron. \n");
		strncpy(env->ddr_vendor, "Micron", 8);
	} else if (mrr_value == DRAM_VID_SKHYNIX) {	// Hynix
		printk(BIOS_INFO,"DRAM vendor is 0x6, Hynix. \n");
		strncpy(env->ddr_vendor, "Hynix", 8);
	} else {
		printk(BIOS_INFO,"DRAM vendor is 0x%x, Unknown Vendor. \n", mrr_value);
		strncpy(env->ddr_vendor, "Unknown", 8);
	}

	check_otp_ddr_info(mrr_value);

	return mrr_value_r;
}

static void tmrs_enter(int ch)
{
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 &= ~(0x7 << 12);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (0x5 << 12);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL1 &= ~(0xffff << 0);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL1 |= (0x920 << 0);	

	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 &= ~(0x1 << 0);	
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (0x1 << 31);	
	while ((__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 & (0x1 << 31)) != 0x0);
	udelay(10);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (0x1 << 31);	
	while ((__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 & (0x1 << 31)) != 0x0);
	udelay(10);	
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (0x1 << 31);	
	while ((__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 & (0x1 << 31)) != 0x0);	
	udelay(10);	

	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 &= ~(0x7 << 12);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (0x5 << 12);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL1 &= ~(0xffff << 0);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL1 |= (0x9ff << 0);	

	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 &= ~(0x1 << 0);	
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (0x1 << 31);	
	while ((__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 & (0x1 << 31)) != 0x0);
	udelay(10);
	
	udelay(50);	
}

static void tmrs_latch(int ch)
{
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 &= ~(0x7 << 12);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (0x0 << 12);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL1 &= ~(0xffff << 0);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL1 |= (0x900 << 0);	

	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 &= ~(0x1 << 0);	
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (0x1 << 31);	
	while ((__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 & (0x1 << 31)) != 0x0);
	udelay(10);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (0x1 << 31);	
	while ((__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 & (0x1 << 31)) != 0x0);
	udelay(10);	
}

static void tmrs_by_code(int ch, int a, int b, int c)
{
	unsigned int mr_addr = 0;
	unsigned int mr_data = 0;
	
	if (a <= 7 && a >= 0) {
		mr_addr = 0;
		mr_data = (0x1 << a);
	} else if (a <= 10) {
		mr_addr = (0x1 << (a - 8));
		mr_data = 0;
	} else {
		mr_addr = 0;
		mr_data = 0;
	}
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 &= ~(0x7 << 12);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (mr_addr << 12);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL1 &= ~(0xffff << 0);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL1 |= (0x900 << 0) | (mr_data << 0);	

	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 &= ~(0x1 << 0);	
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (0x1 << 31);	
	while ((__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 & (0x1 << 31)) != 0x0);
	udelay(10);

	if (b <= 7 && b >= 0) {
		mr_addr = 0;
		mr_data = (0x1 << b);
	} else if (b <= 10) {
		mr_addr = (0x1 << (b - 8));
		mr_data = 0;
	} else {
		mr_addr = 0;
		mr_data = 0;
	}
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 &= ~(0x7 << 12);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (mr_addr << 12);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL1 &= ~(0xffff << 0);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL1 |= (0x900 << 0) | (mr_data << 0);	

	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 &= ~(0x1 << 0);	
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (0x1 << 31);	
	while ((__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 & (0x1 << 31)) != 0x0);
	udelay(10);

	if (c <= 7 && c >= 0) {
		mr_addr = 0;
		mr_data = (0x1 << c);
	} else if (c <= 10) {
		mr_addr = (0x1 << (c - 8));
		mr_data = 0;
	} else {
		mr_addr = 0;
		mr_data = 0;
	}
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 &= ~(0x7 << 12);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (mr_addr << 12);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL1 &= ~(0xffff << 0);
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL1 |= (0x900 << 0) | (mr_data << 0);	

	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 &= ~(0x1 << 0);	
	__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 |= (0x1 << 31);	
	while ((__DMC_MIF[ch]->UMCTL2_REGS_MRCTRL0 & (0x1 << 31)) != 0x0);
	udelay(10);	
}

static void tmrs_code_all_ch(int a, int b, int c)
{
		for (int ch = 0; ch < 8; ch++) {
			tmrs_by_code(ch, a, b, c);
			tmrs_latch(ch);
		}	
}
	
static void tmrs_for_samsung(void)
{
	int ch;

	printk(BIOS_INFO, "TMRS for Samsung \n");

	for (ch = 0; ch < 8; ch++) {
		tmrs_enter(ch);
	}
	
	/* Phase1 */
	tmrs_code_all_ch(0, 0, 0);
	/* For locking */
	tmrs_code_all_ch(3, 9, 0);
	/* Phase2 */
	tmrs_code_all_ch(1, 2, 0);
	/* Clock buffer sensitivity */
	tmrs_code_all_ch(9, 1, 4);
	tmrs_code_all_ch(8, 9, 0);
	tmrs_code_all_ch(8, 8, 5);
	/* Internal ISI */
	//tmrs_code_all_ch(9, 6, 8);
}

void mem_ctrl_init(unsigned int memclk, unsigned char warm_reset)
{
	/* PHY_PRESET release */
	writel(1, &trav_power->logic_reset_ddrphy_option);
	udelay(3000);
	umctl2_init_nLPDDR4_Xm_all_ch(memclk);
	/* ARESETn/DDRC_RESETn, PRESETn */
	writel(0x111, &trav_power->logic_reset_ddrphy_option);
	umctl2_init_nLPDDR4_Xm_set_default_MR_all_ch();
	umctl2_init2_nLPDDR4_Xm_all_ch();
	pub_init_nLPDDR4_Xm_all_ch(memclk);

	pub_1d_training_all_ch(memclk);

	if (memclk != 533) {
		pub_2d_training_all_ch(memclk);
		redo_initial_training = 1;
		pub_1d_training_all_ch(memclk);		
	}
	
	pub_init2_nLPDDR4_Xm_all_ch(memclk);
	
	/* dfi init start and wait for dfi init complete */
	check_init_nLPDDR4_Xm_all_ch();

	//Apply Vendor Specific Workaround
	unsigned int dram_vid = get_dram_vendor_info();
	if (DRAM_VID_SAMSUNG == dram_vid) {
	    tmrs_for_samsung();
	} else if (DRAM_VID_SKHYNIX == dram_vid) {	// if it's Hynix, apply the wdqs extention.
		printk(BIOS_INFO, "WDQS for Hynix \n");
		// reset to PHY
		writel(0x0, 0x114031a0);
		writel(0x0, 0x11403180);
		udelay(1000);	//64 dficlk
		// reset release to PHY
		writel(0x3, 0x114031a0);
		writel(0x3, 0x11403180);
		udelay(1000);		// 16 apbclk

		/* PHY_PRESET release */
		writel(1, &trav_power->logic_reset_ddrphy_option);
		udelay(3000);
		umctl2_init_nLPDDR4_Xm_all_ch(memclk);
		/* ARESETn/DDRC_RESETn, PRESETn */
		writel(0x111, &trav_power->logic_reset_ddrphy_option);
		umctl2_init_nLPDDR4_Xm_set_default_MR_all_ch();
		umctl2_init2_nLPDDR4_Xm_all_ch();
		pub_init_nLPDDR4_Xm_all_ch(memclk);
		post_init_wdqs_all_ch(memclk);

		redo_initial_training = 0;
		pub_1d_training_all_ch(memclk);

		if (memclk != 533) {
			pub_2d_training_all_ch(memclk);
			redo_initial_training = 1;
			pub_1d_training_all_ch(memclk);
		}

		pub_init2_nLPDDR4_Xm_all_ch(memclk);

		/* dfi init start and wait for dfi init complete */
		check_init_nLPDDR4_Xm_all_ch();
	}	
#ifdef ECC_ENABLED
	/*
	 * only save and retore oops for warm reset and non-delayed warm boot.
	 * for delayed warm boot, accessing ram before ecc parity get initialized
	 * will make atf bl31 trigger panic for abort/serror.
	 */
	if (warm_reset == A72_BOOT_FROM_WARM_RESET) {
		uint64_t i;

		for (i = 0; i < CONFIG_BOOT_OOPS_RAM_MEM_SIZE; i += 8)
			writell(readll(CONFIG_BOOT_OOPS_RAM_MEM_BASE + i),
				 CONFIG_BOOT_OOPS_BUF_MEM_BASE + i);
	}

	initialization_writes_for_parity_all_ch();
	printk(BIOS_INFO, "Initialization writes done for ECC parity\n");

	if (warm_reset == A72_BOOT_FROM_WARM_RESET) {
		uint64_t i;

		for (i = 0; i < CONFIG_BOOT_OOPS_RAM_MEM_SIZE; i += 8)
			writell(readll(CONFIG_BOOT_OOPS_BUF_MEM_BASE + i),
				 CONFIG_BOOT_OOPS_RAM_MEM_BASE + i);
	}
#endif
}

