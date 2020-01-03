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

#ifndef _UFS_SCSI_H_
#define _UFS_SCSI_H_

#include <types.h>
#define SCSI_CDB_LEN		16
#define SCSI_CMD_CONTROL_VENDOR_MASK			0xc0
#define SCSI_CMD_CONTROL_NACA					0x04	/* normal aca */
#define SCSI_CMD_CONTROL_LINK					0x01	/* multiple commands */

/* inquiry command format */
#define INQUIRY_CMD_DT				0x02	/* command support data */
#define INQUIRY_EVPD				0x01	/* enable vital product data */

/* inquiry standard data format */
#define INQUIRY_QUALIFIER_CONNECTED			0x00	/* device is connected */
#define INQUIRY_QUALIFIER_NOT_CONNECTED		0x20	/* device is not connected */
#define INQUIRY_QUALIFIER_RESERVED			0x40
#define INQUIRY_QUALIFIER_NOT_CAPABLE		0x60	/* not capable device */
#define INQUIRY_DEVICE_TYPE_DIRECT_ACCESS	0x00	/* direct-access device */
#define INQUIRY_DEVICE_TYPE_SEQUENTIAL		0x01	/* sequential-access device */
#define INQUIRY_DEVICE_TYPE_PRINTER			0x02	/* printer device */
#define INQUIRY_DEVICE_TYPE_PROCESSOR		0x03	/* processor device */
#define INQUIRY_DEVICE_TYPE_WRITE_ONCE		0x04	/* write-once device */
#define INQUIRY_DEVICE_TYPE_CR_ROM			0x05	/* cd-rom devices */
#define INQUIRY_DEVICE_TYPE_SCANNER			0x06	/* scanner device */
#define INQUIRY_DEVICE_TYPE_OPTICAL			0x07	/* optical memory device */
#define INQUIRY_DEVICE_TYPE_SMC				0x08	/* medium changer device */
#define INQUIRY_DEVICE_TYPE_COMM			0x09	/* communications device */
#define INQUIRY_DEVICE_TYPE_RAID			0x0c	/* storage array controller device */
#define INQUIRY_DEVICE_TYPE_SES				0x0d	/* enclosure services device */
#define INQUIRY_DEVICE_TYPE_RBC				0x0e	/* simplified direct-access device */
#define INQUIRY_DEVICE_TYPE_OCRW			0x0f	/* optical card reader/writer device */
#define INQUIRY_DEVICE_TYPE_OSD				0x10	/* object-based storage device */
#define INQUIRY_DEVICE_TYPE_NONE			0x1f	/* unknown or no device type */

#define INQUIRY_RMB		0x80	/* removable media */

#define INQUIRY_VERSION_ISO_MASK	0xc0
#define INQUIRY_VERSION_ISO_NONE	0x00
#define INQUIRY_VERSION_ISO_9316	0x80
#define INQUIRY_VERSION_ECMA_MASK	0x38
#define INQUIRY_VERSION_ECMA_NONE	0x00
#define INQUIRY_VERSION_ANSI_MASK	0x07
#define INQUIRY_VERSION_ANSI_NONE	0x00
#define INQUIRY_VERSION_ANSI_X3_131	0x02
#define INQUIRY_VERSION_ANSI_X3_301	0x03
#define INQUIRY_VERSION_ANSI_SPC_2	0x04

#define INQUIRY_AERC				0x80	/* asynchronous event reporting capability */
#define INQUIRY_NORM_ACA			0x20	/* normal aca */
#define INQUIRY_HI_SUP				0x10	/* hierarchical support */
#define INQUIRY_RESPONSE_DATA_FORMAT_MASK	0x0f
#define INQUIRY_RESPONSE_DATA_FORMAT_SPC2	0x02	/* spc-2 */

#define INQUIRY_SCCS				0x80	/* scc supported */

#define INQUIRY_VENDOR_INFO_LEN		8
#define INQUIRY_PRODUCT_ID_LEN		16
#define INQUIRY_PRODUCT_REV_LEN		4

#define INQUIRY_DATA_LEN			36

/* inquiry vital product data format */

#define INQUIRY_VPD_PAGE_SUPPORTED_PAGES	0x00	/* supported vpd pages */
#define INQUIRY_VPD_PAGE_SERIAL_NUMBER		0x80	/* unit serial number */
#define INQUIRY_VPD_PAGE_DEV_ID				0x83	/* device id */

#define INQUIRY_VPD_CODE_SET_BINARY			0x01	/* code set, binary */
#define INQUIRY_VPD_CODE_SET_ASCII			0x02	/* code set, ascii */

#define INQUIRY_VPD_ASSOCIATE_DEVICE		0x00	/* id is associated with the device */
#define INQUIRY_VPD_ASSOCIATE_PORT			0x10	/* id is associated with the port */

#define INQUIRY_VPD_ID_TYPE_VENDOR			0x01	/* id type, first 8 bytes is vendor id */
#define INQUIRY_VPD_ID_TYPE_EUI_64			0x02	/* id type, eui-64 */
#define INQUIRY_VPD_ID_TYPE_FC_FS			0x03	/* id type, fc-fs name id */
#define INQUIRY_VPD_ID_TYPE_PORT			0x04	/* id type, associated port */

/* maintenance out command format */
#define SERVICE_ACTION_MASK					0x0f	/* service action */
#define ADD_PERI_COMP_DEVICE				0x00	/* add peripheral device component device */


/* mode sense command format */
#define MODE_DBD							0x08	/* disable block descriptor */

#define MODE_PC_MASK						0xc0	/* page control */
#define MODE_PC_CURRENT						0x00	/* current values */
#define MODE_PC_CHANGEABLE					0x40	/* changeable values */
#define MODE_PC_DEFAULT						0x80	/* default values */
#define MODE_PC_SAVED						0xc0	/* saved values */
#define MODE_PAGE_MASK						0x3f	/* page code */
#define MODE_PAGE_VENDOR_SPECIFIC			0x00	/* vendor specific, 0x00, 0x20 ~ 0x3e */
#define MODE_PAGE_DEVICE_SPECIFIC			0x01	/* device specific, 0x01 ~ 0x1f */
#define MODE_PAGE_ALL						0x3f	/* return all pages */

/* mode sense data format */
#define MODE_MEDIUM_TYPE_DEFAULT			0x00	/* default medium */
#define MODE_WP								0x80	/* write protected */
#define MODE_PARAM_SPC2_HEADER_LEN 4

/* request sense data format */
#define SENSE_VALID							0x80	/* information field is valid */
#define SENSE_RESPONSE_CODE_MASK			0x7f	/* response code */
#define SENSE_RESPONSE_CODE_CURRENT			0x70	/* current error */
#define SENSE_RESPONSE_CODE_DEFERRED		0x71	/* deffered error */

#define SENSE_FILEMARK						0x80	/* meet filemark or setmark */
#define SENSE_EOM							0x40	/* end of medium */
#define SENSE_ILI							0x20	/* incorrect length indicator */
#define SENSE_SENSE_KEY_MASK				0x0f	/* sense key */

#define REQUEST_SENSE_DATA_LEN				18

/* start-stop unit command format */

#define UNIT_LO_EJ							0x02	/* load eject */
#define UNIT_START							0x01	/* 0:stop, 1:start */

/* sense key definitions */
#define SENSE_KEY_NO_SENSE					0x00
#define SENSE_KEY_RECOVERED_ERROR			0x01
#define SENSE_KEY_NOT_READY					0x02
#define SENSE_KEY_MEDIUM_ERROR				0x03
#define SENSE_KEY_HARDWARE_ERROR			0x04
#define SENSE_KEY_ILLEGAL_REQUEST			0x05
#define SENSE_KEY_UNIT_ATTENTION			0x06
#define SENSE_KEY_DATA_PROTECT				0x07
#define SENSE_KEY_BLACK_CHECK				0x08
#define SENSE_KEY_VENDOR_SPECIFIC			0x09
#define SENSE_KEY_ABORTED_COMMAND			0x0b
#define SENSE_KEY_VOLUME_OVERFLOW			0x0d
#define SENSE_KEY_MISCOMPARE				0x0e

/* asc / ascq list */
#define NO_ADDITIONAL_SENSE_INFORMATION		0x0000
#define NO_SEEK_COMPLETE					0x0200
#define WRITE_FAULT							0x0300
#define LOGICAL_DRIVE_BECOMING_READY		0x0401
#define LOGICAL_DRIVE_INITIALIZATION_REQUIRED 0x0402
#define LOGICAL_DRIVE_FORMAT_IN_PROGRESS	0x0404
#define LOGICAL_DRIVE_DEVICE_IS_BUSY		0x04ff
#define NO_REFERENCE_POSITION_FOUND			0x0600
#define LOGICAL_UNIT_COMMUNICATION_FAILURE	0x0800
#define LOGICAL_UNIT_COMMUNICATION_TIME_OUT	0x0801
#define LOGICAL_UNIT_COMMUNICATION_OVERRUN	0x0880
#define ID_CRC_ERROR						0x1000
#define UNRECOVERED_READ_ERROR				0x1100
#define ADDRESS_MARK_NOT_FOUND_FOR_ID_FIELD	0x1200
#define ADDRESS_MARK_NOT_FOUND_FOR_DATA_FIELD 0x1300
#define RECORDED_ENTITY_NOT_FOUND			0x1400
#define RECOVERED_DATA_WITH_RETRIES			0x1701
#define RECOVERED_DATA_WITH_ECC				0x1800
#define PARAMETER_LIST_LENGTH_ERROR			0x1a00
#define INVALID_COMMAND_OPERATION_CODE		0x2000
#define LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE	0x2100
#define INVALID_FIELD_IN_CDB				0x2400
#define LOGICAL_UNIT_NOT_SUPPORTED			0x2500
#define INVALID_FIELD_IN_PARAMETER_LIST		0x2600
#define PARAMETER_NOT_SUPPORTED				0x2601
#define PARAMETER_VALUE_INVALID				0x2602
#define WRITE_PROTECTED_MEDIA				0x2700
#define NOT_READY_TO_TRANSITION_MEDIA_CHANGED 0x2800
#define POWER_ON_RESET_OR_BUS_DEVICE_RESET_OCCURRED 0x2900
#define COMMANDS_CLEARED_BY_ANOTHER_INITIATOR 0x2f00
#define CANNOT_READ_MEDIUM_UNKNOWN_FORMAT	0x3001
#define FORMAT_COMMAND_FAILED				0x3101
#define SAVING_PARAMETERS_NOT_SUPPORT		0x3900
#define MEDIUM_NOT_PRESENT					0x3a00
#define DIAGNOSTIC_FAILURE_ON_COMPONENT(NN)	(0x4000 | nn)
#define OVERLAPPED_COMMAND_ATTEMPTED		0x4e00
#define UFS_TO_HOST_SYSTEM_INTERFACE_FAILURE 0x5400
#define INSUFFICIENT_RESOURCES				0x8000
#define UNKNOWN_ERROR						0xffff

#define ASC(w)		((u8)(((w) >> 8) & 0xff))
#define ASCQ(w)		((u8)(((w) >> 0) & 0xff))


/* INQUIRY DATA FORMAT */

#define INQUIRY_RESPONSE_DATA_FORMAT_UFI		0x01	/* UFI */

/* READ FORMAT CAPACITY DATA FORMAT */
#define MCD_CODE_UNFORMATTED					0x01	/* MAXIMUM FORMATTABLE CAPACITY */
#define MCD_CODE_FORMATTED						0x02	/* CURRENT MEDIA CAPACITY */
#define MCD_CODE_NO_CATRIDGE					0x03	/* MAXIMUM FORMATTABLE CAPACITY */

#define CLH_LEN									4	/* CAPACITY LIST HEADER LENGTH */
#define MCD_LEN									8	/* CURRENT/MAXIMUM CAPACITY DESCRIPTOR LENGTH */
#define FCD_LEN									8	/* FORMAT CAPACITY DESCRIPTOR LENGTH */

#define READ_FORMAT_CAPACITY_DATA_LEN			(CLH_LEN + MCD_LEN + FCD_LEN * 30)

/* READ CAPACITY DATA FORMAT */
#define CAPACITY_REL_ADR						0x01
#define CAPACITY_REL_PMI						0x01
#define READ_CAPACITY_DATA_LEN					8

/* MODE SENSE DATA FORMAT */
#define MODE_PAGE_RW_ERROR_RECOVERY				0x01	/* READ-WRITE ERROR RECOVERY PAGE */
#define MODE_PAGE_FLEXIBLE_DISK					0x05	/* FLEXIBLE DISK PAGE */
#define MODE_PAGE_RBAC							0x1B	/* REMOVABLE BLOCK ADDRESS CAPABILITIES PAGE */
#define MODE_PAGE_TIMER_AND_PROTECT				0x1C	/* TIMER AND PROTECT PAGE */

#define MODE_PARAM_HEADER_LEN					8
#define MODE_PARAM_RW_ERROR_RECOVERY_LEN		12
#define MODE_PARAM_FLEXIBLE_DISK_LEN			32
#define MODE_PARAM_RBAC_LEN						12
#define MODE_PARAM_TIMER_AND_PROTECT_LEN		8
#define MODE_PARAM_ALL_LEN						72

#define MODE_SENSE_DATA_LEN						192


/* MODE PARAMETER HEADER */
#define MODE_MEDIUM_TYPE_720K					0x1E	/* 729 KBYTE*/
#define MODE_MEDIUM_TYPE_1250K					0x93	/* 1.25 MBYTE*/
#define MODE_MEDIUM_TYPE_1440K					0x94	/* 1.44 MBYTE*/

#define MODE_WP									0x80	/* WRITE PROTECT */
#define MODE_DPOUFA								0x10

#define MODE_PS									0x80	/* PARAMETERS SAVABLE */

/* READ-WRITE ERROR RECOVERY PAGE */

#define MODE_AWRE								0x80	/* AUTOMATIC WRITE REALLOCATION ENABLE */
#define MODE_RC									0x10	/* READ CONTINIOUS */
#define MODE_PER								0x04	/* POST ERROR */
#define MODE_DCR								0x01	/* DISABLE CORRECTION */

/* FLEXIBLE DISK PAGE */

#define MODE_TRANSFER_RATE_250K					0x00FA	/* 250 KBIT/S */
#define MODE_TRANSFER_RATE_300K					0x012C	/* 300 KBIT/S */
#define MODE_TRANSFER_RATE_500K					0x01F4	/* 500 KBIT/S */
#define MODE_TRANSFER_RATE_1M					0x03E8	/* 1 MBIT/S */
#define MODE_TRANSFER_RATE_2M					0x07D0	/* 2 MBIT/S */
#define MODE_TRANSFER_RATE_5M					0x1388	/* 5 MBIT/S */

/* REMOVABLE BLOCK ACCESS CAPABILITIES PAGE */

#define MODE_SFLP								0x80	/* SYSTEM FLOPPY TYPE DEVICE */
#define MODE_SRFP								0x40	/* SUPPORT REPORTING FORMAT PROGRESS */

#define MODE_NCD								0x80	/* NON-CD OPTICAL DEVICE */
#define MODE_SML								0x40	/* SINGLE/MULTIPLE LUN SUPPORTED */
#define MODE_TLUN_MASK							0x07	/* TOTAL NUMBER OF LUNS */

/* TIMER AND PROTECT PAGE */

#define MODE_ITM_MASK							0x0F	/* INACTIVITY TIMER MULTIPLIER */
#define	  MODE_ITM_INFINITE						0x00	/* INFINITE */
#define	  MODE_ITM_125MS						0x01	/* 125 MILLI SECONDS */
#define	  MODE_ITM_250MS						0x02	/* 250 MILLI SECONDS */
#define	  MODE_ITM_500MS						0x03	/* 500 MILLI SECONDS */
#define	  MODE_ITM_1S							0x04	/* 1 SECOND */
#define	  MODE_ITM_2S							0x05	/* 2 SECONDS */
#define	  MODE_ITM_4S							0x06	/* 4 SECONDS */
#define	  MODE_ITM_8S							0x07	/* 8 SECONDS */
#define	  MODE_ITM_16S							0x08	/*16 SECONDS */
#define	  MODE_ITM_32S							0x09	/*32 SECONDS */
#define	  MODE_ITM_1M							0x0A	/* 1 MINUTE */
#define	  MODE_ITM_2M							0x0B	/* 2 MINUTES */
#define	  MODE_ITM_4M							0x0C	/* 4 MINUTES */
#define	  MODE_ITM_8M							0x0D	/* 8 MINUTES */
#define	  MODE_ITM_16M							0x0E	/*16 MINUTES */
#define	  MODE_ITM_32M							0x0F	/*32 MINUTES */

#define MODE_DISP								0x02	/* DISABLE MEDIA ACCESS UNTIL POWER CYCLE */
#define MODE_SWPP								0x01	/* SOFTWARE WRITE PROTECT UNTIL POWER CYCLE */

/* VERIFY COMMAND FORMAT */
#define VERIFY_DPO								0x10
#define VERIFY_BYTE_CHK							0x02
#define VERIFY_REL_ADR							0x01

/* INQUIRY DATA FORMAT */
#define INQUIRY_RESPONSE_DATA_FORMAT_8070I		0x01	/* SFF-8070I */

/* MODE SENSE DATA FORMAT */
#define MODE_PAGE_CACHING						0x08	/* CACHING PAGE */
#define MODE_PARAM_CACHING_LEN					32

#define UFS_VENDOR_CHECK_MINI_MON_DEV			0x00	/* VENDOR SPECIFIC FOR MINI-MONITOR */
#define UFS_VENDOR_SET_MINI_MON_MODE			0x01	/* VENDOR SPECIFIC FOR MINI-MONITOR */
#define UFS_VENDOR_CMD_DATA_LEN					 32

typedef union {
	u8 val;
	struct {
		u8 opcode: 6;
		u8 dd: 1;
		u8 hd: 1;
	} __attribute__ ((__packed__)) format;
} __attribute__ ((__packed__)) tran_type;


typedef enum {
	scsi = 0,
	query = 1,
} ufs_cmd;


typedef enum {
	task_attr_simple = 0,
	task_attr_ordered = 1,
	task_attr_head_of_queue = 2,
	task_attr_aca = 3,
} task_attr;

typedef union {
	u8 val;
	struct _flags_format {
		u8 task_attribute : 2;
		u8 reserved: 2;
		u8 op: 4;
	} __attribute__ ((__packed__)) format;
} __attribute__ ((__packed__)) flags;

typedef struct {
	tran_type type;
	flags flags;
	u8 lun;
	u8 task_tag;

	u8 cmd_set_type;
	u8 query_fn;
	u8 response;
	u8 status;

	u8 ehs_length;
	u8 device_info;
	u16 data_segment_length;
	u32 tsf[5];
} __attribute__ ((__packed__)) upiu_header;

typedef struct {
	upiu_header header;
	u32 *pehs;
	u32 headercrc;
	u32 *pdata;
	u32 datacrc;
} __attribute__ ((__packed__)) upiu;

typedef union {
u32 val;
	struct {
		unsigned cci : 8;
		unsigned r2 : 15;
		unsigned ce : 1;
		unsigned i : 1;
		unsigned dd : 2;
		unsigned r1 : 1;
		unsigned ct : 4;
	} __attribute__ ((__packed__)) format;
} __attribute__ ((__packed__)) utrd_dw0;

typedef struct {
	u8 ocs;
	u8 reserved[3];
} __attribute__ ((__packed__)) utrd_dw2;

typedef struct {
	u32 base;
} __attribute__ ((__packed__)) utrd_dw4;

typedef struct {
	u32 base64;
} __attribute__ ((__packed__)) utrd_dw5;

typedef struct {
	u16 resp_upiu_length;
	u16 resp_upiu_offset;
} __attribute__ ((__packed__)) utrd_dw6;

typedef struct {
	u16 prdt_length;
	u16 prdt_offset;
} __attribute__ ((__packed__)) utrd_dw7;

typedef union {
	u32 val;
	struct {
		unsigned r2 : 24;
		unsigned i : 1;
		unsigned r1 : 7;
	} __attribute__ ((__packed__)) format;
} __attribute__ ((__packed__)) utmrd_dw0;

typedef struct {
	u8 ocs;
	u8 reserved[3];
} __attribute__ ((__packed__)) utmrd_dw2;

typedef struct {
	u32 base;
	u32 base64;
	u32 reserved_1;
	u16 data_bytes;
	u16 reserved_2;
} __attribute__ ((__packed__)) prdt_t;


typedef union {
	u16 val;
	__attribute__ ((__packed__)) struct {
		u16 r1  : 10;
		u16 fkl : 1;
		u16 dkl : 1;
		u16 fas : 2;
		u16 das : 2;
	} format;
} __attribute__ ((__packed__)) aes_algorithm;

#define F_IV_VAL		4
#define F_KEY_VAL		8
#define F_TWKEY_VAL		8
#define D_IV_VAL		4

typedef struct {
	u32 base;
	u32 base64;
	u32 reserved_1;
	u16 data_bytes;
	aes_algorithm aes_sel;
	u32 f_iv[F_IV_VAL];				// File IV [127:0]
	u32 f_enckey[F_KEY_VAL];		//File Enc Key [255:0]
	u32 f_tweakkey[F_TWKEY_VAL];	//File Tweak Key [255:0]
	u32 d_iv[D_IV_VAL];				// File IV [127:0]
	u32 reserved_2;
	u32 reserved_3;
	u32 reserved_4;
	u32 reserved_5;
} __attribute__ ((__packed__)) prdt_fmp;


#define MAX_UPIU_DW_SIZE  0x90//write configuration//32; /* 32 * 4 = 128 */ //20130905
#define UPIU_HEADER_SIZE  32
#define UPIU_SENSE_DATA_SIZE  32
#define UPIU_MAX_SIZE  (UPIU_HEADER_SIZE + (MAX_UPIU_DW_SIZE * sizeof(u32)))

#if IS_ENABLED(CONFIG_BOOTBLOCK_X)
  #define MAX_NUM_OF_PRDT	32
#else
  #define MAX_NUM_OF_PRDT	1024
#endif

#define MAX_UFS_UTRL 16
#define MAX_UFS_UTMRL 4

typedef struct {
	utrd_dw0 dw0;
	u32 dw1;
	utrd_dw2 dw2;
	u32 dw3;
	utrd_dw4 dw4;
	utrd_dw5 dw5;
	utrd_dw6 dw6;
	utrd_dw7 dw7;
} __attribute__ ((__packed__)) utrd_t;

typedef struct {
	utmrd_dw0 dw0;
	u32 dw1;
	utmrd_dw2 dw2;
	u32 dw3;
	upiu_header request_upiu;
	upiu_header response_upiu;
} __attribute__ ((__packed__)) utmrd_t;

typedef struct {
	upiu_header request_upiu;
	u32 request_data[MAX_UPIU_DW_SIZE];
	upiu_header response_upiu;
	u32 response_data[MAX_UPIU_DW_SIZE];
#if 0
	prdt_fmp prdt[MAX_NUM_OF_PRDT];
#else
	prdt_t prdt[MAX_NUM_OF_PRDT];
#endif

} __attribute__ ((__packed__)) ucd_t;

typedef enum {
	task_status_invalid = 0x00000000,
	task_status_new_task = 0x00000001,
	task_status_data_out = 0x00000002,
	task_status_data_out_new = 0x00000003,
	task_status_rtt = 0x00000004,
	task_status_data_in = 0x00000008,
	task_status_response = 0x00000010,
	task_status_complete = 0x00000020,
	task_status_abort = 0x00000040,
} task_status;

typedef enum {
	opcode_nop_out = 0x00,
	opcode_nop_in = 0x20,

	opcode_command = 0x01,
	opcode_response = 0x21,

	opcode_data_out = 0x02,
	opcode_data_in = 0x22,

	opcode_tm_request = 0x04,
	opcode_tm_response = 0x24,

	opcode_rtt = 0x31,

	opcode_query_request = 0x16,
	opcode_query_response = 0x36,

	opcode_reject = 0x3f,
} upiu_opcode;

typedef enum {
	cmd_invalid = 0xff,
	cmd_inquiry = 0x12,
	cmd_mode_select = 0x55,
	cmd_mode_sense = 0x5a,
	cmd_read6 = 0x08,
	cmd_read10 = 0x28,
	cmd_read16 = 0x88,
	cmd_read_buffer = 0x3c,
	cmd_write6 = 0x0a,
	cmd_write10 = 0x2a,
	cmd_write16 = 0x8a,
	cmd_write_buffer = 0x3b,
	cmd_read_capacity10 = 0x25,
	cmd_read_capacity16 = 0x9e,
	cmd_report_luns = 0xa0,
	cmd_format_unit = 0x04,
	cmd_test_unit_ready = 0x00,
	cmd_verify10 = 0x2f,
	cmd_start_stop_unit = 0x1b,
	cmd_request_sense = 0x03,
	cmd_send_diagnostic = 0x1d,
	cmd_synchronize_cache = 0x35,
	cmd_synchronize_cache16 = 0x91,
	cmd_prefetch10 = 0x34,
	cmd_prefetch16 = 0x90,
	cmd_unmap = 0x42,
	cmd_security_protocol_out = 0xb5,
	cmd_security_protocol_in = 0xa2,
} ufs_scsi_cmd;

typedef enum {
	scsi_status_reserved = 0x00,
	scsi_status_good = 0x00,
	scsi_status_check_condition = 0x02,
	scsi_status_condition_met = 0x04,
	scsi_status_busy = 0x08,
	scsi_status_reservation_confilict = 0x18,
	scsi_status_task_set_full = 0x28,
	scsi_status_aca_active = 0x30,
	scsi_status_task_aborted = 0x40
} scsi_status;

typedef enum {
	scsi_response_target_success = 0x00,
	scsi_response_target_failure = 0x01,
} scsi_response;

typedef enum {
	scsi_query_response_success = 0x00,
	scsi_query_response_para_not_readable = 0xF6,
	scsi_query_response_para_not_writeable = 0xF7,
	scsi_query_response_para_already_written = 0xF8,
	scsi_query_response_invalid_length = 0xF9,
	scsi_query_response_invalid_value = 0xFA,
	scsi_query_response_invalid_selector = 0xFB,
	scsi_query_response_invalid_index = 0xFC,
	scsi_query_response_invalid_idn = 0xFD,
	scsi_query_response_invalid_opcode = 0xFE,
	scsi_query_response_general_failure = 0xFF,
} scsi_query_response;

typedef enum {
	scsi_tm_response_function_complete = 0x00,
	scsi_tm_response_function_not_supported = 0x04,
	scsi_tm_response_function_failed = 0x05,
	scsi_tm_response_function_succeeded = 0x08,
	scsi_tm_response_incorrect_logical_unit_number = 0x09,
} scsi_tm_service_response;

typedef enum {
	scsi_rpmb_authen_key_pgm_req = 0x0001,
	scsi_rpmb_read_wr_cnt_req = 0x0002,
	scsi_rpmb_authen_data_write_req = 0x0003,
	scsi_rpmb_authen_data_read_req = 0x0004,
	scsi_rpmb_authen_result_read_req = 0x0005,
	scsi_rpmb_authen_secure_wr_prot_cfg_blk_write_req = 0x0006, //v2.1
	scsi_rpmb_authen_secure_wr_prot_cfg_blk_read_req = 0x0007, //v2.1
} scsi_rpmb_req_msg_type;

typedef enum {
	scsi_rpmb_authen_key_pgm_res = 0x0100,
	scsi_rpmb_read_wr_cnt_res = 0x0200,
	scsi_rpmb_authen_data_write_res = 0x0300,
	scsi_rpmb_authen_data_read_res = 0x0400,
	scsi_rpmb_authen_secure_wr_prot_cfg_blk_write_res = 0x0600, //v2.1
	scsi_rpmb_authen_secure_wr_prot_cfg_blk_read_res = 0x0700, //v2.1
} scsi_rpmb_res_msg_type;

typedef enum {
	scsi_rpmb_operation_ok = 0x0000,
	scsi_rpmb_general_failure = 0x0001,
	scsi_rpmb_authen_failure = 0x0002,
	scsi_rpmb_counter_failure = 0x0003,
	scsi_rpmb_address_failure = 0x0004,
	scsi_rpmb_write_failure = 0x0005,
	scsi_rpmb_read_failure = 0x0006,
	scsi_rpmb_authenkey_not_programed = 0x0007,
	scsi_rpmb_secure_wr_prot_cfg_blk_access_failure = 0x0008, //v2.1
	scsi_rpmb_invalid_secure_wr_prot_cfg_parameter = 0x0009, //v2.1
	scsi_rpmb_secure_wr_prot_not_applicable = 0x000A, //v2.1
} scsi_rpmb_operation_result;

typedef enum {
	scsi_rpmb_wpt_nv_type = 0x0, //WPF bit is persistent through power cycle and hardware reset. WPF flag value may only be changed by writing to Secure Write Protect Configuration Block.
	scsi_rpmb_wpt_p_type = 0x1, //WPF bit is automatically cleared to 0b after power cycle or hardware reset.
	scsi_rpmb_wpt_nv_awp_type = 0x2, //WPF bit is automatically set to 1b after power cycle or hardware reset.

} scsi_rpmb_secure_wr_prot_type;

typedef enum {
	qr_opcode_nop			   = 0x00,
	qr_opcode_read_descriptor   = 0x01,
	qr_opcode_write_descriptor  = 0x02,
	qr_opcode_read_attribute	= 0x03,
	qr_opcode_write_attribute   = 0x04,
	qr_opcode_read_flag		 = 0x05,
	qr_opcode_set_flag		  = 0x06,
	qr_opcode_clear_flag		= 0x07,
	qr_opcode_toggle_flag	   = 0x08,
} upiu_tsf_qr_opcode;

typedef enum {
	qr_flag_idn_bdeviceinit				= 0x01,
	qr_flag_idn_wp_perm					= 0x02,
	qr_flag_idn_wp_por					= 0x03,
	qr_flag_idn_background_ops_en		= 0x04,
	qr_flag_idn_reserved				= 0x05,
	qr_flag_idn_purge_en				= 0x06,
	qr_flag_idn_reserved2				= 0x07,
	qr_flag_idn_fphyresourceremoval		= 0x08,
	qr_flag_idn_fbusyrtc				= 0x09,
	qr_flag_idn_reserved3				= 0x0a,
	qr_flag_idn_fpermanentdisablefwupdate	= 0x0b,
} upiu_tsf_qr_flag_idn;

typedef enum {
	qf_standard_read_req	= 0x01,
	qf_vendor_read_func_00	= 0x40,
	qf_standard_write_req	= 0x81,
	qf_vendor_write_func_00	= 0xc0,
} upiu_query_func;


typedef enum {
	qr_descr_idn_device	   = 0x00,
	qr_descr_idn_config	   = 0x01,
	qr_descr_idn_unit		 = 0x02,
	qr_descr_idn_rfu_03	   = 0x03,
	qr_descr_idn_interconn	= 0x04,
	qr_descr_idn_string	   = 0x05,
	qr_descr_idn_rfu_06	   = 0x06,
	qr_descr_idn_geometry	 = 0x07,
	qr_descr_idn_pwr		  = 0x08,
	qr_descr_idn_health		= 0x09,
} upiu_tsf_qr_descr_idn;

typedef enum {
	qr_attr_idn_bboot_lun_en				= 0x00,
	qr_attr_idn_bcurrent_power_mode		= 0x02,
	qr_attr_idn_bactive_icc_level		= 0x03,
	qr_attr_idn_bout_of_order_data_en		= 0x04,
	qr_attr_idn_bback_ground_op_status	= 0x05,
	qr_attr_idn_bpurge_status				= 0x06,
	qr_attr_idn_bmax_data_in_size		= 0x07,
	qr_attr_idn_bmax_data_out_size		= 0x08,
	qr_attr_idn_bdyn_cap_needed			= 0x09,
	qr_attr_idn_bref_clk_freq				= 0x0a,
	qr_attr_idn_bconfig_descr_lock		= 0x0b,
	qr_attr_idn_bmax_num_of_rtt			= 0x0c,
	qr_attr_idn_wexception_event_control  = 0x0d,
	qr_attr_idn_wexception_event_status	= 0x0e,
	qr_attr_idn_dseconds_passed			= 0x0f,
	qr_attr_idn_wcontext_conf				= 0x10,
	qr_attr_idn_dcorr_prg_blk_num		= 0x11,
	//UFS v2.1 supported items
	qr_attr_idn_bdeviceffustatus		= 0x14,
	qr_attr_idn_bpsastate				= 0x15,
	qr_attr_idn_dpsadatasize			= 0x16,
	qr_attr_idn_max						= 0x16,
} upiu_tsf_qr_attr_idn;

typedef enum {
	task_management_function_abort_task = 0x01,
	task_management_function_abort_task_set = 0x02,
	task_management_function_clear_task_set = 0x04,
	task_management_function_logical_unit_reset = 0x08,
	task_management_function_query_task = 0x80,
	task_management_function_query_task_set = 0x81,
} task_management_function;


typedef enum {
	string_index_manufacturer_name = 0x01,
	string_index_product_name = 0x02,
	string_index_serial_number = 0x03,
	string_index_oem_id = 0x04,
	string_index_product_revision_level = 0x05,
} string_descriptor_index;

typedef enum {
	powermode_idle = 0x00,
	powermode_preactive = 0x10,
	powermode_active = 0x11,
	powermode_presleep = 0x20,
	powermode_ufssleep = 0x22,
	powermode_prepowerdown = 0x30,
	powermode_ufspowerdown = 0x33,
} current_power_mode;

#define ufs_release_0110				0x0110
#define ufs_release_0200				0x0200

#define ufs_max_device_address		  127	 /* function address, 0:default, 1 ~ 127 */
#define ufs_max_endpoint				15	  /* endpoint number, 0:default, 1 ~ 15 */
#define ufs_max_descr_len			   255	 /* descriptor length */
#define ufs_max_hub_port				255	 /* hub port, 0:reserved, 1 ~ 255 */

/* defaults until set_address command is received */
#define ufs_default_device_address	  0	   /* default device address */
#define ufs_default_max_packet		  8	   /* maximum packet size of the default endpoint */

/* ufs device classes */

#define ufs_dev_class_msc			   0x00	/* device class is mass storage class */
#define ufs_dev_class_comm			  0x02	/* communication device or wmc 1.0 */
#define ufs_dev_class_hub			   0x09	/* hub device */
#define ufs_dev_class_diagnostic		0xdc	/* diagnostic device */
#define ufs_dev_class_wireless		  0xe0	/* wireless controller */
#define ufs_dev_class_miscell		   0xef	/* miscellaneous device */
#define ufs_dev_class_vendor_specific   0xff	/* vendor specific device */

/* ufs interface classes */
#define ufs_if_class_mass_storage	   0x00	/* mass storage interface */

#define ufs_if_class_audio			  0x01	/* audio interface */
#define ufs_if_class_cdc				0x02	/* cdc-control interface */
#define ufs_if_class_hid				0x03	/* human interface */
#define ufs_if_class_monitor			0x04	/* monitor interface */
#define ufs_if_class_physical		   0x05	/* physical interface */
#define ufs_if_class_image			  0x06	/* image interface (subclass & protocol must be 1) */
#define ufs_if_class_printer			0x07	/* printing interface */
//#define ufs_if_class_mass_storage	   0x08	/* mass storage interface */
#define ufs_if_class_hub				0x09	/* hub interface */
#define ufs_if_class_data			   0x0a	/* cdc-data interface */
#define ufs_if_class_ccid			   0x0b	/* chip card interface */
#define ufs_if_class_content_security   0x0d	/* content security interface */
#define ufs_if_class_video			  0x0e	/* video interface */
#define ufs_if_class_diagnostic		 0xdc	/* diagnostic device */
#define ufs_if_class_wireless		   0xe0	/* wireless controller */
#define ufs_if_class_app				0xfe	/* application specific interface */
#define ufs_if_class_vendor_specific	0xff	/* vendor specific interface */

/* ufs request types */

#define ufs_req_type_direction_mask   0x80
#define ufs_req_type_host_to_dev	  0x00
#define ufs_req_type_dev_to_host	  0x80
#define ufs_req_type_category_mask	0x60
#define ufs_req_type_standard		 0x00
#define ufs_req_type_class			0x20
#define ufs_req_type_vendor		   0x40
#define ufs_req_type_reserved		 0x60
#define ufs_req_type_recipient_mask   0x03
#define ufs_req_type_device		   0x00
#define ufs_req_type_interface		0x01
#define ufs_req_type_endpoint		 0x02
#define ufs_req_type_other			0x03

/* ufs task management request */
#define ufs_req_type_tm_abort_task	  0x01
#define ufs_req_type_tm_abort_task_set  0x02
#define ufs_req_type_tm_clear_task_set  0x04
#define ufs_req_type_tm_lu_reset		0x08
#define ufs_req_type_tm_query_task	  0x80
#define ufs_req_type_tm_query_task_set  0x81

/* ufs request codes */
#define ufs_req_get_status			  0x00
#define ufs_req_clear_feature		   0x01
#define ufs_req_get_state			   0x02
#define ufs_req_set_feature			 0x03
#define ufs_req_reserved_04			 0x04
#define ufs_req_set_address			 0x05
#define ufs_req_get_descriptor		  0x06
#define ufs_req_set_descriptor		  0x07
#define ufs_req_get_configuration	   0x08
#define ufs_req_set_configuration	   0x09
#define ufs_req_get_interface		   0x0a
#define ufs_req_set_interface		   0x0b
#define ufs_req_sync_frame			  0x0c

/* ufs hub class request codes */

#define ufs_req_clear_tt_buffer		 0x08
#define ufs_req_reset_tt				0x09
#define ufs_req_get_tt_state			0x0a
#define ufs_req_stop_tt				 0x0b

/* ufs request length */

#define ufs_req_len_get_dev_status	  2
#define ufs_req_len_get_if_status	   2
#define ufs_req_len_get_ep_status	   2
#define ufs_req_len_get_hub_status	  4
#define ufs_req_len_get_port_status	 4
#define ufs_req_len_clear_feature	   0
#define ufs_req_len_set_feature		 0
#define ufs_req_len_set_address		 0
#define ufs_req_len_get_descriptor	  (-1)
#define ufs_req_len_set_descriptor	  (-1)
#define ufs_req_len_get_configuration   1
#define ufs_req_len_set_configuration   0
#define ufs_req_len_get_interface	   1
#define ufs_req_len_set_interface	   0
#define ufs_req_len_get_sync_frame	  2

/* ufs port state */

#define ufs_port_state_not_configured   1	   /* port is not configured */
#define ufs_port_state_powered_off	  2	   /* port is powered off */
#define ufs_port_state_disconnected	 3	   /* port is disconnedted */
#define ufs_port_state_disabled		 4	   /* port is disabled */
#define ufs_port_state_resetting		5	   /* port is resetting */
#define ufs_port_state_enabled		  6	   /* port is enabled */
#define ufs_port_state_transmit		 7	   /* port is transmit */
#define ufs_port_state_suspended		8	   /* port is suspended */
#define ufs_port_state_resuming		 9	   /* port is resuming */

/* ufs device state */

#define ufs_device_state_attached	   1	   /* device is attached */
#define ufs_device_state_powered		2	   /* device is powered */
#define ufs_device_state_default		3	   /* device has been reset */
#define ufs_device_state_address		4	   /* device address is assigned */
#define ufs_device_state_configured	 5	   /* device is configured */
#define ufs_device_state_suspended	  6	   /* device is suspended */

/* ufs device status */

#define ufs_status_device_self_powered  0x0001
#define ufs_status_device_remote_wakeup 0x0002

/* max power available from a hub port, ma */

#define ufs_power_self_powered		  500
#define ufs_power_bus_powered		   100

/* ufs endpoint status */

#define ufs_status_endpoint_halt		0x0001

/* ufs hub status */

#define ufs_status_hub_over_current	 0x0002
#define ufs_status_hub_local_power	  0x0001

/* ufs hub status change */

#define ufs_status_c_hub_over_current   0x0002
#define ufs_status_c_hub_local_power	0x0001

/* ufs port status */

#define ufs_status_port_indicator	   0x1000
#define ufs_status_port_test			0x0800
#define ufs_status_port_high_speed	  0x0400
#define ufs_status_port_low_speed	   0x0200
#define ufs_status_port_power		   0x0100
#define ufs_status_port_reset		   0x0010
#define ufs_status_port_over_current	0x0008
#define ufs_status_port_suspend		 0x0004
#define ufs_status_port_enable		  0x0002
#define ufs_status_port_connection	  0x0001

/* ufs port status change */

#define ufs_status_c_port_reset		 0x0010
#define ufs_status_c_port_over_current  0x0008
#define ufs_status_c_port_suspend	   0x0004
#define ufs_status_c_port_enable		0x0002
#define ufs_status_c_port_connection	0x0001

/* ufs core defined standard feature selectors */

#define ufs_fsel_endpoint_halt		  0x0000
#define ufs_fsel_device_remote_wakeup   0x0001
#define ufs_fsel_device_test_mode	   0x0002
#define ufs_fsel_otg_b_hnp_enable	   0x0003
#define ufs_fsel_otg_a_hnp_support	  0x0004
#define ufs_fsel_otg_a_alt_hnp_support  0x0005

/* ufs core defined hub class feature selectors */

#define ufs_fsel_c_hub_local_power	  0x0000
#define ufs_fsel_c_hub_over_current	 0x0001

#define ufs_fsel_port_connection		0x0000
#define ufs_fsel_port_enable			0x0001
#define ufs_fsel_port_suspend		   0x0002
#define ufs_fsel_port_over_current	  0x0003
#define ufs_fsel_port_reset			 0x0004
#define ufs_fsel_port_power			 0x0008
#define ufs_fsel_port_low_speed		 0x0009
#define ufs_fsel_c_port_connection	  0x0010
#define ufs_fsel_c_port_enable		  0x0011
#define ufs_fsel_c_port_suspend		 0x0012
#define ufs_fsel_c_port_over_current	0x0013
#define ufs_fsel_c_port_reset		   0x0014

/* ufs core defined test selectors */

#define ufs_test_reserved			   0x00
#define ufs_test_j					  0x01
#define ufs_test_k					  0x02
#define ufs_test_se0_nak				0x03
#define ufs_test_packet				 0x04
#define ufs_test_force_enable		   0x05
/*
0x06 ~ 0x3f reserved for standard test selectors
0x40 ~ 0xbf reserved
0xc0 ~ 0xff reserved for vendor-specific test modes
*/

/* ufs dwg defined feature selectors */

#define ufs_fsel_interface_power_d0	 0x0002
#define ufs_fsel_interface_power_d1	 0x0003
#define ufs_fsel_interface_power_d2	 0x0004
#define ufs_fsel_interface_power_d3	 0x0005

/* ufs descriptor types */

#define ufs_descr_device			0x00
#define ufs_descr_configuration		0x01
#define ufs_descr_unit				0x03

#if 1
#define ufs_descr_interface			 0x04
#define ufs_descr_endpoint			  0x05
#define ufs_descr_device_qualifier	  0x06
#define ufs_descr_other_speed_config	0x07
#define ufs_descr_interface_power	   0x08
#define ufs_descr_otg				   0x09
#define ufs_descr_debug				 0x0a
#define ufs_descr_interface_association 0x0b
#endif

/* ufs descriptor device type */

#define ufs_descr_dev_type_device	   0x00
#define ufs_descr_dev_type_host		 0x01

/* ufs dwg defined descriptor types */

#define ufs_descr_reserved			  0x06
#define ufs_descr_config_power		  0x07
#define ufs_descr_interface_power	   0x08
#define ufs_descr_otg				   0x09
#define ufs_descr_hid				   0x21
#define ufs_descr_report				0x22
#define ufs_descr_physical			  0x23
#define ufs_descr_hub				   0x29

#define ufs_descr_make_type_and_index(d, i) ((((ushort)d) << 8 | i))

/* definitions for bits in the bmattributes field of a configuration descriptor */

#define ufs_config_powered_mask		 0xc0
#define ufs_config_bus_powered		0x80
#define ufs_config_self_powered	   0x40
#define ufs_config_remote_wakeup		0x20

/* endpoint direction bits in the bendpointaddress field of a endpoint descriptor */

#define ufs_endpoint_direction_mask	 0x80
#define ufs_endpoint_direction_out	0x00
#define ufs_endpoint_direction_in	 0x80
#define ufs_endpoint_address_mask	   0x0f

/* endpoint type bits in the bmattributes field of an endpoint descriptor */

#define ufs_endpoint_usage_type_mask	0x30
#define ufs_endpoint_type_data		0x00
#define ufs_endpoint_type_feedback	0x10
#define ufs_endpoint_type_feedback_data 0x20
#define ufs_endpoint_synch_type_mask	0x0c
#define ufs_endpoint_type_nosynch	 0x00
#define ufs_endpoint_type_asynch	  0x04
#define ufs_endpoint_type_adaptive	0x08
#define ufs_endpoint_type_synch	   0x0c
#define ufs_endpoint_transfer_type_mask 0x03
#define ufs_endpoint_type_control	 0x00
#define ufs_endpoint_type_isochronous 0x01
#define ufs_endpoint_type_bulk		0x02
#define ufs_endpoint_type_interrupt   0x03

/* additional transaction bits in the wmaxpacketsize field of an endpoint descriptor */
#define ufs_endpoint_mps_mask		   0x07ff
#define ufs_endpoint_transaction_mask   0x1800
#define ufs_endpoint_transaction_1	0x0000
#define ufs_endpoint_transaction_2	0x0800
#define ufs_endpoint_transaction_3	0x1000
#define ufs_endpoint_transaction_4	0x1800

/* definitions for bits in the bmattributes field of an otg descriptor */

#define ufs_otg_hnp					 0x02
#define ufs_otg_srp					 0x01

/* hub characteristic bits in the bhubcharacteristics field of an hub descriptor */
#define ufs_hub_psm					 0x01	/* power switching mode, 0:ganged, 1:indivisual */
#define ufs_hub_nps					 0x02	/* no power switching */
#define ufs_hub_comp					0x04	/* compound device */
#define ufs_hub_ocpm					0x08	/* over current protection mode, 0:global, 1:indivisual */
#define ufs_hub_nocp					0x10	/* no over current protection */

typedef union {
	u8 val;
	struct _sense_data0_format {
		unsigned response_code: 7;
		unsigned valid: 1;
	} __attribute__ ((__packed__)) format;
} __attribute__ ((__packed__)) sense_data_0;

typedef union {
	u8 val;
	struct _sense_data1_format {
		unsigned sensekey: 4;
		unsigned d2_reserved: 1;
		unsigned ili: 1;
		unsigned eom: 1;
		unsigned filemark: 1;
	} __attribute__ ((__packed__)) formate;
} __attribute__ ((__packed__)) sense_data_2;

typedef struct {
	u16 size;
	sense_data_0 data0;
	u8 obsolete;
	sense_data_2 data2;
	u32 information;
	u8 additional_length;
	u32 command_specific;
	u8 asc;
	u8 ascq;
	u8 fruc;
	u8 sensekey_specific[3];
} __attribute__ ((__packed__)) sense_data;

typedef struct {
	u8 opcode;
	u8 lba_msb;
	u8 lba_m;
	u8 lba_lsb;
	u8 length;
	u8 control;
} __attribute__ ((__packed__)) scsi_read_6;

typedef struct {
	u8 opcode;
	u8 flags;
	u16 lba_msb;
	u16 lba_lsb;
	u8 group_number;
	u8 length_msb;
	u8 length_lsb;
	u8 control;
} __attribute__ ((__packed__)) scsi_read_10;

typedef struct {
	u8 opcode;
	u8 flags;
	u32 lba_msb;
	u32 lba_lsb;
	u32 length;
	u8 group_number;
	u8 control;
} __attribute__ ((__packed__)) scsi_read_16;

typedef struct {
	u8 opcode;
	u8 mode;
	u8 buffer_id;
	u8 buffer_offset_lba_0;
	u8 buffer_offset_lba_1;
	u8 buffer_offset_lba_2;
	u8 param_list_length_0;
	u8 param_list_length_1;
	u8 param_list_length_2;
	u8 control;
} __attribute__ ((__packed__)) scsi_read_buffer;

typedef struct {
	u8 opcode;
	u8 lba_msb;
	u8 lba_m;
	u8 lba_lsb;
	u8 length;
	u8 control;
} __attribute__ ((__packed__)) scsi_write_6;

typedef struct {
	u8 opcode;
	u8 flags;
	u16 lba_msb;
	u16 lba_lsb;
	u8 group_number;
	u8 length_msb;
	u8 length_lsb;
	u8 control;
} __attribute__ ((__packed__)) scsi_write_10;

typedef struct {
	u8 opcode;
	u8 flags;
	u32 lba_msb;
	u32 lba_lsb;
	u32 length;
	u8 group_number;
	u8 control;
} __attribute__ ((__packed__)) scsi_write_16;

typedef struct {
	u8 opcode;
	u8 secure_protocol;
	u16 secure_protocol_specific;
	u8 inc_512;
	u8 reserved1;
	u32 alloc_trans_length;
	u8 reserved2;
	u8 control;
} __attribute__ ((__packed__)) scsi_secure_protocol;

typedef struct {
	u8 stuff_bytes[196];
	u8 key_mac[32];
	u8 data[256];
	u8 nonce[16];
	u32 write_counter;
	u16 address;
	u16 block_count;
	u16 result;
	u16 req_res;
} __attribute__ ((__packed__)) scsi_rpmb_message;

typedef struct {
	struct _wpt_wpf_byte_0_format {
		unsigned wpf: 1; //Write Prtect Flag (0b:Secure Write Protection is disabeled. 1b:Secure Write Protection is enabled)
		unsigned wpt: 2; //Write Protect Type (00b:NV-type, 01b:P-type, 10b:NV_AWP-type)
		unsigned reserved: 5;
	} __attribute__ ((__packed__)) byte_0;

	u8 reserved_byte_1;
	u8 reserved_byte_2;
	u8 reserved_byte_3;
	u32 lba_msb;
	u32 lba_lsb;
	u32 num_of_blocks;
} __attribute__ ((__packed__)) scsi_rpmb_secure_wr_prot_entry;

//v2.1
typedef struct {
	u8 lun;
	u8 data_length;

	//14 bytes
	u32 reserved_part_0_0;
	u32	reserved_part_0_1;
	u32 reserved_part_0_2;
	u16 reserved_part_0_3;

	scsi_rpmb_secure_wr_prot_entry entry[4];

	//176 bytes
	u64 reserved_part_1[22];

} __attribute__ ((__packed__)) scsi_rpmb_secure_wr_prot_config_block;

typedef struct {
	u8 opcode;
	u8 mode;
	u8 buffer_id;
	u8 buffer_offset_lba_0;
	u8 buffer_offset_lba_1;
	u8 buffer_offset_lba_2;
	u8 param_list_length_0;
	u8 param_list_length_1;
	u8 param_list_length_2;
	u8 control;
} __attribute__ ((__packed__)) scsi_write_buffer;

typedef struct {
	u8 opcode;
	u8 reserved_0;
	u8 reserved_1;
	u8 reserved_2;
	u8 reserved_3;
	u8 control;
} __attribute__ ((__packed__)) scsi_testunitready;

typedef struct {
	u8 opcode;
	u8 reserved_0;
	u8 sel_report;
	u8 reserved_1;
	u8 reserved_2;
	u8 reserved_3;
	u16 alloc_length_msb;
	u16 alloc_length_lsb;
	u8 reserved_4;
	u8 control;
} __attribute__ ((__packed__)) scsi_report_luns;

typedef struct {
	u8 opcode;
	u8 obsolete;
	u16 lba_msb;
	u16 lba_lsb;
	u16 reserved;
	u8 pmi;
	u8 control;
} __attribute__ ((__packed__)) scsi_read_capa_10;

typedef struct {
	u8 opcode;
	u8 service_action;
	u16 lba_msb0;
	u16 lba_msb1;
	u16 lba_lsb0;
	u16 lba_lsb1;
	u16 alloc_length_msb;
	u16 alloc_length_lsb;
	u8 pmi;
	u8 control;
} __attribute__ ((__packed__)) scsi_read_capa_16;

typedef struct {
	u8 opcode;
	u8 bytchk;
	u16 lba_msb;
	u16 lba_lsb;
	u8 grpnum;
	u8 length_msb;
	u8 length_lsb;
	u8 control;
} __attribute__ ((__packed__)) scsi_verify_10;

typedef struct {
	u8 opcode;
	u8 immed;
	u32 lba;
	u8 grpnum;
	u16 prefetch_length;
	u8 control;
} __attribute__ ((__packed__)) scsi_prefetch_10;

typedef struct {
	u8 opcode;
	u8 immed;
	u32 lba_msb;
	u32 lba_lsb;
	u32 prefetch_length;
	u8 grpnum;
	u8 control;
} __attribute__ ((__packed__)) scsi_prefetch_16;

typedef union {
	u8 val;
	struct _mode_sense_byte_2_format {
		unsigned pc: 2;
		unsigned page_code: 6;
	} __attribute__ ((__packed__)) format;
} __attribute__ ((__packed__)) mode_sense_byte_2;

typedef struct {
	u8 opcode;
	struct {
		unsigned sp: 1;
		unsigned b1_reserved_1: 3;
		unsigned pf: 1;
		unsigned b1_reserved_0: 3;
	} __attribute__ ((__packed__)) byte1;
	u8 reserved_0;
	u8 reserved_1;
	u8 reserved_2;
	u8 reserved_3;
	u8 reserved_4;
	u8 paramlist_length_msb;
	u8 paramlist_length_lsb;
	u8 control;
} __attribute__ ((__packed__)) scsi_mode_select_10;

typedef struct {
	u8 opcode;
	u8 reserved_1;
	u8 pc_page_code;
	u8 sub_page_code;
	u8 reserved_2;
	u8 reserved_3;
	u8 reserved_4;
	u8 alloc_length_msb;
	u8 alloc_length_lsb;
	u8 control;
} __attribute__ ((__packed__)) scsi_mode_sense_10;
/*(
typedef union {
	u8 val;
	struct {
		unsigned unit_offl: 1;
		unsigned dev_offl: 1;
		unsigned selftest: 1;
		unsigned zero: 1;
		unsigned pf: 1;
		unsigned selftest_code: 3;
	} __attribute__ ((__packed__)) format;
} __attribute__ ((__packed__)) send_diagnostic_byte_1;
*/
typedef struct {
	u8 opcode;
	//send_diagnostic_byte_1 byte1;
	struct {
		unsigned unit_offl: 1;
		unsigned dev_offl: 1;
		unsigned selftest: 1;
		unsigned zero: 1;
		unsigned pf: 1;
		unsigned selftest_code: 3;
	} __attribute__ ((__packed__)) byte1;
	u8 reserved;
	u8 paralistlength_msb;
	u8 paralistlength_lsb;
	u8 control;
} __attribute__ ((__packed__)) scsi_send_diagnostic;

typedef union {
	u8 val;
	struct {
		unsigned start: 1;
		unsigned loej: 1;
		unsigned no_flush: 1;
		unsigned reserved: 1;
		unsigned power_condition: 4;
	} __attribute__ ((__packed__)) format;
} __attribute__ ((__packed__)) start_stop_b4;

typedef struct {
	u8 opcode;
	u8 byte1;
	u8 vendor_specific;
	u8 reserved_0;
	u8 reserved_1;
	u8 control;
} __attribute__ ((__packed__)) scsi_format_unit;

typedef struct {
	u8 opcode;
	u8 evpd;
	u8 page_code;
	u8 alloc_length_msb;
	u8 alloc_length_lsb;
	u8 control;
} __attribute__ ((__packed__)) scsi_inquiry;

typedef struct {
	u8 opcode;
	u8 desc;
	u16 reserved;
	u8 alloc_length;
	u8 control;
} __attribute__ ((__packed__)) scsi_request_sense;

typedef struct {
	u8 opcode;
	u8 immed;
	u16 lba_msb;
	u16 lba_lsb;
	u8 grpnum;
	u8 length_msb;
	u8 length_lsb;
	u8 control;
} __attribute__ ((__packed__)) scsi_sync_cache_10;

typedef struct {
	u8 opcode;
	u8 immed;
	u32 lba_msb;
	u32 lba_lsb;
	u32 length;
	u8 grpnum;
	u8 control;
} __attribute__ ((__packed__)) scsi_sync_cache_16;

typedef struct {
	u8 opcode;
	u8 immed;
	u8 reserved;
	u8 pcm;
	start_stop_b4 data;
	u8 control;
} __attribute__ ((__packed__)) scsi_start_stop_unit;

typedef struct {
	struct _sidf_byte0_format {
		unsigned peri_qualifier: 3;
		unsigned peri_device_type: 1;
	} __attribute__ ((__packed__)) byte0;

	struct _sidf_byte1_format {
		unsigned rmb: 1;
		unsigned reserved: 7;
	} __attribute__ ((__packed__)) byte1;

	u8 version;

	struct _sidf_byte3_format {
		unsigned obsolete0: 1;
		unsigned obsolete1: 1;
		unsigned normach: 1;
		unsigned hisup: 1;
		unsigned resp_data_format: 4;
	} __attribute__ ((__packed__)) byte3;

	u8 additionallength;

	struct _sidf_byte5_format {
		unsigned sccs: 1;
		unsigned acc: 1;
		unsigned tpgs: 2;
		unsigned threepc: 1;
		unsigned reserved: 2;
		unsigned protect: 1;
	} __attribute__ ((__packed__)) byte5;

	struct _sidf_byte6_format {
		unsigned obsolete0: 1;
		unsigned encserv: 1;
		unsigned vs: 1;
		unsigned multiip: 1;
		unsigned obsolete1: 1;
		unsigned obsolete2: 1;
		unsigned obsolete3: 1;
		unsigned addr16: 1;
	} __attribute__ ((__packed__)) byte6;

	struct _sidf_byte7_format {
		unsigned obsolete0: 1;
		unsigned obsolete1: 1;
		unsigned wbus16: 1;
		unsigned sync: 1;
		unsigned obsolete2: 1;
		unsigned obsolete3: 1;
		unsigned cmdque: 1;
		unsigned vs: 1;
	} __attribute__ ((__packed__)) byte7;

	u32 msb_vendor_id;
	u32 lsb_vendor_id;

	u32 msb_upper_product_id;
	u32 msb_product_id;
	u32 lsb_upper_product_id;
	u32 lsb_product_id;

	u32 product_revision_level;
} __attribute__ ((__packed__)) std_inquiry_data_format;

typedef struct {
	u16 modedatalength;
	u8 mediumtype;

	struct _device_specific_param {
		unsigned wp: 1;
		unsigned reserved_0: 2;
		unsigned dpo_fua: 1;
		unsigned reserved_1: 4;
	} __attribute__ ((__packed__)) device_specific_param;

	u8 reserved_0;
	u8 reserved_1;
	u16 blockdesclength;
} __attribute__ ((__packed__)) mode_parameter_header;

typedef struct {
	struct _byte0_format {
		unsigned page_code: 6;
		unsigned spf: 1;
		unsigned ps: 1;
	} __attribute__ ((__packed__)) byte0;

	u8 pagelength;

	struct _byte2_format {
		unsigned reserved: 5;
		unsigned tst: 3;
	} __attribute__ ((__packed__)) byte2;

	/* queue algorithm modifier = 0001b, nuar = 0b, qerr = 00b, obsolete = 0b */
	u8 byte3;

	/* vs = 0b, rac = 0b, ua_intlck_ctrl = 00b */
	struct _cmp_byte4_format {
		unsigned obsolete: 3;
		unsigned swp: 1;
		unsigned reserved: 4;
	} __attribute__ ((__packed__)) byte4;

	/* ato = 0b, tas = 0b, atmpe = 0b, rwwp = 0b, reserved = 0b, autoload mode = 000b */
	u8 byte5;
	u8 byte6;
	u8 byte7;
	u16 busytoperiod;
	u16 extendedselftestcomptime;

} __attribute__ ((__packed__)) control_mode_page;

typedef struct {
	struct _rw_ermp_byte0_format {
		unsigned page_code: 6;
		unsigned spf: 1;
		unsigned ps: 1;
	} __attribute__ ((__packed__)) byte0;


	u8 pagelength;
	/* awre = 1b, arre = 0b, tb = 0b, rc = 0b, eer = 0b, per = 0b, dte = 0b, dcr = 0b */
	u8 byte2;
	u8 rretry_count;

	u8 obsolete0;
	u8 obsolete1;
	u8 obsolete2;

	/* tpere = 0b, reserved = 00000b, restricted for mmc-6 = 00b */
	u8 byte7;

	u8 wretry_count;
	u8 reserved;
	u16 recovery_timelimit;

} __attribute__ ((__packed__)) rw_error_recovery_mode_page;

typedef struct {
	struct _cache_mp_byte0_format {
		unsigned page_code: 6;
		unsigned spf: 1;
		unsigned ps: 1;
	} __attribute__ ((__packed__)) byte0;

	/* 12h */
	u8 pagelength;

	struct _cache_mp_byte2_format {
		unsigned rcd: 1;
		unsigned mf: 1;
		unsigned wce: 1;
		unsigned size: 1;
		unsigned disc: 1;
		unsigned cap: 1;
		unsigned abpf: 1;
		unsigned ic: 1;
	} __attribute__ ((__packed__)) byte2;

	/* demand read retention priority = 0000b, write retention priority = 0000b */
	u8 rw_priority;

	u16 disable_pre_fetch_trans_length;
	u16 minimum_pre_fetch;
	u16 maximum_pre_fetch;
	u16 maximum_pre_fetch_ceiling;

	/* fsw = 0b, lbcss = 0b, dra = 0b, vendor specific = 00b, reserved = 00b, nv_dis = 0b */
	u8 byte12;
	u8 num_of_cache_segments;
	u16 cache_segment_size;
	u8 reserved;
	u8 obsolete0;
	u8 obsolete1;
	u8 obsolete2;
} __attribute__ ((__packed__)) caching_mode_page;

typedef struct {
/* the unmap data length does not include the number of bytes in the unmap data length field. */
/* the unmap block descriptor data length should be a multiple of 16. */
	u16 datalength;
	u16 blockdescdatalength;
	u32 reserved;
} __attribute__ ((__packed__)) unmap_param_list;

typedef struct {
	u32 msb_lba;
	u32 lsb_lba;
	u32 num_of_block;
	u32 reserved;
} __attribute__ ((__packed__)) unmap_block_descriptor;

typedef struct {
	u8 opcode;
	u8 anchor;
	u8 reserved_0;
	u8 reserved_1;
	u8 reserved_2;
	u8 reserved_3;
	u8 grpnum; //00h
	u8 paramlistlength_msb;
	u8 paramlistlength_lsb;
	u8 control;
} __attribute__ ((__packed__)) scsi_unmap;

typedef struct {
	u8 opcode;
	u8 osf0;
	u8 osf1;
	u8 osf2;
	u8 osf3;
	u8 osf4;
	u16 osf5;
	u32 osf6;
	u32 osf7;
} __attribute__ ((__packed__)) osf;

typedef struct {
	u8 opcode;
	u8 vendor_opcode;
	u32 password;
	u8 reserved_b0;
	u8 reserved_b1;
	u8 reserved_b2;
	u8 reserved_b3;
	u8 reserved_b4;
	u8 reserved_b5;
	u32 alloc_length;
} __attribute__ ((__packed__)) ss_vendor_set_password;

typedef struct {
	u8 opcode;
	u8 vendor_opcode;
	u32 customer_signature;
	u32 password;
	u8 reserved_b0;
	u8 reserved_b1;
	u32 alloc_length;
} __attribute__ ((__packed__)) ss_vendor_enter;

typedef struct {
	u8 opcode;
	u8 vendor_opcode;
	u8 reserved_b0;
	u8 reserved_b1;
	u8 reserved_b2;
	u8 reserved_b3;
	u8 reserved_b4;
	u8 reserved_b5;
	u8 reserved_b6;
	u8 reserved_b7;
	u8 reserved_b8;
	u8 reserved_b9;
	u32 alloc_length;
} __attribute__ ((__packed__)) ss_vendor_exit;


typedef struct {
	u8 opcode; //0xa2
	u8 security_protocol;
	u16 security_protocol_specific;
	u8 inc_512_reserved_1; //should be 0x0
	u8 reserved_2;
	u16 alloc_length;
	u8 reserved_3;
	u8 control; //should be 0x0
} __attribute__ ((__packed__)) scsi_security_protocol_in;

typedef struct {
	u8 opcode; //0xb5
	u8 security_protocol;
	u16 security_protocol_specific;
	u8 inc_512_reserved_1;  //should be 0x0
	u8 reserved_2;
	u16 trans_length;
	u8 reserved_3;
	u8 control; //should be 0x0
} __attribute__ ((__packed__)) scsi_security_protocol_out;

typedef struct {
	u8   bstuffbytes[196];
	u8   bkey_mac[32];
	u8   bdata[256];
	u8   bnonce[16];
	u8   bwritecounter[4];
	u8   baddress[2];
	u8   bblockcount[2];
	u8   bresult[2];
	u8   brequest_response[2];
} __attribute__ ((__packed__)) ufs_rpmb_msg_data_frame;



typedef struct {
//    UPIU_HEADER_t   upiu_header;
	u8 bLength;
	u8 bDescriptorType;
	u8 bDevice;
	u8 bDeviceClass;
	u8 bDeviceSubClass;
	u8 bProtocol;
	u8 bNumberLU;
	u8 iNumberWLU;
	u8 bBootEnable;
	u8 bDescrAccessEn;
	u8 bInitPowerMode;
	u8 bHighPriorityLUN;
	u8 bSecureRemovalType;
	u8 bSecurityLU;
	u8 bBackgroundOpsTermLat;
	u8 bInitActiveICCLevel;
	unsigned short wSpecVersion;
	unsigned short wManufactureData;
	u8 iManufacturerName;
	u8 iProductName;
	u8 iSerialNumber;
	u8 iOemID;
	unsigned short wManufacturerID;
	u8 bUD0BaseOffset;
	u8 bUDConfigPlength;
	u8 bDeviceRTTCap;
	unsigned short wPeriodicRTCUpdate;
	//UFS v2.1 supported items
	u8 bUFSFeaturesSupport;
	u8 bFFUTimeout;
	u8 bQueueDepth;
	unsigned short wDeviceVersion;
	u8 bNumSecureWPArea;
	u32 dPSAMaxDataSize;
	u8 bPSAStateTimeout;
	u8 iProductRevisionLevel;
} __attribute__ ((__packed__)) ufs_device_descriptor;

typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bUnitIndex;
	u8 bLUEnable;
	u8 bBootLunID;
	u8 bLUWriteProtect;
	u8 bLUQueueDepth;
	u8 bPSASensitive;	//v2.1
	u8 bMemoryType;
	u8 bDataReliability;
	u8 bLogicalBlockSize;
	u8 qLogicalBlockCount_B7;
	u8 qLogicalBlockCount_B6;
	u8 qLogicalBlockCount_B5;
	u8 qLogicalBlockCount_B4;
	u8 qLogicalBlockCount_B3;
	u8 qLogicalBlockCount_B2;
	u8 qLogicalBlockCount_B1;
	u8 qLogicalBlockCount_B0;
	u32 dEraseBlockSize; // 4 bytes
	u8 bProvisioningType;
	unsigned long long qPhyMemResourceCount;
	unsigned short wContextCapabilities; // 2 bytes
	u8 bLargeUnitSize_M1;
} __attribute__ ((__packed__)) ufs_unit_descriptor;

typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bUnitIndex;
	u8 bLUEnable;
	u8 bBootLunID;
	u8 bLUWriteProtect;
	u8 bLUQueueDepth;
	u8 Reserved1;
	u8 bMemoryType;
	u8 Reserved2;
	u8 bLogicalBlockSize;
	u32 qLogicalBlockCount_U;
	u32 qLogicalBlockCount_L;
	u32 dEraseBlockSize; // 4 bytes
	u8 bProvisioningType;
	u32 qPhyMemResourceCount_U;
	u32 qPhyMemResourceCount_L;
	u8 Reserved3;
	u8 Reserved4;
	u8 Reserved5;
} __attribute__ ((__packed__)) ufs_unit_descriptor_RPMB;

#if 1 //v2.1
typedef struct {
//    UPIU_HEADER_t   upiu_header;
	u8 bLength;
	u8 bDescriptorType;
	u8 bMediaTechnology;
	u8 Reserved1;
	unsigned long long qTotalRawDeviceCapacity;
	u8 bMaxNumberLU; //v2.1
	u32 dSegmentSize;
	u8 bAllocationUnitSize;
	u8 bMinAddrBlockSize;
	u8 bOptimalReadBlockSize;
	u8 bOptimalWriteBlockSize;
	u8 bMaxInBufferSize;
	u8 bMaxOutBufferSize;
	u8 bRPMB_ReadWriteSize; //v2.1
	u8 bDynamicCapacityResourcePolicy; //v2.1
	u8 bDataOrdering;
	u8 bMaxContexIDNumber;
	u8 bSysDataTagUnitSize;
	u8 bSysDataTagResSize;
	u8 bSupportedSecRTypes;
	unsigned short wSupportedMemoryTypes;
	u32 dSystemCodeMaxNAllocU;
	unsigned short wSystemCodeCapAdjFac;
	u32 dNonPersistMaxNAllocU;
	unsigned short wNonPersistCapAdjFac;
	u32 dEnhanced1MaxNAllocU;
	unsigned short wEnhanced1CapAdjFac;
	u32 dEnhanced2MaxNAllocU;
	unsigned short wEnhanced2CapAdjFac;
	u32 dEnhanced3MaxNAllocU;
	unsigned short wEnhanced3CapAdjFac;
	u32 dEnhanced4MaxNAllocU;
	unsigned short wEnhanced4CapAdjFac;
	//UFS v2.1 supported items
	u32 dOptimalLogicalBlockSize;
} __attribute__ ((__packed__)) ufs_geometry_descriptor;
#else
typedef struct {
//    UPIU_HEADER_t   upiu_header;
	u8 bLength;
	u8 bDescriptorType;
	u8 bMediaTechnology;
	u8 Reserved1;
	unsigned long long qTotalRawDeviceCapacity;
	u8 Reserved2;
	u32 dSegmentSize;
	u8 bAllocationUnitSize;
	u8 bMinAddrBlockSize;
	u8 bOptimalReadBlockSize;
	u8 bOptimalWriteBlockSize;
	u8 bMaxInBufferSize;
	u8 bMaxOutBufferSize;
	u8 Reserved3;
	u8 bDataOrdering;
	u8 bMaxContexIDNumber;
	u8 bSysDataTagUnitSize;
	u8 bSysDataTagResSize;
	u8 bSupportedSecRTypes;
	unsigned short wSupportedMemoryTypes;
	u32 dSystemCodeMaxNAllocU;
	unsigned short wSystemCodeCapAdjFac;
	u32 dNonPersistMaxNAllocU;
	unsigned short wNonPersistCapAdjFac;
	u32 dEnhanced1MaxNAllocU;
	unsigned short wEnhanced1CapAdjFac;
	u32 dEnhanced2MaxNAllocU;
	unsigned short wEnhanced2CapAdjFac;
	u32 dEnhanced3MaxNAllocU;
	unsigned short wEnhanced3CapAdjFac;
	u32 dEnhanced4MaxNAllocU;
	unsigned short wEnhanced4CapAdjFac;
	//UFS v2.1 supported items
	u32 dOptimalLogicalBlockSize;
} __attribute__ ((__packed__)) ufs_geometry_descriptor;
#endif

typedef struct {
//    UPIU_HEADER_t   upiu_header;
	u8 bLength;
	u8 bDescriptorType;
	unsigned short bcdUniproVersion;
	unsigned short bcdMphyVersion;
} __attribute__ ((__packed__)) ufs_interconnect_descriptor;


typedef struct {
//    UPIU_HEADER_t   upiu_header;
	u8 bLength;
	u8 bDescriptorType;
	unsigned long long  wActiveICCLevelsVCC_3;
	unsigned long long  wActiveICCLevelsVCC_2;
	unsigned long long  wActiveICCLevelsVCC_1;
	unsigned long long  wActiveICCLevelsVCC_0;
	unsigned long long  wActiveICCLevelsVCCQ_3;
	unsigned long long  wActiveICCLevelsVCCQ_2;
	unsigned long long  wActiveICCLevelsVCCQ_1;
	unsigned long long  wActiveICCLevelsVCCQ_0;
	unsigned long long  wActiveICCLevelsVCCQ2_3;
	unsigned long long  wActiveICCLevelsVCCQ2_2;
	unsigned long long  wActiveICCLevelsVCCQ2_1;
	unsigned long long  wActiveICCLevelsVCCQ2_0;
} __attribute__ ((__packed__)) ufs_power_parameters_descriptor;

typedef struct {
//    UPIU_HEADER_t   upiu_header;
	u8 bLength;
	u8 bDescriptorType;
	u8 bPreEOLInfo;
	u8 bDeviceLifeTimeEstA;
	u8 bDeviceLifeTimeEstB;
	u8 VendorPropInfo[32];
} __attribute__ ((__packed__)) ufs_health_descriptor;

typedef struct {
//    UPIU_HEADER_t   upiu_header;
	u8 bLength;
	u8 bDescriptorType;
	unsigned short cUnicode[126];
} __attribute__ ((__packed__)) ufs_string_descriptor;


typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bConfDescContinue;		//140324
	u8 bBootEnable;
	u8 bDescrAccessEn;
	u8 bInitPowerMode;
	u8 bHighPriorityLUN;
	u8 bSecureRemovalType;
	u8 bInitActiveICCLevel;
	unsigned short wPeriodicRTCUpdate;
	u8 reserved_0;
	unsigned short reserved_1;
	unsigned short reserved_2;
} __attribute__ ((__packed__)) ufs_config_descr_header;

typedef struct {
	u8 bLUEnable;
	u8 bBootLunID;
	u8 bLUWriteProtect;
	u8 bMemoryType;
	//ULONG           dNumAllocUnits;
	u32 dNumAllocUnits;
	u8 bDataReliability;
	u8 bLogicalBlockSize;
	u8 bProvisioningType;
	unsigned short wContextCapabilities;
	unsigned short reserved_3;
	u8 reserved_4;
} __attribute__ ((__packed__)) ufs_config_param_unit_descriptor;

typedef struct {
	ufs_config_descr_header header;
	ufs_config_param_unit_descriptor ud[8];
} __attribute__ ((__packed__)) ufs_config_descriptor;

typedef struct {
	u32 lun_list_length;
	u32 reserved;
	struct _report_luns_lun_addr_format {
		u8 byte0;
		u8 lun;
		u16 byte2_3;
		u16 byte4_5;
		u16 byte6_7;
	} __attribute__ ((__packed__)) lun_addr_format[30];
} __attribute__ ((__packed__)) ufs_report_luns_lun_addr_format;

typedef enum {
	lu_addressing = 0,
	well_known_lu_addressing = 1,
	all_lu_addressing = 2,
} ufs_report_luns_rule_check;

#endif
