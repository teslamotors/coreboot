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
#include <delay.h>
#include <arch/io.h>
#include <console/console.h>

#include <soc/ufs.h>
#include <soc/ufs_unipro.h>
#include <soc/ufs_trav_reg.h>
#include <soc/ufs_hci.h>

void ufs_wait_int(struct ufs_host *host, u32 int_mask);
u32 UFS_READ_VS_IS(struct ufs_host *host);
void UFS_CLEAR_INT(struct ufs_host *host, u32 intr);
void UFS_CLEAR_VS_INT(struct ufs_host *host, u32 intr);

void dev_init_pmu(struct ufs_host *host)
{
	if ((__raw_readl(0x11400724) & 1) == 0)
		__raw_writel(0x01, 0x11400724);
}

void dev_init_gpio(struct ufs_host *host)
{
	__raw_writel(0x00331300, 0x15020088);
	__raw_writel(0x00222222, 0x15020080);
}

void dev_init_clock(struct ufs_host *host)
{
	u32 pll_locktime_pll_shared0;
	u32 pll_con0_pll_shared0;
	u32 pll_locktime_pll_shared1;
	u32 pll_con0_pll_shared1;

	pll_locktime_pll_shared0 = 0x11c10000;
	pll_con0_pll_shared0 = 0x11c10100;
	pll_locktime_pll_shared1 = 0x11c10004;
	pll_con0_pll_shared1 = 0x11c10120;

	__raw_writel(0x000000ff, pll_locktime_pll_shared0);
	__raw_writel(0x80fa0300, pll_con0_pll_shared0);
	__raw_writel(0x80fa0310, pll_con0_pll_shared0);
	__raw_writel(0x000000ff, pll_locktime_pll_shared1);
	__raw_writel(0x80c80200, pll_con0_pll_shared1);
	__raw_writel(0x0000f340, 0x11c10104); //pll_con1_pll_shared0
	__raw_writel(0x0000f340, 0x11c10124); //pll_con1_pll_shared1
	__raw_writel(0x80c80210, 0x11c10120);//pll_con0_pll_shared1
	__raw_writel(0x000000ff, 0x11c10008);//pll_locktime_pll_shared2
	__raw_writel(0x00000002, 0x11c1185c);//clk_con_div_pll_shared0_div3
	__raw_writel(0x00000001, 0x11c11864);//clk_con_div_pll_shared0_div6
	__raw_writel(0x00000003, 0x11c11870);//clk_con_div_pll_shared1_div4
	__raw_writel(0x00000010, 0x15010100);//pll_con0_clkcmu_fsys0_unipro
	__raw_writel(0x00000010, 0x15010140);//pll_con0_clk_fsys0_slavebusclk
	__raw_writel(0x00000001, 0x15011800);//clk_con_div_div_clk_unipro
	__raw_writel(0x00000001, 0x15011808);//clk_con_div_div_peribus_grp
	__raw_writel(0x00000004, 0x15011814);//clk_con_div_pcie_phy_oscclk
}

void hci_sw_reset(struct ufs_host *host)
{
	UFS_WRITE(UFSHCI_VS_SW_RST, 0x3);
	while (UFS_READ(UFSHCI_VS_SW_RST) != 0)
		;
}

void hci_idle_indicate(struct ufs_host *host)
{
	u32 reg;

	reg = UFS_READ(UFSHCI_VS_IS);
	if (GET_BITS(reg, 20, 0x01))
		UFS_WRITE(UFSHCI_VS_IS, reg);
}

void hci_enable(struct ufs_host *host, bool enable)
{
	UFS_WRITE(UFSHCI_HCE, enable);

	while (UFS_READ(UFSHCI_HCE) != enable)
		;
}

void hci_device_reset_control(struct ufs_host *host, bool enable)
{
	UFS_WRITE(UFSHCI_VS_GPIO_OUT, (enable));
}

void hci_device_refclkout_control(struct ufs_host *host, bool enable)
{
	u32 value;

	value = UFS_READ(UFSHCI_VS_CLKSTOP_CTRL) & (~(REFCLKOUT_STOP_EN));
	value |= ((~enable) << REFCLKOUT_STOP_EN_BIT_POS);

	UFS_WRITE(UFSHCI_VS_CLKSTOP_CTRL, value);

	UFS_WRITE(UFSHCI_VS_MISC, (UFS_READ(UFSHCI_VS_MISC) | (MISC_REFCLKOUT_STOP_EN)));
}

void hci_vs_dfes_error_layber_enable(struct ufs_host *host)
{
	UFS_WRITE(UFSHCI_VS_DEFS_ERROR_EN_DL_LAYER, 0x80002060);
	UFS_WRITE(UFSHCI_VS_DEFS_ERROR_EN_N_LAYER, 0x80000007);
	UFS_WRITE(UFSHCI_VS_DEFS_ERROR_EN_T_LAYER, 0x8000007F);
}

void hci_vs_set_burst_len(struct ufs_host *host, u8 nval)
{
	u32 burst_length = 0;

	if (host->wlu_enable)
		burst_length |= WLU_ENABLE;

	burst_length = burst_length | ((nval << WLU_BUSRT_SIZE_BIT_POS)
			| (nval << AXIDMA_BURST_SIZE_BIT_POS));

	UFS_WRITE(UFSHCI_VS_AXIDMA_RWDATA_BURST_LEN, burst_length);
}

void ufs_vs_set_dfes_ocs_enable(struct ufs_host *host, u32 val)
{
	u32 nVal;

	nVal = UFS_READ(UFSHCI_VS_UFSHCI_V2P1_CTRL);
	nVal = (nVal & (~UFSHCI_VS_DFES_OCS_EN)) | (val & 0x01);
	UFS_WRITE(UFSHCI_VS_UFSHCI_V2P1_CTRL, nVal);
}

void ufshic_set_interrupt(struct ufs_host *host)
{
	host->int_enable = UFSHCI_IE_UTRCSE|
		UFSHCI_IE_UDEPRIE|
		UFSHCI_IE_UEE|
		UFSHCI_IE_UTMSE|
		UFSHCI_IE_UPMSE|
		UFSHCI_IE_UHXSE|
		UFSHCI_IE_UHESE|
		UFSHCI_IE_ULLSE|
		UFSHCI_IE_ULSSE|
		UFSHCI_IE_UTMRCSE|
		UFSHCI_IE_UCCSE|
		UFSHCI_IE_UFESE|
		UFSHCI_IE_UTPES|
		UFSHCI_IE_HCFESE|
		UFSHCI_IE_SBFESE|
		UFSHCI_IE_CEFES;
}

void ufshic_set_vs_interrupt(struct ufs_host *host)
{
	host->int_vs_enable = UFSHCI_VS_IE_UTRL0_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL1_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL2_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL3_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL4_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL5_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL6_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL7_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL8_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL9_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL10_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL11_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL12_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL13_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL14_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_UTRL15_RESP_UPIU_ACCEPT_wPHASE|
		UFSHCI_VS_IE_RX_UPIU_LEN_ERROR|
		UFSHCI_VS_IE_RX_UPIU_HIT_ERROR|
		UFSHCI_VS_IE_UFS_IDLE_INDICATOR|
		UFSHCI_VS_IE_SMU_MISMATCH|
		UFSHCI_VS_IE_UFS_PM_INT|
		UFSHCI_VS_IE_FMP_FAULT|
		UFSHCI_VS_IE_DBR_TIMER_INT|
		UFSHCI_VS_IE_DBR_DUPLICATION_INT|
		UFSHCI_VS_IE_MPHY_PCS_INT|
		UFSHCI_VS_IE_UNIPRO_INTERRUPT;
}

inline void ufs_interrupt_enable(struct ufs_host *host, u32 unmask)
{
	UFS_WRITE(UFSHCI_IE, UFS_READ(UFSHCI_IE) | unmask);
}

void ufs_vs_interrupt_enable(struct ufs_host *host, u32 unmask)
{
	UFS_WRITE(UFSHCI_VS_IE, UFS_READ(UFSHCI_VS_IE) | unmask);
}

void ufs_wait_int(struct ufs_host *host, u32 int_mask)
{
	while ((UFS_READ(UFSHCI_IS) & int_mask) != int_mask)
		udelay(10);

	UFS_WRITE(UFSHCI_IS, int_mask);

	while ((UFS_READ(UFSHCI_IS) & int_mask) != 0)
		udelay(10);
}

int ufshci_wait_complete(struct ufs_host *host, u32 int_mask)
{
	u32 vs_status;
	u32 reg;
	u32 res;
	u32 timeout = 10000;

	while ((UFS_READ(UFSHCI_IS) & int_mask) != int_mask && --timeout)
		udelay(10);
	vs_status = UFS_READ_VS_IS(host);
	reg =  UFS_READ(UFSHCI_IS);
	if (!timeout)
		printk(BIOS_ERR, "%s: UFSHCI_IS: %#x, vs_status: %#x, "
				 "mask: %#x\n",
				__func__, reg, vs_status, int_mask);

	UFS_CLEAR_INT(host, reg);
	UFS_CLEAR_VS_INT(host, vs_status);

	timeout = 1000;
	while ((UFS_READ(UFSHCI_IS) & int_mask) != 0 && --timeout)
		udelay(10);
	if (!timeout)
		printk(BIOS_ERR, "UFSHCI_IS: timeout again!!!\n");
	res = ufshci_check_is(host, reg, vs_status);
	return res;
}

u32 UFS_READ_VS_IS(struct ufs_host *host)
{
	if (host->irq_mode)
		return host->int_vs_status;
	else
		return UFS_READ(UFSHCI_VS_IS);
}

void UFS_WAIT_CLEAR_INT(struct ufs_host *host, u32 intr)
{
	ufs_wait_int(host, intr);
}

void UFS_CLEAR_INT(struct ufs_host *host, u32 intr)
{
	if (host->irq_mode)
		host->int_status &= ~intr;
	else
		UFS_WRITE(UFSHCI_IS, intr);
}

void UFS_CLEAR_VS_INT(struct ufs_host *host, u32 intr)
{
	if (host->irq_mode)
		host->int_vs_status &= ~intr;
	else
		UFS_WRITE(UFSHCI_VS_IS, intr);
}

bool ufshci_check_is(struct ufs_host *host, u32 status, u32 vs_status)
{
	bool res = true;

	if (status & UFSHCI_IS_UDEPRI) {
		UFS_CLEAR_INT(host, UFSHCI_IS_UDEPRI);
		if (host->debug)
			printk(BIOS_ERR, "U UDEPRI\n");
	}
	if (status & UFSHCI_IS_UE) {
		UFS_CLEAR_INT(host, UFSHCI_IS_UE);
		if (host->debug)
			printk(BIOS_ERR, "U UE\n");
		//UFS_check_err_cnt(host);
		res = true;
	}
	if (status & UFSHCI_IS_UTMS) {
		UFS_CLEAR_INT(host, UFSHCI_IS_UTMS);
		if (host->debug)
			printk(BIOS_ERR, "U UTMS\n");
	}
	if (status & UFSHCI_IS_UPMS) {
		UFS_CLEAR_INT(host, UFSHCI_IS_UPMS);
		if (host->debug)
			printk(BIOS_ERR, "U UPMS\n");
	}
	if (status & UFSHCI_IS_UHXS) {
		UFS_CLEAR_INT(host, UFSHCI_IS_UHXS);
		if (host->debug)
			printk(BIOS_ERR, "U UHXS\n");
	}
	if (status & UFSHCI_IS_UHES) {
		UFS_CLEAR_INT(host, UFSHCI_IS_UHES);
		if (host->debug)
			printk(BIOS_ERR, "U UHES\n");
	}
	if (status & UFSHCI_IS_ULLS) {
		UFS_CLEAR_INT(host, UFSHCI_IS_ULLS);
		if (host->debug)
			printk(BIOS_ERR, "U ULLS\n");
	}
	if (status & UFSHCI_IS_ULSS) {
		UFS_CLEAR_INT(host, UFSHCI_IS_ULSS);
		if (host->debug)
			printk(BIOS_ERR, "U ULSS\n");
	}
	if (status & UFSHCI_IS_UTMRCS) {
		UFS_CLEAR_INT(host, UFSHCI_IS_UTMRCS);
		if (host->debug)
			printk(BIOS_ERR, "U UTMRCS\n");
	}
	if (status & UFSHCI_IS_UCCS) {
		UFS_CLEAR_INT(host, UFSHCI_IS_UCCS);
		if (host->debug)
			printk(BIOS_ERR, "U UCCS\n");
	}
	if (status & UFSHCI_IS_DFES) {
		UFS_CLEAR_INT(host, UFSHCI_IS_DFES);
		if (host->debug)
			printk(BIOS_ERR, "U DFES\n");
		res = false;
	}
	if (status & UFSHCI_IS_UTPES) {
		UFS_CLEAR_INT(host, UFSHCI_IS_UTPES);
		if (host->debug)
			printk(BIOS_ERR, "U UTPES\n");
		res = false;
	}
	if (status & UFSHCI_IS_HCFES) {
		UFS_CLEAR_INT(host, UFSHCI_IS_HCFES);
		if (host->debug)
			printk(BIOS_ERR, "U HCFES\n");
		res = false;
	}
	if (status & UFSHCI_IS_SBFES) {
		UFS_CLEAR_INT(host, UFSHCI_IS_SBFES);
		if (host->debug)
			printk(BIOS_ERR, "U SBFES\n");
		res = false;
	}

	//vendor specific IS
	if (vs_status & UFSHCI_VS_IS_RX_UPIU_LEN_ERROR) {
		UFS_CLEAR_VS_INT(host, UFSHCI_VS_IS_RX_UPIU_LEN_ERROR);
		if (host->debug)
			printk(BIOS_ERR, "U RX_UPIU_LEN_ERROR\n");
		res = false;
	}
	if (vs_status & UFSHCI_VS_IS_RX_UPIU_HIT_ERROR) {
		UFS_CLEAR_VS_INT(host, UFSHCI_VS_IS_RX_UPIU_HIT_ERROR);

		if (host->debug) {
			u32 hit_error;

			printk(BIOS_ERR, "U RX_UPIU_HIT_ERROR\n");

			printk(BIOS_ERR, "RX UPIU hit error : ");
			hit_error = UFS_READ(UFSHCI_VS_RS_FMPIU_MATCH_ERROR_CODE);

			if (hit_error & 0x1)
				printk(BIOS_ERR, "UTR_RESP\n");
			if (hit_error & 0x2)
				printk(BIOS_ERR, "UTMR_RESP\n");
			if (hit_error & 0x4)
				printk(BIOS_ERR, "RX DATA\n");
			if (hit_error & 0x8)
				printk(BIOS_ERR, "RTT\n");
		}
		res = false;
	}
	if (vs_status & UFSHCI_VS_IS_UFS_IDLE_INDICATOR) {
		UFS_CLEAR_VS_INT(host, UFSHCI_VS_IS_UFS_IDLE_INDICATOR);
		if (host->debug)
			printk(BIOS_ERR, "U UFS_IDLE_INDICATOR\n");
	}
	if (vs_status & UFSHCI_VS_IS_SMU_MISMATCH) {
		UFS_CLEAR_VS_INT(host, UFSHCI_VS_IS_SMU_MISMATCH);
		if (host->debug)
			printk(BIOS_ERR, "U SMU_MISMATCH\n");
		res = false;
	}
	if (vs_status &  UFSHCI_VS_IS_UFS_PM_INT) {
		UFS_CLEAR_VS_INT(host, UFSHCI_VS_IS_UFS_PM_INT);
		if (host->debug)
			printk(BIOS_ERR, "U UFS_PM_INT\n");
		res = false;
	}
	if (vs_status & UFSHCI_VS_IS_FMP_FAULT) {
		UFS_CLEAR_VS_INT(host, UFSHCI_VS_IS_FMP_FAULT);
		if (host->debug)
			printk(BIOS_ERR, "U FMP_FAULT\n");
		res = false;
	}
	if (vs_status & UFSHCI_VS_IS_DBR_TIMER_INT) {
		UFS_CLEAR_VS_INT(host, UFSHCI_VS_IS_DBR_TIMER_INT);
		if (host->debug)
			printk(BIOS_ERR, "U DBR_TIMER_INT\n");
	}
	if (vs_status & UFSHCI_VS_IS_DBR_DUPLICATION_INT) {
		UFS_CLEAR_VS_INT(host, UFSHCI_VS_IS_DBR_DUPLICATION_INT);
		if (host->debug)
			printk(BIOS_ERR, "U DBR_DUPLICATION_INT\n");
	}
	if (vs_status & UFSHCI_VS_IS_MPHY_PCS_INT) {
		UFS_CLEAR_VS_INT(host, UFSHCI_VS_IS_MPHY_PCS_INT);
		if (host->debug)
			printk(BIOS_ERR, "U MPHY_PCS_INT\n");
	}
	if (vs_status & UFSHCI_VS_IS_UNIPRO_INTERRUPT) {
		UFS_CLEAR_VS_INT(host, UFSHCI_VS_IS_UNIPRO_INTERRUPT);
		if (host->debug)
			printk(BIOS_ERR, "U UNIPRO_INTERRUPT\n");
	}
	return res;
}




bool fmp_set_uprsecurity(struct ufs_host *host, ufsp_upsec eUPSEC, u32 uInput)
{
	u32 uReg;
	bool bRet = false;

	uReg = UFS_FMP_READ(UPRSECURITY);

	switch (eUPSEC) {
	case upsec_awprotdata:
	case upsec_awprotdesc:
	case upsec_arprotdata:
	case upsec_arprotdesc:
		uReg = uReg & ~(0x7 << eUPSEC);
		uReg = (uReg | ((uInput & 0x7) << eUPSEC));
		bRet = true;
			break;
	case upsec_useotpmask:
	case upsec_nsuser:
	case upsec_nssmu:
	case upsec_nskeyreg:
	case upsec_fmp_on:
	case upsec_protbytzpc:
	case upsec_unmap_smu:
	case upsec_cs:
		uReg = uReg & ~(0x1 << eUPSEC);
		uReg = (uReg | ((uInput & 0x1) << eUPSEC));
		bRet = true;
		break;
	case upsec_arprotprdt:
		uReg = uReg & ~(0x7 << eUPSEC);
		uReg = (uReg | ((uInput & 0x7) << eUPSEC));
		bRet = true;
		break;
	case upsec_desctype:
		uReg = uReg & ~(0x3 << eUPSEC);
		uReg = (uReg | ((uInput & 0x3) << eUPSEC));
		bRet = true;
		break;
	default:
		printk(BIOS_ERR, "\n Wrong Input!! (UFSP_Set_UPRSECURITY-2)");
		while (1)
		;
		break;
	}

	UFS_FMP_WRITE(UPRSECURITY, uReg);
	return bRet;
}

void ufs_vs_dma_blocking_enable(struct ufs_host *host, bool enable)
{
	if (enable)
		UFS_WRITE(UFSHCI_VS_AXI_DMA_IF_CTRL, UFS_READ(UFSHCI_VS_AXI_DMA_IF_CTRL) | 0x01);
	else
		UFS_WRITE(UFSHCI_VS_AXI_DMA_IF_CTRL, UFS_READ(UFSHCI_VS_AXI_DMA_IF_CTRL) & 0xfe);
}
void ufs_vs_set_1us_to_cnt(struct ufs_host *host, u32 nsec)
{
	u32 nVal;

	nVal = UFS_READ(UFSHCI_VS_UFSHCI_V2P1_CTRL);
	nVal |= UFSHCI_VS_IA_TICK_SEL;
	UFS_WRITE(UFSHCI_VS_UFSHCI_V2P1_CTRL, nVal);
	UFS_WRITE(UFSHCI_VS_1US_TO_CNT_VAL_1US, 0xa6); //bharat added

}

void ufs_vs_set_prdt(struct ufs_host *host)
{
	u32 prdt;
	/* DMA little endian, order change */
	prdt = find_one_in_bitmap(ufs_get_config_prdt_size(host));
	UFS_WRITE(UFSHCI_VS_TXPRDT_ENTRY_SIZE, 0x80000000ul | prdt);
	UFS_WRITE(UFSHCI_VS_RXPRDT_ENTRY_SIZE, prdt);
}

void ufs_vs_init_nexus_type(struct ufs_host *host)
{

	UFS_WRITE(UFSHCI_VS_UTRL_NEXUS_TYPE, 0xFFFFFFFF);
	UFS_WRITE(UFSHCI_VS_UMTRL_NEXUS_TYPE, 0xFFFFFFFF);
}

void ufs_vs_set_v2p1_ctrl(struct ufs_host *host, u32 nval)
{
	/* v2p1_ctrl */
	UFS_WRITE(UFSHCI_VS_UFSHCI_V2P1_CTRL, nval);
}
void ufs_vs_data_reorder(struct ufs_host *host, u32 nval)
{

	/* DMA little endian, order change */
	UFS_WRITE(UFSHCI_VS_DATA_REORDER, nval);
}
void ufs_set_utr_nexus_type(struct ufs_host *host)
{
	u32 db;

	db = ufs_get_doorbell_num(host);

	UFS_WRITE(UFSHCI_VS_UTRL_NEXUS_TYPE, UFS_READ(UFSHCI_VS_UTRL_NEXUS_TYPE) | (0x01ul << db));
}

void ufs_clear_utr_nexus_type(struct ufs_host *host)
{
	u32 db;

	db = ufs_get_doorbell_num(host);

	UFS_WRITE(UFSHCI_VS_UTRL_NEXUS_TYPE, UFS_READ(UFSHCI_VS_UTRL_NEXUS_TYPE) & ~(0x01ul << db));
}

bool set_mib(struct ufs_host *host, u32 mib_addr, u32 mib, u32 lane)
{
	u32 result;

	UFS_WRITE(UFSHCI_UCMDARG1, ((((mib_addr) & 0xFFFF) << 16) | ((lane) & 0xFFFF)));
	UFS_WRITE(UFSHCI_UCMDARG3, mib);
	UFS_WRITE(UFSHCI_UICCMDR, H_UIC_DME_SET);
	ufs_wait_int(host, H_UFS_INTR_GETSET_CNF);
	result = UFS_READ(UFSHCI_UCMDARG2);

	if (result)
		return false;
	else
		return true;
	return true;
}

s32 get_mib(struct ufs_host *host, u32 mib_addr, u32 lane)
{
	s32 mib = 0;

	if (host->mib_direct_access == 0) {
		u32 result;

		UFS_WRITE(UFSHCI_UCMDARG1, (mib_addr << 16) | lane);
		UFS_WRITE(UFSHCI_UICCMDR, H_UIC_DME_GET);
		ufs_wait_int(host, H_UFS_INTR_GETSET_CNF);
		mib = UFS_READ(UFSHCI_UCMDARG3);
		result = UFS_READ(UFSHCI_UCMDARG2);
		if (result != 0)
			mib = -1;
		//DEBUG_UFSH_FUNC(("\n[Success] Hostget_mib: Value = 0x%x", mib));
	}
	return mib;
}
