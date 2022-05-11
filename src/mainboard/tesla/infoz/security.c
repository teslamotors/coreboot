/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Security fusing functions for InfoZ platform
 */

#include <amdblocks/psp.h>
#include <amdblocks/reset.h>
#include <amdblocks/psp.h>
#include <amdblocks/spi.h>
#include <bootstate.h>
#include <boot_device.h>
#include <console/console.h>
#include <spi_flash.h>
#include <soc/psb.h>
#include <soc/rpmc.h>

#if CONFIG(FAILURE_ANALYSIS)
static void enforce_board_demotion(void *unused) {
	int ret;
	struct psb_fuse_config cfg;
	uint32_t customer_key_lock;
	uint32_t root_key_select;

	ret = psp_get_psb_fuse_config(&cfg);
	if (ret != 0) {
		printk(BIOS_ERR, "PSB: Failed to get config: %d\n", ret);
		cold_reset();
	}

	customer_key_lock = (cfg.config >> 28) & 1;
	root_key_select = (cfg.config >> 16) & 0xFF;
	if (customer_key_lock != 1 || root_key_select != 0x08) {
		printk(BIOS_ERR, "PSB: board is not demoted: \
				customer_key_lock=%#x, root_key_select=%#x\n",
				customer_key_lock, root_key_select);
		cold_reset();
	}
}
BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, enforce_board_demotion, NULL);
#endif

static void enable_secure_boot(void *unused)
{
	int ret;
	struct psb_fuse_config psb;

	ret = psb_enable();
	if (ret) {
		if (!CONFIG(REQUIRE_PLATFORM_SECURE_BOOT)) {
			if (ret != 1)
				return;
		}
		printk(BIOS_NOTICE, "PSB: Rebooting\n");
		cold_reset();
	}

	ret = psp_get_psb_fuse_config(&psb);
	if (ret != 0) {
		printk(BIOS_ERR, "PSB: Failed to get config: %d\n", ret);
		return;
	}

	// Ensure rollback protection is enabled on all hardware but not
	// incremented on engineering hardware.
	if (!(psb.config & PSB_CONFIG_CUSTOMER_KEY_LOCK_BIT))
		psb_update_anti_rollback();
// Avoid in-field fusing until fusing failures are root caused.
#if 0
	else if (psb.bios_key_rev_id > 0)
		psb_update_anti_rollback();
#endif
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, enable_secure_boot, NULL);

#if CONFIG(PSP_RPMC_ROOT_KEY_PROVISIONING)

/*
 * Request the RPMC monotonic counter with an invalid signature and check
 * the resultant status to determine whether the RPMC root key is provisioned.
 */
static int root_key_provisioned(const struct spi_flash *flash)
{
	int ret;
	u32 key;
	u8 sig[SPI_RPMC_SIG_LEN];

	key = 0;
	memset(&sig, 0, sizeof(sig));

	ret = spi_flash_rpmc_init(flash, CONFIG_PSP_RPMC_ROOT_KEY_ADDRESS,
				  key, sig);
	if (ret == -1)
		return -1;

	printk(BIOS_NOTICE, "RPMC: ESR = %02x\n", ret);

	if (ret == 0) {
		printk(BIOS_WARNING, "RPMC: Initialization unexpectedly succeeded\n");
		return -1;
	}

	/*
	 * If the RPMC root key is not provisioned, the ESR is expected to
	 * report the monotonic counter as uninitialized.
	 */
	if (ret & SPI_FLASH_RPMC_ERR_OTHER)
		return 0;

	/*
	 * If the RPMC root key is provisioned, the ESR is expected to report
	 * the signature as invalid.
	 */
	if (ret & SPI_FLASH_RPMC_ERR_INVALID)
		return 1;

	return -ret;
}

static int alt_root_key_provisioned(const struct spi_flash *flash)
{
	int ret;
	u8 cs = spi_read8(SPI_ALT_CS);

	spi_write8(SPI_ALT_CS, (cs & ~SPI_ALT_CS_EN_MASK) | 1);

	ret = root_key_provisioned(flash);

	spi_write8(SPI_ALT_CS, cs);

	return ret;
}

/*
 * Check whether the RPMC root key is provisioned on the alternate SPI ROM
 * to determine whether the lock flag should be set in the BIOS-PSP command
 * to provision RPMC root key on the primary SPI ROM.
 */
static void provision_rpmc(void *unused)
{
	int ret;
	bool lock = false;
	const struct spi_flash *flash;

	/*
	 * Rev-C boards have Micron SPI flash that does not properly report
	 * RPMC capabilities in the SFDP.
	 */
	ret = rpmc_supported();
	if (ret != 1)
		return;

	/*
	 * Do nothing if RPMC provisioning is already completed or if the
	 * RPMC root key was provisioned by PSP on this boot.
	 */
	ret = rpmc_provisioned();
	if (ret != 0)
		return;

	flash = boot_device_spi_flash();
	if (!flash) {
		printk(BIOS_ERR, "RPMC: Failed to find SPI device\n");
		return;
	}

	/* Check primary SPI RPMC root key provisioning state. */
	ret = root_key_provisioned(flash);
	if (ret < 0) {
		printk(BIOS_ERR, "RPMC: Failed to check SPI root key: %d\n", ret);
		return;
	}

	/* Do nothing if primary SPI RPMC root key is provisioned. */
	if (ret) {
		printk(BIOS_NOTICE, "RPMC: Root key provisioned\n");
		return;
	}

	printk(BIOS_NOTICE, "RPMC: Root key unprovisioned\n");

	/* Check alternate SPI RPMC root key provisioning state. */
	ret = alt_root_key_provisioned(flash);
	if (ret < 0) {
		printk(BIOS_ERR, "RPMC: Failed to check Alt SPI root key: %d\n", ret);
		return;
	}

	/*
	 * Set lock flag in BIOS-PSP RPMC root key provisioning command if
	 * alternate SPI RPMC root key is provisioned.
	 */
	if (ret) {
		printk(BIOS_NOTICE, "RPMC: Alt SPI root key provisioned\n");
		lock = true;
	} else {
		printk(BIOS_NOTICE, "RPMC: Alt SPI root key unprovisioned\n");
	}

	ret = rpmc_provision(lock);
	if (ret < 0) {
		printk(BIOS_ERR, "RPMC: Failed to provision root key\n");
		return;
	}
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, provision_rpmc, NULL);

#endif
