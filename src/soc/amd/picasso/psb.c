/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <soc/psb.h>
#include <soc/amd/common/block/psp/psp_def.h>
#include <types.h>
#include <delay.h>
#include "cc6.h"

#define MSR_CU_CBBCFG		0xc00110a2
#define PSB_STATUS_OFFSET	0x10994
#define PSB_HSTI_STATUS_OFFSET	0x10998

#define FUSE_PLATFORM_SECURE_BOOT_EN	BIT(24)

#define PSB_TEST_STATUS_MASK		0xff
#define PSB_FUSING_READY_MASK		0x100
#define SPL_UPDATE_REQ_MASK		0x1000
#define SPL_TEST_STATUS_MASK		0xe000
#define SPL_TEST_STATUS_SHIFT		13

#define PSB_TEST_STATUS_FUSE_READ_ERR		0x3e
#define PSB_TEST_STATUS_BIOS_KEY_BAD_USAGE	0x81
#define PSB_TEST_STATUS_BIOS_RTM_SIG_NOENT	0x82
#define PSB_TEST_STATUS_BIOS_RTM_COPY_ERR	0x83
#define PSB_TEST_STATUS_BIOS_RTM_BAD_SIG	0x84
#define PSB_TEST_STATUS_BIOS_KEY_BAD_SIG	0x85
#define PSB_TEST_STATUS_PLATFORM_BAD_ID		0x86
#define PSB_TEST_STATUS_BIOS_COPY_BIT_UNSET	0x87
#define PSB_TEST_STATUS_BIOS_CA_BAD_SIG		0x8a
#define PSB_TEST_STATUS_BIOS_CA_BAD_USAGE	0x8b
#define PSB_TEST_STATUS_BIOS_KEY_BAD_REVISION	0x8c

static const char *psb_test_status_fuse_read_err = "error: Error reading fuse info";
static const char *psb_test_status_bios_key_bad_usage = "error: OEM BIOS signing key usage flag violation";
static const char *psb_test_status_bios_rtm_sig_noent = "error: BIOS RTM signature entry not found";
static const char *psb_test_status_bios_rtm_copy_err = "error: BIOS copy to DRAM failed";
static const char *psb_test_status_bios_rtm_bad_sig = "error: BIOS RTM signature verification failed";
static const char *psb_test_status_bios_key_bad_sig = "error: OEM BIOS signing key failed signature verification";
static const char *psb_test_status_platform_bad_id = "error: Platform vendor id and/or model id binding violation";
static const char *psb_test_status_bios_copy_bit_unset = "error: BIOS copy bit unset for reset image";
static const char *psb_test_status_bios_ca_bad_sig = "error: OEM BIOS signing CA key failed signature verification";
static const char *psb_test_status_bios_ca_bad_usage = "error: OEM BIOS signing CA key usage flag violation";
static const char *psb_test_status_bios_key_bad_revision = "error: OEM BIOS signing key revision violation";
static const char *psb_test_status_unknown = "error: Unknown failure";

static const char *psb_test_status_to_string(u32 status)
{
	switch (status) {
	case PSB_TEST_STATUS_FUSE_READ_ERR:
		return psb_test_status_fuse_read_err;
	case PSB_TEST_STATUS_BIOS_KEY_BAD_USAGE:
		return psb_test_status_bios_key_bad_usage;
	case PSB_TEST_STATUS_BIOS_RTM_SIG_NOENT:
		return psb_test_status_bios_rtm_sig_noent;
	case PSB_TEST_STATUS_BIOS_RTM_COPY_ERR:
		return psb_test_status_bios_rtm_copy_err;
	case PSB_TEST_STATUS_BIOS_RTM_BAD_SIG:
		return psb_test_status_bios_rtm_bad_sig;
	case PSB_TEST_STATUS_BIOS_KEY_BAD_SIG:
		return psb_test_status_bios_key_bad_sig;
	case PSB_TEST_STATUS_PLATFORM_BAD_ID:
		return psb_test_status_platform_bad_id;
	case PSB_TEST_STATUS_BIOS_COPY_BIT_UNSET:
		return psb_test_status_bios_copy_bit_unset;
	case PSB_TEST_STATUS_BIOS_CA_BAD_SIG:
		return psb_test_status_bios_ca_bad_sig;
	case PSB_TEST_STATUS_BIOS_CA_BAD_USAGE:
		return psb_test_status_bios_ca_bad_usage;
	case PSB_TEST_STATUS_BIOS_KEY_BAD_REVISION:
		return psb_test_status_bios_key_bad_revision;
	default:
		return psb_test_status_unknown;
	}
}

#define FUSE_STATUS_NOT_ALLOWED	0x09
#define FUSE_STATUS_FUSING_ERR	0x0a
#define FUSE_STATUS_BOOT_DONE	0x0b

static const char *fuse_status_not_allowed = "error: Fusing not allowed or already done";
static const char *fuse_status_fusing_err = "error: Fuse programming failed";
static const char *fuse_status_boot_done = "error: Issued after BOOT DONE";
static const char *fuse_status_unknown = "error: Unknown failure";

static const char *fuse_status_to_string(u32 status)
{
	switch (status) {
	case FUSE_STATUS_NOT_ALLOWED:
		return fuse_status_not_allowed;
	case FUSE_STATUS_FUSING_ERR:
		return fuse_status_fusing_err;
	case FUSE_STATUS_BOOT_DONE:
		return fuse_status_boot_done;
	default:
		return fuse_status_unknown;
	}
}

#define SPL_TEST_STATUS_FUSE_ERR	0x1
#define SPL_TEST_STATUS_ENTRY_ERR	0x2
#define SPL_TEST_STATUS_TABLE_ERR	0x4

static void print_spl_test_status(u32 status)
{
	if (status & SPL_TEST_STATUS_TABLE_ERR)
		printk(BIOS_ERR, "PSB: error: SPL table not found\n");
	if (status & SPL_TEST_STATUS_ENTRY_ERR)
		printk(BIOS_ERR, "PSB: error: Invalid SPL table entry\n");
	if (status & SPL_TEST_STATUS_FUSE_ERR)
		printk(BIOS_ERR, "PSB: error: Invalid SPL table version\n");
}

static int get_psb_status(uint32_t *status)
{
	uintptr_t psp_mmio;

	psp_mmio = rdmsr(MSR_CU_CBBCFG).lo;
	if (psp_mmio == 0xffffffff) {
		printk(BIOS_WARNING, "PSB: MSR_CU_CBBCFG uninitialized\n");
		return -1;
	}

	*status = read32((void *)(psp_mmio + PSB_STATUS_OFFSET));
	return 0;
}

int psb_get_hsti_status(uint32_t *status)
{
	uintptr_t psp_mmio;

	psp_mmio = rdmsr(MSR_CU_CBBCFG).lo;
	if (psp_mmio == 0xffffffff) {
		printk(BIOS_WARNING, "PSB: MSR_CU_CBBCFG uninitialized\n");
		return -1;
	}

	*status = read32((void *)(psp_mmio + PSB_HSTI_STATUS_OFFSET));
	return 0;
}

static int update_psp_anti_rollback(u32 status)
{
	u32 spl_test_status;
	int cmd_status;
	u32 fuse_status;
	struct mbox_default_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		}
	};

	spl_test_status = status & SPL_TEST_STATUS_MASK;
	spl_test_status >>= SPL_TEST_STATUS_SHIFT;

	if (spl_test_status) {
		print_spl_test_status(spl_test_status);
		return -1;
	}

	if (!(status & SPL_UPDATE_REQ_MASK)) {
		printk(BIOS_DEBUG, "PSB: SPL fuse update not required\n");
		return 0;
	}

	printk(BIOS_DEBUG, "PSB: SPL fuse... ");

	cc6_disable();
	/* Give cores in CC6 a chance to exit */
	mdelay(100);

	cmd_status = send_psp_command(MBOX_BIOS_CMD_SPL_FUSE, &buffer);

	cc6_enable();

	psp_print_cmd_status(cmd_status, &buffer.header);

	fuse_status = read32(&buffer.header.status);

	if (fuse_status) {
		printk(BIOS_ERR, "PSB: Failed to program SPL fuse: %u\n",
			fuse_status);
		return -1;
	}

	return 1;
}

int psb_update_anti_rollback(void)
{
	u32 status;

	if (psb_get_hsti_status(&status)) {
		printk(BIOS_ERR, "PSB: Failed to read HSTI status\n");
		return -1;
	}

	printk(BIOS_INFO, "PSB: HSTI = %x\n", status);

	return update_psp_anti_rollback(status);
}

/*
 * Request Platform Secure Boot enablement via the PSP if it is not already
 * enabled.  Upon receiving this command, the PSP will program all PSB fuses
 * so long as the BIOS signing key token is valid.
 */
int psb_enable(void)
{
	u32 status;
	u32 psb_test_status;
	int cmd_status;
	u32 fuse_status;
	struct mbox_default_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		}
	};

	if (get_psb_status(&status)) {
		printk(BIOS_ERR, "PSB: Failed to read PSB status\n");
		return -1;
	}

	printk(BIOS_INFO, "PSB: Status = %x\n", status);

	if (status & FUSE_PLATFORM_SECURE_BOOT_EN) {
		printk(BIOS_DEBUG, "PSB: Already enabled\n");
		return 0;
	}

	if (psb_get_hsti_status(&status)) {
		printk(BIOS_ERR, "PSB: Failed to read HSTI status\n");
		return -1;
	}

	printk(BIOS_INFO, "PSB: HSTI = %x\n", status);

	psb_test_status = status & PSB_TEST_STATUS_MASK;

	if (psb_test_status) {
		printk(BIOS_ERR, "PSB: %s\n",
			psb_test_status_to_string(psb_test_status));
		return -1;
	}

	if (!(status & PSB_FUSING_READY_MASK)) {
		printk(BIOS_ERR, "PSB: Fusing not allowed\n");
		return -1;
	}

	printk(BIOS_DEBUG, "PSB: Enable... ");

	cc6_disable();
	/* Give cores in CC6 a chance to exit */
	mdelay(100);

	cmd_status = send_psp_command(MBOX_BIOS_CMD_PSB_AUTO_FUSING, &buffer);

	cc6_enable();

	psp_print_cmd_status(cmd_status, &buffer.header);

	if (cmd_status) {
		printk(BIOS_ERR, "PSB: Fusing request failed: %d\n", cmd_status);
		return -1;
	}

	fuse_status = read32(&buffer.header.status);
	if (fuse_status) {
		printk(BIOS_ERR, "PSB: %s\n",
			fuse_status_to_string(fuse_status));
		return -1;
	}

	return 1;
}
