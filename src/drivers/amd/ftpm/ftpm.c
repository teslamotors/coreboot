/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <amdblocks/spi.h>
#include <amdblocks/psp.h>
#include <amdblocks/reset.h>
#include <console/console.h>
#include <fmap.h>
#include <soc/psp.h>
#include <string.h>

#include "ftpm.h"

#define CAP_TPM_SUPPORTED		(1 << 0)
#define CAP_TPM_REQ_FACTORY_RESET	(1 << 1)
#define CAP_FTPM_NEED_RECOVER		(1 << 2)

#define NAME_PSP_NVRAM			"PSP_NVRAM"
#define NAME_PSP_NVRAM_BACKUP		"PSP_NVRAM_BACKUP"
#define NAME_PSP_NVRAM_DEBUG		"PSP_NVRAM_DEBUG"
#define NAME_PSP_NVRAM_BACKUP_DEBUG	"PSP_NVRAM_BACKUP_DEBUG"

typedef ssize_t store_readat(const struct region_device *rd, void *b,
			size_t offset, size_t size);

typedef ssize_t store_writeat(const struct region_device *rd, const void *b,
			size_t offset, size_t size);

static uint8_t buffer[4096];

static int erase_store(struct region_device *store)
{
	if (rdev_eraseat(store, 0, region_device_sz(store))
	    != region_device_sz(store))
		return -1;

	return 0;
}

static int erase_region(const char *name)
{
	struct region_device store;

	if (fmap_locate_area_as_rdev_rw(name, &store)) {
		printk(BIOS_ERR, "FTPM: Unable to find %s\n", name);
		return -1;
	}

	return erase_store(&store);
}

static int write_store(struct region_device *dst_store, store_writeat *dst_writeat,
			struct region_device *src_store, store_readat *src_readat)
{
	size_t dst_size;
	size_t src_size;
	size_t size;
	size_t off;

	dst_size = region_device_sz(dst_store);
	src_size = region_device_sz(src_store);

	if (dst_size < src_size) {
		printk(BIOS_ERR, "FTPM: Destination size %zx less than source size %zx\n",
			dst_size, src_size);
		return -1;
	}

	size = sizeof(buffer);

	if (src_size < size) {
		printk(BIOS_ERR, "FTPM: Source size %zx too small\n", src_size);
		return -1;
	}

	if (src_size % size) {
		printk(BIOS_ERR, "FTPM: Source size %zx not aligned\n", src_size);
		return -1;
	}

	for (off = 0; off < src_size; off += size) {
		if (src_readat(src_store, buffer, off, size) != size) {
			printk(BIOS_ERR, "FTPM: Failed to read source at %zx\n", off);
			return -1;
		}
		if (dst_writeat(dst_store, buffer, off, size) != size) {
			printk(BIOS_ERR, "FTPM: Failed to write destination at %zx\n", off);
			return -1;
		}
	}

	return 0;
}

static int do_copy_store(struct region_device *dst_store, store_writeat *dst_writeat,
			struct region_device *src_store, store_readat *src_readat)
{
	if (erase_store(dst_store)) {
		printk(BIOS_ERR, "FTPM: Failed to erase destination store\n");
		return -1;
	}

	return write_store(dst_store, dst_writeat, src_store, src_readat);
}

static int copy_store(struct region_device *dst_store, struct region_device *src_store)
{
	return do_copy_store(dst_store, rdev_writeat, src_store, rdev_readat);
}

static int copy_region(const char *dst_name, const char *src_name)
{
	struct region_device dst_store;
	struct region_device src_store;

	if (fmap_locate_area_as_rdev_rw(dst_name, &dst_store)) {
		printk(BIOS_ERR, "FTPM: Unable to find %s\n", dst_name);
		return -1;
	}

	if (fmap_locate_area_as_rdev_rw(src_name, &src_store)) {
		printk(BIOS_ERR, "FTPM: Unable to find %s\n", src_name);
		return -1;
	}

	return copy_store(&dst_store, &src_store);
}

static int do_compare_store(struct region_device *dst_store, store_readat *dst_readat,
			struct region_device *src_store, store_readat *src_readat)
{
	size_t dst_size;
	size_t src_size;
	uint8_t *dst;
	uint8_t *src;
	size_t size;
	size_t off;

	dst_size = region_device_sz(dst_store);
	src_size = region_device_sz(src_store);

	if (dst_size < src_size) {
		printk(BIOS_ERR, "FTPM: Destination size %zx less than source size %zx\n",
			dst_size, src_size);
		return -1;
	}

	size = sizeof(buffer) / 2;

	if (src_size < size) {
		printk(BIOS_ERR, "FTPM: Source size %zx too small\n", src_size);
		return -1;
	}

	if (src_size % size) {
		printk(BIOS_ERR, "FTPM: Source size %zx not aligned\n", src_size);
		return -1;
	}

	src = buffer;
	dst = buffer + size;

	for (off = 0; off < src_size; off += size) {
		if (src_readat(src_store, src, off, size) != size) {
			printk(BIOS_ERR, "FTPM: Failed to read srouce at %zx\n", off);
			return -1;
		}
		if (dst_readat(dst_store, dst, off, size) != size) {
			printk(BIOS_ERR, "FTPM: Failed to read destination at %zx\n", off);
			return -1;
		}
		if (memcmp(src, dst, size))
			return 1;
	}

	return 0;
}

static int compare_store(struct region_device *dst_store, struct region_device *src_store)
{
	return do_compare_store(dst_store, rdev_readat, src_store, rdev_readat);
}

static int compare_region(const char *dst_name, const char *src_name)
{
	struct region_device dst_store;
	struct region_device src_store;

	if (fmap_locate_area_as_rdev_rw(dst_name, &dst_store)) {
		printk(BIOS_ERR, "FTPM: Unable to find %s\n", dst_name);
		return -1;
	}

	if (fmap_locate_area_as_rdev_rw(src_name, &src_store)) {
		printk(BIOS_ERR, "FTPM: Unable to find %s\n", src_name);
		return -1;
	}

	return compare_store(&dst_store, &src_store);
}

static int copy_nvram(const char *dst_name, const char *src_name)
{
	if (copy_region(dst_name, src_name)) {
		printk(BIOS_ERR, "FTPM: Failed to copy %s to %s\n", src_name, dst_name);
		return -1;
	}

	printk(BIOS_NOTICE, "FTPM: Copied %s to %s\n", src_name, dst_name);

	return 0;
}

static int copy_nvram_debug(const char *dst_name, const char *src_name)
{
	struct region_device dst_store;
	struct region_device src_store;
	size_t dst_size;
	size_t src_size;
	struct soc_debug_info info;

	if (fmap_locate_area_as_rdev_rw(dst_name, &dst_store)) {
		printk(BIOS_ERR, "FTPM: Unable to find %s\n", dst_name);
		return -1;
	}

	if (fmap_locate_area_as_rdev_rw(src_name, &src_store)) {
		printk(BIOS_ERR, "FTPM: Unable to find %s\n", src_name);
		return -1;
	}

	if (copy_store(&dst_store, &src_store)) {
		printk(BIOS_ERR, "FTPM: Failed to copy %s to %s\n", src_name, dst_name);
		return -1;
	}

	dst_size = region_device_sz(&dst_store);
	src_size = region_device_sz(&src_store);

	if (src_size + sizeof(info) > dst_size)
		return 0;

	if (soc_get_debug_info(&info))
		return -1;

	if (rdev_writeat(&dst_store, &info, src_size, sizeof(info)) != sizeof(info)) {
		printk(BIOS_ERR, "FTPM: Failed to write debug info at %zx\n", src_size);
		return -1;
	}

	return 0;
}

static int erase_nvram(const char *name)
{
	int ret = -1;
	u8 cs;

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT)) {
		cs = spi_read8(SPI_ALT_CS);
		spi_write8(SPI_ALT_CS, cs & ~SPI_ALT_CS_EN_MASK);
	}

	if (erase_region(name)) {
		printk(BIOS_ERR, "FTPM: Failed to erase %s\n", name);
		goto out;
	}

	printk(BIOS_NOTICE, "FTPM: Erased %s\n", name);

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT)) {
		spi_write8(SPI_ALT_CS, (cs & ~SPI_ALT_CS_EN_MASK) | 1);
		if (erase_region(name)) {
			printk(BIOS_ERR, "FTPM: Failed to erase alternate %s\n", name);
			goto out;
		}

		printk(BIOS_NOTICE, "FTPM: Erased alternate %s\n", name);
	}

	ret = 0;
out:
	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT))
		spi_write8(SPI_ALT_CS, cs);
	return ret;
}

static ssize_t pri_readat(const struct region_device *rd, void *b,
				size_t offset, size_t size)
{
	u8 cs = spi_read8(SPI_ALT_CS);
	spi_write8(SPI_ALT_CS, cs & ~SPI_ALT_CS_EN_MASK);
	return rdev_readat(rd, b, offset, size);
}

static ssize_t alt_readat(const struct region_device *rd, void *b,
				size_t offset, size_t size)
{
	u8 cs = spi_read8(SPI_ALT_CS);
	spi_write8(SPI_ALT_CS, (cs & ~SPI_ALT_CS_EN_MASK) | 1);
	return rdev_readat(rd, b, offset, size);
}

static ssize_t alt_writeat(const struct region_device *rd, const void *b,
			size_t offset, size_t size)
{
	u8 cs = spi_read8(SPI_ALT_CS);
	spi_write8(SPI_ALT_CS, (cs & ~SPI_ALT_CS_EN_MASK) | 1);
	return rdev_writeat(rd, b, offset, size);
}

static int synchronize_nvram(const char *name)
{
	int ret = -1;
	u8 cs;
	struct region_device pri_store;
	struct region_device alt_store;

	cs = spi_read8(SPI_ALT_CS);

	spi_write8(SPI_ALT_CS, cs & ~SPI_ALT_CS_EN_MASK);

	if (fmap_locate_area_as_rdev_rw(name, &pri_store)) {
		printk(BIOS_ERR, "FTPM: Unable to find primary %s\n", name);
		goto out;
	}

	spi_write8(SPI_ALT_CS, (cs & ~SPI_ALT_CS_EN_MASK) | 1);

	if (fmap_locate_area_as_rdev_rw(name, &alt_store)) {
		printk(BIOS_ERR, "FTPM: Unable to find alternate %s\n", name);
		goto out;
	}

	ret = do_compare_store(&alt_store, alt_readat, &pri_store, pri_readat);
	if (ret == -1) {
		printk(BIOS_ERR, "FTPM: Failed to compare primary and alternate %s\n", name);
		goto out;
	}

	// Do not restore when destination and source are identical.
	if (ret == 0) {
		printk(BIOS_NOTICE, "FTPM: Primary and alternate %s match\n", name);
		goto out;
	}

	ret = -1;

	spi_write8(SPI_ALT_CS, (cs & ~SPI_ALT_CS_EN_MASK) | 1);

	copy_nvram(NAME_PSP_NVRAM_DEBUG, NAME_PSP_NVRAM);

	if (erase_store(&alt_store)) {
		printk(BIOS_ERR, "FTPM: Failed to erase alternate %s\n", name);
		goto out;
	}

	if (write_store(&alt_store, alt_writeat, &pri_store, pri_readat)) {
		printk(BIOS_ERR, "FTPM: Failed to copy primary %s to alternate\n", name);
		goto out;
	}

	printk(BIOS_NOTICE, "FTPM: Restored alternate %s from primary\n", name);

	ret = 1;
out:
	spi_write8(SPI_ALT_CS, cs);
	return ret;
}

/*
 * ftpm_active()
 *
 * Check if fTPM is supported/functional in PSP capabilities.
 *
 * Return true if active, false otherwise.
 */
bool ftpm_active(void)
{
	int ret;
	uint32_t caps;

	if (psp_get_caps(&caps))
		return false;

	if (caps & CAP_TPM_SUPPORTED) {
		if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT))
			synchronize_nvram(NAME_PSP_NVRAM);
		return true;
	}

	if (caps & CAP_TPM_REQ_FACTORY_RESET) {
		printk(BIOS_WARNING, "FTPM: NVRAM corrupted.\n");

		ret = compare_region(NAME_PSP_NVRAM, NAME_PSP_NVRAM_BACKUP);
		if (ret == 0) {
			printk(BIOS_NOTICE, "FTPM: %s already matches %s\n", NAME_PSP_NVRAM, NAME_PSP_NVRAM_BACKUP);
			copy_nvram_debug(NAME_PSP_NVRAM_BACKUP_DEBUG, NAME_PSP_NVRAM_BACKUP);
			erase_nvram(NAME_PSP_NVRAM_BACKUP);
		} else {
			copy_nvram_debug(NAME_PSP_NVRAM_DEBUG, NAME_PSP_NVRAM);
			if (ret == 1) {
				if (copy_region(NAME_PSP_NVRAM, NAME_PSP_NVRAM_BACKUP) == 0) {
					printk(BIOS_NOTICE, "FTPM: Restored %s from %s\n", NAME_PSP_NVRAM, NAME_PSP_NVRAM_BACKUP);
					cold_reset();
				}
			}
		}

		erase_nvram(NAME_PSP_NVRAM);
		cold_reset();
	}

	if (caps & CAP_FTPM_NEED_RECOVER)
		printk(BIOS_NOTICE, "FTPM: Binary corrupted.\n");

	return false;
}
