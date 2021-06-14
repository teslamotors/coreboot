/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMD_PSP_DEF_H__
#define __AMD_PSP_DEF_H__

#include <types.h>
#include <commonlib/helpers.h>
#include <amdblocks/psp.h>

/* x86 to PSP commands */
#define MBOX_BIOS_CMD_DRAM_INFO			0x01
#define MBOX_BIOS_CMD_SMM_INFO			0x02
#define MBOX_BIOS_CMD_SX_INFO			0x03
#define MBOX_BIOS_CMD_SX_INFO_SLEEP_TYPE_MAX	0x07
#define MBOX_BIOS_CMD_RSM_INFO			0x04
#define MBOX_BIOS_CMD_PSP_QUERY			0x05
#define MBOX_BIOS_CMD_BOOT_DONE			0x06
#define MBOX_BIOS_CMD_CLEAR_S3_STS		0x07
#define MBOX_BIOS_CMD_S3_DATA_INFO		0x08
#define MBOX_BIOS_CMD_NOP			0x09
#define MBOX_BIOS_CMD_HSTI_QUERY		0x14
#define MBOX_BIOS_CMD_SMU_FW			0x19
#define MBOX_BIOS_CMD_SMU_FW2			0x1a
#define MBOX_BIOS_CMD_PSB_AUTO_FUSING		0x21
#define MBOX_BIOS_CMD_PSP_V2_QUERY		0x27
#define MBOX_BIOS_CMD_SPL_FUSE			0x2d
#define MBOX_BIOS_CMD_SET_RPMC_ADDRESS		0x39
#define MBOX_BIOS_CMD_QUERY_PSB_FUSE_CONFIG	0x43
#define MBOX_BIOS_CMD_GET_CUSTOMER_DEVICE_STATE	0x44
#define MBOX_BIOS_CMD_BIOS_KEY_REVID_FUSE	0x48
#define MBOX_BIOS_CMD_ABORT			0xfe

/* PSP to x86 commands */
#define MBOX_PSP_CMD_SPI_INFO	0x83
#define MBOX_PSP_CMD_SPI_READ	0x84
#define MBOX_PSP_CMD_SPI_WRITE	0x85
#define MBOX_PSP_CMD_SPI_ERASE	0x86

/* generic PSP interface status, v1 */
#define PSPV1_STATUS_INITIALIZED	BIT(0)
#define PSPV1_STATUS_ERROR		BIT(1)
#define PSPV1_STATUS_TERMINATED		BIT(2)
#define PSPV1_STATUS_HALT		BIT(3)
#define PSPV1_STATUS_RECOVERY		BIT(4)

/* generic PSP interface status, v2 */
#define PSPV2_STATUS_ERROR		BIT(30)
#define PSPV2_STATUS_RECOVERY		BIT(31)

/* PSP to x86 status */
#define MBOX_PSP_SUCCESS		0x00
#define MBOX_PSP_INVALID_PARAMETER	0x01
#define MBOX_PSP_CRC_ERROR		0x02
#define MBOX_PSP_COMMAND_PROCESS_ERROR	0x04
#define MBOX_PSP_UNSUPPORTED		0x08
#define MBOX_PSP_SPI_BUSY_ASYNC		0x0A
#define MBOX_PSP_SPI_BUSY		0x0B

/* psp_mbox consists of hardware registers beginning at PSPx000070
 *   mbox_command: BIOS->PSP command, cleared by PSP when complete
 *   mbox_status:  BIOS->PSP interface status
 *   cmd_response: pointer to command/response buffer
 */
struct pspv1_mbox {
	u32 mbox_command;
	u32 mbox_status;
	u64 cmd_response; /* definition conflicts w/BKDG but matches agesa */
} __packed;

struct pspv2_mbox {
	union {
		u32 val;
		struct pspv2_mbox_cmd_fields {
			u16 mbox_status;
			u8 mbox_command;
			u32 reserved:6;
			u32 recovery:1;
			u32 ready:1;
		} __packed fields;
	};
	u64 cmd_response;
} __packed;

/* command/response format, BIOS builds this in memory
 *   mbox_buffer_header: generic header
 *   mbox_buffer:        command-specific buffer format
 *
 * AMD reference code aligns and pads all buffers to 32 bytes.
 */
struct mbox_buffer_header {
	u32 size;	/* total size of buffer */
	u32 status;	/* command status, filled by PSP if applicable */
} __packed;

/*
 * command-specific buffer definitions:  see NDA document #54267
 * The following commands need a buffer definition if they are to be used.
 * All other commands will work with the default buffer.
 * MBOX_BIOS_CMD_SMM_INFO		MBOX_BIOS_CMD_PSP_QUERY
 * MBOX_BIOS_CMD_SX_INFO		MBOX_BIOS_CMD_S3_DATA_INFO
 * MBOX_BIOS_CMD_RSM_INFO
 */

struct mbox_default_buffer {	/* command-response buffer unused by command */
	struct mbox_buffer_header header;
} __attribute__((packed, aligned(32)));

#define C2P_BUFFER_MAXSIZE 0xc00 /* Core-to-PSP buffer */
#define P2C_BUFFER_MAXSIZE 0xc00 /* PSP-to-core buffer */

struct smm_req_buffer {
	uint64_t smm_base;		/* TSEG base */
	uint64_t smm_mask;		/* TSEG mask */
	uint64_t psp_smm_data_region;	/* PSP region in SMM space */
	uint64_t psp_smm_data_length;	/* PSP region length in SMM space */
	struct smm_trigger_info smm_trig_info;
#if CONFIG(SOC_AMD_COMMON_BLOCK_PSP_GEN2)
	struct smm_register_info smm_reg_info;
#endif
	uint64_t psp_mbox_smm_buffer_address;
	uint64_t psp_mbox_smm_flag_address;
} __packed;

struct mbox_cmd_smm_info_buffer {
	struct mbox_buffer_header header;
	struct smm_req_buffer req;
} __attribute__((packed, aligned(32)));

struct mbox_cmd_sx_info_buffer {
	struct mbox_buffer_header header;
	u8 sleep_type;
} __attribute__((packed, aligned(32)));

struct caps_req_buffer {
	u32 capabilities;
} __packed;

struct mbox_cmd_psp_query_buffer {
	struct mbox_buffer_header header;
	struct caps_req_buffer req;
} __packed;

struct hsti_req_buffer {
	u32 state;
} __packed;

struct mbox_cmd_hsti_query_buffer {
	struct mbox_buffer_header header;
	struct hsti_req_buffer req;
} __packed;

struct mbox_cmd_query_psb_fuse_config_buffer {
	struct mbox_buffer_header header;
	struct psb_fuse_config config;
} __packed;

struct oem_state_req_buffer {
	u8 state;
} __packed;

struct mbox_cmd_get_oem_state_buffer {
	struct mbox_buffer_header header;
	struct oem_state_req_buffer req;
} __packed;

struct set_rpmc_address_req_buffer {
	u32 address;
	u32 lock;
} __packed;

struct mbox_cmd_set_rpmc_address_buffer {
	struct mbox_buffer_header header;
	struct set_rpmc_address_req_buffer req;
} __packed;

/* psp_bios_mbox consists of SMM registers
 *   mbox_command: PSP->BIOS command, cleared by BIOS when complete
 *   mbox_status:  PSP->BIOS interface status
 *   cmd_buffer:   pointer to PSP command buffer
 */
struct pspv1_bios_mbox {
	u32 mbox_command;
	u32 mbox_status;
	struct mbox_default_buffer buffer;
} __packed;

struct pspv2_bios_mbox {
	u32 mbox_command;
	union {
		u32 val;
		struct smi_mbox_status {
			u32 checksum:8;
			u32 checksum_en:1;
			u32 reserved:22;
			u32 command_ready:1;
		} __packed fields;
	};
	struct mbox_default_buffer buffer;
} __packed;

/* command-specific request definitions: see NDA document #55758 */
struct pspv1_spi_info_request {
	u64 lba;
	u64 block_size;
	u64 num_blocks;
} __packed;

struct mbox_pspv1_cmd_spi_info {
	struct mbox_buffer_header header;
	struct pspv1_spi_info_request req;
} __packed;

struct pspv1_spi_read_write_request {
	u64 lba;
	u64 offset;
	u64 num_bytes;
	u8 buffer[0];
} __packed;

struct mbox_pspv1_cmd_spi_read_write {
	struct mbox_buffer_header header;
	struct pspv1_spi_read_write_request req;
} __packed;

struct pspv1_spi_erase_request {
	u64 lba;
	u64 num_blocks;
} __packed;

struct mbox_pspv1_cmd_spi_erase {
	struct mbox_buffer_header header;
	struct pspv1_spi_erase_request req;
} __packed;

#define SMI_TARGET_NVRAM	0
#define SMI_TARGET_RPMC_NVRAM	5

struct pspv2_spi_info_request {
	u64 id;
	u64 lba;
	u64 block_size;
	u64 num_blocks;
} __packed;

struct mbox_pspv2_cmd_spi_info {
	struct mbox_buffer_header header;
	struct pspv2_spi_info_request req;
} __packed;

struct pspv2_spi_read_write_request {
	u64 id;
	u64 lba;
	u64 offset;
	u64 num_bytes;
	u8 buffer[0];
} __packed;

struct mbox_pspv2_cmd_spi_read_write {
	struct mbox_buffer_header header;
	struct pspv2_spi_read_write_request req;
} __packed;

struct pspv2_spi_erase_request {
	u64 id;
	u64 lba;
	u64 num_blocks;
} __packed;

struct mbox_pspv2_cmd_spi_erase {
	struct mbox_buffer_header header;
	struct pspv2_spi_erase_request req;
} __packed;

#define PSP_INIT_TIMEOUT 10000 /* 10 seconds */
#define PSP_CMD_TIMEOUT 1000 /* 1 second */

void psp_print_cmd_status(int cmd_status, struct mbox_buffer_header *header);

/* This command needs to be implemented by the generation specific code. */
int send_psp_command(u32 command, void *buffer);

void enable_psp_smi(void *buffer);
void disable_psp_smi(void *buffer);

u32 get_psp_command(void *buffer);
void clear_psp_command(void *buffer);

u32 check_psp_command(void *buffer);
struct mbox_default_buffer *get_psp_command_buffer(void *buffer);

bool valid_psp_spi_info(struct mbox_default_buffer *buffer);
bool valid_psp_spi_read_write(struct mbox_default_buffer *buffer);
bool valid_psp_spi_erase(struct mbox_default_buffer *buffer);

u64 get_psp_spi_info_id(struct mbox_default_buffer *buffer);
void set_psp_spi_info(struct mbox_default_buffer *buffer,
			u64 lba, u64 block_size, u64 num_blocks);
void get_psp_spi_read_write(struct mbox_default_buffer *buffer,
				u64 *id, u64 *lba, u64 *offset,
				u64 *num_bytes, u8 **data);
void get_psp_spi_erase(struct mbox_default_buffer *buffer,
			u64 *id, u64 *lba, u64 *num_blocks);

#endif /* __AMD_PSP_DEF_H__ */
