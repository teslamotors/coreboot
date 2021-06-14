/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <cbfs.h>
#include <region_file.h>
#include <timer.h>
#include <console/console.h>
#include <amdblocks/psp.h>
#include <soc/iomap.h>
#include <soc/northbridge.h>
#include "psp_def.h"

static u32 rd_mbox_sts(struct pspv1_mbox *mbox)
{
	return read32(&mbox->mbox_status);
}

static void wr_mbox_cmd(struct pspv1_mbox *mbox, u32 cmd)
{
	write32(&mbox->mbox_command, cmd);
}

static u32 rd_mbox_cmd(struct pspv1_mbox *mbox)
{
	return read32(&mbox->mbox_command);
}

static void wr_mbox_cmd_resp(struct pspv1_mbox *mbox, void *buffer)
{
	write64(&mbox->cmd_response, (uintptr_t)buffer);
}

static int wait_initialized(struct pspv1_mbox *mbox)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, PSP_INIT_TIMEOUT);

	do {
		if (rd_mbox_sts(mbox) & PSPV1_STATUS_INITIALIZED)
			return 0;
	} while (!stopwatch_expired(&sw));

	return -PSPSTS_INIT_TIMEOUT;
}

static int wait_command(struct pspv1_mbox *mbox)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, PSP_CMD_TIMEOUT);

	do {
		if (!rd_mbox_cmd(mbox))
			return 0;
	} while (!stopwatch_expired(&sw));

	return -PSPSTS_CMD_TIMEOUT;
}

int send_psp_command(u32 command, void *buffer)
{
	struct pspv1_mbox *mbox = soc_get_mbox_address();
	if (!mbox)
		return -PSPSTS_NOBASE;

	/* check for PSP error conditions */
	if (rd_mbox_sts(mbox) & PSPV1_STATUS_HALT)
		return -PSPSTS_HALTED;

	if (rd_mbox_sts(mbox) & PSPV1_STATUS_RECOVERY)
		return -PSPSTS_RECOVERY;

	/* PSP must be finished with init and ready to accept a command */
	if (wait_initialized(mbox))
		return -PSPSTS_INIT_TIMEOUT;

	if (wait_command(mbox))
		return -PSPSTS_CMD_TIMEOUT;

	/* set address of command-response buffer and write command register */
	wr_mbox_cmd_resp(mbox, buffer);
	wr_mbox_cmd(mbox, command);

	/* PSP clears command register when complete */
	if (wait_command(mbox))
		return -PSPSTS_CMD_TIMEOUT;

	/* check delivery status */
	if (rd_mbox_sts(mbox) & (PSPV1_STATUS_ERROR | PSPV1_STATUS_TERMINATED))
		return -PSPSTS_SEND_ERROR;

	return 0;
}

/*
 * Tell the PSP to load a firmware blob from a location in the BIOS image.
 */
int psp_load_named_blob(enum psp_blob_type type, const char *name)
{
	int cmd_status;
	u32 command;
	void *blob;
	struct cbfsf cbfs_file;
	struct region_device rdev;

	switch (type) {
	case BLOB_SMU_FW:
		command = MBOX_BIOS_CMD_SMU_FW;
		break;
	case BLOB_SMU_FW2:
		command = MBOX_BIOS_CMD_SMU_FW2;
		break;
	default:
		printk(BIOS_ERR, "BUG: Invalid PSP blob type %x\n", type);
		return -PSPSTS_INVALID_BLOB;
	}

	/* type can only be BLOB_SMU_FW or BLOB_SMU_FW2 here, so don't re-check for this */
	if (!CONFIG(SOC_AMD_PSP_SELECTABLE_SMU_FW)) {
		printk(BIOS_ERR, "BUG: Selectable firmware is not supported\n");
		return -PSPSTS_UNSUPPORTED;
	}

	if (cbfs_boot_locate(&cbfs_file, name, NULL)) {
		printk(BIOS_ERR, "BUG: Cannot locate blob for PSP loading\n");
		return -PSPSTS_INVALID_NAME;
	}

	cbfs_file_data(&rdev, &cbfs_file);
	blob = rdev_mmap_full(&rdev);
	if (!blob) {
		printk(BIOS_ERR, "BUG: Cannot map blob for PSP loading\n");
		return -PSPSTS_INVALID_NAME;
	}

	printk(BIOS_DEBUG, "PSP: Load blob type %x from @%p... ", type, blob);

	/* Blob commands use the buffer registers as data, not pointer to buf */
	cmd_status = send_psp_command(command, blob);
	psp_print_cmd_status(cmd_status, NULL);

	rdev_munmap(&rdev, blob);
	return cmd_status;
}

static void wr_mbox_init(struct pspv1_bios_mbox *mbox, bool init)
{
	u32 status = read32(&mbox->mbox_status);
	if (init)
		status |= 1;
	else
		status &= ~1;
	write32(&mbox->mbox_status, status);
}

void enable_psp_smi(void *buffer)
{
	struct pspv1_bios_mbox *mbox = (struct pspv1_bios_mbox *)buffer;
	wr_mbox_init(true);
}

void disable_psp_smi(void *buffer)
{
	struct pspv1_bios_mbox *mbox = (struct pspv1_bios_mbox *)buffer;
	wr_mbox_init(false);
}

void clear_psp_command(void *buffer)
{
	struct pspv1_bios_mbox *mbox = (struct pspv1_bios_mbox *)buffer;
	write32(&mbox->mbox_command, 0);
}

u32 check_psp_command(void *buffer)
{
	return MBOX_PSP_SUCCESS;
}

u32 get_psp_command(void *buffer)
{
	struct pspv1_bios_mbox *mbox = (struct pspv1_bios_mbox *)buffer;
	read32(&mbox->mbox_command);
}

struct mbox_default_buffer *get_psp_command_buffer(void *buffer)
{
	struct pspv1_bios_mbox *mbox = (struct pspv1_bios_mbox *)buffer;
	return &mbox->buffer;
}

bool valid_psp_spi_info(struct mbox_default_buffer *buffer)
{
	return true;
}

bool valid_psp_spi_read_write(struct mbox_default_buffer *buffer)
{
	return true;
}

bool valid_psp_spi_erase(struct mbox_default_buffer *buffer)
{
	return true;
}

u64 get_psp_spi_info_id(struct mbox_default_buffer *buffer)
{
	return SMI_TARGET_NVRAM;
}

void set_psp_spi_info(struct mbox_default_buffer *buffer,
			u64 lba, u64 block_size, u64 num_blocks)
{
	struct mbox_pspv1_cmd_spi_info *cmd = (struct mbox_pspv1_cmd_spi_info *)buffer;
	write32(&cmd->req.lba, lba);
	write32(&cmd->req.block_size, block_size);
	write32(&cmd->req.num_blocks, num_blocks);
}

void get_psp_spi_read_write(struct mbox_default_buffer *buffer,
				u64 *id, u64 *lba, u64 *offset,
				u64 *num_bytes, u8 **data)
{
	struct mbox_pspv1_cmd_spi_read_write *cmd = (struct mbox_pspv1_cmd_spi_read_write *)buffer;
	*id = SMI_TARGET_NVRAM;
	*lba = read64(&cmd->req.lba);
	*offset = read64(&cmd->req.offset);
	*num_bytes = read64(&cmd->req.num_bytes);
	*data = cmd->req.buffer;
}

void get_psp_spi_erase(struct mbox_default_buffer *buffer,
			u64 *id, u64 *lba, u64 *num_blocks)
{
	struct mbox_pspv1_cmd_spi_erase *cmd = (struct mbox_pspv1_cmd_spi_erase *)buffer;
	*id = SMI_TARGET_NVRAM;
	*lba = read64(&cmd->req.lba);
	*num_blocks = read64(&cmd->req.num_blocks);
}
