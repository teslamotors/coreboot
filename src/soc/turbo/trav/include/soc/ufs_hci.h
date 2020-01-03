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

#ifndef _UFS_HCI_H_
#define _UFS_HCI_H_

#define UFSHCI_CAP                         0x0000
#define UFSHCI_VER                         0x0008
#define UFSHCI_HCDDID                      0x0010
#define UFSHCI_HCPMID						0x0014
#define UFSHCI_AHIT						0x0018

/* Operation and Runtime */
#define UFSHCI_IS							0x0020
#define UFSHCI_IE							0x0024

#define UFSHCI_HCS							0x0030
#define UFSHCI_HCE							0x0034
#define UFSHCI_UECPA						0x0038
#define UFSHCI_UECDL						0x003C
#define UFSHCI_UECN						0x0040
#define UFSHCI_UECT						0x0044
#define UFSHCI_UECDME						0x0048
#define UFSHCI_UTRIACR						0x004C

/* UTP Transfer */
#define UFSHCI_UTRLBA						0x0050
#define UFSHCI_UTRLBAU						0x0054
#define UFSHCI_UTRLDBR						0x0058
#define UFSHCI_UTRLCLR						0x005C
#define UFSHCI_UTRLRSR						0x0060
#define UFSHCI_UTRLCNR						0x0064


/* UTP Task Management */
#define UFSHCI_UTMRLBA						0x0070
#define UFSHCI_UTMRLBAU					0x0074
#define UFSHCI_UTMRLDBR					0x0078
#define UFSHCI_UTMRLCLR					0x007C
#define UFSHCI_UTMRLRSR					0x0080

/* UIC Command */
#define UFSHCI_UICCMDR						0x0090
#define UFSHCI_UCMDARG1					0x0094
#define UFSHCI_UCMDARG2					0x0098
#define UFSHCI_UCMDARG3					0x009C

#define UFSHCI_CRYPTOCFG				0x0400


/* UFS Protector registers */

#define UPRCTRL							0x0000
#define UPRSTAT							0x0008
#define UPRSECURITY						0x0010
#define UPVERSION						0x001C
#define UPRENCKEY(NUM)					(0x0020 + (NUM*4))
#define UPRTWKEY(NUM)					(0x0040 + (NUM*4))
#define UPWCTRL							0x0100
#define UPWSTAT							0x0108
#define UPFEKM(NUM)						(0x0120 + (NUM*4))
#define UPFTKM(NUM)						(0x0140 + (NUM*4))
#define UPSBEGIN(NUM)					(0x0200 + (NUM*0x10))
#define UPSEND(NUM)						(0x0204 + (NUM*0x10))
#define UPSCTRL(NUM)					(0x020C + (NUM*0x10))
#define UPSSLUN(NUM)					(0x0208 + (NUM*0x10))

/*==================================================================
 *REGISTER STRUCTURE
 *==================================================================
 */

// rUFSHCI_CAP - Controller Capabilities (offset : 0x0000)*/
typedef union {
	u32 data;	// reset value :
	struct {
		unsigned num_utp_tranf_req_slot : 5;			// bit[4:0] : Number of UTP Transfer Request Slots (NUTRS)
		unsigned reserved_7_5 : 3;				// bit[7:5] : Reserved
		unsigned num_outstaing_rtt : 8;				// bit[15:8] : Number of outstanding READY TO TRANSFER (RTT)
		unsigned num_utp_task_man_req_slot : 3;			// bit[16:18] : Number of UTP Task Management Request Slots (NUTMRS)
		unsigned reserved_22_19 : 4;					// bit[22:19] : Reserved
		unsigned auto_hibern_support : 1;				// bit[23] : Auto hibernation Support (AUTOH8)
		unsigned addr_support : 1;						// bit[24] : 64-bit addressing supported
		unsigned out_order_data_deliv_support : 1;		// bit[25] : Out of order data delivery supported (OODDS)
		unsigned uic_dme_test_mode : 1;					// bit[26] : UIC DME_TEST_MODE command supprted
		unsigned reserved_31_27 : 5;					// bit[31:27] : Reserved
	} b;
} UFSHCI_CAP_t;

// rUFSHCI_VER - UFS Version (offset : 0x0008)
typedef union {
	u32 data;	// reset value :
	struct {
		unsigned version_suffix : 4;			// bit[3:0] : Version Suffix(VS)
		unsigned minor_version_num : 4;			// bit[7:4] : Minor version number (MNR)
		unsigned major_version_num : 8;			// bit[15:8] : Major version Number (MJR)
		unsigned reserved_31_16 : 16;			// bit[31:16] : Reserved
	} b;
} UFSHCI_VER_t;


// rUFSHCI_HCMID - Host Controller Identifacation Descriptor - Product ID and Manufacturer ID (offset : 0x0014)
typedef union {
	u32 data;
	struct {
		unsigned manufacturer_id_code : 8;		// bit[7:0] : Manufacturer ID code(MIC)
		unsigned bank_index :  8;				// bit[8:15] : Bank Index(BI)
		unsigned reserved_31_16 : 16;			// bit[31:16] : Reserved
	} b;
} UFSHCI_HCMID_t;

// rUFSHCI_IS - Interrupt Status (offset : 0x0020
typedef union {
	u32 data;
	struct {
		unsigned utp_xfer_req_compl_status : 1;		// bit[0] : UTP Transfer Request Completion Status (UTRCS)
		unsigned uic_dme_ep_rst_ind : 1;			// bit[1] : UIC DME_ENDPOINTRESET indication (UDEPRI)
		unsigned uic_err : 1;						// bit[2] : UIC Error (UE)
		unsigned uic_test_mode_status : 1;			// bit[3] : UIC Test Mode Status (UTMS)
		unsigned uic_pwr_mode_status : 1;			// bit[4] : UIC Power Mode Status (UPMS)
		unsigned uic_hiber_exit_status : 1;			// bit[5] : UIC Hibernate Exit Status (UHXS)
		unsigned uic_hiber_enter_status : 1;		// bit[6] : UIC Hibernate Enter Status (UHES)
		unsigned uic_link_lost_status : 1;			// bit[7] : UIC Link Lost Status (ULLS)
		unsigned uic_link_startup_status : 1;		// bit[8] : UIC Link Startup Status (ULSS)
		unsigned utp_tsk_manage_req_status : 1;		// bit[9] : UTP Task Management Request Completion Status (UTMRCS)
		unsigned uic_cmd_compl : 1;					// bit[10] : UIC Command Completion Status (UCCS)
		unsigned dev_fatal_err : 1;					// bit[11] : Device Fata Error (DFES)
		unsigned utp_err_status : 1;				// bit[12] : UTP Error Status (UTPES)
		unsigned reserved_15_13 : 3;				// bit[15:13] : Reserved
		unsigned host_contr_fatal_err_status : 1;	// bit[16] : Host Controller Fatal Error Status Enable (HCFES)
		unsigned sys_bus_fatal_err : 1;				// bit[17] : System Bus Fatal Error Status (SBFES)
		unsigned reserved_31_18 : 14;				// bit[31:18] : Reserved
	} b;
} UFSHCI_IS_t;

// rUFSHCI_IE - Interrupt Enable (offset : 0x0024)
typedef union {
	u32 data;
	struct {
		unsigned utp_xfer_req_compl_status_ena : 1;		// bit[0] : UTP Transfer Request Completion Status Enable (UTRCSE)
		unsigned uic_dme_ep_rst_ind_ena : 1;			// bit[1] : UIC DME_ENDPOINTRESET indication Enable (UDEPRIE)
		unsigned uic_err_ena : 1;						// bit[2] : UIC Error Enable (UEE)
		unsigned uic_test_mode_status_ena : 1;			// bit[3] : UIC Test Mode Status Enable (UTMSE)
		unsigned uic_pwr_mode_status_ena : 1;			// bit[4] : UIC Power Mode Status Enable (UPMSE)
		unsigned uic_hiber_exit_status_ena : 1;			// bit[5] : UIC Hibernate Exit Status Enable (UHXSE)
		unsigned uic_hiber_enter_status_ena : 1;		// bit[6] : UIC Hibernate Enter Status Enable (UHESE)
		unsigned uic_link_lost_status_ena : 1;			// bit[7] : UIC Link Lost Status Enable (ULLSE)
		unsigned uic_link_startup_status_ena : 1;		// bit[8] : UIC Link Startup Status Enable (ULSSE)
		unsigned utp_tsk_manage_req_status_ena : 1;		// bit[9] : UTP Task Management Request Completion Status Enable (UTMRCSE)
		unsigned uic_cmd_compl_ena : 1;					// bit[10] : UIC Command Completion Status Enable (UCCSE)
		unsigned dev_fatal_err_ena : 1;					// bit[11] : Device Fata Error Enable (DFESE)
		unsigned utp_err_ena : 1;						// bit[12] : UTP Error Enable (UTPEE)
		unsigned reserved_15_13 : 4;					// bit[15:13] : Reserved
		unsigned host_contr_fatal_err_status_ena : 1;	// bit[16] : Host Controller Fatal Error Status Enable (HCFESE)
		unsigned sys_bus_fatal_err_ena : 1;				// bit[17] : System Bus Fatal Error Status Enable (SBFESE)
		unsigned reserved_31_18 : 14;					// bit[31:18] : Reserved
	} b;
} UFSHCI_IE_t;

// rUFSHCI_HCS - Host Controller Status (offset : 0x0030)
typedef union {
	u32 data;
	struct {
		unsigned dev_present : 1;					// bit[0] : Device Present (DP)
		unsigned utp_xfer_req_lst_rdy : 1;			// bit[1] : UTP Transfer Request List Ready (UTRLRDY)
		unsigned utp_tsk_manage_req_lst_rdy : 1;	// bit[2] : UTP Task Mansgement Request List Ready (UTMRLRDY)
		unsigned uic_cmd_rdy : 1;					// bit[3] : UIC Command Ready (UCRDY)
		unsigned reserved_7_4 : 4;					// bit[7:4] : Reserved
		unsigned uic_pwr_mode_chan_req_status : 3;  // bit[10:8] : UIC Power Mode Change Request Status (UPMCRS)
		unsigned reserved_11 : 1;					// bit[11] : Reserved
		unsigned utp_err_code : 4;					// bit[15:12] : UTP Error Code
		unsigned tast_tag_utp_err : 8;				// bit[23:16] : Task Tag of UTP Error (TTAGUTPE)
		unsigned target_lun_utp_err :8;				// bit[31:24] : Target LUN of UTP error (TLUNUTPE)
	} b;
} UFSHCI_HCS_t;

// rUFSHCI_HCE - Host Controller Enable (offset : 0x0034)
typedef union {
	u32 data;
	struct {
		unsigned reserved_31_1 : 31;				// bit[31:1] : Reserved
		unsigned hst_contr_ena : 1;					// bit[0] : Host Controller Enable (HCE)
	} b;
} UFSHCI_HCE_t;

// rUFSHCI_UECPA - Host UIC Error Code PHY Adaptor Layer (0x0038)
typedef union {
	u32 data;
	struct {
		unsigned uic_pa_lyr_err_code : 5;			// bit[4:0] : UIC PHY Adapter Layer Error Code (EC)
		unsigned reserved_30_5 : 26;				// bit[30:5] : Reserved
		unsigned uic_pa_lyr_err : 1;				// bit[31] : UIC PHY Adapter Layer Error (ERR)
	} b;
} UFSHCI_UECPA_t;

// rUFSHCI_UECDL - Host UIC Error Code Data Link Layer (0x003C)
typedef union {
	u32 data;
	struct {
		unsigned uic_dl_lyr_err_code : 15;			// bit[14:0] : UIC Data Link Layer Error Code (EC)
		unsigned reserved_30_15 : 16;				// bit[30:15] : Reserved
		unsigned uic_dl_lyr_err : 1;				// bit[31] : UIC Data Link Layer Error (ERR)
	} b;
} UFSHCI_UECDL_t;

// rUFSHCI_UECN - Host UIC Error Code Network Layer (offset : 0x0040)
typedef union {
	u32 data;
	struct {
		unsigned uic_net_lyr_err_code : 3;			// bit[2:0] : UIC Network Layer Error Code (EC)
		unsigned reserved_30_3 : 28;				// bit[30:3] : Reserved
		unsigned uic_net_lyr_err : 1;				// bit[31] : UIC Network Layer Error (ERR)
	} b;
} UFSHCI_UECN_t;

// rUFSHCI_UECT - Host UIC Error Code Transport Layer (offset : 0x0044)
typedef union {
	u32 data;
	struct {
		unsigned uic_transp_lyr_err_code : 7;		// bit[6:0] : UIC Transport Layer Error Code (EC
		unsigned reserved_30_7 : 24;				// bit[30:7] : Reserved
		unsigned uic_transp_lyr_err : 1;			// bit[31] : UIC Transport Layer Error (ERR
	} b;
} UFSHCI_UECT_t;

// rUFSHCI_UECDME - Host UIC Error Code DME (offset : 0x0048)
typedef union {
	u32 data;
	struct {
		unsigned uic_dme_err_code : 1;				// bit[0] : UIC DME Error Code (EC)
		unsigned reserved_30_1 : 30;				// bit[30:1] : Reserved
		unsigned uic_dme_err : 1;					// bit[31] : UIC DME Error (ERR)
	} b;
} UFSHCI_UECDME_t;

// rUFSHCI_UTRIACR - UTP Transfer Request Interrupt Aggregation Control Register (offset : 0x004C)
typedef union {
	u32 data;
	struct {
		unsigned intr_aggr_timeout_val : 8;			// bit[7:0] : Interrupt Aggregation Timeout Value (IATOVAL)
		unsigned intr_aggr_countr_thresh : 5;		// bit[12:8] : Interrupt Aggregation Counter Threshold (IACTH)
		unsigned reserved_15_13 : 3;				// bit[15:13] : Reserved
		unsigned countr_timer_rst : 1;				// bit[16] : Counter and Timer Reset (CTR)
		unsigned reserved_19_17 : 3;				// bit[19:17] : Reserved
		unsigned intr_aggr_status_bit : 1;			// bit[20] : Interrupt Aggregation Status Bit (IASB)
		unsigned reserved_23_21 : 3;				// bit[23:21] : Reserved
		unsigned intr_aggr_param_write_ena : 1;		// bit[24] : Interrupt Aggregatio Parameter Write Enable (IAPWEN)
		unsigned reserved_30_25 : 6;				// bit[30:25] : Reserved
		unsigned intr_aggr_ena_disa : 1;			// bit[31] : Interrupt Aggregation Enable/Disable (IAEN)
	} b;
} UFSHCI_UTRIACR_t;

// rUFSHCI_UTRLBA - UTP Transfer Request List Base Address (0x0050)

typedef union {
	u32 data;
	struct {
		unsigned utrl_offset_en : 1;				// bit[0] : UTRL_OFFSET_EN
		unsigned reserved_1 : 1;					// bit[1] : Reserved
		unsigned utp_xfer_req_lst_base_addr : 30;	// bit[31:2] : UTP Transfer Request List Base Address (UTRLBA
	} b;
} UFSHCI_UTRLBA_t;


// rUFSHCI_UTRLBAU - UTP Transfer Request List Base Address Upper 32-bits (0x0054)
typedef union {
	u32 data;
	struct {
		unsigned utp_xfer_req_lst_base_addr_up : 32;	// bit[31:0] : UTP Transfer Request List Base Address Upper 32-bits (UTRLBAU
	} b;
} UFSHCI_UTRLBAU_t;

// rUFSHCI_UTRLDBR - UTP Transfer Request List Door Bell Register (0x0058)
typedef union {
	u32 data;
	struct {
		unsigned utp_xfer_req_lst_door_bell : 16;		// bit[15:0] : UTP Transfer Request List Door Bell (UTRLDBR)
		unsigned reserved_31_16 : 16;					// bit[31:16] : Reserved
	} b;
} UFSHCI_UTRLDBR_t;

// rUFSHCI_UTRLCLR - UTP Transfer Request List Clear Register (0x005C
typedef union {
	u32 data;
	struct {
		unsigned utp_xfer_req_lst_clr : 16;				// bit[15:0] : UTP Transfer Request List Clear (UTRLCLR)
		unsigned reserved_31_16 : 16;					// bit[31:16] : Reserved
	} b;
} UFSHCI_UTRLCLR_t;


// rUFSHCI_UTRLRSR - UTP Transfer Request List Run Stop (0x0060)

typedef union {
	u32 data;
	struct {
		unsigned utp_xfer_req_lst_run_stop : 1;			// bit[0] : UTP Transfer Request List Run Stop (UTRLRSR)
		unsigned reserved_31_1 : 31;					// bit[31:1] : Reserved
	} b;
} UFSHCI_UTRLRSR_t;


// rUFSHCI_UTMRLBA - UTP Task Management Request List Base Address (0x0070)
typedef union {
	u32 data;
	struct {
		unsigned reserved_1_0 : 2;						// bit[1:0] : Reserved
		unsigned utp_tm_req_lst_base_addr : 30;			// bit[31:2] : UTP Task Management Request List Base Address (UTMRLBA)
	} b;
} UFSHCI_UTMRLBA_t;


// rUFSHCI_UTMRLBAU - UTP Task Management Request List Base Address Upper 32-bits (0x0074)
typedef union {
	u32 data;
	struct {
		unsigned utp_tm_req_lst_base_addr_up : 32;		// bit[31:0] : UTP Task Management Request List Base Address Upper (UTMRLBAU
	} b;
} UFSHCI_UTMRLBAU_t;

// rUFSHCI_UTMRLDBR - UTP Task Management Request List Door Bell (0x0078)
typedef union {
	u32 data;
	struct {
		unsigned utp_tm_req_lst_door_bell : 4;			// bit[3:0] : UTP Task Management Request List Door Bell (UTMRLDBR)
		unsigned reserved_31_8 : 28;					// bit[31:4] : Reserved
	} b;
} UFSHCI_UTMRLDBR_t;

// rUFSHCI_UTMRLCLR - UTP Task Management Request List Clear Register (0x007C)
typedef union {
	u32 data;
	struct {
		unsigned utp_tm_req_lst_clr : 4;				// bit[3:0] : UTP Task Management Request List Clear (UTMRLCLR
		unsigned reserved_31_8 : 28;					// bit[31:8] : Reserved
	} b;
} UFSHCI_UTMRLCLR_t;

// rUFSHCI_UTMRLRSR - UTP Task Management Request List Run Stop (0x0080)
typedef union {
	u32 data;
	struct {
		unsigned utp_tm_req_lst_run_stop : 1;			// bit[0] : UTP Task Management Request Run Stop (UTMRLRSR
		unsigned reserved_31_1 : 31;					// bit[31:1] : Reserved
	} b;
} UFSHCI_UTMRLRSR_t;

// rUFSHCI_UICCMD - UIC Command (offset : 0x0090)
typedef union {
	u32 data;
	struct {
		unsigned cmd_opcode : 8;						// bit[7:0] : Command Opcode (CMDOP)
		unsigned reserved_31_8 : 24;					// bit[31:8] : Reserved
	} b;
} UFSHCI_UICCMD_t;

//(offset : 0x015c)
typedef union {
	u32 data;
	struct {
		unsigned utr_resp: 1;							// bit[0] : UTR Response UPIU is not matched
		unsigned utmr_resp: 1;							// bit[1] : UTMR Response UPIU is not matched
		unsigned rx_data: 1;							// bit[2] : Inbound RX Data UPIU is not matched
		unsigned rtt: 1;								// bit[3] :RTT UPIU is not matched
		unsigned reserved_31_4 : 1;						//bit[31:4]  Reserved
	} b;
} UFSHCI_VS_RX_UPIU_MATCH_ERROR_t;

#endif /* _UFS_HCI_H_ */
