/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Tesla Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <string.h>
#include <version.h>
#include <delay.h>

#include <console/console.h>

#include <soc/boot_control.h>
#include <soc/scs_helpers.h>
#include <soc/sms_helpers.h>

#include <soc/gpio.h>

static int board_id_x = -1;
static int cpu_rev_x = -1;

#if (!IS_ENABLED(CONFIG_BOOTBLOCK_X) && !ENV_BOOTBLOCK) || IS_ENABLED(CONFIG_BOOTBLOCK_X)

static int get_gpg_pull(int n, int pin, int pull)
{
	struct trav_gpio_part2 *gpio_peric =
			(struct trav_gpio_part2 *)TRAV_GPIO_PERIC;
	struct trav_gpio_bank *bank = &gpio_peric->gpg[n];

	trav_gpio_set_pull(bank, pin, pull);
	trav_gpio_cfg_pin(bank, pin, TRAV_GPIO_INPUT);

	return trav_gpio_get_value(bank, pin);
}

#endif

#if IS_ENABLED(CONFIG_BOOTBLOCK_X)

static int get_gpg(int n, int pin)
{
	return get_gpg_pull(n, pin, TRAV_GPIO_PULL_DOWN);
}

int trav_board_id(void)
{
	if (board_id_x < 0) {
		struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

		if (env->board_id_x != 0) {
			board_id_x = env->board_id_x;

			return board_id_x;
		}

		/*
		  BOARD:   ID2:  FF: ID3: ID1: ID0
		  E1-MSX:    0:   1:   0:   1:   1  ==> 0x0b
		  E1-M3:     0:   1:   1:   1:   1  ==> 0x0f
		  B1-MY:     1:   1:   1:   0:   0  ==> 0x1c

		    gpio56/gpg7-0: ID2  gpio43/gpg5-3: ID3
		    gpio52/gpg6-4: ID1  gpio33/gpg4-1: ID0
		 */
		board_id_x = (get_gpg(7, 0)<<4) | (1<<3) |
			     (get_gpg(5, 3)<<2) | (get_gpg(6, 4)<<1) |
			      get_gpg(4, 1);
	}

	return board_id_x;
}

int trav_cpu_rev(void)
{
	if (cpu_rev_x < 0)
		cpu_rev_x = get_turbo_cpu_rev();

	return cpu_rev_x;
}

void check_a72bl1_versions(void)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	strncpy(env->a72bl1_coreboot_version, (char *) coreboot_version, sizeof (env->a72bl1_coreboot_version));
}

#else

int trav_board_id(void)
{
	if (board_id_x < 0) {
		struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

		board_id_x = env->board_id_x;
	}

	return board_id_x;
}

int trav_cpu_rev(void)
{
	if (cpu_rev_x < 0) {
		struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

		cpu_rev_x = env->cpu_rev;
	}

	return cpu_rev_x;
}

bool is_trav_board_hw31(void)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;
	int board_id;

	if (env->hw_minor >= 1)
		return true;

	board_id = trav_board_id();

	return (board_id == 0x1c || board_id == 0x1d || board_id == 0x1e);
}

#if ENV_BOOTBLOCK
void check_a72bootblock_versions(void)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	strncpy(env->a72bootblock_coreboot_version, (char *) coreboot_version,
		 sizeof (env->a72bootblock_coreboot_version));
}
#endif

#endif

int trav_from_a72_warm_reset(void)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	return env->a72_warm_reset;
}

static void set_gpg_pull_drv(int n, int pin, int pull, int drv)
{
	struct trav_gpio_part2 *gpio_peric =
			(struct trav_gpio_part2 *)TRAV_GPIO_PERIC;
	struct trav_gpio_bank *bank = &gpio_peric->gpg[n];

	trav_gpio_set_pull(bank, pin, pull);
	trav_gpio_set_drv(bank, pin, drv);
}

static void set_gpg(int n, int pin, int val)
{
	struct trav_gpio_part2 *gpio_peric =
			(struct trav_gpio_part2 *)TRAV_GPIO_PERIC;
	struct trav_gpio_bank *bank = &gpio_peric->gpg[n];

	trav_gpio_direction_output(bank, pin, val);
}

#if ENV_BOOTBLOCK

/* gpio16 TURBO-AP-HEALTHY: gpg2-0 C1 */
static void set_gpio16(int val)
{
	set_gpg_pull_drv(2, 0, TRAV_GPIO_PULL_NONE, TRAV_GPIO_DRV_1X);
	set_gpg(2, 0, val);
}

void set_ap_healthy(int val)
{
	int board_id = trav_board_id();

	/* FF C1 and above */
	if ((board_id & 0x0a) == 0x0a || (board_id & 0xf0) != 0x00)
		set_gpio16(val);
}

/* gpq0-2 need to set to function 2 */
void a72wdt_gpio_conf(void)
{
	struct trav_gpio_part3 *gpio_pmu =
			(struct trav_gpio_part3 *)TRAV_GPIO_PMU;
	struct trav_gpio_bank *bank = &gpio_pmu->gpq0;
	int pin = 2;

	trav_gpio_cfg_pin(bank, pin, TRAV_GPIO_FUNC(2));
}

/* use coreboot.rom from qspi when gpio is 1 */
int is_boot_from_qspi(void)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

#if !IS_ENABLED(CONFIG_BOOTBLOCK_X)
	/* check if bootblock-x already set the value for bootblock in coreboot */
	if (env->boot_from[0] == 'r')
		return 1;

	if (env->boot_from[0] == 'u')
		return 0;
#else
	/* need to boot -r ? */
	if (env->bootx_bank_data & (1<<2)) {
		env->boot_from[0] = 'r';
		printk(BIOS_INFO, "boot from -r: bank_data\n");
		return 1;
	} else {
		env->boot_from[0] = 'u';
		return 0;
	}

#endif
	return 1;
}

int check_boot_file_layout(struct boot_file_layout *file_layout)
{
	if (memcmp(file_layout->signature, "BOOT_FILE_LAYOUT", 16)) {
		printk(BIOS_INFO, "Invalid boot_file_layout\n");

		return -1;
	}

	return 0;
}

#else /* RAMSTAGE */

#include <program_loading.h>

static void update_dtb_bootfrom_cmdline(unsigned char *load, int size)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;
	char *p;
	int nn = strlen("bootfrom=");

	p = strnstr((char *)load, size, "bootfrom=", nn);

	if (!p) {
		printk(BIOS_INFO, "Bad dtb that does not have bootfrom=ua or bootfrom=ra in bootargs\n");
		die("...");
	}

	p += nn;
	if (p[1] != 'a') {
		printk(BIOS_INFO, "Bad dtb that does not have bootfrom=ua or bootfrom=ra in bootargs\n");
		die("...");
	}

	if ((p[0] != env->boot_from[0]) || (p[1] != env->boot_from[1])) {
		p[0] = env->boot_from[0];
		p[1] = env->boot_from[1];

	}
	printk(BIOS_INFO, "bootfrom=%c%c\n", p[0], p[1]);
}

static void update_dtb_nstr(unsigned char *load, int size, const char *from, int nn,
			    char *to, int n_to, int binary)
{
	char *p;

	if (n_to > nn)
		return;

	p = strnstr((char *)load, size, from, nn);

	if (!p)
		return;

	memcpy(p, to, n_to);
	if (nn > n_to)
		memset(p+n_to, 0, (nn-n_to));

	if (!binary)
		printk(BIOS_DEBUG, "dtb: %s ==> %s\n", from, to);
	else {
		int i;

		printk(BIOS_DEBUG, "dtb: ");
		for (i = 0; i < nn; i++)
			printk(BIOS_DEBUG, "%02x ", from[i]);
		printk(BIOS_DEBUG, "==> ");
		for (i = 0; i < n_to; i++)
			printk(BIOS_DEBUG, "%02x ", to[i]);
		printk(BIOS_DEBUG, "\n");
	}
}

static void update_dtb_str(unsigned char *load, int size, const char *from, char *to)
{
	int nn = strlen(from);
	int n_to = strlen(to);

	return update_dtb_nstr(load, size, from, nn, to, n_to, 0);
}

static void update_dtb_rootfs_dev(unsigned char *load, int size)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	if (env->boot_from[0] != 'u')
		return;

	update_dtb_str(load, size, "DEV_00112233445566778899AABBCCDDEEFF", env->rootfs_part_uuid);
}

static int is_turbo_a(void)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	if (env->turbo_a_b[0] == 'a')
		return 1;

	if (env->turbo_a_b[0] == 'b')
		return 0;

	return -1;
}

/* ff: TURBO-PCIE0-REFCLK-EN: gpg2-6 */
/* turbo: gpio22 TURBO-PCIE0_nPERST_3V3/TURBOB-PCIE0_nPERST_3V3: gpg2-6 */
static void set_gpio22(int val)
{
	set_gpg_pull_drv(2, 6, TRAV_GPIO_PULL_UP, TRAV_GPIO_DRV_2X);
	set_gpg(2, 6, val);
}

/* ff: gpio23 TURBO-SSD_nPERST: gpg2-7 */
/* turbo: gpio23 TURBO-PCIE1_nPERST_3V3/TURBOB-PCIE1_nPERST_3V3: gpg2-7 */
static void set_gpio23(int val)
{
	set_gpg_pull_drv(2, 7, TRAV_GPIO_PULL_UP, TRAV_GPIO_DRV_2X);
	set_gpg(2, 7, val);
}

/* gpio8: TURBO-SSD-REFCLK-EN: gpg1-0 */
static void set_gpio8(int val)
{
	set_gpg(1, 0, val);
}

/* TURBO-3V3-SW-SSD-EN: gpio42 gpg5-2 */
static void set_gpio42(int val)
{
	set_gpg_pull_drv(5, 2, TRAV_GPIO_PULL_NONE, TRAV_GPIO_DRV_2X);
	set_gpg(5, 2, val);
}

static int get_gpio42(void)
{
	return get_gpg_pull(5, 2, TRAV_GPIO_PULL_UP);
}

/* gpio53 TURBO-PCIE0_nPERST: gpg6-5 */
static void set_gpio53(int val)
{
	set_gpg(6, 5, val);
}

/* gpio54 TURBO-PCIE1_nPERST: gpg6-6 */
static void set_gpio54(int val)
{
	set_gpg(6, 6, val);
}

static void update_dtb_pcie_sts(unsigned char *load, int size)
{
	int board_id;
	char okay_str[] = "okay";
	char disabled_str[] = "disabled";

	/* read again */
	board_id = trav_board_id();
	printk(BIOS_INFO, "boardid=%02x\n", board_id);

	if (board_id >= 8) {
		if (is_turbo_a() == 1) {
			/* TURBO A */
			int use_pcie4 = 0;

			if (!get_gpio42()) /* M2 socket is installed */ {
				use_pcie4 = 1;
				printk(BIOS_INFO, "M2 socket is installed\n");
			}

			if (!use_pcie4) {
				/* not enabled for FF E1 and above */
				update_dtb_str(load, size, "PCIE4STS", disabled_str);
				update_dtb_str(load, size, "PCIE4PHY", disabled_str);
			} else {
				update_dtb_str(load, size, "PCIE4STS", okay_str);
				update_dtb_str(load, size, "PCIE4PHY", okay_str);
				set_gpio8(1);  /* Enable REF CLK */
				set_gpio42(1); /* pcie 3.3v en */
				set_gpio54(1); /* for PCIE4 */
				set_gpio23(1); /* for PCIE4 */
			}

			update_dtb_str(load, size, "PCIE0STS", okay_str);
			update_dtb_str(load, size, "PCIE0EPSTS", disabled_str);
			update_dtb_str(load, size, "PCIE0PHY", okay_str);
			set_gpio53(1); /* for PCIE0 */
			set_gpio22(1); /* for PCIE0 */
		} else  {
			/* TURBO B */
			update_dtb_str(load, size, "PCIE4STS", disabled_str);
			update_dtb_str(load, size, "PCIE4PHY", disabled_str);

			update_dtb_str(load, size, "PCIE0STS", disabled_str);
			update_dtb_str(load, size, "PCIE0EPSTS", okay_str);
			update_dtb_str(load, size, "PCIE0PHY", okay_str);
			set_gpio53(1); /* for PCIE0 */
			set_gpio22(1); /* for PCIE0 */
		}
	}

	printk(BIOS_INFO, "pcie slots: %s\n", (board_id != 0) ? "enabled" : "disabled");
}

static void update_dtb_board_version(unsigned char *load, int size)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;
	int board_id;
	char board_version[8] = { 0 };
	char hw_minor[8] = { 0 };
	char turbo[] = "A";

	board_id = trav_board_id();
	if (board_id >= 0x10)
		snprintf(board_version, sizeof(board_version), "0x%x", board_id);
	else
		board_version[0] = 'A' + board_id;
	update_dtb_str(load, size, "TURBO_BOARD_VERSION_TOKEN_STRING", board_version);

	snprintf(hw_minor, sizeof(hw_minor), "%d", env->hw_minor);
	update_dtb_str(load, size, "TURBO_HW_MINOR_TOKEN_STRING", hw_minor);

	if (board_id > 1 && is_turbo_a() != 1)
		turbo[0] = 'B';
	update_dtb_str(load, size, "TURBO_A_B_TOKEN_STRING", turbo);
}

static void update_dtb_coreboot_version(unsigned char *load, int size)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	update_dtb_str(load, size, "A72BL1_COREBOOT_VERSION_TOKEN_STRING",
				env->a72bl1_coreboot_version);
	update_dtb_str(load, size, "A72BOOTBLOCK_COREBOOT_VERSION_TOKEN_STRING",
				env->a72bootblock_coreboot_version);

	update_dtb_str(load, size, "COREBOOT_VERSION_TOKEN_STRING",
				(char *) coreboot_version);
	update_dtb_str(load, size, "COREBOOT_EXTRA_VERSION_TOKEN_STRING",
				(char *) coreboot_extra_version);
	update_dtb_str(load, size, "COREBOOT_BUILD_TOKEN_LONG_STRING",
				(char *) coreboot_build);
}

static void update_dtb_bootfrom(unsigned char *load, int size)
{
	char boot_from[] = "xx";
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	boot_from[0] = env->boot_from[0];
	boot_from[1] = env->boot_from[1];
	update_dtb_str(load, size, "BOOTFROM_TOKEN_STRING", boot_from);
}

static void update_dtb_bootxfrom(unsigned char *load, int size)
{
	char boot_x_from[] = "x";
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	boot_x_from[0] = (char) (env->bootx_booted_bank + 'a');
	update_dtb_str(load, size, "BOOTXFROM_TOKEN_STRING", boot_x_from);
}

static void update_dtb_otp_chip_id(unsigned char *load, int size)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	update_dtb_str(load, size, "TURBO_OTP_CHIP_ID_32_TOKEN_STRING", env->otp_chip_id);
}

static void update_dtb_soc_id(unsigned char *load, int size)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	update_dtb_str(load, size, "TURBO_SOC_ID_TOKEN_STRING", env->soc_id);
}

static void update_dtb_eth_sts(unsigned char *load, int size)
{
	int board_id;
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;
	int mac_addr_index = 0;

	board_id = trav_board_id();
	if (board_id >= 8) {
		char none_phy_str[] = "NONE_PHY";

		/* no phy for eth0 and eth1 */
		update_dtb_str(load, size, "ETH0_PHY", none_phy_str);
		update_dtb_str(load, size, "ETH1_PHY", none_phy_str);
		if (is_turbo_a() == 1) {
			/* TURBO A */
		} else {
			/* TURBO B */
			mac_addr_index = 2;
		}
	}

	env->mac_addr[5] = mac_addr_index;
	update_dtb_nstr(load, size, "ETH0MC", 6, env->mac_addr, 6, 1);
	env->mac_addr[5] = mac_addr_index + 1;
	update_dtb_nstr(load, size, "ETH1MC", 6, env->mac_addr, 6, 1);
}

static void update_dtb_all_versions(unsigned char *load, int size)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;
	char ver[] = "2";

	update_dtb_str(load, size, "BOARD_PCBA_VERSION_TOKEN_STRING",
				env->board_pcba_version);

	update_dtb_str(load, size, "SCSBL1_BUILD_GIT_HASH_TOKEN_STRING_1234567890",
				env->scsbl1_build_git_hash);

	update_dtb_str(load, size, "SCSBL2_BUILD_GIT_HASH_TOKEN_STRING_1234567890",
				env->scsbl2_build_git_hash);

	update_dtb_str(load, size, "SMS_BUILD_GIT_HASH_TOKEN_STRING_1234567890",
				env->sms_build_git_hash);

	update_dtb_str(load, size, "BOOTX_BL1_DATA_VER_STRING", ver);
}

static void update_dtb_ddr_vendor(unsigned char *load, int size)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	update_dtb_str(load, size, "DDR_VENDOR_TOKEN_STRING", env->ddr_vendor);
}

static void update_dtb_a72_warm_reset(unsigned char *load, int size)
{
	char warm_reset_str[] = "0";
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	warm_reset_str[0] = (char) (env->a72_warm_reset + '0');

	update_dtb_str(load, size, "A72_WARM_RESET_STRING", warm_reset_str);
}

void update_dtb(unsigned char *load, int size)
{
	update_dtb_bootfrom_cmdline(load, size);
	update_dtb_rootfs_dev(load, size);
	update_dtb_pcie_sts(load, size);
	update_dtb_board_version(load, size);
	update_dtb_coreboot_version(load, size);
	update_dtb_bootfrom(load, size);
	update_dtb_bootxfrom(load, size);
	update_dtb_otp_chip_id(load, size);
	update_dtb_soc_id(load, size);
	update_dtb_eth_sts(load, size);
	update_dtb_all_versions(load, size);
	update_dtb_ddr_vendor(load, size);
	update_dtb_a72_warm_reset(load, size);
}

int use_dtb2(void)
{
	return (trav_cpu_rev() >= 0x10) && !is_trav_board_hw31(); /* B0 */
}

#endif
