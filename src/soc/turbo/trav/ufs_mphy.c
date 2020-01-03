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
#include <console/console.h>
#include <soc/ufs.h>
#include <soc/ufs_hci.h>
#include <soc/ufs_unipro.h>
#include <soc/ufs_mphy.h>
#include <soc/ufs_trav_reg.h>

static const struct ufs_phy_cfg init_cfg[] = {
	{0x9514, 0x0A, PMD_ALL, UNIPRO_DBG_PRD},
	{0x201, 0x12, PMD_ALL, PHY_PCS_COMN},
	{0x200, 0x40, PMD_ALL, PHY_PCS_COMN},
	{TX_EQUALIZER_SETTING_CAPABILITY, 0x06, PMD_ALL, PHY_PCS_RX_PRD},//0x12
	{0xAA, 0x06, PMD_ALL, PHY_PCS_TX_PRD},
	{RX_MIN_ACTIVATETIME_CAPABILITY, 0x3F, PMD_ALL, PHY_PCS_TX},//0x8F
	{0x5C, 0x38, PMD_ALL, PHY_PCS_RX},
	{TX_HIBERN8TIME_CAPABILITY, 0x0, PMD_ALL, PHY_PCS_RX},//0x0F
	{0x65, 0x01, PMD_ALL, PHY_PCS_RX},
	{0x69, 0x01, PMD_ALL, PHY_PCS_RX},
	{TX_MODE, 0x00, PMD_ALL, PHY_PCS_RX},
	{TX_HSRATE_SERIES, 0x00, PMD_ALL, PHY_PCS_RX},
	{0x200, 0x0, PMD_ALL, PHY_PCS_COMN},
	{0x9536, 0x4E20, PMD_ALL, UNIPRO_DBG_MIB},
	{0x9564, 0x2e820183, PMD_ALL, UNIPRO_DBG_MIB},
	{0x155E, 0x0, PMD_ALL, UNIPRO_STD_MIB},
	{N_DeviceID, 0, PMD_ALL, UNIPRO_STD_MIB},
	{N_DeviceID_valid, 1, PMD_ALL, UNIPRO_STD_MIB},
	{T_PeerDeviceID, 1, PMD_ALL, UNIPRO_STD_MIB},
	{T_ConnectionState, 1, PMD_ALL, UNIPRO_STD_MIB},
	//PMA INIT
	{0x3C, 0xFA, PMD_ALL, PHY_PMA_COMN},
	{0x40, 0x96, PMD_ALL, PHY_PMA_COMN},
	{0x44, 0x14, PMD_ALL, PHY_PMA_COMN},
	{0x5C, 0x94, PMD_ALL, PHY_PMA_COMN},
	{0xC4, 0xC4, PMD_ALL, PHY_PMA_TRSV},
	{0xC8, 0xC4, PMD_ALL, PHY_PMA_TRSV},
	{0xCC, 0x80, PMD_ALL, PHY_PMA_TRSV},
	{0xD0, 0x25, PMD_ALL, PHY_PMA_TRSV},
	{0xD4, 0x40, PMD_ALL, PHY_PMA_TRSV},
	{0xD8, 0x0, PMD_ALL, PHY_PMA_TRSV},
	{0xDC, 0x3, PMD_ALL, PHY_PMA_TRSV},
	{0x108, 0x38, PMD_ALL, PHY_PMA_TRSV},
	{0x10C, 0xa4, PMD_ALL, PHY_PMA_TRSV},
	{0x120, 0x69, PMD_ALL, PHY_PMA_TRSV},
	{0x124, 0x3, PMD_ALL, PHY_PMA_TRSV},
	{0x128, 0x10, PMD_ALL, PHY_PMA_TRSV},
	{0x130, 0x1b, PMD_ALL, PHY_PMA_TRSV},
	{0x134, 0x1, PMD_ALL, PHY_PMA_TRSV},
	{0x14c, 0x0, PMD_ALL, PHY_PMA_TRSV},
	{0x160, 0x1, PMD_ALL, PHY_PMA_TRSV},
	{0x170, 0x14, PMD_ALL, PHY_PMA_TRSV},
	{0x1b4, 0x0, PMD_ALL, PHY_PMA_TRSV},
	{0x1bc, 0x0, PMD_ALL, PHY_PMA_TRSV},
	// Lane 2 settings
	{0x1C4, 0xC4, PMD_ALL, PHY_PMA_TRSV},
	{0x1C8, 0xC4, PMD_ALL, PHY_PMA_TRSV},
	{0x1CC, 0x80, PMD_ALL, PHY_PMA_TRSV},
	{0x1D0, 0x25, PMD_ALL, PHY_PMA_TRSV},
	{0x1D4, 0x40, PMD_ALL, PHY_PMA_TRSV},
	{0x1D8, 0x0, PMD_ALL, PHY_PMA_TRSV},
	{0x1DC, 0x3, PMD_ALL, PHY_PMA_TRSV},
	{0x208, 0x38, PMD_ALL, PHY_PMA_TRSV},
	{0x20C, 0xa4, PMD_ALL, PHY_PMA_TRSV},
	{0x220, 0x69, PMD_ALL, PHY_PMA_TRSV},
	{0x224, 0x3, PMD_ALL, PHY_PMA_TRSV},
	{0x228, 0x10, PMD_ALL, PHY_PMA_TRSV},
	{0x230, 0x1b, PMD_ALL, PHY_PMA_TRSV},
	{0x234, 0x1, PMD_ALL, PHY_PMA_TRSV},
	{0x24c, 0x0, PMD_ALL, PHY_PMA_TRSV},
	{0x260, 0x1, PMD_ALL, PHY_PMA_TRSV},
	{0x270, 0x14, PMD_ALL, PHY_PMA_TRSV},
	{0x2b4, 0x0, PMD_ALL, PHY_PMA_TRSV},
	{0x2bc, 0x0, PMD_ALL, PHY_PMA_TRSV},
	{0x0, 3000, PMD_ALL, COMMON_WAIT},
	{},
};

static const struct ufs_phy_cfg post_cfg[] = {
	{0x200, 0x40, PMD_ALL, PHY_PCS_COMN},
	{0x35, 0x05, PMD_ALL, PHY_PCS_RX},
	{0x73, 0x01, PMD_ALL, PHY_PCS_RX},
	{0x41, 0x02, PMD_ALL, PHY_PCS_RX},
	{0x42, 0xAC, PMD_ALL, PHY_PCS_RX},
	{0x200, 0x00, PMD_ALL, PHY_PCS_COMN},
	{},
};

static struct ufs_phy_cfg calib_of_hs_rate_a[] = {
	{PA_TxTermination, 0x1, PMD_HS, UNIPRO_STD_MIB},
	{PA_RxTermination, 0x1, PMD_HS, UNIPRO_STD_MIB},
	{DL_FC0ProtTimeOutVal, 8064, PMD_HS, UNIPRO_STD_MIB},
	{DL_TC0ReplayTimeOutVal, 28224, PMD_HS, UNIPRO_STD_MIB},
	{DL_AFC0ReqTimeOutVal, 20160, PMD_HS, UNIPRO_STD_MIB},
	{PA_PWRModeUserData0, 12000, PMD_HS, UNIPRO_STD_MIB},
	{PA_PWRModeUserData1, 32000, PMD_HS, UNIPRO_STD_MIB},
	{PA_PWRModeUserData2, 16000, PMD_HS, UNIPRO_STD_MIB},
	{0x7888, 8064, PMD_HS, UNIPRO_DBG_APB},
	{0x788C, 28224, PMD_HS, UNIPRO_DBG_APB},
	{0x7890, 20160, PMD_HS, UNIPRO_DBG_APB},
	{0x78B8, 12000, PMD_HS, UNIPRO_DBG_APB},
	{0x78BC, 32000, PMD_HS, UNIPRO_DBG_APB},
	{0x78C0, 16000, PMD_HS, UNIPRO_DBG_APB},
	{},
};

static struct ufs_phy_cfg calib_of_hs_rate_b[] = {
	{PA_TxTermination, 0x1, PMD_HS, UNIPRO_STD_MIB},
	{PA_RxTermination, 0x1, PMD_HS, UNIPRO_STD_MIB},
	{DL_FC0ProtTimeOutVal, 8064, PMD_HS, UNIPRO_STD_MIB},
	{DL_TC0ReplayTimeOutVal, 28224, PMD_HS, UNIPRO_STD_MIB},
	{DL_AFC0ReqTimeOutVal, 20160, PMD_HS, UNIPRO_STD_MIB},
	{PA_PWRModeUserData0, 12000, PMD_HS, UNIPRO_STD_MIB},
	{PA_PWRModeUserData1, 32000, PMD_HS, UNIPRO_STD_MIB},
	{PA_PWRModeUserData2, 16000, PMD_HS, UNIPRO_STD_MIB},
	{0x7888, 0x02fff, PMD_HS, UNIPRO_DBG_APB},
	{0x788C, 0x02fff, PMD_HS, UNIPRO_DBG_APB},
	{0x7890, 0x02fff, PMD_HS, UNIPRO_DBG_APB},
	{0x78B8, 0x02fff, PMD_HS, UNIPRO_DBG_APB},
	{0x78BC, 0x02fff, PMD_HS, UNIPRO_DBG_APB},
	{0x78C0, 0x02fff, PMD_HS, UNIPRO_DBG_APB},
	{},
};

static struct ufs_phy_cfg post_calib_of_hs_rate_a[] = {
	{},
};

static struct ufs_phy_cfg post_calib_of_hs_rate_b[] = {
	{},
};

static u32 UFS_PMA_READ(struct ufs_host *host, u32 offset)
{
	u32 val;

	UFS_WRITE(UFSHCI_VS_CLKSTOP_CTRL, (UFS_READ(UFSHCI_VS_CLKSTOP_CTRL) & ~MPHY_APBCLK_STOP_EN));
	val = __raw_readl(host->mphy + (offset));
	UFS_WRITE(UFSHCI_VS_CLKSTOP_CTRL, (UFS_READ(UFSHCI_VS_CLKSTOP_CTRL) | MPHY_APBCLK_STOP_EN));
	return val;
}

static u32 UFS_PMA_WRITE(struct ufs_host *host, u32 offset, u32 val)
{
	UFS_WRITE(UFSHCI_VS_CLKSTOP_CTRL, (UFS_READ(UFSHCI_VS_CLKSTOP_CTRL) & ~MPHY_APBCLK_STOP_EN));
	__raw_writel((val), host->mphy + (offset));
	UFS_WRITE(UFSHCI_VS_CLKSTOP_CTRL, (UFS_READ(UFSHCI_VS_CLKSTOP_CTRL) | MPHY_APBCLK_STOP_EN));
	return 0;
}

void ufs_pre_link(struct ufs_host *host)
{
	ufs_config_phy(host, init_cfg);
}
void ufs_post_link(struct ufs_host *host)
{
	ufs_config_phy(host, post_cfg);
}

void ufs_pre_pmc_cfg(struct ufs_host *host)
{
	struct ufs_uic_pwr_mode *pInfo;

	pInfo = ufs_get_uic_pwr_mode(host);
	if (IS_PWR_MODE_HS(pInfo->pwr_mode)) {
		switch (pInfo->hs_series) {
		case 1:
			ufs_config_phy(host, calib_of_hs_rate_a); //Series A
			break;

		case 2:
			ufs_config_phy(host, calib_of_hs_rate_b); //Series B
			break;

		default:
			printk(BIOS_ERR, "\nWrong Sieries Info!!");
			break;
		}
	}
}

// cmn_trsv : 0(Common register), 1(Transceiver register)
u32 ufs_get_pma_addr(struct ufs_host *host, u32 addr, u32 lane, u32 cmn_trsv)
{
	u32 value;

	if (cmn_trsv == 0)
		value = UFS_PMA_READ(host, MPHY_PMA_CMN_ADDR(addr));
	else if (cmn_trsv == 1)
		value = UFS_PMA_READ(host, MPHY_PMA_TRSV_ADDR(addr, lane));
	else
		return -1;
	return value;
}
void ufs_post_pmc_cfg(struct ufs_host *host)
{
	struct ufs_uic_pwr_mode *pInfo;

	pInfo = ufs_get_uic_pwr_mode(host);

	if (IS_PWR_MODE_HS(pInfo->pwr_mode)) {

		switch (pInfo->hs_series) {
		case 1:
			ufs_config_phy(host, post_calib_of_hs_rate_a); //Series A
			ufs_get_pma_addr(host, CMN_REG01, 0, 0);
			break;
		case 2:
			ufs_config_phy(host, post_calib_of_hs_rate_b); //Series A
			break;
		default:
			printk(BIOS_ERR, "\nWrong Sieries Info!!");
			break;
		}

	}
}

bool UFS_MPHY_WaitMphyCdrLock(struct ufs_host *host, u32 tx_lane, u32 rx_lane)
{
	return true;
}
void set_TxRx_Gear(struct ufs_host *host, u32 gear)
{
	set_mib(host, PA_TxGear, gear, UNIPRO_CMD);
	set_mib(host, PA_RxGear, gear, UNIPRO_CMD);
}

void set_HSSeries(struct ufs_host *host, u32 rate)
{
	set_mib(host, PA_HSSeries, rate, UNIPRO_CMD);
}

void set_TxRx_Termination(struct ufs_host *host, u32 nVal)
{
	set_mib(host, PA_TxTermination, nVal, UNIPRO_CMD);
	set_mib(host, PA_RxTermination, nVal, UNIPRO_CMD);

}

bool set_PA_PWRMode(struct ufs_host *host, u32 pwr_mode)
{
	bool res;

	res = set_mib(host, PA_PWRMode, pwr_mode, UNIPRO_CMD);

	return res;
}
u32 read_unipro_powermode_ind_result(struct ufs_host *host)
{
	return UFS_UNIPRO_READ(DME_POWERMODE_IND_RESULT);
}

int ufs_set_after_pmc_done(struct ufs_host *host, bool hs_mode, u32 gear, u32 rate)
{
	u32 num_tx_lane, num_rx_lane;
	struct ufs_uic_pwr_mode *pInfo;

	pInfo = ufs_get_uic_pwr_mode(host);
	num_tx_lane = ufs_get_num_of_active_tx_lanes(host);
	num_rx_lane = ufs_get_num_of_active_rx_lanes(host);

	ufs_post_pmc_cfg(host);

	if (IS_PWR_MODE_HS(pInfo->pwr_mode)) {
		if (!UFS_MPHY_WaitMphyCdrLock(host, num_tx_lane, num_rx_lane))
			printk(BIOS_ERR, "\nUFS MPHY CDR Lock FAIL\n");
		else {
			if (PRINT_LOG)
				printk(BIOS_INFO, "\nUFS MPHY CDR Lock PASS\n");
		}
	}

	return true;
}

static inline bool __match_mode_by_cfg(struct ufs_host *host, u32 mode)
{
	bool match = false;
	u8 _m, _l, _g;
	struct ufs_uic_pwr_mode *pInfo;

	pInfo = ufs_get_uic_pwr_mode(host);

	_m = pInfo->pwr_mode;
	_g = pInfo->gear;
	_l = ufs_get_num_of_lanes(host);

	if (mode == PMD_ALL)
		match = true;
	else if (IS_PWR_MODE_HS(_m) && mode == PMD_HS)
		match = true;
	else if (IS_PWR_MODE_PWM(_m) && mode == PMD_PWM)
		match = true;
	else if (IS_PWR_MODE_HS(_m) && _g == 1 && _l == 1 && (mode & (PMD_HS_G1_L1|PMD_HS_G1_L2)))
		match = true;
	else if (IS_PWR_MODE_HS(_m) && _g == 1 && _l == 2 && (mode & (PMD_HS_G1_L1|PMD_HS_G1_L2)))
		match = true;
	else if (IS_PWR_MODE_HS(_m) && _g == 2 && _l == 1 && (mode & (PMD_HS_G2_L1|PMD_HS_G2_L2)))
		match = true;
	else if (IS_PWR_MODE_HS(_m) && _g == 2 && _l == 2 && (mode & (PMD_HS_G2_L1|PMD_HS_G2_L2)))
		match = true;
	else if (IS_PWR_MODE_HS(_m) && _g == 3 && _l == 1 && (mode & (PMD_HS_G3_L1|PMD_HS_G3_L2)))
		match = true;
	else if (IS_PWR_MODE_HS(_m) && _g == 3 && _l == 2 && (mode & (PMD_HS_G3_L1|PMD_HS_G3_L2)))
		match = true;
	else if (IS_PWR_MODE_PWM(_m) && _g == 1 && _l == 1 && (mode & (PMD_PWM_G1_L1|PMD_PWM_G1_L2)))
		match = true;
	else if (IS_PWR_MODE_PWM(_m) && _g == 1 && _l == 2 && (mode & (PMD_PWM_G1_L1|PMD_PWM_G1_L2)))
		match = true;
	else if (IS_PWR_MODE_PWM(_m) && _g == 2 && _l == 1 && (mode & (PMD_PWM_G2_L1|PMD_PWM_G2_L2)))
		match = true;
	else if (IS_PWR_MODE_PWM(_m) && _g == 2 && _l == 2 && (mode & (PMD_PWM_G2_L1|PMD_PWM_G2_L2)))
		match = true;
	else if (IS_PWR_MODE_PWM(_m) && _g == 3 && _l == 1 && (mode & (PMD_PWM_G3_L1|PMD_PWM_G3_L2)))
		match = true;
	else if (IS_PWR_MODE_PWM(_m) && _g == 3 && _l == 2 && (mode & (PMD_PWM_G3_L1|PMD_PWM_G3_L2)))
		match = true;
	else if (IS_PWR_MODE_PWM(_m) && _g == 4 && _l == 1 && (mode & (PMD_PWM_G4_L1|PMD_PWM_G4_L2)))
		match = true;
	else if (IS_PWR_MODE_PWM(_m) && _g == 4 && _l == 2 && (mode & (PMD_PWM_G4_L1|PMD_PWM_G4_L2)))
		match = true;
	else if (IS_PWR_MODE_PWM(_m) && _g == 5 && _l == 1 && (mode & (PMD_PWM_G5_L1|PMD_PWM_G5_L2)))
		match = true;
	else if (IS_PWR_MODE_PWM(_m) && _g == 5 && _l == 2 && (mode & (PMD_PWM_G5_L1|PMD_PWM_G5_L2)))
		match = true;

	return match;
}

void ufs_config_phy(struct ufs_host *host, const struct ufs_phy_cfg *cfg)
{
	u32 i;

	for_each_phy_cfg(cfg) {
		for_each_ufs_lane(host, i) {
			if (!__match_mode_by_cfg(host, cfg->flg))
				continue;

			switch (cfg->lyr) {
			case PHY_PCS_COMN:
			case UNIPRO_STD_MIB:
			case UNIPRO_DBG_MIB:
				if (i == 0)
					set_mib(host, cfg->addr, cfg->val, LANE_0/*==UNIPRO_CMD*/);
				break;
			case UNIPRO_DBG_PRD:
				if (i == 0) {
					u32 mclk_period;

					mclk_period = host->config->hci_config_info.mclk_period;
					set_mib(host, cfg->addr, mclk_period, UNIPRO_CMD);
				}
				break;

			case PHY_PCS_RXTX:
				break;

			case PHY_PCS_RX:
				set_mib(host, cfg->addr, cfg->val, RX_LANE_0 + (UFS_LANE)i);
				break;

			case PHY_PCS_TX:
				set_mib(host, cfg->addr, cfg->val, TX_LANE_0 + (UFS_LANE)i);
				break;

			case PHY_PCS_RX_PRD:
				{
					u32 pclk_period;

					pclk_period = host->config->hci_config_info.pclk_period;
					set_mib(host, cfg->addr, pclk_period, RX_LANE_0 + (UFS_LANE)i);
				}
				break;

			case PHY_PCS_TX_PRD:
				{
					u32 pclk_period;

					pclk_period = host->config->hci_config_info.pclk_period;
					set_mib(host, cfg->addr, pclk_period, TX_LANE_0 + (UFS_LANE)i);
				}
				break;

			case PHY_PMA_COMN:
				if (i == 0)
					UFS_PMA_WRITE(host, cfg->addr, cfg->val);
				break;

			case PHY_PMA_TRSV:
				if (i == 0)
					UFS_PMA_WRITE(host, cfg->addr, cfg->val);
				break;
			case COMMON_WAIT:
				if (i == 0)
					udelay(cfg->val);
				break;
			case UNIPRO_DBG_APB:
				if (i == 0)
					UFS_UNIPRO_WRITE(cfg->addr, cfg->val);
				break;
			}
		}
	}
}

void ufs_check_num_of_lanes(struct ufs_host *host)
{
	u32 tx_lane, rx_lane;
	u32 num_of_lane;

	tx_lane = get_mib(host, PA_AvailTxDataLanes, UNIPRO_CMD);
	rx_lane = get_mib(host, PA_AvailRxDataLanes, UNIPRO_CMD);

	if (tx_lane == rx_lane)
		num_of_lane = tx_lane;
	else if (tx_lane > rx_lane)
		num_of_lane = rx_lane;
	else
		num_of_lane = tx_lane;
	host->config->hci_config_info.numoflane = num_of_lane;
	host->config->hci_config_info.numoflane = 2;
}

static u32 _UFS_Get_Unipro_Mclk_Period(struct ufs_host *host)
{

	float fSclk_ufsunipro20;
	float fUfsunipro_Mclk_Period;

#ifdef ZEBU
	fSclk_ufsunipro20 = (float)ZEBU_SCLK;
#else
	fSclk_ufsunipro20 = (float)clk_getrate(host->sclk_id);
#endif


	fSclk_ufsunipro20 = UFS_DIVIDE(fSclk_ufsunipro20, 1000000);
	fUfsunipro_Mclk_Period = (UFS_DIVIDE(1000, fSclk_ufsunipro20) + 0.5);

	host->config->hci_config_info.mclk_period = (u32)fUfsunipro_Mclk_Period;

	return (u32)fUfsunipro_Mclk_Period;

}

static u32 _UFS_Get_PCS_Pclk_Period(struct ufs_host *host)
{

	float fSclk;
	float fPCS_Pclk_Period;
#ifdef ZEBU
	fSclk = (float)ZEBU_SCLK;
#else
	fSclk = (float)clk_getrate(host->sclk_id);
#endif

	fSclk = UFS_DIVIDE(fSclk, 1000000);

	fPCS_Pclk_Period = (UFS_DIVIDE(1000, fSclk) + 0.5);

	host->config->hci_config_info.pclk_period = (u32)fPCS_Pclk_Period;

	return (u32)fPCS_Pclk_Period;
}

void ufs_set_clock_period(struct ufs_host *host)
{
	_UFS_Get_Unipro_Mclk_Period(host);
	_UFS_Get_PCS_Pclk_Period(host);
}

u32 read_unipro_linkstartup_cnf_result(struct ufs_host *host)
{
	return UFS_UNIPRO_READ(DME_LINKSTARTUP_CNF_RESULT);
}
