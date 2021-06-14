/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <timer.h>
#include <amdblocks/psp.h>
#include <soc/iomap.h>
#include <commonlib/region.h>
#include "psp_def.h"

static u16 rd_mbox_sts(struct pspv2_mbox *mbox)
{
	union {
		u32 val;
		struct pspv2_mbox_cmd_fields fields;
	} tmp = { 0 };

	tmp.val = read32(&mbox->val);
	return tmp.fields.mbox_status;
}

static void wr_mbox_cmd(struct pspv2_mbox *mbox, u8 cmd)
{
	union {
		u32 val;
		struct pspv2_mbox_cmd_fields fields;
	} tmp = { 0 };

	/* Write entire 32-bit area to begin command execution */
	tmp.fields.mbox_command = cmd;
	write32(&mbox->val, tmp.val);
}

static u8 rd_mbox_recovery(struct pspv2_mbox *mbox)
{
	union {
		u32 val;
		struct pspv2_mbox_cmd_fields fields;
	} tmp = { 0 };

	tmp.val = read32(&mbox->val);
	return !!tmp.fields.recovery;
}

static void wr_mbox_cmd_resp(struct pspv2_mbox *mbox, void *buffer)
{
	write64(&mbox->cmd_response, (uintptr_t)buffer);
}

static int wait_command(struct pspv2_mbox *mbox, bool wait_for_ready)
{
	struct pspv2_mbox and_mask = { .val = ~0 };
	struct pspv2_mbox expected = { .val = 0 };
	struct stopwatch sw;
	u32 tmp;

	/* Zero fields from and_mask that should be kept */
	and_mask.fields.mbox_command = 0;
	and_mask.fields.ready = wait_for_ready ? 0 : 1;

	/* Expect mbox_cmd == 0 but ready depends */
	if (wait_for_ready)
		expected.fields.ready = 1;

	stopwatch_init_msecs_expire(&sw, PSP_CMD_TIMEOUT);

	do {
		tmp = read32(&mbox->val);
		tmp &= ~and_mask.val;
		if (tmp == expected.val)
			return 0;
	} while (!stopwatch_expired(&sw));

	return -PSPSTS_CMD_TIMEOUT;
}

int send_psp_command(u32 command, void *buffer)
{
	struct pspv2_mbox *mbox = soc_get_mbox_address();
	if (!mbox)
		return -PSPSTS_NOBASE;

	if (rd_mbox_recovery(mbox))
		return -PSPSTS_RECOVERY;

	if (wait_command(mbox, true))
		return -PSPSTS_CMD_TIMEOUT;

	/* set address of command-response buffer and write command register */
	wr_mbox_cmd_resp(mbox, buffer);
	wr_mbox_cmd(mbox, command);

	/* PSP clears command register when complete.  All commands except
	 * SxInfo set the Ready bit. */
	if (wait_command(mbox, command != MBOX_BIOS_CMD_SX_INFO))
		return -PSPSTS_CMD_TIMEOUT;

	/* check delivery status */
	if (rd_mbox_sts(mbox))
		return -PSPSTS_SEND_ERROR;

	return 0;
}

static void wr_bios_mbox_ready(struct pspv2_bios_mbox *mbox, u8 ready)
{
	union {
		u32 val;
		struct smi_mbox_status fields;
	} tmp = { 0 };

	tmp.fields.command_ready = ready;
	write32(&mbox->val, tmp.val);
}

void enable_psp_smi(void *buffer)
{
	struct pspv2_bios_mbox *mbox = (struct pspv2_bios_mbox *)buffer;
	wr_bios_mbox_ready(mbox, 1);
}

void disable_psp_smi(void *buffer)
{
	struct pspv2_bios_mbox *mbox = (struct pspv2_bios_mbox *)buffer;
	wr_bios_mbox_ready(mbox, 0);
}

void clear_psp_command(void *buffer)
{
	struct pspv2_bios_mbox *mbox = (struct pspv2_bios_mbox *)buffer;
	write32(&mbox->mbox_command, 0);
}

static u8 rd_bios_mbox_checksum_en(struct pspv2_bios_mbox *mbox)
{
	union {
		u32 val;
		struct smi_mbox_status fields;
	} tmp = { 0 };

	tmp.val = read32(&mbox->val);
	return !!tmp.fields.checksum_en;
}

static u8 rd_bios_mbox_checksum(struct pspv2_bios_mbox *mbox)
{
	union {
		u32 val;
		struct smi_mbox_status fields;
	} tmp = { 0 };

	tmp.val = read32(&mbox->val);
	return tmp.fields.checksum;
}

static u8 checksum8(const u8 *data, size_t size)
{
	u8 checksum = 0;
	int i;

	for (i = 0; i < size; i++)
		checksum += read8(data + i);

	return checksum;
}

u32 check_psp_command(void *buffer)
{
	struct pspv2_bios_mbox *mbox = (struct pspv2_bios_mbox *)buffer;
	struct mbox_buffer_header *header = &mbox->buffer.header;
	u8 checksum;

	if (rd_bios_mbox_checksum_en(mbox)) {
		checksum = checksum8((const u8 *)&mbox->buffer, read32(&header->size));
		if (checksum != rd_bios_mbox_checksum(mbox))
			return MBOX_PSP_CRC_ERROR;
	}

	return MBOX_PSP_SUCCESS;
}

u32 get_psp_command(void *buffer)
{
	struct pspv2_bios_mbox *mbox = (struct pspv2_bios_mbox *)buffer;
	return read32(&mbox->mbox_command);
}

struct mbox_default_buffer *get_psp_command_buffer(void *buffer)
{
	struct pspv2_bios_mbox *mbox = (struct pspv2_bios_mbox *)buffer;
	return &mbox->buffer;
}

static bool valid_psp_spi_id(u64 id)
{
	return id == SMI_TARGET_NVRAM ||
	       id == SMI_TARGET_RPMC_NVRAM;
}

bool valid_psp_spi_info(struct mbox_default_buffer *buffer)
{
	struct mbox_pspv2_cmd_spi_info *cmd = (struct mbox_pspv2_cmd_spi_info *)buffer;
	return valid_psp_spi_id(read64(&cmd->req.id));
}

bool valid_psp_spi_read_write(struct mbox_default_buffer *buffer)
{
	struct mbox_pspv2_cmd_spi_read_write *cmd = (struct mbox_pspv2_cmd_spi_read_write *)buffer;
	return valid_psp_spi_id(read64(&cmd->req.id));
}

bool valid_psp_spi_erase(struct mbox_default_buffer *buffer)
{
	struct mbox_pspv2_cmd_spi_erase *cmd = (struct mbox_pspv2_cmd_spi_erase *)buffer;
	return valid_psp_spi_id(read64(&cmd->req.id));
}

u64 get_psp_spi_info_id(struct mbox_default_buffer *buffer)
{
	struct mbox_pspv2_cmd_spi_info *cmd = (struct mbox_pspv2_cmd_spi_info *)buffer;
	return read64(&cmd->req.id);
}

void set_psp_spi_info(struct mbox_default_buffer *buffer,
			u64 lba, u64 block_size, u64 num_blocks)
{
	struct mbox_pspv2_cmd_spi_info *cmd = (struct mbox_pspv2_cmd_spi_info *)buffer;
	write32(&cmd->req.lba, lba);
	write32(&cmd->req.block_size, block_size);
	write32(&cmd->req.num_blocks, num_blocks);
}

void get_psp_spi_read_write(struct mbox_default_buffer *buffer,
				u64 *id, u64 *lba, u64 *offset,
				u64 *num_bytes, u8 **data)
{
	struct mbox_pspv2_cmd_spi_read_write *cmd = (struct mbox_pspv2_cmd_spi_read_write *)buffer;
	*id = read64(&cmd->req.id);
	*lba = read64(&cmd->req.lba);
	*offset = read64(&cmd->req.offset);
	*num_bytes = read64(&cmd->req.num_bytes);
	*data = cmd->req.buffer;
}

void get_psp_spi_erase(struct mbox_default_buffer *buffer,
			u64 *id, u64 *lba, u64 *num_blocks)
{
	struct mbox_pspv2_cmd_spi_erase *cmd = (struct mbox_pspv2_cmd_spi_erase *)buffer;
	*id = read64(&cmd->req.id);
	*lba = read64(&cmd->req.lba);
	*num_blocks = read64(&cmd->req.num_blocks);
}
