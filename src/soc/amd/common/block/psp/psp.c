/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <bootstate.h>
#include <console/console.h>
#include <amdblocks/psp.h>
#include <soc/iomap.h>
#include "psp_def.h"

static const char *psp_status_nobase = "error: PSP BAR3 not assigned";
static const char *psp_status_halted = "error: PSP in halted state";
static const char *psp_status_recovery = "error: PSP recovery required";
static const char *psp_status_errcmd = "error sending command";
static const char *psp_status_init_timeout = "error: PSP init timeout";
static const char *psp_status_cmd_timeout = "error: PSP command timeout";
static const char *psp_status_noerror = "";

static const char *status_to_string(int err)
{
	switch (err) {
	case -PSPSTS_NOBASE:
		return psp_status_nobase;
	case -PSPSTS_HALTED:
		return psp_status_halted;
	case -PSPSTS_RECOVERY:
		return psp_status_recovery;
	case -PSPSTS_SEND_ERROR:
		return psp_status_errcmd;
	case -PSPSTS_INIT_TIMEOUT:
		return psp_status_init_timeout;
	case -PSPSTS_CMD_TIMEOUT:
		return psp_status_cmd_timeout;
	default:
		return psp_status_noerror;
	}
}

static u32 rd_resp_sts(struct mbox_buffer_header *header)
{
	return read32(&header->status);
}

/*
 * Print meaningful status to the console.  Caller only passes a pointer to a
 * buffer header if it's expected to contain its own status.
 */
void psp_print_cmd_status(int cmd_status, struct mbox_buffer_header *header)
{
	if (header && rd_resp_sts(header))
		printk(BIOS_DEBUG, "buffer status=0x%x ", rd_resp_sts(header));

	if (cmd_status)
		printk(BIOS_DEBUG, "%s\n", status_to_string(cmd_status));
	else
		printk(BIOS_DEBUG, "OK\n");
}

/*
 * Notify the PSP that DRAM is present.  Upon receiving this command, the PSP
 * will load its OS into fenced DRAM that is not accessible to the x86 cores.
 */
int psp_notify_dram(void)
{
	int cmd_status;
	struct mbox_default_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		}
	};

	printk(BIOS_DEBUG, "PSP: Notify that DRAM is available... ");

	cmd_status = send_psp_command(MBOX_BIOS_CMD_DRAM_INFO, &buffer);

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &buffer.header);

	return cmd_status;
}

/*
 * Query the PSP capabilities.  Upon receiving this command, the PSP will
 * update the capability field in the command/response buffer.
 */
int psp_get_caps(u32 *caps)
{
	int cmd_status;
	struct mbox_cmd_psp_query_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		},
	};

	printk(BIOS_DEBUG, "PSP: Querying capabilities... ");

	cmd_status = send_psp_command(MBOX_BIOS_CMD_PSP_QUERY, &buffer);

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &buffer.header);

	if (!cmd_status)
		*caps = read32(&buffer.req.capabilities);

	return cmd_status;
}

/*
 * Query the PSP capabilities (version 2).  Upon receiving this command, the
 * PSP will update the capability field in the command/response buffer.
 */
int psp_get_caps_v2(u32 *caps)
{
	int cmd_status;
	struct mbox_cmd_psp_query_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		},
	};

	printk(BIOS_DEBUG, "PSP: Querying capabilities v2... ");

	cmd_status = send_psp_command(MBOX_BIOS_CMD_PSP_V2_QUERY, &buffer);

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &buffer.header);

	if (!cmd_status)
		*caps = read32(&buffer.req.capabilities);

	return cmd_status;
}

/*
 * Query the HSTI state.  Upon receiving this command, the PSP will
 * update the state field in the command/response buffer.
 */
int psp_get_hsti_state(u32 *state)
{
	int cmd_status;
	struct mbox_cmd_hsti_query_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		},
	};

	printk(BIOS_DEBUG, "PSP: Querying HSTI state... ");

	cmd_status = send_psp_command(MBOX_BIOS_CMD_HSTI_QUERY, &buffer);

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &buffer.header);

	if (!cmd_status)
		*state = read32(&buffer.req.state);

	return cmd_status;
}

/*
 * Query the PSB fuse configuration.  Upon receiving this command, the PSP
 * will update the config structure in the command/response buffer.
 */
int psp_get_psb_fuse_config(struct psb_fuse_config *cfg)
{
	int cmd_status;
	struct mbox_cmd_query_psb_fuse_config_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		},
	};

	printk(BIOS_DEBUG, "PSP: Querying PSB fuse config... ");

	cmd_status = send_psp_command(MBOX_BIOS_CMD_QUERY_PSB_FUSE_CONFIG, &buffer);

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &buffer.header);

	if (!cmd_status)
		*cfg = buffer.config;

	return cmd_status;
}

/*
 * Query the OEM fuse state.  Upon receiving this command, the PSP will
 * update the state field in the command/response buffer.
 */
int psp_get_oem_state(u8 *state)
{
	int cmd_status;
	struct mbox_cmd_get_oem_state_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		},
	};

	printk(BIOS_DEBUG, "PSP: Querying OEM state... ");

	cmd_status = send_psp_command(MBOX_BIOS_CMD_GET_CUSTOMER_DEVICE_STATE, &buffer);

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &buffer.header);

	if (!cmd_status)
		*state = read8(&buffer.req.state);

	return cmd_status;
}

/*
 * Notify the PSP that the system is completing the boot process.  Upon
 * receiving this command, the PSP will only honor commands where the buffer
 * is in SMM space.
 */
static void psp_notify_boot_done(void *unused)
{
	int cmd_status;
	struct mbox_default_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		}
	};

	printk(BIOS_DEBUG, "PSP: Notify that POST is finishing... ");

	cmd_status = send_psp_command(MBOX_BIOS_CMD_BOOT_DONE, &buffer);

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &buffer.header);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY,
		psp_notify_boot_done, NULL);
