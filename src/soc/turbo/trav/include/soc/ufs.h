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

#ifndef _UFS_H_
#define _UFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <soc/ufs_scsi.h>
#include <soc/common_definition.h>
#include <types.h>

#define UFS_MAX_GPIO 10
#define UFS_NUM_GPIO 4
#define UFS_NUM_DTB_GPIO 4
#define UFS_MAX_LU 8

#define ZEBU
#ifdef ZEBU
#define ZEBU_ACLK	333000000
#define ZEBU_SCLK	166000000
#endif

#define __iomem
	struct ufs_host {
	int id;
	void __iomem *base;
	void __iomem *fmp;
	void __iomem *unipro;
	void __iomem *mphy;
	void __iomem *iop;
	void __iomem *iop_code;
	void __iomem *pmu;
	void __iomem *sysreg;
	void __iomem *sysreg_monitor_sel;
	void __iomem *cmu;

	int irq;

	struct resource *gpio[UFS_MAX_GPIO];

	struct device *dev;

	/* clock source */
	int aclk_gate_id;
	int sclk_id;
	int sclk_phy_26m_id;
	int phy_tx0_gate_id;
	int phy_rx0_gate_id;
	int phy_rx1_gate_id;

	int sclk_cfg_id;
	int phy_tx0_pwm_gate_id;
	int phy_rx0_pwm_gate_id;
	int phy_refclk_id;

	int debug;
	int irq_mode;
	int auto_h8;
	int utrd_cacheable;
	int sysreg_enable;
	bool link_done;

	ucd_t *ucd;
	utrd_t *utrd;
	utmrd_t *utmrd;

	struct ufs_config *config;

	int isr_blocking;
	int isr_count;

	int async;
	int queue_depth;
	int utrd_ucd_map[32];

	unsigned int int_enable;
	unsigned int int_vs_enable;

	unsigned int int_status;
	unsigned int int_vs_status;

	unsigned int next_db;
	unsigned int complete_req;
	unsigned int ucd_map;
	unsigned int saved_db;

	/* tick counter is 64bit */
	/* zero is temporal */
	/* 1~16: db number */
	unsigned long long perf_mon[17][6];

	unsigned int fmp_type;
	unsigned int fpga;
	unsigned int mib_direct_access;
	unsigned int print_trans_data;
	unsigned int wlu_enable;
	unsigned int refclk_src;

	unsigned int device_power_off;

	ufs_unit_descriptor device_unitdesc[UFS_MAX_LU];
};

typedef enum {
	UFS_PERF_TYPE_SEQ = 0,
	UFS_PERF_TYPE_RANDOM = 1,
} UFS_PERF_TYPE;

typedef enum {
	UFS_AES_ALGO_BYPASS = 0,
	UFS_AES_ALGO_CBC = 1,
	UFS_AES_ALGO_XTS = 2,
} UFS_AES_ALGO;

typedef enum {
	UFS_AES_KEY_SIZE_128 = 0,
	UFS_AES_KEY_SIZE_256 = 1,
} UFS_AES_KEY_SIZE;

typedef enum {
	UFS_AES_ENCRYPT_MODE_DISK = 0,
	UFS_AES_ENCRYPT_MODE_FILE = 1,
	UFS_AES_ENCRYPT_MODE_DISK_N_FILE = 2,
} UFS_AES_ENCRYPT_MODE;

typedef enum {
	UFS_CHANGE_IV_VAL = 0,
	UFS_CHANGE_KEY_VAL = 1,
	UFS_CHANGE_TW_VAL = 2,
	UFS_NO_CHANGE_VAL = 6,
} UFS_CHANGE_VAL;

struct ufs_fmp_auto_measure_table {

	char name[100];

	UFS_AES_ENCRYPT_MODE encrypt_mode;
	UFS_AES_ALGO aes_enc;
	UFS_AES_KEY_SIZE enc_key;
	UFS_AES_ALGO aes_dec;
	UFS_AES_KEY_SIZE dec_key;

	UFS_CHANGE_VAL change_val;
	bool test_result;

};

#define UFS_EVT1 1

#if UFS_EVT1
//#define UFS_USE_DTB_SIGNAL
#endif

//#define UFS_HW_SPEC_MIN_TACTIVATE 1 //HW Spec 300us
#define UFS_HW_SPEC_MIN_TACTIVATE 3 //HW Spec 300us
//#define UFS_HW_SPEC_MIN_THIBERN8 1 //HW Spec 500us
#define UFS_HW_SPEC_MIN_THIBERN8 2 //HW Spec 500us

#define UFS_UPPER_ADDRESS		0xF

#define UFS_WRITE(offset, val)	__raw_writel((val), host->base + (offset))
#define UFS_READ(offset)		__raw_readl(host->base + (offset))
#define UFS_FMP_WRITE(offset, val)	__raw_writel((val), host->fmp + (offset))
#define UFS_FMP_READ(offset)		__raw_readl(host->fmp + (offset))

#define UFS_PMU_WRITE(val)	__raw_writel((val), host->pmu)
#define UFS_PMU_READ(offset)__raw_readl(host->pmu + (offset))
#define UFS_SYSREG_WRITE(val)	__raw_writel((val), host->sysreg)
#define UFS_SYSREG_READ(offset)__raw_readl(host->sysreg + (offset))
#define UFS_SYSREG_MONITOR_WRITE(val)	__raw_writel((val), host->sysreg_monitor_sel)
#define UFS_SYSREG_MONITOR_READ(offset)__raw_readl(host->sysreg_monitor_sel + (offset))

#define UFS_MPHY_WRITE(offset, val)	__raw_writel((val), host->mphy + (offset))
#define UFS_MPHY_READ(offset)		__raw_readl(host->mphy + (offset))
#define UFS_UNIPRO_WRITE(offset, val)	__raw_writel((val), host->unipro + (offset))
#define UFS_UNIPRO_READ(offset)		__raw_readl(host->unipro + (offset))

#define UFS_SYSREG_WRITE_OFFSET(offset, val)	__raw_writel((val), host->sysreg + (offset))
#define UFS_SYSREG_READ_OFFSET(offset)					__raw_readl(host->sysreg + (offset))

#define SHIFT(n, s)							((n) << (s))
#define VAL(val, s)							((val) << (s))

bool set_mib(struct ufs_host *host, u32 mib_addr, u32 mib, u32 lane);
s32 get_mib(struct ufs_host *host, u32 mib_addr, u32 lane);
bool set_peer_mib(struct ufs_host *host, u32 mib_addr, u32 mib, u32 lane);
s32 get_peer_mib(struct ufs_host *host, u32 nMibAddr, u32 lane);
int find_one_in_bitmap(unsigned int bitmap);




/*TODO : Check the MASK, Have a compile error*/
/* n number of ones, shift s bits */
/*  { (1 << n) - 1 } << s */
//#define MASK(n, s)							(SHIFT(SHIFT(1, n)-1), (s))


//#define UFSHCI_CAP_NUTRS_MASK				MASK(5, 0)
//#define UFSHCI_CAP_NUTMRS_MASK				MASK(3, 16)
#define UFSHCI_CAP_64AS						SHIFT(1, 24)
#define UFSHCI_CAP_OODDS					SHIFT(1, 25)
#define UFSHCI_CAP_UICDMETMS				SHIFT(1, 26)

//#define UFSHCI_VER_MNR_MASK					MASK(0, 16)
//#define UFSHCI_VER_MJR_MASK					MASK(16, 0)

//#define UFSHCI_HCDDID_HCDDID_MASK			MASK(16, 0)
//#define UFSHCI_HCDDID_DC_MASK				MASK(8, 24)

//#define UFSHCI_HCPMID_MID_MASK				MASK(16, 0)
//#define UFSHCI_HCPMID_PID_MASK				MASK(16, 16)

#define H_UFS_INTR_HIBERNATE_ENTER_CNF			(1<<10)
#define H_UFS_INTR_HIBERNATE_EXIT_CNF			(1<<10)
#define H_UFS_INTR_HIBERNATE_ENTER_IND			(1<<6)
#define H_UFS_INTR_HIBERNATE_EXIT_IND			(1<<5)
#define H_UFS_INTR_ENABLE_CNF					(1<<10)
#define H_UFS_INTR_RESET_CNF					(1<<10)
#define H_UFS_INTR_LINKSTARTUP_CNF				(1<<10)
#define H_UFS_INTR_LINKSTARTUP_IND				(1<<8)
#define H_UFS_INTR_POWERMODE_IND				(1<<4)
#define H_UFS_INTR_PEERGETSET_CNF				(1<<10)
#define H_UFS_INTR_GETSET_CNF					(1<<10)
#define H_UFS_INTR_LINKLOST_IND					(1<<7)
#define H_UFS_INTR_POWERON_CNF					(1<<10)
#define H_UFS_INTR_POWEROFF_CNF					(1<<10)
#define H_UFS_INTR_ENDPOINTRESET_CNF			(1<<10)
#define H_UFS_INTR_ENDPOINTRESET_IND			(1<<1)
#define H_UFS_INTR_TEST_MODE_CNF				(1<<10)
#define H_UFS_INTR_TEST_MODE_IND				(1<<3)
#define H_UFS_INTR_ERROR_IND					(1<<2)

#define UFSHCI_IS_UTRCS						SHIFT(1, 0)
#define UFSHCI_IS_UDEPRI					SHIFT(1, 1)
#define UFSHCI_IS_UE						SHIFT(1, 2)
#define UFSHCI_IS_UTMS						SHIFT(1, 3)
#define UFSHCI_IS_UPMS						SHIFT(1, 4)
#define UFSHCI_IS_UHXS						SHIFT(1, 5)
#define UFSHCI_IS_UHES						SHIFT(1, 6)
#define UFSHCI_IS_ULLS						SHIFT(1, 7)
#define UFSHCI_IS_ULSS						SHIFT(1, 8)
#define UFSHCI_IS_UTMRCS					SHIFT(1, 9)
#define UFSHCI_IS_UCCS						SHIFT(1, 10)
#define UFSHCI_IS_DFES						SHIFT(1, 11)
#define UFSHCI_IS_UTPES						SHIFT(1, 12)
#define UFSHCI_IS_HCFES						SHIFT(1, 16)
#define UFSHCI_IS_SBFES						SHIFT(1, 17)
#define UFSHCI_IS_CEFES						SHIFT(1, 18)

#define UFSHCI_IE_UTRCSE					SHIFT(1, 0)
#define UFSHCI_IE_UDEPRIE					SHIFT(1, 1)
#define UFSHCI_IE_UEE						SHIFT(1, 2)
#define UFSHCI_IE_UTMSE						SHIFT(1, 3)
#define UFSHCI_IE_UPMSE						SHIFT(1, 4)
#define UFSHCI_IE_UHXSE						SHIFT(1, 5)
#define UFSHCI_IE_UHESE						SHIFT(1, 6)
#define UFSHCI_IE_ULLSE						SHIFT(1, 7)
#define UFSHCI_IE_ULSSE						SHIFT(1, 8)
#define UFSHCI_IE_UTMRCSE					SHIFT(1, 9)
#define UFSHCI_IE_UCCSE						SHIFT(1, 10)
#define UFSHCI_IE_UFESE						SHIFT(1, 11)
#define UFSHCI_IE_UTPES						SHIFT(1, 12)
#define UFSHCI_IE_HCFESE					SHIFT(1, 16)
#define UFSHCI_IE_SBFESE					SHIFT(1, 17)
#define UFSHCI_IE_CEFES						SHIFT(1, 18)

#define UFSHCI_HCS_DP					SHIFT(1, 0)
#define UFSHCI_HCS_UTRLRDY				SHIFT(1, 1)
#define UFSHCI_HCS_UTMRLRDY				SHIFT(1, 2)
#define UFSHCI_HCS_UCRDY				SHIFT(1, 3)


#define UFSHCI_IS_ERR (UFSHCI_IS_CEFES\
		| UFSHCI_IS_SBFES\
		| UFSHCI_IS_HCFES\
		| UFSHCI_IS_UTPES\
		| UFSHCI_IS_DFES\
		| UFSHCI_IS_UE)


#define UFSHCI_HCE_HCE					SHIFT(1, 0)

//#define UFSHCI_UECPA_EC_MASK			MASK(5, 0)
#define UFSHCI_UECPA_EC_VAL(val)		VAL(val, 0)
#define UFSHCI_UECPA_ERR				SHIFT(1, 31)

//#define UFSHCI_UECDL_EC_MASK			MASK(15, 0)
#define UFSHCI_UECDL_EC_VAL(val)		VAL(val, 0)
#define UFSHCI_UECDL_ERR				SHIFT(1, 31)

//#define UFSHCI_UECN_EC_MASK				MASK(3, 0)
#define UFSHCI_UECN_EC_VAL(val)			VAL(val, 0)
#define UFSHCI_UECN_ERR					SHIFT(1, 31)

//#define UFSHCI_UECT_EC_MASK				MASK(7, 0)
#define UFSHCI_UECT_EC_VAL(val)			VAL(val, 0)
#define UFSHCI_UECT_ERR					SHIFT(1, 31)

//#define UFSHCI_UECDME_EC_MASK			MASK(1, 0)
#define UFSHCI_UECDME_EC_VAL(val)		VAL(val, 0)
#define UFSHCI_UECDME_ERR				SHIFT(1, 31)

//#define UFSHCI_UTRIACR_IATOVAL_MASK		MASK(8, 0)
#define UFSHCI_UTRIACR_IATOVAL_VAL(val)	VAL(val, 0)
//#define UFSHCI_UTRIACR_IACTH_MASK		MASK(5, 8)
#define UFSHCI_UTRIACR_IACTH_VAL(val)	VAL(val, 8)
#define UFSHCI_UTRIACR_CTR				SHIFT(1, 16)
#define UFSHCI_UTRIACR_IASB				SHIFT(1, 20)
#define UFSHCI_UTRIACR_IAPWEN			SHIFT(1, 24)
#define UFSHCI_UTRIACR_IAPEN			SHIFT(1, 31)

//#define UFSHCI_UTRLBA					MASK(30, 2)
#define UFSHCI_UTRLBA_VAL(val)			VAL(val, 2)
//#define UFSHCI_UTRLBAU                   MASK(32, 0)

//#define UFSHCI_UTRLDBR					MASK(16, 0)
#define UFSHCI_UTRLDBR_VAL(val)			VAL(val, 0)

//#define UFSHCI_UTMRLBA					MASK(30, 2)
#define UFSHCI_UTMRLBA_VAL(val)			VAL(val, 2)

//#define UFSHCI_UTMRLDBR					MASK(4, 0)
#define UFSHCI_UTMRLDBR_VAL(val)		VAL(val, 0)
//#define UFSHCI_UTRLCLR					MASK(4, 0)
#define UFSHCI_UTRLCLR_VAL(val)			VAL(val, 0)

#define UFSHCI_UTMRLSRS_MASK			SHIFT(1, 0)

//#define UFSHCI_UICCMD_CMDOP_MASK		MASK(8, 0)
#define UFSHCI_UICCMD_CMDOP_VAL(val)	VAL(val, 0)


typedef long UFSD_STATUS;

/* UFSD Error Code Definitions */
#define UFSDS_SUCCESS					((UFSD_STATUS)0x00000000L)
#define UFSDS_PENDING                   ((UFSD_STATUS)0x40000000L)
#define UFSDS_ERROR                     ((UFSD_STATUS)0x80000000L)
#define UFSDS_HALTED                    ((UFSD_STATUS)0xC0000000L)

#define UFSD_SUCCESS(Status)            (((UFSD_STATUS)(Status) < 0))
#define UFSD_PENDING(Status)            (((UFSD_STATUS)(Status) & UFSDS_PENDING) == UFSDS_PENDING)
#define UFSD_ERROR(Status)              (((UFSD_STATUS)(Status) & UFSDS_ERROR) == UFSDS_ERROR)
#define UFSD_HALTED(Status)             (((UFSD_STATUS)(Status) & UFSDS_HALTED) == UFSDS_HALTED)

/* Status Codes from the TD */
#define UFSDS_NOERROR                   ((UFSD_STATUS)0x00000000L)
#define UFSDS_CRC                       ((UFSD_STATUS)0xC0000001L)
#define UFSDS_BTSTUFF                   ((UFSD_STATUS)0xC0000002L)
#define UFSDS_DATA_TOGGLE_MISMATCH      ((UFSD_STATUS)0xC0000003L)
#define UFSDS_STALL_PID                 ((UFSD_STATUS)0xC0000004L)
#define UFSDS_DEV_NOT_RESPONDING        ((UFSD_STATUS)0xC0000005L)
#define UFSDS_PID_CHECK_FAILURE         ((UFSD_STATUS)0xC0000006L)
#define UFSDS_UNEXPECTED_PID            ((UFSD_STATUS)0xC0000007L)
#define UFSDS_DATA_OVERRUN              ((UFSD_STATUS)0xC0000008L)
#define UFSDS_DATA_UNDERRUN             ((UFSD_STATUS)0xC0000009L)
#define UFSDS_BUFFER_OVERRUN            ((UFSD_STATUS)0xC000000CL)
#define UFSDS_BUFFER_UNDERRUN           ((UFSD_STATUS)0xC000000DL)
#define UFSDS_NOT_ACCESSED              ((UFSD_STATUS)0xC000000FL)

/* Status Codes from the HCD */
#define UFSDS_FIFO                      ((UFSD_STATUS)0xC0000010L)
#define UFSDS_ENDPOINT_HALTED           ((UFSD_STATUS)0xC0000030L)

#define UFSDS_NO_MEMORY                 ((UFSD_STATUS)0x80000100L)
#define UFSDS_INVALID_URB_REQUEST       ((UFSD_STATUS)0x80000200L)
#define UFSDS_INVALID_PARAMETER         ((UFSD_STATUS)0x80000300L)
#define UFSDS_ERROR_BUSY                ((UFSD_STATUS)0x80000400L)
#define UFSDS_REQUEST_FAILED            ((UFSD_STATUS)0x80000500L)
#define UFSDS_INVALID_PIPE_HANDLE       ((UFSD_STATUS)0x80000600L)
#define UFSDS_NO_BANDWIDTH              ((UFSD_STATUS)0x80000700L)
#define UFSDS_INTERNAL_HC_ERROR         ((UFSD_STATUS)0x80000800L)
#define UFSDS_ERROR_SHORT_TRANSFER      ((UFSD_STATUS)0x80000900L)
#define UFSDS_BAD_START_FRAME           ((UFSD_STATUS)0xC0000A00L)
#define UFSDS_ISOCH_REQUEST_FAILED      ((UFSD_STATUS)0xC0000B00L)

#define UFSDS_CANCELED                  ((UFSD_STATUS)0x00010000L)
#define UFSDS_CANCELING                 ((UFSD_STATUS)0x00020000L)


#define UFS_DIVIDE(a, b)		((b) ? ((a)/(b)) : 0)
#define UFS_REMAINDER(a, b)	((b) ? ((a)%(b)) : 0)


//UTRD bit[24] - Interrupt (I): This field indicates the type of command with regard to interrupt generation.
#define UFSHCI_UTRD_BIT24_I_USE_IA_REGULAR_CMD			0
#define UFSHCI_UTRD_BIT24_I_NOT_USE_IA_INTERRUPT_CMD		1


#define UFSHCI_ERR_CODE_PA_PHY_ERR_L0						SHIFT(1, 0)
#define UFSHCI_ERR_CODE_PA_PHY_ERR_L1						SHIFT(1, 1)
#define UFSHCI_ERR_CODE_PA_PHY_ERR_L2						SHIFT(1, 2)
#define UFSHCI_ERR_CODE_PA_PHY_ERR_L3						SHIFT(1, 3)
#define UFSHCI_ERR_CODE_PA_PHY_GENERIC						SHIFT(1, 4)


#define UFSHCI_ERR_CODE_DL_NAC_RECIEVED						SHIFT(1, 0)
#define UFSHCI_ERR_CODE_DL_TC_REPLAY_TIMER_EXPIRED						SHIFT(1, 1)
#define UFSHCI_ERR_CODE_DL_AFC_REQUEST_TIMER_EXPIRED						SHIFT(1, 2)
#define UFSHCI_ERR_CODE_DL_FC_PROTECTION_TIMER_EXPIRED						SHIFT(1, 3)
#define UFSHCI_ERR_CODE_DL_CRC_ERROR						SHIFT(1, 4)
#define UFSHCI_ERR_CODE_DL_RX_BUFFER_OVERFLOW						SHIFT(1, 5)
#define UFSHCI_ERR_CODE_DL_MAX_FRAME_LENGTH_EXCEEDED						SHIFT(1, 6)
#define UFSHCI_ERR_CODE_DL_WRONG_SEQUENCE_NUMBER						SHIFT(1, 7)
#define UFSHCI_ERR_CODE_DL_AFC_FRAME_SYNTAX_ERROR						SHIFT(1, 8)
#define UFSHCI_ERR_CODE_DL_NAC_FRAME_SYNTAX_ERROR						SHIFT(1, 9)
#define UFSHCI_ERR_CODE_DL_EOF_SYNTAX_ERROR						SHIFT(1, 10)
#define UFSHCI_ERR_CODE_DL_FRAME_SYNTAX_ERROR						SHIFT(1, 11)
#define UFSHCI_ERR_CODE_DL_NAC_BAD_CTRL_SYMBOL_TYPE						SHIFT(1, 12)
#define UFSHCI_ERR_CODE_DL_PA_INIT_ERROR						SHIFT(1, 13)
#define UFSHCI_ERR_CODE_DL_PA_ERROR_IND_RECEIVED						SHIFT(1, 14)



#define UFSHCI_ERR_CODE_N_UNSUPPORTED_HEADER_TYPE						SHIFT(1, 0)
#define UFSHCI_ERR_CODE_N_BAD_DEVICEID_ENC						SHIFT(1, 1)
#define UFSHCI_ERR_CODE_N_LHDR_TRAP_PACKET_DROPPING						SHIFT(1, 2)



#define UFSHCI_ERR_CODE_T_UNSUPPORTED_HEADER_TYPE						SHIFT(1, 0)
#define UFSHCI_ERR_CODE_T_UNKNOWN_CPORTID						SHIFT(1, 1)
#define UFSHCI_ERR_CODE_T_NO_CONNECTION_RX						SHIFT(1, 2)
#define UFSHCI_ERR_CODE_T_CONTROLLED_SEGMENT_DROPPING						SHIFT(1, 3)
#define UFSHCI_ERR_CODE_T_BAD_TC						SHIFT(1, 4)
#define UFSHCI_ERR_CODE_T_E2E_CREDIT_OVERFLOW						SHIFT(1, 5)
#define UFSHCI_ERR_CODE_T_SAFETY_VALVE_DROPPING						SHIFT(1, 6)

/* SCSI Well-Known ID */
#define WLUN_ID_REPORT_LUNS			0x81u
#define WLUN_ID_DEVICE				0xD0u
#define WLUN_ID_RPMB				0xC4u
#define WLUN_ID_BOOT				0xB0u


typedef enum {
	UFS_UTRD_CLEAN_DCACHE,
	UFS_UTRD_INVALIDATE_DCACHE,
	UFS_UTRD_OCS_INVALIDATE_DCACHE,
	UFS_UCD_CLEAN_DCACHE,
	UFS_UCD_INVALIDATE_DCACHE,
	UFS_UCD_DEVICE_INFO_INVALIDATE_DCACHE,
} UFS_DCACHE_CTRL;

typedef enum {
	UFS_UPMCRS_PWR_OK = 0,
	UFS_UPMCRS_PWR_LOCAL = 1,
	UFS_UPMCRS_PWR_REMOTE = 2,
	UFS_UPMCRS_PWR_BUSY = 3,
	UFS_UPMCRS_PWR_ERROR_CAP = 4,
	UFS_UPMCRS_PWR_FATAL_ERROR = 5,
	UFS_UPMCRS_PWR_RESERVED_0 = 6,
	UFS_UPMCRS_PWR_RESERVED_1 = 7,
} UFS_UPMCRS; //UIC Power Mode Change Request Status

typedef enum {
	UFS_FSM_SEL_STANDARD_SFR = 0,
	UFS_FSM_SEL_UTR_RESPONSE = 1,
	UFS_FSM_SEL_UTMR_RESPONSE = 2,
	UFS_FSM_SEL_RX_DATA		= 3,
	UFS_FSM_SEL_TX_DATA		= 4,
} UFS_FSM_SEL;

typedef enum {
	UFS_FSM_STD_SFR_IDLE = 0,
	UFS_FSM_STD_SFR_BUSY1 = 1,
	UFS_FSM_STD_SFR_BUSY2 = 2,
	UFS_FSM_STD_SFR_BUSY3 = 3,
	UFS_FSM_STD_SFR_UIC_CLR = 4,	//UIC_CLR_ADDR_PHASE
	UFS_FSM_STD_SFR_IS_CLR = 5,	//IS_CLR_ADDR_PHASE
	UFS_FSM_STD_SFR_UIC_CLR_BUSY = 6,
	UFS_FSM_STD_SFR_UICCMD_WRITE_BUSY = 7,
} UFS_FSM_STD_SFR;

typedef enum {
	UFS_FSM_UTR_RSP_IDLE = 0,
	UFS_FSM_UTR_RSP_UPIU_SAVE = 1,
	UFS_FSM_UTR_RSP_WAIT_UPIU_SAVE_DONE = 2,
	UFS_FSM_UTR_RSP_SAVE_OCS = 3,
	UFS_FSM_UTR_RSP_WAIT_OCS_DONE = 4,
	UFS_FSM_UTR_RSP_DONE = 5,
} UFS_FSM_UTR_RSP;

typedef enum {
	UFS_FSM_UTMR_RSP_IDLE = 0,
	UFS_FSM_UTMR_RSP_UPIU_SAVE = 1,
	UFS_FSM_UTMR_RSP_WAIT_UPIU_SAVE_DONE = 2,
	UFS_FSM_UTMR_RSP_SAVE_OCS = 3,
	UFS_FSM_UTMR_RSP_WAIT_OCS_DONE = 4,
	UFS_FSM_UTMR_RSP_DONE = 5,
} UFS_FSM_UTMR_RSP;

typedef enum {
	UFS_FSM_RX_DATA_IDLE = 0,
	UFS_FSM_RX_DATA_WAIT_DMA_FIFO_READY = 1,
	UFS_FSM_RX_DATA_REQ_PRDT = 2,
	UFS_FSM_RX_DATA_WAIT_PRDT = 3,
	UFS_FSM_RX_DATA_BUILD_AXI_DMA_DESC = 4,
	UFS_FSM_RX_DATA_SAVE_AXI_DMA_DESC_TO_FIFO = 5,
	UFS_FSM_RX_DATA_UPDATE_PRDT_FETCH_ADDR = 6,
	UFS_FSM_RX_DATA_DONE = 7,
} UFS_FSM_RX_DATA;

typedef enum {
	UFS_FSM_TX_DATA_IDLE = 0,
	UFS_FSM_TX_DATA_CHECK_DATA_ADDR_VALIDITY = 1,
	UFS_FSM_TX_DATA_WAIT_RTT_FIFO_READY = 2,
	UFS_FSM_TX_DATA_WAIT_AXIDMA_DESC_FIFO_READY	= 3,
	UFS_FSM_TX_DATA_REQ_PRDT = 4,
	UFS_FSM_TX_DATA_WAIT_PRDT = 5,
	UFS_FSM_TX_DATA_CRATE_TXDATA_DMA_DESC = 6,
	UFS_FSM_TX_DATA_SET_TXDMA_DMA_DESC = 7,
	UFS_FSM_TX_DATA_UPDATE_PRDT_FETCH_ADDR = 8,
	UFS_FSM_TX_DATA_DONE = 9,
} UFS_FSM_TX_DATA;


typedef enum {
	dir_read = 0,
	dir_write = 1,
} ufsp_dir;

typedef enum {
	smu_slot_0 = 0,
	smu_slot_1 = 1,
	smu_slot_2 = 2,
	smu_slot_3 = 3,
	smu_slot_4 = 4,
	smu_slot_5 = 5,
	smu_slot_6 = 6,
	smu_slot_7 = 7,
} ufsp_smu_slot;

typedef enum {
	upctrl_auser = 0,
	upctrl_acache = 5,
} ufsp_upctrl;

typedef enum {
	upstat_busy = 0,
	upstat_abort = 1,
	upstat_addr36 = 2,
} ufsp_upstat;


typedef enum {
	upsec_awprotdata = 0,
	upsec_awprotdesc = 3,
	upsec_arprotdata = 6,
	upsec_arprotdesc = 9,
	upsec_useotpmask = 12,
	upsec_nsuser = 13,
	upsec_nssmu = 14,
	upsec_nskeyreg = 15,
	upsec_arprotprdt = 16,
	upsec_desctype = 19,
	upsec_unmap_smu = 28,
	upsec_fmp_on = 29,
	upsec_cs = 30,
	upsec_protbytzpc = 31,
} ufsp_upsec;

typedef enum {
	inline_cryptokey = 0,
	inline_dusize = 512,
	inline_capidx = 520,
	inline_cfge = 543,
	inline_vsb = 560,
} ufsp_inline;

typedef enum {
	key128 = 0,
	key256 = 1,
} ufsp_key256;

typedef enum {
	sr_off = 0,
	sr_on = 1,
} ufsp_sr;

typedef enum {
	sw_off = 0,
	sw_on = 1,
} ufsp_sw;

typedef enum {
	nsr_off = 0,
	nsr_on = 1,
} ufsp_nsr;

typedef enum {
	nsw_off = 0,
	nsw_on = 1,
} ufsp_nsw;

typedef enum {
	fuse_key_off = 0,
	fuse_key_on = 1,
} ufsp_ufk;

typedef enum {
	enc_off = 0,
	enc_on = 1,
} ufsp_enc;


typedef enum {
	not_valid = 0,
	valid = 1,
} ufsp_valid;


typedef enum {
	ufsp_bypass = 0,
	ufsp_cbc = 1,
	ufsp_xts = 2,
} ufsp_enc_mode;




typedef enum {
	utrd_ocs_success = 0x0,
	utrd_ocs_invalid_commad_table_attributes = 0x01,
	utrd_ocs_invalid_prtd_attributes = 0x2,
	utrd_ocs_mismatch_data_buffer_size = 0x3,
	utrd_ocs_mismatch_respnse_upiu_size = 0x4,
	utrd_ocs_peer_communication_feliure = 0x5,
	utrd_ocs_aborted = 0x06,
	utrd_ocs_fatal_error = 0x7,
	utrd_ocs_invalid_value = 0xf,
} utrd_ocs_e;

typedef enum {
	utmrd_ocs_success = 0x0,
	utmrd_ocs_invalid_task_management_funtion_attributes = 0x01,
	utmrd_ocs_mismatch_request_size = 0x02,
	utmrd_ocs_mismatch_respnse_size = 0x03,
	utmrd_ocs_peer_communication_failure = 0x04,
	utmrd_ocs_aborted = 0x05,
	utmrd_ocs_fatal_error = 0x06,
	utmrd_ocs_invalid_value = 0x0f,
} utmrd_ocs_e;


typedef enum {
	task_abort_task = 0x01,
	task_abort_task_set = 0x02,
	task_clean_task_set = 0x04,
	task_logical_unit_reset = 0x08,
	task_query_task = 0x80,
	task_query_task_set = 0x81,
} task_management_fun_e;



typedef enum {
	test_all			= 0x00,
	func_lv_1			= 0x01,
	aging				= 0x02,
	debug				= 0x03,
	func_lv_2			= 0x05,
	func_lv_3			= 0x06,
	func_lv_4			= 0x07,
} ufs_test_type;

typedef enum {
	silicon		= 0x00,
	fpga		= 0x01,
} ufs_media;

typedef enum {
	tsb				= 1,
	samsung			= 2,
	skh				= 3,
	k_x				= 6,
	cs_x			= 8,
} ufs_device_info;



typedef enum {
	prj_evt0	= 0,
	prj_evt1	= 1,
	prj_evt2	= 2,
	prj_evt3	= 3,
	prj_evt4	= 4,
	prj_evt5	= 5,
} test_ver;



typedef enum {
	oversampling	= 1,
	integrator		= 2,
} config_opt1;

typedef enum {
	use2lane		= 2,
	use1lane		= 1,
} ufs_lane;

typedef enum {
	all_error_mark     = 0x7 /*default*/,
	only_fetal_error   = 0x1 /*default*/,
	only_warning_error = 0x2/*default*/,
	only_single_error  = 0x4/*default*/,
} ufs_defs;

typedef enum {
	opt_no_selection        = 0/*default*/,
	opt_pa_error_selection  = 1,
	//opt_all_pa_errors       = 2,
	opt_dl_error_selection  = 3,
	//opt_all_dl_errors       = 4,
	opt_n_error_selection   = 5,
	//opt_all_n_errors        = 6,
	opt_t_error_selection   = 7,
	//opt_all_t_errors        = 8,
	//opt_dme_error_selection = 9,
	//opt_all_dme_errors      = 10,
} ufs_defs_by_layers;


struct ufs_host_config {
	ufs_test_type		type;
	ufs_media			media;
	ufs_device_info		device;
	char				*prjname;
	test_ver			prjver;
	ufs_lane			numoflane;
	char				*numoflanedesc;
	ufs_defs			defs;
	ufs_defs_by_layers	defs_layer;
	config_opt1			testopt1;
	char				*testopt1desc;

	u32 pclk_period;
	u32 mclk_period;

	u32 block_size;
	u32 prdt_size;
	bool dma_blocking;

	u32 pwm_cmn_ctrl_divider;
	float pwm_g1_freq_MHz;
	bool mphy_bist_test;
	bool lcc_enable;
	bool bgr_sq_off;
	bool measure_perf_enable;
};


struct ufs_uic_pwr_mode {
	u32 gear;				//Gear
	u32 hs_series;			//Series A :1, Series B :2
	//u32 ls_hs_mode;			//LS:1, HS:2
	//u32 is_auto_not_burst;	//burst:0, auto:1
	u32 pwr_mode;	//FAST:1, SLOW:2, FASTAUTO:4, SLOWAUTO:5, UNCHANGED:7

	u32 prev_gear;			//Previous Gear
	u32 prev_hs_series;		//Previous Series
	//u32 prev_ls_hs_mode;	//Previous Mode
	//u32 prev_is_auto_not_burst;	//burst:0, auto:1
	u32 prev_pwr_mode;	//FAST:1, SLOW:2, FASTAUTO:4, SLOWAUTO:5, UNCHANGED:7

	u32 type_of_refclk_pll;		//24MHz/26MHz
	u32 pwm_os_integator;		//Oversampling:1, Integator:2

	u32 connected_tx_data_lane;
	u32 connected_rx_data_lane;

	u32 active_tx_data_lanes;
	u32 active_rx_data_lanes;

	u32 gear_hs_series_sel;
};


struct ufs_des_info {
	u32 lun;
	u32 tag;
	u32 doorbell;
	u32 save_doorbell;
	u32 lba;
	u32 num_lba;
	u32 prdt_address;
	u32 nexus_type;
	bool fua;
	bool dpo;
	upiu_tsf_qr_flag_idn qr_flag;
};

struct ufs_fmp_info {
	ufsp_enc_mode das;
	ufsp_enc_mode fas;
	ufsp_key256 dkl;
	ufsp_key256 fkl;
	u32 f_iv_data[4];
	u32 f_key_data[8];
	u32 f_twkey_data[8];
	u32 d_iv_data[4];
	u32 d_key_data[8];
	u32 d_twkey_data[8];
};

struct ufs_is_cnt {
	u32 sbfes;
	u32 hcfes;
	u32 utpes;
	u32 dfes;
	u32 uccs;
	u32 utmrcs;
	u32 ulss;
	u32 ulls;
	u32 uhes;
	u32 uhxs;
	u32 upms;
	u32 utms;
	u32 ue;
	u32 udepri;
	u32 utrcs;
};

struct ufs_vs_is_cnt {
	u32 unipro_int;
	u32 pio_empty_cnt;
	u32 pio_data_cnt;
	u32 unipro_double_mk2_received_cnf;
	u32 unipro_doule_mk2_sent_cnf;
	u32 unipro_deepstall_exit_ind;
	u32 unipro_deepstall_enter_ind;
	u32 mphy_pcs_int;
	u32 dbr_duplication_int;
	u32 dbr_timer_int;
	u32 fmp_fault;
	u32 ufs_pm_int;
	u32 smu_mismatch;
	u32 ufs_idle_indicator;
	u32 rx_upiu_hit_error;
	u32 rx_upiu_len_error;
};

struct ufs_err_cnt {
	u32 unipro_cur_pwrmode;
	u32 unipro_cur_tx_gear;
	u32 unipro_cur_tx_termination;
	u32 unipro_cur_rx_gear;
	u32 unipro_cur_rx_termination;

	u32 pmc_mismatch_during_h8_aging;
	u32 h8_enter_fail;
	u32 h8_exit_fail;
	u32 pmc_ls_2_hs_fail;
	u32 pmc_hs_2_ls_fail;
	u32 peer_get_fail;

	/* UIC PA error code in UniPro*/
	u32 pa_tx_linereset_asserted;
	u32 pa_rx_linereset_detected;
	u32 pa_unknown;

	/* UIC Data Link error code in UniPro */
	u32 dl_nac_recieved;
	u32 dl_tc_replay_timer_expired;
	u32 dl_afc_request_timer_expired;
	u32 dl_fc_protection_timer_expired;
	u32 dl_crc_error;
	u32 dl_rx_buffer_overflow;
	u32 dl_max_frame_length_exceeded;
	u32 dl_wrong_sequency_number;
	u32 dl_afc_frame_syntax_error;
	u32 dl_nac_frame_syntax_error;
	u32 dl_eof_syntax_error;
	u32 dl_frame_syntax_error;
	u32 dl_bad_ctrl_symbol_type;
	u32 dl_pa_init_error;
	u32 dl_pa_error_ind_received;
	u32 dl_unknown;

	/* UIC Network error code  in UniPro*/
	u32 n_unsupported_header_type;
	u32 n_bad_deviceid_enc;
	u32 n_lhdr_trap_packet_dropping;
	u32 n_unknown;

	/* UIC Tranport error code  in UniPro*/
	u32 t_unsupported_header_type;
	u32 t_unknown_cportid;
	u32 t_no_connection_rx;
	u32 t_controlled_segment_dropping;
	u32 t_bad_tc;
	u32 t_e2e_credit_overflow;
	u32 t_safety_falue_dropping;
	u32 t_unknown;
};


typedef union {
	u32 val;
	struct {
		unsigned dusize : 8;
		unsigned capidx : 8;
		unsigned reserved : 15;
		unsigned cfge : 1;
	} __attribute__ ((__packed__)) format;
} __attribute__ ((__packed__)) inline_set_dw16;


struct inline_set {
	u32 inline_key1[8];
	u32 inline_key2[8];
	inline_set_dw16 inline_dw16;
};

struct ufs_config {
	struct ufs_host_config		hci_config_info;
	struct ufs_uic_pwr_mode		hci_uic_pwr_mode_info;
	struct ufs_err_cnt			hci_err_cnt_info;
	struct ufs_des_info			hci_des_info;
	struct ufs_fmp_info			hci_fmp_info;
	struct inline_set			hci_inline_info;

	//struct ufs_is_cnt prev_is_cnt;
	//struct ufs_vs_is_cnt prev_vs_is_cnt;
	struct ufs_is_cnt	is_cnt;
	struct ufs_vs_is_cnt	vs_is_cnt;

};

struct ufs_query {
	u32 db;
	u32 tag;
	u32 id;
	u32 index;
	u32 length;
};

/* UFS CAL >> */
typedef enum {
	PHY_CFG_NONE = 0,
	PHY_PCS_COMN,
	PHY_PCS_RXTX,
	PHY_PCS_RX,
	PHY_PCS_TX,
	PHY_PCS_RX_PRD,
	PHY_PCS_TX_PRD,
	PHY_PMA_COMN,
	PHY_PMA_TRSV,
	PHY_PMA_TRSV_LANE1,
	UNIPRO_STD_MIB,
	UNIPRO_DBG_MIB,
	UNIPRO_DBG_APB,
	UNIPRO_DBG_PRD,
	COMMON_WAIT,
} ufs_config_types;

typedef enum {
	__PMD_PWM_G1_L1,
	__PMD_PWM_G1_L2,
	__PMD_PWM_G2_L1,
	__PMD_PWM_G2_L2,
	__PMD_PWM_G3_L1,
	__PMD_PWM_G3_L2,
	__PMD_PWM_G4_L1,
	__PMD_PWM_G4_L2,
	__PMD_PWM_G5_L1,
	__PMD_PWM_G5_L2,
	__PMD_HS_G1_L1,
	__PMD_HS_G1_L2,
	__PMD_HS_G2_L1,
	__PMD_HS_G2_L2,
	__PMD_HS_G3_L1,
	__PMD_HS_G3_L2,
} ufs_pmc_type;

typedef enum {
	FAST_MODE	= 1,
	SLOW_MODE	= 2,
	FASTAUTO_MODE	= 4,
	SLOWAUTO_MODE	= 5,
	UNCHANGED	= 7,
} ufs_pwr_mode;

#define IS_PWR_MODE_HS(m)	(((m) == FAST_MODE) || ((m) == FASTAUTO_MODE))
#define IS_PWR_MODE_PWM(m)	(((m) == SLOW_MODE) || ((m) == SLOWAUTO_MODE))

#define for_each_phy_cfg(cfg) \
	for (; (cfg)->flg != PHY_CFG_NONE; (cfg)++)

#define for_each_ufs_lane(ufs, i) \
	for (i = 0; i < (ufs)->config->hci_config_info.numoflane; i++)

#define PMD_PWM_G1_L1	(1U << __PMD_PWM_G1_L1)
#define PMD_PWM_G1_L2	(1U << __PMD_PWM_G1_L2)
#define PMD_PWM_G2_L1	(1U << __PMD_PWM_G2_L1)
#define PMD_PWM_G2_L2	(1U << __PMD_PWM_G2_L2)
#define PMD_PWM_G3_L1	(1U << __PMD_PWM_G3_L1)
#define PMD_PWM_G3_L2	(1U << __PMD_PWM_G3_L2)
#define PMD_PWM_G4_L1	(1U << __PMD_PWM_G4_L1)
#define PMD_PWM_G4_L2	(1U << __PMD_PWM_G4_L2)
#define PMD_PWM_G5_L1	(1U << __PMD_PWM_G5_L1)
#define PMD_PWM_G5_L2	(1U << __PMD_PWM_G5_L2)
#define PMD_HS_G1_L1	(1U << __PMD_HS_G1_L1)
#define PMD_HS_G1_L2	(1U << __PMD_HS_G1_L2)
#define PMD_HS_G2_L1	(1U << __PMD_HS_G2_L1)
#define PMD_HS_G2_L2	(1U << __PMD_HS_G2_L2)
#define PMD_HS_G3_L1	(1U << __PMD_HS_G3_L1)
#define PMD_HS_G3_L2	(1U << __PMD_HS_G3_L2)

#define PMD_ALL		(PMD_HS_G3_L2 - 1)
#define PMD_PWM		(PMD_PWM_G4_L2 - 1)
#define PMD_HS		(PMD_ALL ^ PMD_PWM)

struct ufs_phy_cfg {
	u16 addr;
	u32 val;
	u16 flg;
	u8 lyr;
} __attribute__((packed));

// Memory Area for making UFS UTRD descriptors
#define UFS_USER_LUN (0x00)
#define UFS_WRITE_BUF_ADDR	0x80008000
#define UFS_READ_BUF_ADDR	0x80090000
#define UFS_CBFS_LOAD_ADDR	0xf0000000//0x80090000
#define KERNEL_ADDRESS	0x80080000
#define DTB_ADDRESS	0x88000000

#if IS_ENABLED(CONFIG_BOOTBLOCK_X)
  #define UFS_BUF_ADDR_BASE		0x30000
  #define UFS_SENSE_AUTO_ADDRESS	UFS_BUF_ADDR_BASE
  #define UFS_HOST_CONFIG_ADDRESS	(UFS_BUF_ADDR_BASE + 0x1000)
  #define UFS_HOST_STRUCT_ADDRESS	(UFS_BUF_ADDR_BASE + 0x2000)
  #define UFS_UTRD_ADDRESS		(UFS_BUF_ADDR_BASE + 0x9000)
  #define UFS_UCD_ADDRESS		(UFS_BUF_ADDR_BASE + 0xC000)
  #define UFS_DEVICE_DESC_BASE		(UFS_BUF_ADDR_BASE + 0x14000)
#else  /* other than bootblock-x */
  #define UFS_SENSE_AUTO_ADDRESS	0xA0000000
  #define UFS_HOST_CONFIG_ADDRESS	0xA0001000
  #define UFS_HOST_STRUCT_ADDRESS	0xA0002000
  #define UFS_UTRD_ADDRESS		0xA0009000
  #define UFS_UCD_ADDRESS		0xA000C000
  #define UFS_DEVICE_DESC_BASE		0xE0000000
#endif

#define BLOCK_SIZE	4096
#define RPMB_BLOCK_SIZE	512

#define PRINT_LOG 0
#define PRINT_DETAIL 0

#define SHIFT_R(x, shift)		((x) >> (shift))
#define GET_BITS(x, shift, mask) (SHIFT_R(x, shift) & (mask))

#define NEXUS_I_T		(0x01)
#define NEXUS_I_T_L		(0x02)
#define NEXUS_I_T_L_Q	(0x03)

// Interfaces for ufs.c
void ufs_init(struct ufs_host *host);
int init_ufs_load_from_ufs(void);
void ufs_configure_ufs_device_lun(void);
void load_from_ufs(unsigned char *buffer, u32 lun, u32 pos_4k, u32 size);
void save_to_ufs(unsigned char *buffer, u32 lun, u32 pos_4k, u32 size);
u32 ufs_tc_wr_seq(struct ufs_host *host);
void ufshci_print_buf_align(u32 pBuf, u32 uByteSize, bool bIsByteAlign);
bool UFSHCI_CheckOCS(struct ufs_host *host);
void ufshci_waitcomplete_doorbell(struct ufs_host *host);
u32 ufs_get_num_of_active_tx_lanes(struct ufs_host *host);
u32 ufs_get_num_of_active_rx_lanes(struct ufs_host *host);
void ufs_store_uic_pwr_mode(struct ufs_host *host, u32 gear, u32 pwr_mode, u32 hs_series);
void ufs_post_pmc_cfg(struct ufs_host *host);
void ufs_device_get_info(void);
u32 ufs_cal_mb_size_from_qLogicalBlockCount(ufs_unit_descriptor pUnitDesc);
u32 ufs_pow(u32 x, u32 y);
void clean_invalidate_dcache_range(void *start, void *end);
void ufshci_utrd_invalidatecache(struct ufs_host *host, u32 db);
void ufshci_utrd_clean_and_invalidate(struct ufs_host *host, u32 db);
void ufshci_ucd_clean_and_invalidate(struct ufs_host *host, u32 db);
void ufshci_data_clean_and_invalidate(struct ufs_host *host, u32 db);
void ufshci_ucd_invalidatecache(struct ufs_host *host, u32 db);
void ufshci_utrd_cleancache(struct ufs_host *host, u32 db);
void ufshci_ucd_cleancache(struct ufs_host *host, u32 db);
void ufs_sync_desc(struct ufs_host *host, UFS_DCACHE_CTRL eFlush, u32 db);
void ufs_sync_desc_db(struct ufs_host *host, u32 db);
int ufs_query_read_descriptor(struct ufs_host *host, u8 id, u32 index,
				u8 *resp, u16 respsize);
int ufs_query_write_descriptor(struct ufs_host *host, u8 id, u32 index,
				u8 *descr, u16 size);

//Interfaces for ufs_api.c
void dev_init_pmu(struct ufs_host *host);
void dev_init_gpio(struct ufs_host *host);
void dev_init_clock(struct ufs_host *host);
void hci_idle_indicate(struct ufs_host *host);
void hci_sw_reset(struct ufs_host *host);
void hci_enable(struct ufs_host *host, bool enable);
void hci_device_reset_control(struct ufs_host *host, bool enable);
void hci_device_refclkout_control(struct ufs_host *host, bool enable);
void hci_vs_dfes_error_layber_enable(struct ufs_host *host);
void hci_vs_set_burst_len(struct ufs_host *host, u8 nval);
void ufs_vs_set_dfes_ocs_enable(struct ufs_host *host, u32 val);
void ufshic_set_interrupt(struct ufs_host *host);
void ufshic_set_vs_interrupt(struct ufs_host *host);
void ufs_interrupt_enable(struct ufs_host *host, u32 unmask);
void ufs_vs_interrupt_enable(struct ufs_host *host, u32 unmask);
void *ufs_get_uic_pwr_mode(struct ufs_host *host);
u32 ufs_get_num_of_lanes(struct ufs_host *host);
void ufs_init_unipro_phy(struct ufs_host *host);
int ufshci_wait_complete_dbr_timeout(struct ufs_host *host);
bool ufshci_check_is(struct ufs_host *host, u32 status, u32 vs_status);
int ufshci_wait_complete(struct ufs_host *host, u32 int_mask);
void ufs_vs_set_1us_to_cnt(struct ufs_host *host, u32 nsec);
void ufs_set_config_block_size(struct ufs_host *host, u32 block_size);
int ufs_get_config_prdt_size(struct ufs_host *host);
void ufs_set_config_dma_blocking(struct ufs_host *host, bool enable);
void ufs_set_config_prdt_size(struct ufs_host *host, u32 prdt_size);
bool fmp_set_uprsecurity(struct ufs_host *host, ufsp_upsec eUPSEC, u32 uInput);
void *ufs_get_descriptor_info(struct ufs_host *host);
void *ufs_get_host_config_info(struct ufs_host *host);
int ufs_get_config_block_size(struct ufs_host *host);

void ufs_set_doorbell_num(struct ufs_host *host, u32 db);
u32 ufs_get_doorbell_num(struct ufs_host *host);
//void ufs_get_utp_error_status(struct ufs_host *host, u8 *tlun_utpe, u8 *ttag_utpe, u8 *utpec);
void ufs_set_tag_num(struct ufs_host *host, u32 ntag);
u32 ufs_get_tag_num(struct ufs_host *host);
void ufs_set_lun_num(struct ufs_host *host, u32 nlun);
u32 ufs_get_lun_num(struct ufs_host *host);
void ufs_vs_set_prdt(struct ufs_host *host);
void ufs_vs_init_nexus_type(struct ufs_host *host);
void ufs_vs_set_v2p1_ctrl(struct ufs_host *host, u32 nval);
void ufs_vs_data_reorder(struct ufs_host *host, u32 nval);
void ufs_set_utr_nexus_type(struct ufs_host *host);
void ufs_clear_utr_nexus_type(struct ufs_host *host);
void ufs_vs_dma_blocking_enable(struct ufs_host *host, bool enable);

u32 ufs_get_qr_flag_num(struct ufs_host *host);
void ufs_set_qr_flag_num(struct ufs_host *host, u32 nflag);
u32 ufs_get_lba(struct ufs_host *host);
void ufs_set_lba(struct ufs_host *host, u32 lba);
u32 ufs_get_num_lba(struct ufs_host *host);
void ufs_set_num_lba(struct ufs_host *host, u32 num_lba);
void ufs_set_fua(struct ufs_host *host, bool fua);
bool ufs_get_fua(struct ufs_host *host);
void ufs_set_dpo(struct ufs_host *host, bool dpo);
bool ufs_get_dpo(struct ufs_host *host);
u32 swap_endian(u32 data);
u16 swap_endian_word(u16 word);
void UFS_WAIT_CLEAR_INT(struct ufs_host *host, u32 intr);

//Interfaces for ufs_mphy.c
void ufs_config_phy(struct ufs_host *host, const struct ufs_phy_cfg *cfg);
void ufs_check_num_of_lanes(struct ufs_host *host);
void ufs_set_clock_period(struct ufs_host *host);
u32 read_unipro_linkstartup_cnf_result(struct ufs_host *host);
void ufs_pre_link(struct ufs_host *host);
void ufs_post_link(struct ufs_host *host);
void set_TxRx_Gear(struct ufs_host *host, u32 gear);
void set_HSSeries(struct ufs_host *host, u32 rate);
void set_TxRx_Termination(struct ufs_host *host, u32 nVal);
u32 read_unipro_powermode_ind_result(struct ufs_host *host);
int ufs_set_after_pmc_done(struct ufs_host *host, bool hs_mode, u32 gear, u32 rate);
bool set_PA_PWRMode(struct ufs_host *host, u32 pwr_mode);
void ufs_pre_pmc_cfg(struct ufs_host *host);
bool UFS_MPHY_WaitMphyCdrLock(struct ufs_host *host, u32 tx_lane, u32 rx_lane);
u32 ufs_get_pma_addr(struct ufs_host *host, u32 addr, u32 lane, u32 cmn_trsv);

//Interfaces ufs_cmd.c
u32 ufs_nop_descriptor(struct ufs_host *host);
u32 ufs_setread10command(struct ufs_host *host, u64 *buf);
u32 ufs_setwrite10command(struct ufs_host *host, u64 *buf);
u32 ufs_query_setflag(struct ufs_host *host);
u32 ufs_query_readflag(struct ufs_host *host);
int ufs_query_responseflag(struct ufs_host *host);
int ufs_query_get_flag_value_of_responseflag(struct ufs_host *host);
u32 ufs_setrequestsensecommand(struct ufs_host *host, u8 ualloc_len, u32 *buf);
u32 ufs_settestunitreadycommand(struct ufs_host *host, u8 control);
u32 ufs_query_set_read_descriptor(struct ufs_host *host, u8 id, u16 despsize, u32 index);
int ufs_query_response(struct ufs_host *host, u8 *pDesp, u32 uDespSize);

void ufs_setup_config_descriptor(ufs_config_descriptor *pData,
				 unsigned int capacity);
u32 ufs_query_set_write_descriptor(struct ufs_host *host, u8 id, u16 despsize, u8 *pDesp, u8 index, bool prov);
int configure_ufs_device(struct ufs_host *host);
extern void clean_dcache_range(uint64_t, uint64_t);
extern void inv_dcache_range(uint64_t, uint64_t);
#ifdef __cplusplus
}
#endif

#endif /* _UFS_H_ */
