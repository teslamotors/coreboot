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

#include <console/console.h>
#include <delay.h>
#include <symbols.h>
#include <string.h>

#include <soc/MBOX.h>
#include <soc/mailbox_A72_SMS_interface.h>
#include <soc/sms_helpers.h>
#include <soc/scs_helpers.h>
#include <soc/boot_control.h>

/* to SMS */
    #define A72_MAILBOX                 (0x10090000)
    #define A72_MAILBOX_TO_A72_INT      (A72_MAILBOX + 0x0008)
    #define A72_MAILBOX_TO_SMS_INT      (A72_MAILBOX + 0x001C)
    #define A72_MAILBOX_TO_A72_ISSR_LO  (A72_MAILBOX + 0x0080)
    #define A72_MAILBOX_TO_SMS_ISSR_LO  (A72_MAILBOX + 0x00C0)
    #define A72_MAILBOX_TO_A72_ISSR_HI  (A72_MAILBOX + 0x0100)
    #define A72_MAILBOX_TO_SMS_ISSR_HI  (A72_MAILBOX + 0x0140)
    #define A72_MAILBOX_TO_A72_SEM_LO   (A72_MAILBOX + 0x0180)
    #define A72_MAILBOX_TO_SMS_SEM_LO   (A72_MAILBOX + 0x018C)
    #define A72_MAILBOX_TO_A72_SEM_HI   (A72_MAILBOX + 0x0198)
    #define A72_MAILBOX_TO_SMS_SEM_HI   (A72_MAILBOX + 0x01A4)

#define USE_HIGH_PRIORITY_MAILBOX 1

static struct trav_mbox sms_mbox;

#if !IS_ENABLED(CONFIG_BOOTBLOCK_X) && ENV_BOOTBLOCK
static void a72_notify_sms_ap_booted(void)
{
	int ret;

	/* notify SMS that A72 is ready for handling request */
	ret = test_op_nn_with_nn(&sms_mbox, MAILBOX_SERVICE_ID_BOOT,
				 BOOT_SERVICE_CMD_SET_AP_BOOTED, NULL, 0, NULL, 0);

	if (0 != ret) {
		printk(BIOS_INFO, "Can not notify SMS that A72 is booted\n");
		return;
	}
}
#endif

static int a72_handle_sms_request_nowait(struct trav_mbox *mbox, uint32_t recv_service_id, uint32_t cmd)
{
	return 0;
}

int test_op_nn_with_nn_async(struct trav_mbox *mbox, uint32_t service_id,
			     uint32_t cmd, uint32_t *cmd_arg, int nn,
			     uint32_t resp, uint32_t *return_arg, int return_nn)
{
	int rv;
	uint32_t request_args[NN_MAX + 1];
	uint32_t response_status;
	uint32_t n;

	if (nn > NN_MAX || nn < 0)
		return -1;

	if (nn > 0 && !cmd_arg)
		return -1;

	if (return_nn > RSP_NN_MAX || return_nn < 0)
		return -1;

	if (return_nn > 0 && !return_arg)
		return -1;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// request SMS to set
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	// send the service request
	request_id_counter++;
	request_args[0] = cmd;               // service command id
	for (n = 0; n < nn; n++)
		request_args[1 + n] = cmd_arg[n];
	response_status = 0;
	rv = mailbox_send_request(mbox, service_id, request_id_counter, nn + 1, request_args, &response_status, 0, NULL);
	if (0 != rv || SMS_MAILBOX_STATUS_REQUEST_PENDING != response_status) {
		printk(BIOS_INFO, "request(0x%08x) failed status 0x%08x error 0x%08x\n", request_id_counter, response_status, rv);
		return -1;
	}

	// wait for the asynchronous response
	rv = mailbox_wait_for_response(mbox, request_id_counter, &response_status, resp, return_arg, return_nn,
					a72_handle_sms_request_nowait);
	if (0 != rv) {
		printk(BIOS_INFO, "response(0x%08x) failed!\n", request_id_counter);
		return -1;
	}
	if (0 != response_status) {
		printk(BIOS_INFO, "response(0x%08x) status 0x%08x\n", request_id_counter, response_status);
		return -1;
	}

	return 0;
}

#if !IS_ENABLED(CONFIG_BOOTBLOCK_X) && ENV_BOOTBLOCK

#undef SMS_PRINT_POWER_VOLTAGE

#ifdef  SMS_PRINT_POWER_VOLTAGE
static uint32_t l16_uv(uint32_t val)
{
       int i = 6;

       while (i > 0 && !(val & 0xf0000000)) {
               val *= 10;
               i--;
       }

       val >>= 12;

       while (i > 0) {
               val *= 10;
               i--;
       }

       return val;
}

static int test_get_all_power_voltage(struct trav_mbox *mbox)
{
	int ret = -1;
	uint32_t i;
	uint32_t val;

	for (i = 1; i < 0x0d; i++) {
		ret = test_op_nn_with_nn_async(mbox, MAILBOX_SERVICE_ID_PMIC, PMIC_SERVICE_CMD_POWER_GET, &i, 1, PMIC_SERVICE_RSP_POWER_GET, &val, 1);
		if (0 != ret)
			continue;

		printk(BIOS_INFO, "POWER %d: %08x\n", i, val);
	}

	for (i = 1; i < 6; i++) {
		ret = test_op_nn_with_nn_async(mbox, MAILBOX_SERVICE_ID_PMIC, PMIC_SERVICE_CMD_VOLTAGE_GET, &i, 1, PMIC_SERVICE_RSP_VOLTAGE_GET, &val, 1);
		if (0 != ret)
			continue;

		printk(BIOS_INFO, "VOLTAGE %d: %08x aka %10d uV\n", i, val, l16_uv(val));
	}
	return 0;
}

#else

static int test_get_all_power_voltage(struct trav_mbox *mbox)
{
	return 0;
}

#endif

void check_sms_versions(void)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	check_build_git_hash(&sms_mbox, "SMS", BOOT_SERVICE_CMD_GET_BUILD_GIT_HASH, env->sms_build_git_hash);
}
#endif

static int sms_service_inited;

void sms_service_init(void)
{
	struct trav_mbox *mbox = &sms_mbox;
	struct mbox *mbox_regs = (struct mbox *)A72_MAILBOX;

#ifndef USE_HIGH_PRIORITY_MAILBOX
	mbox->issr_tx = &(mbox_regs->issr_lo1);
	mbox->issr_rx = &(mbox_regs->issr_lo0);
	mbox->pri = 0;
#else
	mbox->issr_tx = &(mbox_regs->issr_hi1);
	mbox->issr_rx = &(mbox_regs->issr_hi0);
	mbox->pri = 1;
#endif

	mbox->mbox_int_rx = (struct mbox_int *)A72_MAILBOX_TO_A72_INT;
	mbox->mbox_int_tx = (struct mbox_int *)A72_MAILBOX_TO_SMS_INT;

#if !IS_ENABLED(CONFIG_BOOTBLOCK_X) && ENV_BOOTBLOCK
	printk(BIOS_INFO, "SMS issr_tx=0x%08lx\n", (unsigned long) mbox->issr_tx);
	printk(BIOS_INFO, "SMS issr_rx=0x%08lx\n", (unsigned long) mbox->issr_rx);
#endif

	sms_service_inited = 1;

#if !IS_ENABLED(CONFIG_BOOTBLOCK_X) && ENV_BOOTBLOCK
	set_ap_healthy(1);
	a72_notify_sms_ap_booted();

	if (trav_from_a72_warm_reset() == 0) {
		test_get_all_power_voltage(mbox);
	}
#endif
}

void sms_reset(int reset_mode)
{
	uint32_t cmd = (reset_mode != RESET_MODE_COLD) ? PMIC_SERVICE_CMD_A72_RESET : PMIC_SERVICE_CMD_PMIC_RESET;
	uint32_t rsp = (reset_mode != RESET_MODE_COLD) ? PMIC_SERVICE_RSP_A72_RESET : PMIC_SERVICE_RSP_PMIC_RESET;

	if (sms_service_inited != 1)
		return;

	if (reset_mode != RESET_MODE_COLD)
		slog_log_message("coreboot: a72 reset issued", 26);
	else
		slog_log_message("coreboot: pmic reset issued", 27);

	test_op_nn_with_nn_async(&sms_mbox, MAILBOX_SERVICE_ID_PMIC, cmd, NULL, 0, rsp, NULL, 0);
}

