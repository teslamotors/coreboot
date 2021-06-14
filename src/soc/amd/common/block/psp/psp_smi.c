/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/spi.h>
#include <cpu/x86/cache.h>
#include <device/mmio.h>
#include <console/console.h>
#include <boot_device.h>
#include <commonlib/region.h>
#include <fmap.h>
#include <spi_flash.h>
#include <soc/smi.h>
#include "psp_def.h"

extern struct {
	u8 buffer[C2P_BUFFER_MAXSIZE];
} __attribute__((aligned(32))) c2p_buffer;

extern struct {
	u8 buffer[P2C_BUFFER_MAXSIZE];
} __attribute__((aligned(32))) p2c_buffer;

static const char *id_to_region_name(u64 id)
{
	switch (id) {
	case SMI_TARGET_NVRAM:
		return "PSP_NVRAM";
	case SMI_TARGET_RPMC_NVRAM:
		return "PSP_RPMC_NVRAM";
	}

	return NULL;
}

/*
 * Do not cache the location to cope with flash changing underneath (e.g. due
 * to an update)
 */
static int lookup_store(u64 id, struct region_device *rstore)
{
	static struct region_device read_rdev, write_rdev;
	static struct incoherent_rdev store_irdev;
	const char *name;
	struct region region;
	const struct region_device *rdev;

	name = id_to_region_name(id);
	if (!name)
		return -1;

	if (fmap_locate_area(name, &region))
		return -1;

	if (boot_device_ro_subregion(&region, &read_rdev) < 0)
		return -1;

	if (boot_device_rw_subregion(&region, &write_rdev) < 0)
		return -1;

	rdev = incoherent_rdev_init(&store_irdev, &region, &read_rdev, &write_rdev);
	if (rdev == NULL)
		return -1;

	return rdev_chain(rstore, rdev, 0, region_device_sz(rdev));
}

static int spi_available(void)
{
	return !(spi_read8(SPI_SEMAPHORE) & (SPI_SEM_LOCKED|SPI_SEM_SMI_DENY));
}

static u32 get_psp_spi_info(struct mbox_default_buffer *buffer)
{
	const struct spi_flash *flash;
	struct region_device store;
	u64 id;
	u64 block_size;
	u64 num_blocks;

	printk(BIOS_DEBUG, "PSP: SPI info request\n");

	if (!valid_psp_spi_info(buffer))
		return MBOX_PSP_UNSUPPORTED;

	id = get_psp_spi_info_id(buffer);

	flash = boot_device_spi_flash();
	if (!flash) {
		printk(BIOS_ERR, "PSP: Unable to find PSP SPI device\n");
		return MBOX_PSP_COMMAND_PROCESS_ERROR;
	}

	if (lookup_store(id, &store)) {
		printk(BIOS_ERR, "PSP: Unable to find PSP SPI region\n");
		return MBOX_PSP_COMMAND_PROCESS_ERROR;
	}

	block_size = flash->sector_size;
	num_blocks = region_device_sz(&store) / block_size;

	set_psp_spi_info(buffer, 0, block_size, num_blocks);

	return MBOX_PSP_SUCCESS;
}

static u32 read_psp_spi(struct mbox_default_buffer *buffer)
{
	u32 ret;
	u64 id;
	u64 lba;
	u64 offset;
	u64 num_bytes;
	u8 *data;
	u8 cs;
	size_t addr;
	const struct spi_flash *flash;
	struct region_device store;

	printk(BIOS_DEBUG, "PSP: SPI read request\n");

	if (!valid_psp_spi_read_write(buffer))
		return MBOX_PSP_UNSUPPORTED;

	if (!spi_available()) {
		printk(BIOS_NOTICE, "PSP: SPI controller busy\n");
		return MBOX_PSP_SPI_BUSY;
	}

	get_psp_spi_read_write(buffer, &id, &lba, &offset, &num_bytes, &data);

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT)) {
		cs = spi_read8(SPI_ALT_CS);
		spi_write8(SPI_ALT_CS, cs & ~SPI_ALT_CS_EN_MASK);
	}

	flash = boot_device_spi_flash();
	if (!flash) {
		printk(BIOS_ERR, "PSP: Unable to find PSP SPI device\n");
		ret = MBOX_PSP_COMMAND_PROCESS_ERROR;
		goto out;
	}

	if (lookup_store(id, &store)) {
		printk(BIOS_ERR, "PSP: Unable to find PSP SPI region\n");
		ret = MBOX_PSP_COMMAND_PROCESS_ERROR;
		goto out;
	}

	addr = (lba * flash->sector_size) + offset;

	printk(BIOS_DEBUG, "PSP: SPI read 0x%llx bytes at 0x%zx\n",
		num_bytes, addr);

	if (rdev_readat(&store, data, addr, (size_t)num_bytes)
	    != (size_t)num_bytes) {
		printk(BIOS_ERR, "PSP: Failed to read NVRAM data\n");
		ret = MBOX_PSP_COMMAND_PROCESS_ERROR;
		goto out;
	}

	ret = MBOX_PSP_SUCCESS;
out:
	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT))
		spi_write8(SPI_ALT_CS, cs);
	return ret;
}

static u32 write_psp_spi(struct mbox_default_buffer *buffer)
{
	u32 ret;
	u64 id;
	u64 lba;
	u64 offset;
	u64 num_bytes;
	u8 *data;
	u8 cs;
	size_t addr;
	const struct spi_flash *flash;
	struct region_device store;

	printk(BIOS_DEBUG, "PSP: SPI write request\n");

	if (!valid_psp_spi_read_write(buffer))
		return MBOX_PSP_UNSUPPORTED;

	if (!spi_available()) {
		printk(BIOS_NOTICE, "PSP: SPI controller busy\n");
		return MBOX_PSP_SPI_BUSY;
	}

	get_psp_spi_read_write(buffer, &id, &lba, &offset, &num_bytes, &data);

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT)) {
		cs = spi_read8(SPI_ALT_CS);
		spi_write8(SPI_ALT_CS, cs & ~SPI_ALT_CS_EN_MASK);
	}

	flash = boot_device_spi_flash();
	if (!flash) {
		printk(BIOS_ERR, "PSP: Unable to find PSP SPI device\n");
		ret = MBOX_PSP_COMMAND_PROCESS_ERROR;
		goto out;
	}

	if (lookup_store(id, &store)) {
		printk(BIOS_ERR, "PSP: Unable to find PSP SPI region\n");
		ret = MBOX_PSP_COMMAND_PROCESS_ERROR;
		goto out;
	}

	addr = (lba * flash->sector_size) + offset;

	printk(BIOS_DEBUG, "PSP: SPI write 0x%llx bytes at 0x%zx\n",
		num_bytes, addr);

	if (rdev_writeat(&store, data, addr, (size_t)num_bytes)
	    != (size_t)num_bytes) {
		printk(BIOS_ERR, "PSP: Failed to write NVRAM data\n");
		ret = MBOX_PSP_COMMAND_PROCESS_ERROR;
		goto out;
	}

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT)) {
		spi_write8(SPI_ALT_CS, (cs & ~SPI_ALT_CS_EN_MASK) | 1);

		printk(BIOS_DEBUG, "PSP: Alt SPI write 0x%llx bytes at 0x%zx\n",
			num_bytes, addr);

		if (rdev_writeat(&store, data, addr, (size_t)num_bytes)
		    != (size_t)num_bytes) {
			printk(BIOS_ERR, "PSP: Failed to write Alt SPI NVRAM data\n");
			ret = MBOX_PSP_COMMAND_PROCESS_ERROR;
			goto out;
		}
	}

	ret = MBOX_PSP_SUCCESS;
out:
	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT))
		spi_write8(SPI_ALT_CS, cs);
	return ret;
}

static u32 erase_psp_spi(struct mbox_default_buffer *buffer)
{
	u32 ret;
	u64 id;
	u64 lba;
	u64 num_blocks;
	u8 cs;
	size_t addr;
	size_t num_bytes;
	const struct spi_flash *flash;
	struct region_device store;

	printk(BIOS_DEBUG, "PSP: SPI erase request\n");

	if (!valid_psp_spi_erase(buffer))
		return MBOX_PSP_UNSUPPORTED;

	if (!spi_available()) {
		printk(BIOS_NOTICE, "PSP: SPI controller busy\n");
		return MBOX_PSP_SPI_BUSY;
	}

	get_psp_spi_erase(buffer, &id, &lba, &num_blocks);

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT)) {
		cs = spi_read8(SPI_ALT_CS);
		spi_write8(SPI_ALT_CS, cs & ~SPI_ALT_CS_EN_MASK);
	}

	flash = boot_device_spi_flash();
	if (!flash) {
		printk(BIOS_ERR, "PSP: Unable to find PSP SPI device\n");
		ret = MBOX_PSP_COMMAND_PROCESS_ERROR;
		goto out;
	}

	if (lookup_store(id, &store)) {
		printk(BIOS_ERR, "PSP: Unable to find PSP SPI region\n");
		ret = MBOX_PSP_COMMAND_PROCESS_ERROR;
		goto out;
	}

	addr = lba * flash->sector_size;
	num_bytes = (size_t)num_blocks * flash->sector_size;

	printk(BIOS_DEBUG, "PSP: SPI erase 0x%zx bytes at 0x%zx\n",
		num_bytes, addr);

	if (rdev_eraseat(&store, addr, num_bytes)
	    != num_bytes) {
		printk(BIOS_ERR, "PSP: Failed to erase NVRAM data\n");
		ret = MBOX_PSP_COMMAND_PROCESS_ERROR;
		goto out;
	}

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT)) {
		spi_write8(SPI_ALT_CS, (cs & ~SPI_ALT_CS_EN_MASK) | 1);

		printk(BIOS_DEBUG, "PSP: Alt SPI erase 0x%zx bytes at 0x%zx\n",
			num_bytes, addr);

		if (rdev_eraseat(&store, addr, num_bytes)
		    != num_bytes) {
			printk(BIOS_ERR, "PSP: Failed to erase Alt SPI NVRAM data\n");
			ret = MBOX_PSP_COMMAND_PROCESS_ERROR;
			goto out;
		}
	}

	ret = MBOX_PSP_SUCCESS;
out:
	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT))
		spi_write8(SPI_ALT_CS, cs);
	return ret;
}

static void handle_psp_command(void)
{
	u32 status;
	u32 cmd;
	struct mbox_default_buffer *buffer = get_psp_command_buffer(p2c_buffer.buffer);

	status = check_psp_command(p2c_buffer.buffer);
	if (status)
		goto out;

	cmd = get_psp_command(p2c_buffer.buffer);

	switch (cmd) {
	case MBOX_PSP_CMD_SPI_INFO:
		status = get_psp_spi_info(buffer);
		break;
	case MBOX_PSP_CMD_SPI_READ:
		status = read_psp_spi(buffer);
		break;
	case MBOX_PSP_CMD_SPI_WRITE:
		status = write_psp_spi(buffer);
		break;
	case MBOX_PSP_CMD_SPI_ERASE:
		status = erase_psp_spi(buffer);
		break;
	default:
		printk(BIOS_ERR, "PSP: Unknown command %d\n", cmd);
		status = MBOX_PSP_INVALID_PARAMETER;
		break;
	}

out:
	write32(&buffer->header.status, status);
	/* TODO: set checksum */
}

void psp_smi_handler(void)
{
	disable_psp_smi(p2c_buffer.buffer);

	handle_psp_command();

	clear_psp_command(p2c_buffer.buffer);
	enable_psp_smi(p2c_buffer.buffer);

	wbinvd();

	reset_psp_smi();
}
