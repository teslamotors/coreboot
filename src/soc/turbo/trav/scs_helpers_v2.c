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

//#include <arch/cache.h>
//#include <arch/cache_helpers.h>
#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <symbols.h>
#include <string.h>

#include <soc/MBOX.h>
#include <soc/mailbox_A72_SCS_interface.h>
#include <soc/scs_helpers.h>
#include <soc/sms_helpers.h>
#include <soc/boot_control.h>
#include <soc/power.h>

static struct trav_mbox scs_mbox;

#if IS_ENABLED(CONFIG_BOOTBLOCK_X)
uint32_t request_id_counter = 0;
#elif ENV_BOOTBLOCK
uint32_t request_id_counter = 0x100;
#elif ENV_ROMSTAGE
uint32_t request_id_counter = 0x200;
#else
uint32_t request_id_counter = 0x300;
#endif

int mailbox_send_request(struct trav_mbox *mbox, uint32_t service_id, uint32_t request_id, uint32_t argc,
			 const uint32_t * args, uint32_t * response_status, uint32_t nn, uint32_t * return_value)
{
	uint32_t n;
	int timeout;

	// copy the arguments
	for (n = 0; n < argc; n++)
		write32(&mbox->issr_tx->issr[ISSR_INDEX_DATA0 + n], args[n]);

	// set the request ID
	write32(&mbox->issr_tx->issr[ISSR_INDEX_REQUEST], request_id);

	// set the command - this will trigger the operation in polling mode
	write32(&mbox->issr_tx->issr[ISSR_INDEX_SERVICE], service_id);

	// send an interrupt - this will trigger the operation in interrupt mode
	write32(&mbox->mbox_int_tx->intgr, 1U<<(mbox->pri?16:0));

	// wait 3s for the other side to acknowledge the request
	timeout = 3000;
	while (timeout--) {
		mdelay(1); /* */

		if (0 != read32(&mbox->issr_tx->issr[ISSR_INDEX_SERVICE]))
			continue;

		if (NULL != response_status)
			*response_status = read32(&mbox->issr_tx->issr[ISSR_INDEX_STATUS]);
		if (NULL != return_value) {
			for (n = 0; n < nn; n++)
				return_value[n] = read32(&mbox->issr_tx->issr[ISSR_INDEX_DATA1 + n]);
		}

		write32(&mbox->mbox_int_rx->intcr, 1U<<(mbox->pri?31:15));

		return 0; // send request succeeded
	}

	// we timed out while waiting for the SCS to acknowlege the receipt of the request
	return -1;
}

#define COPY_QSPI_TIMEOUT      10000
#define COPY_QSPI_MDELAY       1
int mailbox_wait_for_response(struct trav_mbox *mbox,
			      uint32_t request_id, uint32_t * response_status,
			      uint32_t expected_response_code, uint32_t * return_value, uint32_t return_nn,
			      int (*mailbox_handle_request_fn)(struct trav_mbox *, uint32_t, uint32_t))
{
	uint32_t recv_service_id = 0, recv_request_id;
	uint32_t response_code;
	int timeout;
	int rv = 0;
	uint32_t n;

wait_again:
	/* wait 10s if it is not ready*/
	timeout = COPY_QSPI_TIMEOUT;
	while (timeout--) {
		mdelay(COPY_QSPI_MDELAY); /* */
		recv_service_id = read32(&mbox->issr_rx->issr[ISSR_INDEX_SERVICE]);
		if (recv_service_id)
			break;
	}

	if (0 == recv_service_id) {
		printk(BIOS_INFO, "response: timeout! 0x%08x\n", request_id);
		return -1;
	}

	response_code = read32(&mbox->issr_rx->issr[ISSR_INDEX_DATA0]);
	if (0 == (MBOX_COMMAND_RESPONSE_ID & response_code) && mailbox_handle_request_fn) {
		mailbox_handle_request_fn(mbox, recv_service_id, response_code);
		goto wait_again;
	}

	recv_request_id = read32(&mbox->issr_rx->issr[ISSR_INDEX_REQUEST]);

//	printk(BIOS_INFO, "MBRX: 0x%08x, 0x%08x, [0x%08x, 0x%08x]\n", recv_service_id, recv_request_id, issr_rx->issr[0], issr_rx->issr[1]);

	if (request_id != recv_request_id) {
		printk(BIOS_INFO, "response: request_id mismatch! 0x%08x, 0x%08x\n",
				request_id, recv_request_id);
		rv = -1;
		goto done;
	}

	if (expected_response_code != response_code) {
		printk(BIOS_INFO, "response: response_code mismatch! 0x%08x, 0x%08x\n",
				expected_response_code, response_code);
		rv = -1;
		goto done;
	}

	if (NULL != response_status)
		*response_status = read32(&mbox->issr_rx->issr[ISSR_INDEX_DATA1]);

	if (NULL != return_value) {
		for (n = 0; n < return_nn; n++)
			return_value[n] = read32(&mbox->issr_rx->issr[ISSR_INDEX_DATA2 + n]);
	}

done:
	write32(&mbox->mbox_int_rx->intcr, 1U<<(mbox->pri?16:0));

	write32(&mbox->issr_rx->issr[ISSR_INDEX_SERVICE], 0);

	write32(&mbox->mbox_int_tx->intgr, 1U<<(mbox->pri?31:15));

	//printk(BIOS_INFO, "mailbox_wait_for_response: tx intgr %08x intmr: %08x intcr: %08x intsr: %08x\n", read32(&mbox->mbox_int_tx->intgr), read32(&mbox->mbox_int_tx->intmr), read32(&mbox->mbox_int_tx->intcr), read32(&mbox->mbox_int_tx->intsr));

	return rv;
}

int test_op_nn_with_nn(struct trav_mbox *mbox, uint32_t service_id,
				 uint32_t cmd, uint32_t *cmd_arg, int nn,
				 uint32_t *return_arg, int return_nn)
{
	int rv;
	uint32_t request_args[NN_MAX + 1];
	uint32_t response_status;
	uint32_t n;

	if (nn > NN_MAX || nn < 0)
	       return -1;

	if (nn > 0 && !cmd_arg)
		return -1;

	if (return_nn > NN_MAX || return_nn < 0)
               return -1;

	if (return_nn > 0 && !return_arg)
               return -1;

	/* request from SCS ...*/
	request_id_counter++;
	request_args[0] = cmd;
	for (n = 0; n < nn; n++)
		request_args[1 + n] = cmd_arg[n];
	response_status = 0;
	rv = mailbox_send_request(mbox, service_id, request_id_counter, nn + 1, request_args,
					 &response_status, return_nn, return_arg);
	if (0 != rv) {
		printk(BIOS_INFO, "request(0x%08x) failed error 0x%08x\n", request_id_counter, rv);
		return -1;
	}
	if (A72_MAILBOX_STATUS_OK != response_status) {
		printk(BIOS_INFO, "request(0x%08x) failed status 0x%08x\n",
					 request_id_counter, response_status);
		return -1;
	}

	return 0;
}

static void memcpy_io(unsigned char *to, const unsigned char *from, int count)
{
	register unsigned char data;

	while (count > 0) {
		count--;
		data = *from++;
		*to++ = data;
	}
}

static void memset_io(unsigned char *to, unsigned char data, int count)
{
	while (count > 0) {
		count--;
		*to++ = data;
	}
}

int slog_log_message(const char *data, size_t size)
{
	int ret;
	uint32_t val[LOG_MSG_LENGTH_IN_32BIT];
	size_t dest_size = sizeof(uint32_t) * LOG_MSG_LENGTH_IN_32BIT;

	if (size > dest_size)
		size = dest_size;

	memcpy_io((unsigned char *)val, (unsigned char *)data, size);
	if (size < dest_size)
		memset_io((unsigned char *)val + size, ' ', dest_size - size);

	ret = test_op_nn_with_nn_async(&scs_mbox, MAILBOX_SERVICE_ID_SLOG,
                        SLOG_SERVICE_CMD_LOG_MESSAGE, val, LOG_MSG_LENGTH_IN_32BIT,
                        SLOG_SERVICE_RSP_LOG_MESSAGE, NULL, 0);

       return ret;
}

#if ENV_BOOTBLOCK
/* x: 1 or 2 file number */
static int test_op_file(struct trav_mbox *mbox,
			      uint32_t file_service_cmd_load, uint32_t file_service_rsp_load,
			      uint32_t file_id, uint8_t *buffer, uint32_t buffer_size,
			      int x, uint8_t *buffer_crt, uint32_t buffer_crt_size)
{
	int rv;
	uint32_t request_args[16];
	uint32_t response_status;

	// send the FILE_SERVICE_CMD_LOAD_COREBOOT request
	request_id_counter++;
	request_args[0] = file_service_cmd_load;
	request_args[1] = file_id;                                  // file id
	request_args[2] = (uint32_t)((uint64_t)buffer);             // load address lo
	request_args[3] = (uint32_t)(((uint64_t)buffer) >> 32lu);   // load address hi - in the current version of SCS this should be zero
	request_args[4] = buffer_size;                           // maximum size of the load buffer
	if (x == 2) {
		request_args[5] = (uint32_t)((uint64_t)buffer_crt);             // load address lo
		request_args[6] = (uint32_t)(((uint64_t)buffer_crt) >> 32lu);   // load address hi - in the current version of SCS this should be zero
		request_args[7] = buffer_crt_size;                           // maximum size of the load buffer
	}
	response_status = 0;
	rv = mailbox_send_request(mbox, MAILBOX_SERVICE_ID_FILE, request_id_counter,
				 2 + x*3, request_args, &response_status, 0, NULL);
	if (0 != rv || A72_MAILBOX_STATUS_REQUEST_PENDING != response_status) {
		printk(BIOS_INFO, "request(0x%08x) failed status 0x%08x error 0x%08x\n",
			 request_id_counter, response_status, rv);
		return -1;
	}

	// wait for the asynchronous response
	rv = mailbox_wait_for_response(mbox, request_id_counter, &response_status,
					 file_service_rsp_load, NULL, 0, NULL);
	if (0 != rv) {
		printk(BIOS_INFO, "for_response(0x%08x) failed!\n",
				 request_id_counter);
		return -1;
	}

	if (0 != response_status) {
		printk(BIOS_INFO, "for_response(0x%08x) status 0x%08x\n",
			 request_id_counter, response_status);
		return -1;
	}

	return 0;
}

static int test_load_file(struct trav_mbox *mbox, uint32_t file_id,
			  uint8_t *buffer, uint32_t buffer_size, uint32_t *pfile_size, int raw)
{
	uint32_t file_size;
	uint32_t file_service_cmd_get_size;
	uint32_t file_service_cmd_load;
	uint32_t file_service_rsp_load;

	file_service_cmd_get_size = FILE_SERVICE_CMD_GET_FILE_SIZE;
	file_service_cmd_load = FILE_SERVICE_CMD_GET_FILE;    // file service command id
	file_service_rsp_load = FILE_SERVICE_RSP_GET_FILE;

	if (!raw)
	switch (file_id) {
	case FILE_SERVICE_FILE_A72_BOOTBLOCK_A_ID:
	case FILE_SERVICE_FILE_A72_BOOTBLOCK_B_ID:
		file_service_cmd_get_size = FILE_SERVICE_CMD_GET_BOOTBLOCK_SIZE;
		file_service_cmd_load = FILE_SERVICE_CMD_LOAD_BOOTBLOCK;    // file service command id
		file_service_rsp_load = FILE_SERVICE_RSP_LOAD_BOOTBLOCK;
		break;
	case FILE_SERVICE_FILE_A72_COREBOOT_A_ID:
	case FILE_SERVICE_FILE_A72_COREBOOT_B_ID:
		file_service_cmd_get_size = FILE_SERVICE_CMD_GET_COREBOOT_SIZE;
		file_service_cmd_load = FILE_SERVICE_CMD_LOAD_COREBOOT;     // file service command id
		file_service_rsp_load = FILE_SERVICE_RSP_LOAD_COREBOOT;
		break;
	}

	/* request from SCS and print the A72 coreboot_a file size */
	if (0 != test_op_nn_with_nn(mbox, MAILBOX_SERVICE_ID_FILE, file_service_cmd_get_size,
					 &file_id, 1, &file_size, 1))
		return -1;

#if !IS_ENABLED(CONFIG_BOOTBLOCK_X)
	printk(BIOS_INFO, "%d file size: %08x\n", file_id, file_size);
#endif

	if (file_size > buffer_size) {
		printk(BIOS_INFO, "buffer_size is too small : %x < %x\n", buffer_size, file_size);
		return -1;
	}

	/* request SCS to load the A72 coreboot file into A72 SRAM */
	if (0 != test_op_file(mbox, file_service_cmd_load, file_service_rsp_load, file_id,
				buffer, buffer_size, 1, NULL, 0))
		return -1;

	if (pfile_size)
		*pfile_size = file_size;

	return 0;
}
#endif

static int test_get_all_chip_id(struct trav_mbox *mbox)
{
#if !IS_ENABLED(CONFIG_BOOTBLOCK_X) && ENV_BOOTBLOCK
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	printk(BIOS_INFO, "CHIP_ID: %s\n", env->otp_chip_id);
#endif
	return 0;
}

static int scs_file_service_inited;

void scs_service_init(void)
{
	struct trav_mbox *mbox = &scs_mbox;
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
	mbox->mbox_int_tx = (struct mbox_int *)A72_MAILBOX_TO_SCS_INT;

#if !IS_ENABLED(CONFIG_BOOTBLOCK_X) && ENV_BOOTBLOCK
	printk(BIOS_INFO, "issr_tx=0x%08lx\n", (unsigned long) mbox->issr_tx);
	printk(BIOS_INFO, "issr_rx=0x%08lx\n", (unsigned long) mbox->issr_rx);
#endif

	scs_file_service_inited = 1;

	test_get_all_chip_id(mbox);
}

#if ENV_BOOTBLOCK
static int copy_from_qspi(size_t buffer, uint32_t file_id, size_t in_size, int reset_mode)
{
	int ret = -1;

	if (scs_file_service_inited != 1)
		return -1;

#if !IS_ENABLED(CONFIG_BOOTBLOCK_X)
	printk(BIOS_INFO, "copy_from_qspi: %02x [0,%x] to [%x, %x] starting...\n",
		 (unsigned int)file_id, (unsigned int)(in_size - 1),
		 (unsigned int)buffer, (unsigned int)(buffer + in_size - 1) );
#endif

	ret = test_load_file(&scs_mbox, file_id, (uint8_t *) buffer, (uint32_t) in_size, NULL, 0);

	printk(BIOS_INFO, "copy_from_qspi: %02x [0,%x] to [%x, %x] Done wth status=%d\n",
		 (unsigned int)file_id, (unsigned int)(in_size - 1),
		 (unsigned int)buffer, (unsigned int)(buffer + in_size - 1),
		 ret);

	if (0 != ret) {
		sms_reset(reset_mode);
	}

	return ret;
}

int verify_coreboot(int index, size_t buffer, size_t in_size, size_t buffer_crt, size_t in_size_crt)
{
	int ret;
	uint32_t file_id;

	switch (index) {
	case VERIFY_BOOTBLOCK_A:
		file_id = FILE_SERVICE_FILE_A72_BOOTBLOCK_A_ID;
		break;
	case VERIFY_BOOTBLOCK_B:
		file_id = FILE_SERVICE_FILE_A72_BOOTBLOCK_B_ID;
		break;
	case VERIFY_COREBOOT_A:
		file_id = FILE_SERVICE_FILE_A72_COREBOOT_A_ID;
		break;
	case VERIFY_COREBOOT_B:
		file_id = FILE_SERVICE_FILE_A72_COREBOOT_B_ID;
		break;
	default:
		return -1;
	}

	ret = test_op_file(&scs_mbox, FILE_SERVICE_CMD_VERIFY_FILE, FILE_SERVICE_RSP_VERIFY_FILE,
				 file_id, (uint8_t *) buffer, (uint32_t) in_size,
				 2, (uint8_t *) buffer_crt, (uint32_t) in_size_crt);

	printk(BIOS_INFO, "verify_coreboot: %02d [%x, %x] [%x, %x] Done with status=%d\n",
			 (unsigned int)file_id,
			 (unsigned int)buffer, (unsigned int)(buffer + in_size - 1),
			 (unsigned int)buffer_crt, (unsigned int)(buffer_crt + in_size_crt - 1),
			 ret);

	return ret;
}
#endif

#if IS_ENABLED(CONFIG_BOOTBLOCK_X)
void copy_bootblock(void)
{
        struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;
        size_t bootblock_size = 128<<10; /* 128KiB */
        uint32_t file_id = FILE_SERVICE_FILE_A72_BOOTBLOCK_A_ID;
	char c = 'A';

	env->boot_bank = env->bootx_booted_bank;

	env->boot_from[0] = 'r'; /* restore it */
	env->boot_from[1] = 'a';
	if (env->boot_bank == 1) {
		file_id = FILE_SERVICE_FILE_A72_BOOTBLOCK_B_ID;
		c = 'B';
		env->boot_from[1] = 'b';
	}
	env->board_id_x = trav_board_id();
	env->cpu_rev = trav_cpu_rev();

	/* BOOTBLOCK A or B */
//	printk(BIOS_INFO, "Booting from BANK-%c...\n", c);

	/* direct to SRAM */
	copy_from_qspi((size_t) CONFIG_BOOTBLOCK_START, file_id, (size_t) bootblock_size, RESET_MODE_COLD);
}

/* should not be put in this file ! */
void run_bootblock(void)
{
	volatile unsigned int *cmd = (unsigned int *) PMU_INFORM0;
	void (*doit)(void) = (void *) CONFIG_BOOTBLOCK_START;

	/* clear 0xACCESS */
	*cmd = 0;

	doit();
}

int get_turbo_cpu_rev(void)
{
#if 0
	uint32_t chip_id_3 = 0;
	uint32_t id_3 = 3;

	test_op_nn_with_nn(&scs_mbox, MAILBOX_SERVICE_ID_OTP, OTP_SERVICE_CMD_GET_CHIP_ID,
				 &id_3, 1, &chip_id_3, 1);

	/*
	 * upper 4 bits: Main revision, low 4 bits: Subvision
	 * 0x00: A0
	 * 0x01: A1
	 * 0x10: B0
	 */

	return (chip_id_3 >> 16) & 0xff;
#else
	return 0x10; /* B0 */
#endif
}

#elif ENV_BOOTBLOCK

int get_otp_ddr_info(uint32_t *info)
{
	int ret;
	uint32_t val = 0;

	ret = test_op_nn_with_nn(&scs_mbox, MAILBOX_SERVICE_ID_OTP,
			    OTP_SERVICE_CMD_GET_DDR_INFO, NULL, 0, &val, 1);

	if (ret == 0)
		*info = val;

	return ret;
}

int set_otp_ddr_info(uint32_t info)
{
	int ret;

	ret = test_op_nn_with_nn(&scs_mbox, MAILBOX_SERVICE_ID_OTP,
			    OTP_SERVICE_CMD_FUSE_DDR_INFO, &info, 1, NULL, 0);

	return ret;
}

void check_turbo_a_b(void)
{
	return;
}

void check_build_git_hash(struct trav_mbox *mbox, const char *name, uint32_t cmd, char *build_git_hash)
{
	unsigned char p[65], *q;
	uint32_t valx[5];
	int i;

	memset(p, 0, 41);
	if (build_git_hash && 0 == test_op_nn_with_nn(mbox, MAILBOX_SERVICE_ID_BOOT, cmd, NULL, 0, valx, 5)) {
		q = (unsigned char *)valx;
		for (i = 0; i < 20; i++)
			snprintf((char *)(p+2*i), 3, "%02x", q[i]);

		printk(BIOS_INFO, "%s_BUILD_GIT_HASH: %s\n", name, p);
		memcpy_io((unsigned char *)build_git_hash, p, 41);
	}
}

void check_scs_versions(void)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;

	printk(BIOS_INFO, "BOARD_PCBA_VERSION: %s\n", env->board_pcba_version);
	printk(BIOS_INFO, "SCSBL1_BUILD_GIT_HASH: %s\n", env->scsbl1_build_git_hash);
	printk(BIOS_INFO, "SCSBL2_BUILD_GIT_HASH: %s\n", env->scsbl2_build_git_hash);
}

void check_a72_warm_reset(void)
{
	printk(BIOS_INFO, "A72_WARM_RESET: %d\n", trav_from_a72_warm_reset());
}

void copy_coreboot_rom(void)
{
	struct boot_env *env = (struct boot_env *)CONFIG_BOOT_ENV_MEM_BASE;
        uint32_t file_id = FILE_SERVICE_FILE_A72_COREBOOT_A_ID;

        /* COREBOOT A or B */
	if (env->boot_bank == 1)
		file_id = FILE_SERVICE_FILE_A72_COREBOOT_B_ID;

	copy_from_qspi((size_t) _cbfs_cache, file_id, (size_t) _cbfs_cache_size, RESET_MODE_WARM);
}
#endif
