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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <types.h>
#include <string.h>

#include <soc/ufs_hci.h>
#include <soc/ufs.h>
#include <console/console.h>
#include <arch/cache.h>

/* From JESD223D -
   6.1.1 UTP Transfer Request Descriptor */
enum dd
{
	NONE  = 0,
	WRITE = 1,
	READ  = 2,
};

static void ufshci_desc_clean(struct ufs_host *host, u32 db)
{
	//use strongly-ordered type
	ufs_sync_desc(host, UFS_UTRD_CLEAN_DCACHE, db);
	ufs_sync_desc(host, UFS_UCD_CLEAN_DCACHE, db);
}

static void ufshic_desc_invalidate(struct ufs_host *host, u32 db)
{
	//use strongly-ordered type
	ufs_sync_desc(host, UFS_UTRD_INVALIDATE_DCACHE, db);
	ufs_sync_desc(host, UFS_UCD_INVALIDATE_DCACHE, db);
}

u32 ufs_nop_descriptor(struct ufs_host *host)
{
	utrd_t *putrd;
	ucd_t *pucd;

	u32 lun;
	u32 db;
	u32 tag;
	u32 uflagidn;

	db = ufs_get_doorbell_num(host);
	if (db >= MAX_UFS_UTRL) {
		printk(BIOS_INFO, "db: %d is too big!\n", db);
		return false;
	}

	lun = ufs_get_lun_num(host);
	tag = ufs_get_tag_num(host);
	uflagidn = ufs_get_qr_flag_num(host);

	ufs_clear_utr_nexus_type(host);
	putrd = &(host->utrd[db]);
	pucd = &(host->ucd[db]);
	memset(pucd, 0x00, sizeof(ucd_t));
	memset(putrd, 0x00, sizeof(utrd_t));

	putrd->dw0.format.ct = 0x0;
	putrd->dw0.format.dd = 0x0;
	putrd->dw0.format.i = 0x1;
	putrd->dw2.ocs = (u8)(utrd_ocs_invalid_value);
	putrd->dw4.base = (u32)(uintptr_t)(pucd);

	if (((u64)pucd >> 32) & UFS_UPPER_ADDRESS)
		putrd->dw5.base64 = (u64)pucd >> 32;
	else
		putrd->dw5.base64 = 0;

	putrd->dw6.resp_upiu_offset = UPIU_MAX_SIZE;
	putrd->dw6.resp_upiu_length = UPIU_MAX_SIZE;
	putrd->dw7.prdt_offset = 0;
	putrd->dw7.prdt_length = 0;

	memset(&(pucd->request_upiu), 0x00, UPIU_MAX_SIZE);
	memset(&(pucd->response_upiu), 0x00, UPIU_MAX_SIZE);

	pucd->request_upiu.type.format.opcode = opcode_nop_out;
	pucd->request_upiu.flags.val = 0x0;
	pucd->request_upiu.task_tag = tag;
	pucd->request_upiu.lun = 0;

	if (!host->sysreg_enable) {
		ufshci_utrd_clean_and_invalidate(host, db);
		ufshci_ucd_clean_and_invalidate(host, db);
	}
	return true;
}

static u32 ufs_set_utrd_ucd(struct ufs_host *host, enum dd rw, u32 num_prdt)
{
	utrd_t *putrd;
	ucd_t *pucd;
	u32 lun;
	u32 db;
	u32 tag;

	db = ufs_get_doorbell_num(host);

	if (db >= MAX_UFS_UTRL) {
		printk(BIOS_INFO, "db: %d is too big!\n", db);
		return false;
	}

	lun = ufs_get_lun_num(host);
	tag = ufs_get_tag_num(host);

	putrd = &(host->utrd[db]);
	pucd = &(host->ucd[db]);

	memset(putrd, 0x00, sizeof(utrd_t));

	putrd->dw0.format.ct = 0;
	putrd->dw0.format.dd = rw;
	putrd->dw0.format.i = 0x1;
	putrd->dw2.ocs = (u8)(utrd_ocs_invalid_value);
	putrd->dw4.base = (u32)(uintptr_t)(pucd);

	if (((u64)pucd >> 32) & UFS_UPPER_ADDRESS)
		putrd->dw5.base64 = (u64)pucd >> 32;
	else
		putrd->dw5.base64 = 0;
	putrd->dw6.resp_upiu_offset = UPIU_MAX_SIZE;
	putrd->dw6.resp_upiu_length = UPIU_MAX_SIZE;
	if (rw != NONE) {
		putrd->dw7.prdt_offset = putrd->dw6.resp_upiu_offset +
					 putrd->dw6.resp_upiu_length;
		if(host->fmp_type == 3)
			putrd->dw7.prdt_length = num_prdt * sizeof(prdt_fmp);
		else
			putrd->dw7.prdt_length = num_prdt * sizeof(prdt_t);
	}

	memset(&(pucd->request_upiu), 0x00, UPIU_MAX_SIZE);
	memset(&(pucd->response_upiu), 0x00, UPIU_MAX_SIZE);

	pucd->request_upiu.type.format.opcode = opcode_command;
	/* From JESD220D -
	   Table 10-6 — UPIU Flags */
	pucd->request_upiu.flags.val = (rw << 5);
	pucd->request_upiu.task_tag = tag;
	pucd->request_upiu.lun = lun;

	return true;
}

static u32 ufs_set_read_write_10(struct ufs_host *host, u64 *buf, enum dd rw)
{
	ucd_t *pucd;
	u32 i;
	scsi_read_10 *pread;
	u32 prdt_size, block_size;
	u32 num_prdt, remained_data;
	u32 db;
	u32 lba;
	u32 num_lba;
	bool bfua;
	bool bdpo;
	prdt_t *prdt;

	lba = ufs_get_lba(host);
	num_lba = ufs_get_num_lba(host);
	bfua = ufs_get_fua(host);
	bdpo = ufs_get_dpo(host);

	prdt_size = ufs_get_config_prdt_size(host);
	block_size = ufs_get_config_block_size(host);

	num_prdt = (block_size * num_lba) / prdt_size;
	remained_data = (block_size * num_lba) % prdt_size;
	if (remained_data)
		num_prdt++;

	if (PRINT_DETAIL)
		printk(BIOS_INFO, "%s: lba: %#x, num_lba: %#x, prdt_size: 0x%x, block_size: 0x%x, "
				  "num_prdt: 0x%x, dd: %d\n",
				__func__, lba, num_lba, prdt_size, block_size, num_prdt, rw);

	if (!ufs_set_utrd_ucd(host, rw, num_prdt))
		return false;

	db = ufs_get_doorbell_num(host);
	pucd = &(host->ucd[db]);

	/* Expected Data Transfer Length */
	pucd->request_upiu.tsf[0] = swap_endian(num_lba * block_size);

	pread = (scsi_read_10 *) &(pucd->request_upiu.tsf[1]);

	pread->opcode = (rw == READ ? cmd_read10 : cmd_write10);
	pread->flags = ((bfua ? (0x1 << 3) : 0) | (bdpo ? (0x1 << 4) : 0));
	pread->lba_msb = swap_endian_word((0xffff0000ul & lba) >> 16);
	pread->lba_lsb = swap_endian_word(0x0000ffff & lba);
	pread->group_number = 0;
	pread->length_msb = (num_lba & 0xff00) >> 8;
	pread->length_lsb = num_lba & 0xff;
	pread->control = 0;

	if (num_prdt > MAX_NUM_OF_PRDT) {
		printk(BIOS_INFO, "num_prdt: %d is too bigger !\n", num_prdt);
		die("...");
	}
	prdt = (prdt_t *)pucd->prdt;
	memset(prdt, 0x00, sizeof(pucd->prdt));
	for (i = 0; i < num_prdt; i++) {
		if (((u64)buf >> 32) & UFS_UPPER_ADDRESS)
			prdt[i].base64 = (u64)buf >> 32;
		else
			prdt[i].base64 = 0x0;
		prdt[i].base = (u32)(uintptr_t) buf;
		prdt[i].data_bytes = prdt_size - 1;
		buf += prdt_size / sizeof(u64);
		prdt[i].reserved_1 = 0;
		prdt[i].reserved_2 = 0;
	}

	if (!host->sysreg_enable)
		ufs_sync_desc_db(host, db);
	return true;
}

u32 ufs_setread10command(struct ufs_host *host, u64 *buf)
{
	return ufs_set_read_write_10(host, buf, READ);
}

u32 ufs_setwrite10command(struct ufs_host *host, u64 *buf)
{
	return ufs_set_read_write_10(host, buf, WRITE);
}

u32 ufs_query_setflag(struct ufs_host *host)
{
	utrd_t *putrd;
	ucd_t *pucd;
	osf *posf;
	u32 lun;
	u32 db;
	u32 tag;
	u32 uflagidn;

	db = ufs_get_doorbell_num(host);
	if (db >= MAX_UFS_UTRL)
		return false;

	lun = ufs_get_lun_num(host);
	tag = ufs_get_tag_num(host);
	uflagidn = ufs_get_qr_flag_num(host);

	putrd = &(host->utrd[db]);
	pucd = &(host->ucd[db]);

	/* For somereason prdt.data_byte is set to 0xFFFF,
	 * For query command it should be zero
	 * Set this to zero explicitly
	 */
	prdt_t *prdt = (prdt_t *)pucd->prdt;

	prdt[0].data_bytes = 0;

	memset(putrd, 0x00, sizeof(utrd_t));

	putrd->dw0.format.ct = 0;
	putrd->dw0.format.dd = 0x0;
	putrd->dw0.format.i = 0x1;
	putrd->dw2.ocs = (u8)(utrd_ocs_invalid_value);
	putrd->dw4.base = (u32)(uintptr_t)(pucd);

	if (((u64)pucd >> 32) & UFS_UPPER_ADDRESS)
		putrd->dw5.base64 = (u64)pucd >> 32;
	else
		putrd->dw5.base64 = 0;

	putrd->dw6.resp_upiu_offset = UPIU_MAX_SIZE;
	putrd->dw6.resp_upiu_length = UPIU_MAX_SIZE;
	putrd->dw7.prdt_offset = 0;
	putrd->dw7.prdt_length = 0;

	memset(&(pucd->request_upiu), 0x00, UPIU_MAX_SIZE);
	memset(&(pucd->response_upiu), 0x00, UPIU_MAX_SIZE);

	pucd->request_upiu.type.format.opcode = opcode_query_request;
	pucd->request_upiu.task_tag = tag;
	pucd->request_upiu.lun = lun;
	pucd->request_upiu.query_fn = (u8)qf_standard_write_req;

	posf = (osf *)(pucd->request_upiu.tsf);
	posf->opcode = (u8)qr_opcode_set_flag;
	posf->osf0 = uflagidn;
	posf->osf1 = 0; /* reserved */
	posf->osf2 = 0; /* selector */
	posf->osf3 = 0; /* reserved */
	posf->osf4 = 0; /* reserved */
	posf->osf5 = 0; /* reserved */
	posf->osf6 = 0; /* reserved */
	posf->osf7 = 0; /* reserved */

	if (!host->sysreg_enable) {
		ufshci_desc_clean(host, db);
		ufshic_desc_invalidate(host, db);
	}
	return true;
}

u32 ufs_query_readflag(struct ufs_host *host)
{
	utrd_t *putrd;
	ucd_t *pucd;
	osf *posf;
	u32 lun;
	u32 db;
	u32 tag;
	u32 uflagidn;

	db = ufs_get_doorbell_num(host);
	if (db >= MAX_UFS_UTRL)
		return false;

	lun = ufs_get_lun_num(host);
	tag = ufs_get_tag_num(host);
	uflagidn = ufs_get_qr_flag_num(host);

	putrd = &(host->utrd[db]);
	pucd = &(host->ucd[db]);

	prdt_t *prdt = (prdt_t *)pucd->prdt;

	prdt[0].data_bytes = 0;
	memset(putrd, 0x00, sizeof(utrd_t));

	putrd->dw0.format.ct = 0;
	putrd->dw0.format.dd = 0x0;
	putrd->dw0.format.i = 0x1;
	putrd->dw2.ocs = (u8)(utrd_ocs_invalid_value);
	putrd->dw4.base = (u32)(uintptr_t)(pucd);

	if (((u64)pucd >> 32) & UFS_UPPER_ADDRESS)
		putrd->dw5.base64 = (u64)pucd >> 32;
	else
		putrd->dw5.base64 = 0;

	putrd->dw6.resp_upiu_offset = UPIU_MAX_SIZE;
	putrd->dw6.resp_upiu_length = UPIU_MAX_SIZE;
	putrd->dw7.prdt_offset = 0;
	putrd->dw7.prdt_length = 0;

	memset(&(pucd->request_upiu), 0x00, UPIU_MAX_SIZE);
	memset(&(pucd->response_upiu), 0x00, UPIU_MAX_SIZE);

	pucd->request_upiu.type.format.opcode = opcode_query_request;
	pucd->request_upiu.task_tag = tag;
	pucd->request_upiu.lun = lun;
	pucd->request_upiu.query_fn = (u8)qf_standard_read_req;

	posf = (osf *)(pucd->request_upiu.tsf);

	posf->opcode = (u8)qr_opcode_read_flag;
	posf->osf0 = uflagidn;
	posf->osf1 = 0; /* reserved */
	posf->osf2 = 0; /* selector */
	posf->osf3 = 0; /* reserved */
	posf->osf4 = 0; /* reserved */
	posf->osf5 = 0; /* reserved */
	posf->osf6 = 0; /* reserved */
	posf->osf7 = 0; /* reserved */

	if (!host->sysreg_enable) {
		ufshci_desc_clean(host, db);
		ufshic_desc_invalidate(host, db);
	}
	return true;
}

int ufs_query_responseflag(struct ufs_host *host)
{
	int ret = 0;
	ucd_t *pucd;
	u32 db;

	db = ufs_get_doorbell_num(host);
	if (db >= MAX_UFS_UTRL) {
		ret = -301;
		goto out;
	}

	pucd = &(host->ucd[db]);
	if (pucd->response_upiu.response != 0x00) {
		printk(BIOS_ERR, "\nUCD Response Fail : 0x%x", pucd->response_upiu.response);
		ret = -302;
		goto out;
	}

out:
	if (ret)
		printk(BIOS_ERR, "ufs_query_responseflag: fail %d", ret);

	return ret;
}

/* 0: sucess */
int ufs_query_get_flag_value_of_responseflag(struct ufs_host *host)
{
	int ret = 0;

	ucd_t *pucd;
	osf *posf;
	u32 db;

	db = ufs_get_doorbell_num(host);
	if (db >= MAX_UFS_UTRL) {
		ret = -401;
		goto out;
	}

	pucd = &(host->ucd[db]);
	if (pucd->response_upiu.response != 0x00) {
		//printk(BIOS_ERR, "\nUCD Response Fail : 0x%x", pucd->response_upiu.response);
		ret = -402;
		goto out;
	}

	posf = (osf *)(pucd->response_upiu.tsf);
	ret = (posf->osf6 >> 24) & 0xFF;

out:
	return ret;
}

u32 ufs_setrequestsensecommand(struct ufs_host *host, u8 ualloc_len, u32 *buf)
{
	ucd_t *pucd;
	u32 db;
	scsi_request_sense *pscsireqsense;

	if (!ufs_set_utrd_ucd(host, READ, 1))
		return false;

	db = ufs_get_doorbell_num(host);
	pucd = &(host->ucd[db]);

	/* Expected Data Transfer Length */
	pucd->request_upiu.tsf[0] = swap_endian(ualloc_len);

	pscsireqsense = (scsi_request_sense *) &(pucd->request_upiu.tsf[1]);
	pscsireqsense->opcode = cmd_request_sense;
	pscsireqsense->desc = 0;
	pscsireqsense->reserved = 0;
	pscsireqsense->alloc_length = ualloc_len;
	pscsireqsense->control = 0;
	{
		prdt_t *prdt = (prdt_t *)pucd->prdt;

		prdt[0].base64 = 0x0;
		prdt[0].base = (u32)(uintptr_t)(buf);
		prdt[0].data_bytes = BLOCK_SIZE - 1;
	}

	if (!host->sysreg_enable)
		ufs_sync_desc_db(host, db);
	return true;
}

u32 ufs_settestunitreadycommand(struct ufs_host *host, u8 control)
{
	ucd_t *pucd;
	scsi_testunitready *pTurBuffer;
	u32 db;

	if (!ufs_set_utrd_ucd(host, NONE, 0))
		return false;

	db = ufs_get_doorbell_num(host);
	pucd = &(host->ucd[db]);

	pucd->request_upiu.tsf[0] = 0;

	pTurBuffer = (scsi_testunitready *) &(pucd->request_upiu.tsf[1]);
	pTurBuffer->opcode = cmd_test_unit_ready;
	pTurBuffer->control = control;

	if (!host->sysreg_enable)
		ufs_sync_desc_db(host, db);
	return true;
}

u32 ufs_query_set_read_descriptor(struct ufs_host *host, u8 id, u16 despsize, u32 index)
{
	utrd_t *putrd;
	ucd_t *pucd;
	osf *posf;
	u32 lun;
	u32 db;
	u32 tag;

	db = ufs_get_doorbell_num(host);
	if (db >= MAX_UFS_UTRL)
		return false;

	lun = ufs_get_lun_num(host);
	tag = ufs_get_tag_num(host);

	putrd = &(host->utrd[db]);
	pucd = &(host->ucd[db]);

	memset(putrd, 0x00, sizeof(utrd_t));
	putrd->dw0.format.ct = 0;
	putrd->dw0.format.dd = 0x0;
	putrd->dw0.format.i = 0x1;
	putrd->dw2.ocs = (u8)(utrd_ocs_invalid_value);
	putrd->dw4.base = (u32)(uintptr_t)(pucd);

	if (((u64)pucd >> 32) & UFS_UPPER_ADDRESS)
		putrd->dw5.base64 = (u64)pucd >> 32;
	else
		putrd->dw5.base64 = 0;
	putrd->dw6.resp_upiu_offset = UPIU_MAX_SIZE;
	putrd->dw6.resp_upiu_length = UPIU_HEADER_SIZE + ((despsize + 15) & ~0xf);
	putrd->dw7.prdt_offset = 0;
	putrd->dw7.prdt_length = 0;

	memset(&(pucd->request_upiu), 0x00, UPIU_MAX_SIZE);
	memset(&(pucd->response_upiu), 0x00, UPIU_MAX_SIZE);

	pucd->request_upiu.type.format.opcode = opcode_query_request;
	pucd->request_upiu.task_tag = tag;
	pucd->request_upiu.lun = lun;
	pucd->request_upiu.query_fn = (u8)qf_standard_read_req;

	posf = (osf *)(pucd->request_upiu.tsf);

	posf->opcode = (u8)qr_opcode_read_descriptor;
	posf->osf0 = id;
	posf->osf1 = index;
	posf->osf2 = 0;
	posf->osf3 = 0; /* reserved */
	posf->osf4 = 0; /* reserved */
	posf->osf5 = swap_endian_word((u16) despsize);
	posf->osf6 = 0; /* reserved */
	posf->osf7 = 0; /* reserved */

	if (!host->sysreg_enable) {
		ufshci_desc_clean(host, db);
		ufshic_desc_invalidate(host, db);
	}
	return true;
}

int ufs_query_response(struct ufs_host *host, u8 *pDesp, u32 uDespSize)
{
	int ret = 0;
	ucd_t *pucd;
	u32 db;
	u8 nsize;
	u8 *pBuf;

	db = ufs_get_doorbell_num(host);

	if (db >= MAX_UFS_UTRL) {
		ret = -501;
		goto out;
	}

	pucd = &(host->ucd[db]);

	/* No need to confirm Response here.
	   Already checked in ufs_issue_cmd().
	 */
	nsize = swap_endian_word(pucd->response_upiu.data_segment_length);

	if (uDespSize < nsize) {
		ret = -503;
		goto out;
	}

	pBuf = (u8 *)(pucd->response_data);

	memcpy(pDesp, pBuf, nsize);

out:
	//if (ret)
	//	printk(BIOS_ERR, "ufs_query_response: fail %d\n", ret);

	return ret;
}

void ufshci_print_buf_align(u32 pBuf, u32 uByteSize, bool bIsByteAlign)
{
	u32 uSize;
	u32 uNum;
	u32 uItr;

	uSize = uByteSize;

	printk(BIOS_ERR, "\n----------------------------------------------------\n");
	printk(BIOS_ERR, "      ");
	for (uNum = 0; uNum < 16; uNum++)
		printk(BIOS_ERR, "%02x ", uNum);
	printk(BIOS_ERR, "\n----------------------------------------------------");

	if (bIsByteAlign) {
		//Byte Align
		uItr = uByteSize;

		for (uNum = 0; uNum < uItr; uNum++) {
			if (UFS_REMAINDER(uNum, 16) == 0)
				printk(BIOS_ERR, "\n%4x: ", uNum);

			printk(BIOS_ERR, "%02x ", *(u8 *)(uintptr_t)pBuf++);
		}
	} else {
		//Word Align
		uItr = UFS_DIVIDE(uByteSize, 4);
		if (UFS_REMAINDER(uByteSize, 4))
			uItr++;

		for (uNum = 0; uNum < uItr; uNum++) {
			if (UFS_REMAINDER(uNum, 4) == 0)
				printk(BIOS_ERR, "\n%4x: ", (uNum * 4));

			printk(BIOS_ERR, "0x%08x ", *(u32 *)(uintptr_t)(pBuf + (uNum * 4)));
		}
	}

	printk(BIOS_ERR, "\n----------------------------------------------------");
	printk(BIOS_ERR, "\n");
}

u32 ufs_query_set_write_descriptor(struct ufs_host *host, u8 id, u16 despsize, u8 *pDesp, u8 index, bool prov)
{
	utrd_t *putrd;
	ucd_t *pucd;
	osf *posf;
	u32 db;
	u32 tag;
	u8 *pdata;

	db = ufs_get_doorbell_num(host);

	if (db >= MAX_UFS_UTRL)
		return false;

	tag = ufs_get_tag_num(host);

	putrd = &(host->utrd[db]);
	pucd = &(host->ucd[db]);

	putrd->dw0.format.ct = 0;
	putrd->dw0.format.dd = 0x0;
	putrd->dw0.format.i = 0x1;
	putrd->dw2.ocs = (u8)(utrd_ocs_invalid_value);
	putrd->dw4.base = (u32)(uintptr_t)(pucd);
	putrd->dw5.base64 = 0;
	putrd->dw6.resp_upiu_offset = UPIU_MAX_SIZE;
	//putrd->dw6.resp_upiu_length = UPIU_HEADER_SIZE + ((qr->length + 3) & ~3);
	//putrd->dw6.resp_upiu_length = UPIU_HEADER_SIZE + ((despsize + 3) & ~3);
	putrd->dw6.resp_upiu_length = UPIU_HEADER_SIZE + ((despsize + 15) & ~0xf);
	putrd->dw7.prdt_offset = 0;
	putrd->dw7.prdt_length = 0;

	memset(&(pucd->request_upiu), 0x00, UPIU_MAX_SIZE);
	memset(&(pucd->response_upiu), 0x00, UPIU_MAX_SIZE);

	pucd->request_upiu.type.format.opcode = opcode_query_request;
	pucd->request_upiu.task_tag = tag;
	pucd->request_upiu.lun = 0x0;
	pucd->request_upiu.query_fn = (u8)qf_standard_write_req;

	pucd->request_upiu.data_segment_length = swap_endian_word((u16)despsize);


	pdata = (u8 *)(pucd->request_data);
	memcpy(pdata, pDesp, despsize);

	posf = (osf *)(pucd->request_upiu.tsf);

	posf->opcode = (u8)qr_opcode_write_descriptor;
	//posf->osf0 = qr->id;
	//posf->osf1 = qr->index;
	posf->osf0 = id;
	posf->osf1 = index;
	posf->osf2 = 0;
	posf->osf3 = 0; /* reserved */
	posf->osf4 = 0; /* reserved */
	//posf->osf5 = swap_endian_word((u16) qr->length);
	posf->osf5 = swap_endian_word((u16) despsize);
	posf->osf6 = 0; /* reserved */
	posf->osf7 = 0; /* reserved */

	if (!host->sysreg_enable) {
		ufshci_desc_clean(host, db);
		ufshic_desc_invalidate(host, db);
	}

	return true;
}

void ufs_setup_config_descriptor(ufs_config_descriptor *conf_desc,
				 unsigned int capacity)
{
	ufs_config_descr_header *stConfigHeader;
	const ufs_config_param_unit_descriptor stUnitDescrConfigParam[8] = {
		{0x01, 0x00, 0x00, 0x00, 0xcb1d0000, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00},
		{0x00, 0x01, 0x00, 0x03, 0x00000000, 0x01, 0x0C, 0x02, 0x00, 0x00, 0x00},
		{0x00, 0x02, 0x00, 0x03, 0x00000000, 0x01, 0x0C, 0x02, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00000000, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00000000, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00000000, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00000000, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00000000, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00},
	};

	if (!conf_desc || !capacity)
		return;

	// header
	stConfigHeader = &conf_desc->header;
	memset(stConfigHeader, 0x00, sizeof(ufs_config_descr_header));
	stConfigHeader->bLength = sizeof(*conf_desc);
	stConfigHeader->bDescriptorType = 0x1;
	stConfigHeader->bConfDescContinue = 0x0;
	stConfigHeader->bBootEnable = 0x0;
	stConfigHeader->bDescrAccessEn = 0x1;
	stConfigHeader->bInitPowerMode = 0x1;
	stConfigHeader->bHighPriorityLUN = 0x7F;
	stConfigHeader->bSecureRemovalType = 0x0;
	stConfigHeader->bInitActiveICCLevel = 0x0;
	stConfigHeader->wPeriodicRTCUpdate = 0x0;
	stConfigHeader->reserved_0 = 0x0;
	stConfigHeader->reserved_1 = 0x0;
	stConfigHeader->reserved_2 = 0x0;

	// unit descriptor
	memcpy(conf_desc->ud,
			stUnitDescrConfigParam, sizeof(stUnitDescrConfigParam));

	// update lu config with device capacity
	conf_desc->ud[0].dNumAllocUnits = cpu_to_be32(capacity);

	flush_dcache_all(0x1);
}
