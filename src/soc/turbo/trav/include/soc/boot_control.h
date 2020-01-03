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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <types.h>

#ifndef _SOC_TURBO_TRAV_BOOT_CONTROL_H_
#define _SOC_TURBO_TRAV_BOOT_CONTROL_H_

int trav_board_id(void);
int trav_cpu_rev(void);
int trav_bootx_need_reset(void);
int trav_from_a72_warm_reset(void);
void set_ap_healthy(int val);
void a72wdt_gpio_conf(void);
int is_boot_from_qspi(void);
void check_a72bl1_versions(void);
void check_a72bootblock_versions(void);
bool is_trav_board_hw31(void);

struct boot_env {
	/*
	 * The first few fields of this structure represent the boot_control state.
	 * SCS set some fields and a72 will probe others
	 * and are passed to the BL2 of coreboot.
	 */
	union {
		struct {
			char signature[16]; /* BOOTTURBOTRAVENV */	/* NOT USED */
			unsigned char boot_bank;
			unsigned char tried_num[2];			/* NOT USED */
			unsigned char successful[2];			/* NOT USED */

			char boot_from[2]; /* r/u, a/b */

			char rootfs_part_uuid[37];
			char turbo_a_b[1];				/* scsbl2 filled */
			char otp_chip_id[33];				/* scsbl2 filled */
			char mac_addr[8];				/* scsbl2 filled partial */
			unsigned char board_id_x;			/* a72 probe via gpio */
			char board_pcba_version[13];			/* scsbl2 filled */
			char scsbl1_build_version[9];			/* NOT USED */
			char scsbl1_build_version_string[65];		/* NOT USED */
			char scsbl1_build_git_hash[41];			/* scsbl2 filled */
			char scsbl2_build_version[9];			/* NOT USED */
			char scsbl2_build_version_string[65];		/* NOT USED */
			char scsbl2_build_git_hash[41];			/* scsbl2 filled */
			char sms_build_version[9];			/* NOT USED */
			char sms_build_git_hash[41];			/* A72 get from SMS via mailbox */
			char a72bl1_build_version[9];
			char a72bootblock_build_version[9];
			char a72coreboot_build_version[9];
			unsigned char cpu_rev;
			char a72bl1_coreboot_version[32];
			char a72bootblock_coreboot_version[32];
			char soc_id[12];				/* scsbl2 filled */
			unsigned char a72_warm_reset;			/* scsbl2 filled */
			char ddr_vendor[8];
			char pad_1[2];
			unsigned char hw_minor;				/* scsbl2 filled */
			uint32_t bootx_bl1_data_ver;			/* scsbl2 filled */
			uint32_t bootx_booted_bank;			/* scsbl2 filled */
			uint32_t bootx_status;				/* scsbl2 filled */
			uint32_t bootx_start_count[2];			/* scsbl2 filled */
			uint32_t bootx_failed_count[2];			/* scsbl2_filled */
			uint32_t bootx_bank_data;			/* scsbl2_filled */
			uint32_t bootx_bank_data_offset;		/* scsbl2_filled */
		};
		unsigned char pad[4096-2*4];
	};
	/*
	 * The following ssq_size field is just here for info purposes only.  Coreboot
	 * does not use this field. It is here to say that at this offset the updater will
	 * store the ssq size in the UFS bootenv partition.
	 */
	uint32_t image_size[2]; /* size of ssq a/b */	/* persistent on ufs partition */
} __attribute__((packed));

struct offset_size {
	uint32_t offset;
	uint32_t size;
} __attribute__((packed));

struct boot_file_layout {
	char signature[16]; /* BOOT_FILE_LAYOUT */
	unsigned char ver;
	unsigned char reserved_1[15];
	struct offset_size pos[4];
	unsigned char pad[4096-16-1-15-4*8];
} __attribute__((packed));

int check_boot_file_layout(struct boot_file_layout *file_layout);

#define BOOT_TRY_NUM 3

#endif /* _SOC_TURBO_TRAV_BOOT_CONTROL_H_ */
