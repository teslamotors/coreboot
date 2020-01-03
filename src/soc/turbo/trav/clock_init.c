/*
 * Clock setup for TRAV
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

#include <config.h>
#include <delay.h>
#include <console/console.h>
#include <soc/clock_init.h>
#include <soc/cpu.h>
#include <soc/clock.h>
#include <soc/common_definition.h>

volatile struct cmu_cpucl * const cpucl = (struct cmu_cpucl *)TRAV_CMU_CPUCL;
volatile struct cmu_imem * const imem = (struct cmu_imem *)TRAV_CMU_IMEM;
volatile struct cmu_cmu * const cmu = (struct cmu_cmu *)TRAV_CMU_CMU;
volatile struct cmu_core_gt * const core_gt = (struct cmu_core_gt *)TRAV_CMU_CORE_GT;
volatile struct cmu_core_mif * const core_mifl = (struct cmu_core_mif *)TRAV_CMU_CORE_MIFL;
volatile struct cmu_core_mif * const core_mifr = (struct cmu_core_mif *)TRAV_CMU_CORE_MIFR;
volatile struct cmu_mif * const mif0 = (struct cmu_mif *)TRAV_CMU_MIF0;
volatile struct cmu_mif * const mif1 = (struct cmu_mif *)TRAV_CMU_MIF1;
volatile struct cmu_mif * const mif2 = (struct cmu_mif *)TRAV_CMU_MIF2;
volatile struct cmu_mif * const mif3 = (struct cmu_mif *)TRAV_CMU_MIF3;
volatile struct cmu_mif * const mif4 = (struct cmu_mif *)TRAV_CMU_MIF4;
volatile struct cmu_mif * const mif5 = (struct cmu_mif *)TRAV_CMU_MIF5;
volatile struct cmu_mif * const mif6 = (struct cmu_mif *)TRAV_CMU_MIF6;
volatile struct cmu_mif * const mif7 = (struct cmu_mif *)TRAV_CMU_MIF7;
volatile struct cmu_peric * const peric = (struct cmu_peric *)TRAV_CMU_PERIC;
volatile struct cmu_csi * const csi = (struct cmu_csi *)TRAV_CMU_CAM_CSI;
volatile struct cmu_dprx0 * const dprx0 = (struct cmu_dprx0 *)TRAV_CMU_CAM_DPRX0;
volatile struct cmu_dprx1 * const dprx1 = (struct cmu_dprx1 *)TRAV_CMU_CAM_DPRX1;
volatile struct cmu_fsys0 * const fsys0 = (struct cmu_fsys0 *)TRAV_CMU_FSYS0;
volatile struct cmu_fsys1 * const fsys1 = (struct cmu_fsys1 *)TRAV_CMU_FSYS1;
volatile struct cmu_gpu * const gpu = (struct cmu_gpu *)TRAV_CMU_GPU;
volatile struct cmu_mfc * const mfc = (struct cmu_mfc *)TRAV_CMU_MFC;
volatile struct cmu_trip * const trip0 = (struct cmu_trip *)TRAV_CMU_TRIP0;
volatile struct cmu_trip * const trip1 = (struct cmu_trip *)TRAV_CMU_TRIP1;
volatile struct cmu_isp * const isp = (struct cmu_isp *)TRAV_CMU_ISP;

static void cmu_cpucl_init(unsigned int cpucl_clk)
{
	/* PLL setting */
	cpucl->PLL_LOCKTIME_PLL_CPUCL = 0x000000ff;
	if (cpucl_clk == 2400)
		cpucl->PLL_CON0_PLL_CPUCL = CON0_VALUE(300, 3, 0);
	else if (cpucl_clk == 2200)
		cpucl->PLL_CON0_PLL_CPUCL = CON0_VALUE(275, 3, 0);
	else if (cpucl_clk == 2000)
		cpucl->PLL_CON0_PLL_CPUCL = CON0_VALUE(250, 3, 0);
	else if (cpucl_clk == 1600)
		cpucl->PLL_CON0_PLL_CPUCL = CON0_VALUE(200, 3, 0);
	else if (cpucl_clk == 1500)
		cpucl->PLL_CON0_PLL_CPUCL = CON0_VALUE(125, 2, 0);
	else
		cpucl->PLL_CON0_PLL_CPUCL = CON0_VALUE(125, 3, 0); /* 1000MHz */

	/* SWITCHING MUX */
	cpucl->PLL_CON0_CPUCL_SWITCHCLK_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(cpucl->PLL_CON0_CPUCL_SWITCHCLK_MUX);

	/* DIVIDER setting */
	cpucl->CLK_CON_DIV_CLK_CLUSTER_CLK = DIV_VAL(1);
	cpucl->CLK_CON_DIV_CLK_CPUCL_DROOP_DETECTOR = DIV_VAL(2);
	cpucl->CLK_CON_DIV_CPUCL_CMUREF_CLK = DIV_VAL(2);
	cpucl->CLK_CON_DIV_CPUCL_HPM = DIV_VAL(4);
	cpucl->CLK_CON_DIV_CPUCL_PCLK = DIV_VAL(8);
	cpucl->CLK_CON_DIV_CPUCL_PCLK_DD = DIV_VAL(8);
	cpucl->CLK_CON_DIV_DIV_CPUCL0_ACLK = DIV_VAL(2);
	cpucl->CLK_CON_DIV_DIV_CPUCL0_ADB400 = DIV_VAL(1);
	cpucl->CLK_CON_DIV_DIV_CPUCL0_ATCLK = DIV_VAL(4);
	cpucl->CLK_CON_DIV_DIV_CPUCL0_CNTCLK = DIV_VAL(4);
	cpucl->CLK_CON_DIV_DIV_CPUCL0_PCLKDBG = DIV_VAL(8);
	cpucl->CLK_CON_DIV_DIV_CPUCL1_ACLK = DIV_VAL(2);
	cpucl->CLK_CON_DIV_DIV_CPUCL1_ADB400 = DIV_VAL(1);
	cpucl->CLK_CON_DIV_DIV_CPUCL1_ATCLK = DIV_VAL(4);
	cpucl->CLK_CON_DIV_DIV_CPUCL1_CNTCLK = DIV_VAL(4);
	cpucl->CLK_CON_DIV_DIV_CPUCL1_PCLKDBG = DIV_VAL(8);
	cpucl->CLK_CON_DIV_DIV_CPUCL2_ACLK = DIV_VAL(2);
	cpucl->CLK_CON_DIV_DIV_CPUCL2_ADB400 = DIV_VAL(1);
	cpucl->CLK_CON_DIV_DIV_CPUCL2_ATCLK = DIV_VAL(4);
	cpucl->CLK_CON_DIV_DIV_CPUCL2_CNTCLK = DIV_VAL(4);
	cpucl->CLK_CON_DIV_DIV_CPUCL2_PCLKDBG = DIV_VAL(8);

	/* Change cpucl clock source from Switching PLL to PLL */
	cpucl->CLK_CON_MUX_CLK_CLUSTER_CLK_MUX = MUX_SEL(0);
	WAIT_FOR_MUX_SET_DONE(cpucl->CLK_CON_MUX_CLK_CLUSTER_CLK_MUX);

	/* Change cpucl clock source from OSC to PLL */
	WAIT_FOR_LOCKED(cpucl->PLL_CON0_PLL_CPUCL);
	cpucl->PLL_CON0_PLL_CPUCL |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(cpucl->PLL_CON0_PLL_CPUCL);
}

static void cmu_gpu_init(unsigned int gpu_clk)
{
	gpu->PLL_CON1_PLL_GPU = 0x0000f380;
	/* PLL setting */
	gpu->PLL_LOCKTIME_PLL_GPU = 0x000000ff;
	if (gpu_clk == 1000)
		gpu->PLL_CON0_PLL_GPU = CON0_VALUE(0x7d, 0x3, 0);
        else if (gpu_clk == 250)
		gpu->PLL_CON0_PLL_GPU = CON0_VALUE(0xfa, 0x3, 3);
        else //Default will be 700
		gpu->PLL_CON0_PLL_GPU = CON0_VALUE(0xaf, 0x3, 1);

	/* SWITCHING MUX */
	gpu->PLL_CON0_GPU_CLK_SWITCH_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(gpu->PLL_CON0_GPU_CLK_SWITCH_MUX);
	gpu->CLK_CON_MUX_CLK_GPU_BUSD = MUX_SEL(0);
	WAIT_FOR_MUX_SET_DONE(gpu->CLK_CON_MUX_CLK_GPU_BUSD);

	/* DIVIDER setting */
	gpu->CLK_CON_DIV_CLK_GPU_BUSP = DIV_VAL(4);
	gpu->CLK_CON_DIV_CLK_DROOP_DETECTOR_GPU = DIV_VAL(2);
	gpu->CLK_CON_DIV_CLK_GPU_HPM = DIV_VAL(2);
	gpu->CLK_CON_DIV_CLK_GPU_BUSACLK = DIV_VAL(2);
	gpu->CLK_CON_DIV_CLK_GPU_DD_BUSP0 = DIV_VAL(4);
	gpu->CLK_CON_DIV_CLK_GPU_DD_BUSP1 = DIV_VAL(4);

	/* Change source to PLL */
	WAIT_FOR_LOCKED(gpu->PLL_CON0_PLL_GPU);
	gpu->PLL_CON0_PLL_GPU |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(gpu->PLL_CON0_PLL_GPU);
}

static void cmu_trip_init(volatile struct cmu_trip *trip, unsigned int trip_clk)
{
	/* PLL setting */
	trip->PLL_LOCKTIME_PLL_TRIP = 0x000000ff;

	if (trip_clk == 2400)
		trip->PLL_CON0_PLL_TRIP = CON0_VALUE(200, 2, 0);
	else if (trip_clk == 2000)
		trip->PLL_CON0_PLL_TRIP = CON0_VALUE(250, 3, 0);
	else if (trip_clk == 1600)
		trip->PLL_CON0_PLL_TRIP = CON0_VALUE(400, 3, 1);
	else if (trip_clk == 1200)
		trip->PLL_CON0_PLL_TRIP = CON0_VALUE(200, 2, 1);
	else if (trip_clk == 800)
		trip->PLL_CON0_PLL_TRIP = CON0_VALUE(400, 3, 2);
	else if (trip_clk == 400)
		trip->PLL_CON0_PLL_TRIP = CON0_VALUE(400, 3, 3);
	else
		trip->PLL_CON0_PLL_TRIP = CON0_VALUE(0x12c, 0x3, 0);

	/* SWITCHING MUX */
	trip->PLL_CON0_TRIP_SWITCH_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(trip->PLL_CON0_TRIP_SWITCH_MUX);
	trip->CLK_CON_MUX_TRIP_SWITCH_PLL_MUX = MUX_SEL(0);
	WAIT_FOR_MUX_SET_DONE(trip->CLK_CON_MUX_TRIP_SWITCH_PLL_MUX);
	trip->CLK_CON_MUX_ECLK_ACLK_MUX = MUX_SEL(0);
	WAIT_FOR_MUX_SET_DONE(trip->CLK_CON_MUX_ECLK_ACLK_MUX);

	/* DIVIDER setting */
	trip->CLK_CON_DIV_TRIP_ACLK = DIV_VAL(1);
	trip->CLK_CON_DIV_TRIP_PCLK = DIV_VAL(8);
	trip->CLK_CON_DIV_CLK_DROOP_DETECTOR = DIV_VAL(2);
	trip->CLK_CON_DIV_CLK_HPM_TARGET = DIV_VAL(4);
	trip->CLK_CON_DIV_TRIP_PCLK_DD = DIV_VAL(8);

	/* Change source to PLL */
	WAIT_FOR_LOCKED(trip->PLL_CON0_PLL_TRIP);
	trip->PLL_CON0_PLL_TRIP |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(trip->PLL_CON0_PLL_TRIP);
}

static void cmu_core_init(unsigned int clk)
{
	/* MIFL */
	core_mifl->PLL_LOCKTIME_PLL_CORE_MIFL = 0x000000ff;

	if (clk == 2133)
		core_mifl->PLL_CON0_PLL_CORE_MIFL = CON0_VALUE(0x10b, 3, 1);
	else if (clk == 1866)
		core_mifl->PLL_CON0_PLL_CORE_MIFL = CON0_VALUE(0xe8, 3, 1);
	else if (clk == 1066)
		core_mifl->PLL_CON0_PLL_CORE_MIFL = CON0_VALUE(0x215, 6, 2);
	else
		core_mifl->PLL_CON0_PLL_CORE_MIFL = CON0_VALUE(0x215, 6, 3); // 533MHz


	core_mifl->CLK_CON_MUX_PLL_MIFL_CLK = MUX_SEL(0);
	WAIT_FOR_MUX_SET_DONE(core_mifl->CLK_CON_MUX_PLL_MIFL_CLK);

	/* Change source to PLL */
	WAIT_FOR_LOCKED(core_mifl->PLL_CON0_PLL_CORE_MIFL);
	core_mifl->PLL_CON0_PLL_CORE_MIFL |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(core_mifl->PLL_CON0_PLL_CORE_MIFL);

	/* MIFR */
	core_mifr->PLL_LOCKTIME_PLL_CORE_MIFL = 0x000000ff;

	if (clk == 2133)
		core_mifr->PLL_CON0_PLL_CORE_MIFL = CON0_VALUE(0x10b, 3, 1);
	else if (clk == 1866)
		core_mifr->PLL_CON0_PLL_CORE_MIFL = CON0_VALUE(0xe8, 3, 1);
	else if (clk == 1066)
		core_mifr->PLL_CON0_PLL_CORE_MIFL = CON0_VALUE(0x215, 6, 2);
	else
		core_mifr->PLL_CON0_PLL_CORE_MIFL = CON0_VALUE(0x215, 6, 3); // 533MHz

	core_mifr->CLK_CON_MUX_PLL_MIFL_CLK = MUX_SEL(0);
	WAIT_FOR_MUX_SET_DONE(core_mifr->CLK_CON_MUX_PLL_MIFL_CLK);

	/* Change source to PLL */
	WAIT_FOR_LOCKED(core_mifr->PLL_CON0_PLL_CORE_MIFL);
	core_mifr->PLL_CON0_PLL_CORE_MIFL |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(core_mifr->PLL_CON0_PLL_CORE_MIFL);

	/* CORE_GT */
	/* PLL setting */
	core_gt->PLL_LOCKTIME_PLL_CORE_GT = 0x000000ff;
	if (clk == 2133)
		core_gt->PLL_CON0_PLL_CORE_GT = CON0_VALUE(0x10b, 3, 1); // 2133MHz
	else if (clk == 1866)
		core_gt->PLL_CON0_PLL_CORE_GT = CON0_VALUE(0xe8, 3, 1); // 1866MHz
	else if (clk == 1066)
		core_gt->PLL_CON0_PLL_CORE_GT = CON0_VALUE(0x215, 6, 2); // 1866MHz
	else
		core_gt->PLL_CON0_PLL_CORE_GT = CON0_VALUE(0x215, 6, 3); // 533MHz

	/* Divider setting */
	core_gt->CLK_CON_DIV_CLK_CORE_TCU = DIV_VAL(2);
	core_gt->CLK_CON_DIV_PLL_CORE_DIV4 = DIV_VAL(4);
	core_gt->CLK_CON_DIV_DIV_MIF_SWITCHCLK = DIV_VAL(2);
	core_gt->CLK_CON_DIV_DIV_CLK_CORE_ATB = DIV_VAL(2);

	/* MUX setting */
	core_gt->CLK_CON_MUX_CLK_CORE_NOC = MUX_SEL(1);
	WAIT_FOR_MUX_SET_DONE(core_gt->CLK_CON_MUX_CLK_CORE_NOC);
	core_gt->CLK_CON_MUX_CLK_CORE_PCLK = MUX_SEL(1);
	WAIT_FOR_MUX_SET_DONE(core_gt->CLK_CON_MUX_CLK_CORE_PCLK);
	core_gt->CLK_CON_MUX_CLK_CORE_TRIP_MUX = MUX_SEL(1);
	WAIT_FOR_MUX_SET_DONE(core_gt->CLK_CON_MUX_CLK_CORE_TRIP_MUX);

	/* Change to PLL source */
	WAIT_FOR_LOCKED(core_gt->PLL_CON0_PLL_CORE_GT);
	core_gt->PLL_CON0_PLL_CORE_GT |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(core_gt->PLL_CON0_PLL_CORE_GT);
}

static void cmu_mif_init(volatile struct cmu_mif *mif)
{
	mif->CLK_CON_DIV_MIF_BYPASSDIV4_ACLK = DIV_VAL(1);
	mif->CLK_CON_DIV_MIF_PLLDIV_PCLK = DIV_VAL(4);

	mif->PLL_CON0_MIF_PLL_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(mif->PLL_CON0_MIF_PLL_MUX);
}

static void cmu_imem_init(void)
{
	imem->CLK_CON_DIV_DIV_OSCCLK_IMEM_TMUTSCLK = DIV_VAL(8);

	imem->PLL_CON0_CLK_IMEM_ACLK = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(imem->PLL_CON0_CLK_IMEM_ACLK);
	imem->PLL_CON0_CLK_IMEM_INTMEMCLK = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(imem->PLL_CON0_CLK_IMEM_INTMEMCLK);
	imem->PLL_CON0_CLK_IMEM_TCUCLK = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(imem->PLL_CON0_CLK_IMEM_TCUCLK);
}

static void cmu_peric_init(void)
{
	peric->CLK_CON_DIV_DIV_EQOS_BUSCLK = DIV_VAL(2);
	peric->CLK_CON_DIV_DIV_PERIC_MCAN_CLK = DIV_VAL(15);
	peric->CLK_CON_DIV_DIV_RGMII_CLK = DIV_VAL(4);
	peric->CLK_CON_DIV_DIV_RMII_CLK = DIV_VAL(5);
	peric->CLK_CON_DIV_DIV_RII_CLK = DIV_VAL(10);
	peric->CLK_CON_DIV_DIV_UART_CLK = DIV_VAL(1);
	peric->CLK_CON_DIV_DIV_SPI_CLK = DIV_VAL(10);


	peric->PLL_CON0_PERIC_DMACLK_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(peric->PLL_CON0_PERIC_DMACLK_MUX);
	peric->PLL_CON0_PERIC_EQOS_BUSCLK_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(peric->PLL_CON0_PERIC_EQOS_BUSCLK_MUX);
	peric->PLL_CON0_PERIC_PCLK_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(peric->PLL_CON0_PERIC_PCLK_MUX);
	peric->PLL_CON0_PERIC_TBUCLK_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(peric->PLL_CON0_PERIC_TBUCLK_MUX);
	peric->PLL_CON0_SPI_CLK = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(peric->PLL_CON0_SPI_CLK);
	peric->PLL_CON0_SPI_PCLK = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(peric->PLL_CON0_SPI_PCLK);
	peric->PLL_CON0_UART_CLK = PLL_MUX_SEL(0);
	WAIT_FOR_PLL_MUX_SET_DONE(peric->PLL_CON0_UART_CLK);
	peric->PLL_CON0_UART_PCLK = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(peric->PLL_CON0_UART_PCLK);
}

static void cmu_fsys0_init(void)
{
	/* SWITCHING MUX */
	fsys0->PLL_CON0_CLKCMU_FSYS0_UNIPRO = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(fsys0->PLL_CON0_CLKCMU_FSYS0_UNIPRO);
	fsys0->PLL_CON0_CLK_FSYS0_SLAVEBUSCLK = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(fsys0->PLL_CON0_CLK_FSYS0_SLAVEBUSCLK);
	fsys0->PLL_CON0_EQOS_RGMII_125_MUX1 = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(fsys0->PLL_CON0_EQOS_RGMII_125_MUX1);

	/* DIVIDER setting */
	fsys0->CLK_CON_DIV_EQOS_RMIICLK_25 = DIV_VAL(0x14);
	fsys0->CLK_CON_DIV_EQOS_RII_CLK2O5 = DIV_VAL(0xA);
	fsys0->CLK_CON_DIV_DIV_EQS_RGMII_CLK_125 = DIV_VAL(4);
	fsys0->CLK_CON_DIV_DIV_CLK_UNIPRO = DIV_VAL(2);
	fsys0->CLK_CON_DIV_DIV_PERIBUS_GRP = DIV_VAL(2);
	fsys0->CLK_CON_DIV_PCIE_PHY_OSCCLK = DIV_VAL(5);
}

static void cmu_fsys1_init(void)
{
	/* SWITCHING MUX */
	fsys1->PLL_CON0_PCLKL_FSYS1_BUSP_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(fsys1->PLL_CON0_PCLKL_FSYS1_BUSP_MUX);
	fsys1->PLL_CON0_ACLK_FSYS1_BUSP_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(fsys1->PLL_CON0_ACLK_FSYS1_BUSP_MUX);

	/* DIVIDER setting */
	fsys1->CLK_CON_DIV_CLK_FSYS1_PHY0_OSCCLK = DIV_VAL(0x5);
	fsys1->CLK_CON_DIV_CLK_FSYS1_PHY1_OSCCLK = DIV_VAL(0x5);
}

static void cmu_mfc_init(void)
{
	/* PLL setting */
	mfc->PLL_LOCKTIME_PLL_MFC = 0x000000ff;
	mfc->PLL_CON0_PLL_MFC = CON0_VALUE(0x6f, 0x4, 0);

	/* MUX setting */
	mfc->CLK_CON_MUX_MFC_BUSP = MUX_SEL(1);
	WAIT_FOR_MUX_SET_DONE(mfc->CLK_CON_MUX_MFC_BUSP);
	mfc->CLK_CON_MUX_MFC_BUSD = MUX_SEL(1);
	WAIT_FOR_MUX_SET_DONE(mfc->CLK_CON_MUX_MFC_BUSD);

	/* DIVIDER setting */
	mfc->CLK_CON_DIV_MFC_BUSD_DIV4 = DIV_VAL(4);

	/* Change source to PLL */
	WAIT_FOR_LOCKED(mfc->PLL_CON0_PLL_MFC);
	mfc->PLL_CON0_PLL_MFC |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(mfc->PLL_CON0_PLL_MFC);
}

static void cmu_csi_init(void)
{
	/* PLL setting */
	csi->PLL_LOCKTIME_PLL_CAM_CSI = 0x000000ff;
	csi->PLL_CON0_PLL_CAM_CSI = CON0_VALUE(0x215, 0xc, 0);

	/* DIVIDER setting */
	csi->CLK_CON_DIV_DIV_CAM_CSI_BUSD = DIV_VAL(2);
	csi->CLK_CON_DIV_DIV_CAM_CSI0_ACLK = DIV_VAL(3);
	csi->CLK_CON_DIV_DIV_CAM_CSI_BUSP = DIV_VAL(4);
	csi->CLK_CON_DIV_DIV_CAM_CSI1_ACLK = DIV_VAL(3);
	csi->CLK_CON_DIV_DIV_CAM_CSI2_ACLK = DIV_VAL(3);

	/* Change source to PLL */
	WAIT_FOR_LOCKED(csi->PLL_CON0_PLL_CAM_CSI);
	csi->PLL_CON0_PLL_CAM_CSI |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(csi->PLL_CON0_PLL_CAM_CSI);
}

static void cmu_dprx0_init(void)
{
	/* PLL setting */
	dprx0->PLL_LOCKTIME_PLL_CAM_DPRX = 0x000000ff;
	dprx0->PLL_CON0_PLL_CAM_DPRX = CON0_VALUE(0xcd, 0x3, 0);

	/* SWITCHING MUX */
	dprx0->PLL_CON0_DPRX_TOPCMUSWITCHCLK_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(dprx0->PLL_CON0_DPRX_TOPCMUSWITCHCLK_MUX);
	dprx0->PLL_CON0_BLK_DPRX0_DSC_CLK_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(dprx0->PLL_CON0_BLK_DPRX0_DSC_CLK_MUX);
	dprx0->CLK_CON_MUX_DPRX_SWITCH_MUX = MUX_SEL(0);
	WAIT_FOR_MUX_SET_DONE(dprx0->CLK_CON_MUX_DPRX_SWITCH_MUX);

	/* DIVIDER setting */
	dprx0->CLK_CON_DIV_DIV_CLK_BLK_DPRX0_BUSP = DIV_VAL(2);
	dprx0->CLK_CON_DIV_DIV_CLK_DPRX3_RXCLK = DIV_VAL(2);
	dprx0->CLK_CON_DIV_DIV_CLK_DPRX2_RXCLK = DIV_VAL(2);
	dprx0->CLK_CON_DIV_DIV_CLK_BLK_DPRX0_BUSD = DIV_VAL(3);
	dprx0->CLK_CON_DIV_DIV_CLK_DPRX2_DSC_CLK = DIV_VAL(1);
	dprx0->CLK_CON_DIV_DIV_CLK_DPRX3_DSC_CLK = DIV_VAL(1);
	dprx0->CLK_CON_DIV_DIV_HDCP_CLK = DIV_VAL(1);

	/* Change source to PLL */
	WAIT_FOR_LOCKED(dprx0->PLL_CON0_PLL_CAM_DPRX);
	dprx0->PLL_CON0_PLL_CAM_DPRX |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(dprx0->PLL_CON0_PLL_CAM_DPRX);
}

static void cmu_dprx1_init(void)
{
	/* SWITCHING MUX */
	dprx1->PLL_CON0_DPRX1_BUSD_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(dprx1->PLL_CON0_DPRX1_BUSD_MUX);
	dprx1->PLL_CON0_DPRX1_RXCLK_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(dprx1->PLL_CON0_DPRX1_RXCLK_MUX);
	dprx1->PLL_CON0_BLK_DPRX1_DSC_CLK_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(dprx1->PLL_CON0_BLK_DPRX1_DSC_CLK_MUX);

	/* DIVIDER setting */
	dprx1->CLK_CON_DIV_DIV_CLK_BLK_DPRX1_BUSD = DIV_VAL(3);
	dprx1->CLK_CON_DIV_DIV_CLK_BLK_DPRX1_BUSP = DIV_VAL(2);
	dprx1->CLK_CON_DIV_DIV_CLK_DPRX0_RXCLK = DIV_VAL(2);
	dprx1->CLK_CON_DIV_DIV_CLK_DPRX1_RXCLK = DIV_VAL(2);
	dprx1->CLK_CON_DIV_DIV_CLK_DPRX0_DSC_CLK = DIV_VAL(1);
	dprx1->CLK_CON_DIV_DIV_CLK_DPRX1_DSC_CLK = DIV_VAL(1);
}

#define DP_POWERDOWN \
		(!IS_ENABLED(CONFIG_USE_BLK_DPRX0) || \
		 !IS_ENABLED(CONFIG_USE_BLK_DPRX1))

#if DP_POWERDOWN
#define DP_SERDES_CTRL1	0xA134
#define DP_SERDES_CTRL1_PD_PHY	(1 << 3)
static void dp_powerdown(int dp)
{
	uint32_t val;
	uintptr_t dp_reg;

	/*
	DPRX block0:  TRAV_DPRX3, TRAV_DPRX2
	DPRX block1:  TRAV_DPRX1, TRAV_DPRX0
	*/

	/* DPRX0 / 2 */
	dp_reg = (dp ? TRAV_DPRX0 : TRAV_DPRX2) + DP_SERDES_CTRL1;
	val = readl(dp_reg) | DP_SERDES_CTRL1_PD_PHY;
	printk(BIOS_DEBUG, "writing %x to %llx\n", val, dp_reg);
	writel(val, dp_reg);

	/* DPRX1 / 3 */
	dp_reg = (dp ? TRAV_DPRX1 : TRAV_DPRX3) + DP_SERDES_CTRL1;
	val = readl(dp_reg) | DP_SERDES_CTRL1_PD_PHY;
	printk(BIOS_DEBUG, "writing %x to %llx\n", val, dp_reg);
	writel(val, dp_reg);
}
#endif

static void cmu_isp_init(void)
{
	/* PLL setting */
	isp->PLL_LOCKTIME_PLL_ISP = 0x000000ff;
	isp->PLL_CON0_PLL_ISP = CON0_VALUE(0x96, 0x3, 0);

	isp->PLL_CON1_PLL_ISP = 0x0000f380;

	/* DIVIDER setting */
	isp->CLK_CON_DIV_DIV_ISP_ACLK = DIV_VAL(2);
	isp->CLK_CON_DIV_DIV_ISP_PCLK = DIV_VAL(3);

	/* SWITCHING MUX */
	isp->PLL_CON0_ISP_TCUCLK_MUX = PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(isp->PLL_CON0_ISP_TCUCLK_MUX);

	/* Change source to PLL */
	WAIT_FOR_LOCKED(isp->PLL_CON0_PLL_ISP);
	isp->PLL_CON0_PLL_ISP |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(isp->PLL_CON0_PLL_ISP);
}

static void cmu_cmu_init(int cpu_rev)
{
	/* PLL_SHARED0 = 2000MHz */
	cmu->PLL_LOCKTIME_PLL_SHARED0 = 150*3;
	cmu->PLL_CON0_PLL_SHARED0 = CON0_VALUE(0xfa, 3, 0);

	/* PLL_SHARED1 = 2400MHz */
	cmu->PLL_LOCKTIME_PLL_SHARED1 = 150*2;
	cmu->PLL_CON0_PLL_SHARED1 = CON0_VALUE(0xc8, 2, 0);

	/* PLL_SHARED2 = 2400MHz */
	cmu->PLL_LOCKTIME_PLL_SHARED2 = 150*2;
	cmu->PLL_CON0_PLL_SHARED2 = CON0_VALUE(0xc8, 2, 0);

	/* PLL_SHARED3 = 1800MHz */
	cmu->PLL_LOCKTIME_PLL_SHARED3 = 150*2;
	cmu->PLL_CON0_PLL_SHARED3 = CON0_VALUE(0x96, 2, 0);

	cmu->PLL_CON1_PLL_SHARED0 = 0x0000f340;
	cmu->PLL_CON1_PLL_SHARED1 = 0x0000f340;
	cmu->PLL_CON1_PLL_SHARED2 = 0x0000f340;
	cmu->PLL_CON1_PLL_SHARED3 = 0x0000f340;

	/* Change divider */
	cmu->CLK_CON_DIV_CLKCMU_CMU_ACLK = DIV_VAL(4);
	cmu->CLK_CON_DIV_CLKCMU_CPUCL_SWITCH = DIV_VAL(3);
	cmu->CLK_CON_DIV_CLKCMU_FSYS0_SHARED0DIV4 = DIV_VAL(1);
	cmu->CLK_CON_DIV_CLKCMU_FSYS0_SHARED1DIV3 = DIV_VAL(1);
	cmu->CLK_CON_DIV_CLKCMU_FSYS0_SHARED1DIV4 = DIV_VAL(1);
	cmu->CLK_CON_DIV_CLKCMU_FSYS1_SHARED0DIV4 = DIV_VAL(1);
	cmu->CLK_CON_DIV_CLKCMU_FSYS1_SHARED0DIV8 = DIV_VAL(2);
	cmu->CLK_CON_DIV_CLKCMU_GPU_MAIN_SWITCH = DIV_VAL(8);
	cmu->CLK_CON_DIV_CLKCMU_DPRX_SWITCHCLK = DIV_VAL(1);
	cmu->CLK_CON_DIV_CLKCMU_DPRX_DSC_CLK = DIV_VAL(1);
	cmu->CLK_CON_DIV_CLKCMU_ISP_TCUCLK = DIV_VAL(1);
	cmu->CLK_CON_DIV_CLKCMU_IMEM_ACLK = DIV_VAL(1);
	cmu->CLK_CON_DIV_CLKCMU_IMEM_TCUCLK = DIV_VAL(1);
	cmu->CLK_CON_DIV_CLKCMU_PERIC_SHARED0DIV20 = DIV_VAL(5);
	cmu->CLK_CON_DIV_CLKCMU_PERIC_SHARED0DIV3_TBUCLK = DIV_VAL(1);
	cmu->CLK_CON_DIV_CLKCMU_PERIC_SHARED1DIV36 = DIV_VAL(1);
	cmu->CLK_CON_DIV_CLKCMU_TRIP_SWITCHCLK = DIV_VAL(1);
	cmu->CLK_CON_DIV_PLL_SHARED0_DIV2 = DIV_VAL(2);
	cmu->CLK_CON_DIV_PLL_SHARED0_DIV3 = DIV_VAL(3);
	cmu->CLK_CON_DIV_PLL_SHARED0_DIV4 = DIV_VAL(2);
	cmu->CLK_CON_DIV_PLL_SHARED0_DIV6 = DIV_VAL(2);
	cmu->CLK_CON_DIV_PLL_SHARED1_DIV3 = DIV_VAL(3);

	if (cpu_rev >= 0x10) /* B0 */
		cmu->CLK_CON_DIV_PLL_SHARED1_DIV36 = DIV_VAL(2);
	else
		cmu->CLK_CON_DIV_PLL_SHARED1_DIV36 = DIV_VAL(4);

	cmu->CLK_CON_DIV_PLL_SHARED1_DIV4 = DIV_VAL(4);
	cmu->CLK_CON_DIV_PLL_SHARED1_DIV9 = DIV_VAL(3);
	cmu->CLK_CON_DIV_CLKCMU_PERIC_SHARED1DIV4_DMACLK = DIV_VAL(1);
	cmu->CLK_CON_DIV_CLKCMU_CIS0_CLK = DIV_VAL(10);
	cmu->CLK_CON_DIV_CLKCMU_CIS1_CLK = DIV_VAL(10);
	cmu->CLK_CON_DIV_CLKCMU_CIS2_CLK = DIV_VAL(10);
	cmu->CLK_CON_DIV_CLKCMU_IMEM_DMACLK = DIV_VAL(6);

	/* Change MUXes */
	cmu->CLK_CON_MUX_CLKCMU_FSYS1_ACLK_MUX = MUX_SEL(0);
	WAIT_FOR_MUX_SET_DONE(cmu->CLK_CON_MUX_CLKCMU_FSYS1_ACLK_MUX);
	cmu->CLK_CON_MUX_CLKCMU_GPU_SWITCHCLK_MUX = MUX_SEL(0);
	WAIT_FOR_MUX_SET_DONE(cmu->CLK_CON_MUX_CLKCMU_GPU_SWITCHCLK_MUX);
	cmu->CLK_CON_MUX_CLKCMU_TRIP_SWITCHCLK_MUX = MUX_SEL(0);
	WAIT_FOR_MUX_SET_DONE(cmu->CLK_CON_MUX_CLKCMU_TRIP_SWITCHCLK_MUX);
	cmu->CLK_CON_MUX_CLKCMU_DPRX_SWITCHCLK_MUX = MUX_SEL(0);
	WAIT_FOR_MUX_SET_DONE(cmu->CLK_CON_MUX_CLKCMU_DPRX_SWITCHCLK_MUX);
	cmu->CLK_CON_MUX_PLL_SHARED0_MUX = MUX_SEL(1);
	WAIT_FOR_MUX_SET_DONE(cmu->CLK_CON_MUX_PLL_SHARED0_MUX);
	cmu->CLK_CON_MUX_PLL_SHARED1_MUX = MUX_SEL(1);
	WAIT_FOR_MUX_SET_DONE(cmu->CLK_CON_MUX_PLL_SHARED1_MUX);
	cmu->CLK_CON_MUX_CLKCMU_CPUCL_SWITCHMUX = MUX_SEL(1);
	WAIT_FOR_MUX_SET_DONE(cmu->CLK_CON_MUX_CLKCMU_CPUCL_SWITCHMUX);
	cmu->CLK_CON_MUX_CLKCMU_CIS0_CLKMUX = MUX_SEL(1);
	WAIT_FOR_MUX_SET_DONE(cmu->CLK_CON_MUX_CLKCMU_CIS0_CLKMUX);
	cmu->CLK_CON_MUX_CLKCMU_CIS1_CLKMUX = MUX_SEL(1);
	WAIT_FOR_MUX_SET_DONE(cmu->CLK_CON_MUX_CLKCMU_CIS1_CLKMUX);
	cmu->CLK_CON_MUX_CLKCMU_CIS2_CLKMUX = MUX_SEL(1);
	WAIT_FOR_MUX_SET_DONE(cmu->CLK_CON_MUX_CLKCMU_CIS2_CLKMUX);

	/* Change source to PLL */
	WAIT_FOR_LOCKED(cmu->PLL_CON0_PLL_SHARED0);
	cmu->PLL_CON0_PLL_SHARED0 |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(cmu->PLL_CON0_PLL_SHARED0);

	WAIT_FOR_LOCKED(cmu->PLL_CON0_PLL_SHARED1);
	cmu->PLL_CON0_PLL_SHARED1 |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(cmu->PLL_CON0_PLL_SHARED1);

	WAIT_FOR_LOCKED(cmu->PLL_CON0_PLL_SHARED2);
	cmu->PLL_CON0_PLL_SHARED2 |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(cmu->PLL_CON0_PLL_SHARED2);

	WAIT_FOR_LOCKED(cmu->PLL_CON0_PLL_SHARED3);
	cmu->PLL_CON0_PLL_SHARED3 |= PLL_MUX_SEL(1);
	WAIT_FOR_PLL_MUX_SET_DONE(cmu->PLL_CON0_PLL_SHARED3);
}

int clock_system_early_init(void)
{
	cmu_imem_init();
	cmu_peric_init();
	return 0;
}

int clock_system_early_ufs_init(int cpu_rev)
{
	cmu_cmu_init(cpu_rev);
	if (IS_ENABLED(CONFIG_USE_BLK_FSYS0))
		cmu_fsys0_init();
	return 0;
}

int clock_system_init(unsigned int memclk, unsigned int cpucl_clk, unsigned int gpuclk, unsigned int tripclk, int cpu_rev)
{
	cmu_cpucl_init(cpucl_clk);
	cmu_mif_init(mif0);
	cmu_mif_init(mif1);
	cmu_mif_init(mif2);
	cmu_mif_init(mif3);
	cmu_mif_init(mif4);
	cmu_mif_init(mif5);
	cmu_mif_init(mif6);
	cmu_mif_init(mif7);
	cmu_core_init(memclk);
	cmu_cmu_init(cpu_rev);

	if (IS_ENABLED(CONFIG_USE_BLK_GPU))
		cmu_gpu_init(gpuclk);
	if (IS_ENABLED(CONFIG_USE_BLK_TRIP0))
		cmu_trip_init(trip0, tripclk);
	if (IS_ENABLED(CONFIG_USE_BLK_TRIP1))
		cmu_trip_init(trip1, tripclk);
	if (IS_ENABLED(CONFIG_USE_BLK_FSYS0))
		cmu_fsys0_init();
	if (IS_ENABLED(CONFIG_USE_BLK_FSYS1))
		cmu_fsys1_init();
	if (IS_ENABLED(CONFIG_USE_BLK_MFC))
		cmu_mfc_init();
	if (IS_ENABLED(CONFIG_USE_BLK_CSI))
		cmu_csi_init();
	if (IS_ENABLED(CONFIG_USE_BLK_DPRX1))
		cmu_dprx1_init();
	else
		dp_powerdown(1);
	if (IS_ENABLED(CONFIG_USE_BLK_DPRX0))
		cmu_dprx0_init();
	else
		dp_powerdown(0);
	if (IS_ENABLED(CONFIG_USE_BLK_ISP))
		cmu_isp_init();
	return 0;
}
