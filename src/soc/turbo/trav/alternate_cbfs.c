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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <assert.h>
#include <boot_device.h>
#include <cbfs.h>  /* This driver serves as a CBFS media source. */
#include <string.h>
#include <console/console.h>
#include <delay.h>
#include <soc/alternate_cbfs.h>
#include <symbols.h>
#include <soc/ufs.h>
#include <soc/boot_control.h>
#include <soc/gpt.h>
#include <soc/sms_helpers.h>
#include <soc/scs_helpers.h>

#define BOOT_ENV_START_MiB	2
#define BOOT_BANK_START_MiB	4
#define BOOT_BANK_SIZE_MiB	66

int load_bootblock_from_ufs(void)
{
#if ENV_BOOTBLOCK && IS_ENABLED(CONFIG_TRAV_UFS) && IS_ENABLED(CONFIG_BOOTBLOCK_X)
	struct boot_file_layout *file_layout = (struct boot_file_layout *)CONFIG_BOOT_FILE_LAYOUT_MEM_BASE;
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;
	size_t bootblock_size;
	size_t bootblock_start_4k;
	size_t crt_start_4k;
	size_t crt_size;
	size_t start_4k;

	init_ufs_load_from_ufs();

	env->boot_bank = env->bootx_booted_bank;
	env->boot_from[0] = 'u'; /* restore it */
	env->boot_from[1] = env->boot_bank + 'a';
	env->board_id_x = trav_board_id();
	env->cpu_rev = trav_cpu_rev();

//	printk(BIOS_INFO, "Booting from BANK-%c...\n", env->boot_bank + 'A');

	start_4k = ((env->boot_bank * BOOT_BANK_SIZE_MiB) + BOOT_BANK_START_MiB) << (20 - 12);
	/* start from LUN 0, 4MiB or (4 + 66)MiB */
//	printk(BIOS_INFO, "Loading boot_file_layout...\n");
	load_from_ufs((unsigned char *) file_layout, 0, start_4k, 4096);
	if (check_boot_file_layout(file_layout))
		goto out_reset;

	bootblock_start_4k = start_4k + (file_layout->pos[2].offset >> 12);
	bootblock_size = file_layout->pos[2].size;
	if (bootblock_size > ((128<<10) + 0x2d8)) {
		printk(BIOS_INFO, "Wrong bootblock size in boot bank\n");
		goto out_reset;
	}

	/* signed have 0x2d8 on head, ufs read/write need to be 4k aligned, so read the tail more  */
	/* assume 0x28000 to 0x29000 is not used */
	printk(BIOS_INFO, "Loading bootblock...\n");
	load_from_ufs((unsigned char *) CONFIG_BOOTBLOCK_START, 0, bootblock_start_4k,
			 bootblock_size & 0xfffff000);
	load_from_ufs((unsigned char *) CONFIG_BOOTBLOCK_START + (bootblock_size & 0xfffff000),
			 0, bootblock_start_4k + (bootblock_size >> 12), 4096);

	/* load crt */
	crt_start_4k = start_4k + (file_layout->pos[0].offset >> 12);
	crt_size = file_layout->pos[0].size;
	if (crt_size > CONFIG_BOOT_FILE_CRT_MEM_SIZE) {
		printk(BIOS_INFO, "Wrong crt size in boot bank\n");
		goto out_reset;
	}
//	printk(BIOS_INFO, "Loading bootblock crt...\n");
	load_from_ufs((unsigned char *) CONFIG_BOOT_FILE_CRT_MEM_BASE, 0, crt_start_4k,
			 (crt_size + 0xfff) & 0xfffff000);  /* round up to 4K aligned */

	/* scs verify it */
	if (verify_coreboot(VERIFY_BOOTBLOCK_A + env->boot_bank, CONFIG_BOOTBLOCK_START, bootblock_size,
			    CONFIG_BOOT_FILE_CRT_MEM_BASE, crt_size)) {
		printk(BIOS_INFO, "bootblock verify failed\n");
		if (trav_cpu_rev() >= 0x10) /* B0 */
			goto out_reset;
	}

	/* move to the postion that we want */
	memmove((unsigned char *) CONFIG_BOOTBLOCK_START, (unsigned char *) CONFIG_BOOTBLOCK_START + 0x2d8,
		bootblock_size - 0x2d8);

	return 0;

out_reset:
	return -2;

#else

	return 0;

#endif
}

#if ENV_BOOTBLOCK && IS_ENABLED(CONFIG_TRAV_UFS) && !IS_ENABLED(CONFIG_BOOTBLOCK_X)
static void get_rootfs_part_uuid(void)
{
	unsigned char *buf = (unsigned char *)_cbfs_cache;
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;
	int partno = -1;
	char name[32];
	char uuid[37];

	memset(env->rootfs_part_uuid, 0, sizeof (env->rootfs_part_uuid));

	if (env->boot_from[1] == 'a')
		partno = 4;
	else if (env->boot_from[1] == 'b')
		partno = 5;
	else {
		strncpy(env->rootfs_part_uuid, "NOT_FOUND", 36);
		return;
	}

	load_from_ufs(buf, 0, 0, (1<<20)); /* read 1MiB */

	memset(uuid, 0, sizeof (uuid));
	memset(name, 0, sizeof (name));
	get_gpt_part_uuid_name(buf, partno, uuid, sizeof(uuid), name, sizeof (name));

	printk(BIOS_INFO, "part: %s %s\n", name, uuid);

	if (env->boot_from[1] == 'a') {
		if (strcmp(name, "rootfs-a")) {
			strncpy(env->rootfs_part_uuid, "NOT_FOUND for rootfs-a", 36);
			return;
		}
	} else if (env->boot_from[1] == 'b') {
		if (strcmp(name, "rootfs-b")) {
			strncpy(env->rootfs_part_uuid, "NOT_FOUND for rootfs-b", 36);
			return;
		}
	}

	memset(env->rootfs_part_uuid, 0, sizeof (env->rootfs_part_uuid));
	strncpy(env->rootfs_part_uuid, uuid, 36);
}
#endif

void load_coreboot_from_ufs(void)
{
#if ENV_BOOTBLOCK && IS_ENABLED(CONFIG_TRAV_UFS) && !IS_ENABLED(CONFIG_BOOTBLOCK_X)
	struct boot_file_layout *file_layout = (struct boot_file_layout *)CONFIG_BOOT_FILE_LAYOUT_MEM_BASE;
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;
	size_t coreboot_size;
	size_t coreboot_start_4k;
	size_t crt_size;
	size_t crt_start_4k;
	size_t start_4k;
	unsigned char *crt = _cbfs_cache + _cbfs_cache_size + (1<<20);

	init_ufs_load_from_ufs();

	get_rootfs_part_uuid();

	/* start from LUN 0, 4MiB or (4 + 66)MiB */
	start_4k = ((env->boot_bank * BOOT_BANK_SIZE_MiB) + BOOT_BANK_START_MiB) << (20 - 12);

	coreboot_start_4k = start_4k + (file_layout->pos[3].offset >> 12);
	coreboot_size = file_layout->pos[3].size;
	if (coreboot_size > (_cbfs_cache_size + 0x2d8)) {
		printk(BIOS_INFO, "Wrong coreboot size in boot bank\n");
		goto out_reset;
	}

	/* signed have 0x2d8 on head, ufs read/write need to be 4k aligned, so read the tail more  */
	/* assume after 0xf0000000 + 64M, to 0xf0000000 + 65M not used */
	printk(BIOS_INFO, "Loading coreboot...\n");
	load_from_ufs((unsigned char *) _cbfs_cache, 0, coreboot_start_4k,
			 coreboot_size & 0xfffff000);
	load_from_ufs((unsigned char *) _cbfs_cache + (coreboot_size & 0xfffff000),
			 0, coreboot_start_4k + (coreboot_size >> 12), 4096);

	/* load crt */
	crt_start_4k = start_4k + (file_layout->pos[1].offset >> 12);
	crt_size = file_layout->pos[1].size;
	if (crt_size > CONFIG_BOOT_FILE_CRT_MEM_SIZE) {
		printk(BIOS_INFO, "Wrong crt size in boot bank\n");
		goto out_reset;
	}
	printk(BIOS_INFO, "Loading coreboot crt...\n");
	load_from_ufs((unsigned char *) crt, 0, crt_start_4k,
			 (crt_size + 0xfff) & 0xfffff000);  /* round up to 4K aligned */

	/* scs verify it */
	if (verify_coreboot(VERIFY_COREBOOT_A + env->boot_bank, (size_t) _cbfs_cache, coreboot_size,
			     (size_t) crt, crt_size)) {
		printk(BIOS_INFO, "coreboot verify failed\n");
		if (trav_cpu_rev() >= 0x10) /* B0 */
			goto out_reset;
	}

	/* move to the postion that we want */
	memmove((unsigned char *) _cbfs_cache, (unsigned char *) _cbfs_cache + 0x2d8,
		coreboot_size - 0x2d8);

	return;

out_reset:
	sms_reset(RESET_MODE_WARM);

#endif
}

static int ufs_cbfs_open(void)
{
#if ENV_BOOTBLOCK && IS_ENABLED(CONFIG_TRAV_UFS)
	static int first_run = 1;

	if (!first_run)
		return 0;

#if !IS_ENABLED(CONFIG_BOOTBLOCK_X)
	/* -r need to run config, otherwise need to load cbfs */
	if (is_boot_from_qspi())
		ufs_configure_ufs_device_lun();
	else
		load_coreboot_from_ufs();
#endif

	first_run = 0;
#endif
	return 0;
}

static struct mem_region_device alternate_rdev =
	MEM_REGION_DEV_RO_INIT(NULL, 0);

const struct region_device *boot_device_ro(void)
{
	return &alternate_rdev.rdev;
}

void boot_device_init(void)
{
	mem_region_device_ro_init(&alternate_rdev, _cbfs_cache,
			_cbfs_cache_size);

	ufs_cbfs_open();
}
