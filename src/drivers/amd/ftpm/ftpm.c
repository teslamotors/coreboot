/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <amdblocks/spi.h>
#include <amdblocks/psp.h>
#include <amdblocks/reset.h>
#include <boot_device.h>
#include <console/console.h>
#include <fmap.h>

#include "ftpm.h"

#define CAP_TPM_SUPPORTED		(1 << 0)
#define CAP_TPM_REQ_FACTORY_RESET	(1 << 1)
#define CAP_FTPM_NEED_RECOVER		(1 << 2)

static int lookup_store(struct region_device *rstore)
{
	static struct region_device read_rdev, write_rdev;
	static struct incoherent_rdev store_irdev;
	struct region region;
	const struct region_device *rdev;

	if (fmap_locate_area("PSP_NVRAM", &region))
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

static bool erase_store(void)
{
	bool ret;
	u8 cs;
	struct region_device store;

	if (lookup_store(&store)) {
		printk(BIOS_ERR, "FTPM: Unable to find PSP NVRAM\n");
		return false;
	}

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT)) {
		cs = spi_read8(SPI_ALT_CS);
		spi_write8(SPI_ALT_CS, cs & ~SPI_ALT_CS_EN_MASK);
	}

	if (rdev_eraseat(&store, 0, region_device_sz(&store))
	    != region_device_sz(&store)) {
		printk(BIOS_ERR, "FTPM: Failed to erase PSP NVRAM\n");
		ret = false;
		goto out;
	}

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT)) {
		spi_write8(SPI_ALT_CS, (cs & ~SPI_ALT_CS_EN_MASK) | 1);

		if (rdev_eraseat(&store, 0, region_device_sz(&store))
		    != region_device_sz(&store)) {
			printk(BIOS_ERR, "FTPM: Failed to erase Alt SPI PSP NVRAM\n");
			ret = false;
			goto out;
		}
	}

	ret = true;
out:
	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_ALT))
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
	uint32_t caps;

	if (psp_get_caps(&caps))
		return false;

	if (caps & CAP_TPM_SUPPORTED)
		return true;

	if (caps & CAP_TPM_REQ_FACTORY_RESET) {
		printk(BIOS_WARNING, "FTPM: NVRAM corrupted.\n");
		if (erase_store())
			cold_reset();
	}

	if (caps & CAP_FTPM_NEED_RECOVER)
		printk(BIOS_INFO, "FTPM: Binary corrupted.\n");

	return false;
}
