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

#include <string.h>
#include <stdlib.h>
#include <console/console.h>
#include <delay.h>

#include <soc/ufs.h>
#include <soc/ufs_hci.h>
#include <soc/ufs_unipro.h>
#include <soc/cpu.h>

#define UFS_FDEVINIT_MAX 1000

static void address_check(const char *str, unsigned long required_size,
		unsigned long start, unsigned long end)
{
	if (required_size > (end - start)) {
		printk(BIOS_INFO, "%s: [0x%lx, 0x%lx) size:%lx, required size: %lx\n",
				str, start, end, end - start, required_size);

		die("...");
	}
}

extern unsigned char uart_rx_byte_timeout(int idx);

struct ufs_host *ufs;

static int ufs_issue_query(struct ufs_host *host);
static int ufs_issue_cmd(struct ufs_host *host);
static int ufshci_send_request(struct ufs_host *host);
static int ufs_nop_auto(struct ufs_host *host);
static int ufs_fdevice_init_auto(struct ufs_host *host);
static int ufs_test_unit_ready_auto(struct ufs_host *host, u32 nlun);
static int ufs_test_request_sense_auto(struct ufs_host *host, u32 nlun);
static int ufs_power_mode_change(struct ufs_host *host, u32 mode, u32 gear, u32 rate);

u32 ufs_pow(u32 x, u32 y)
{
	u32 i;
	u32 uResult = 1;

	for (i = 0; i < y; i++)
		uResult = uResult * x;

	return uResult;
}

int init_ufs_load_from_ufs(void)
{
	struct ufs_config *config;
	//struct ufs_host_config *pufs_config;

	ufs = (struct ufs_host *)UFS_HOST_STRUCT_ADDRESS;
	address_check("ufs_host", sizeof(struct ufs_host),
			UFS_HOST_STRUCT_ADDRESS, UFS_UTRD_ADDRESS);
	config = (struct ufs_config *)UFS_HOST_CONFIG_ADDRESS;
	address_check("ufs_config", sizeof(struct ufs_config),
			UFS_HOST_CONFIG_ADDRESS, UFS_HOST_STRUCT_ADDRESS);

	ufs->base = (u32 *)TRAV_UFS0_HCI;
	ufs->fmp = (u32 *)(uintptr_t)TRAV_UFS0_FMP;
	ufs->unipro = (u32 *)(uintptr_t)TRAV_UFS0_UNIPRO;
	ufs->mphy = (u32 *)(uintptr_t)(TRAV_UFS0_HCI + 0x4000);
	ufs->pmu = (u32 *)(uintptr_t)(TRAV_CENTRAL_PMU + 0x0724);
	ufs->sysreg = (u32 *)(uintptr_t)TRAV_SYSREG_FSYS0;

	//pufs_config = &(config->hci_config_info);

	ufs->config = config;
	if (PRINT_DETAIL)
		printk(BIOS_ERR, "ufs init...");
	ufs_init(ufs);
	return 0;
}

static void ufs_init_reset(struct ufs_host *host)
{
	hci_sw_reset(host);
	hci_idle_indicate(host);
	hci_enable(host, true);
	hci_device_reset_control(host, false);
	udelay(5);
	hci_device_reset_control(host, true);
	hci_device_refclkout_control(host, true);
	hci_vs_dfes_error_layber_enable(host);
	hci_vs_set_burst_len(host, 0x3);

}

static bool ufshci_check_ready(struct ufs_host *host)
{
	uint32_t val;

	val = UFS_READ(UFSHCI_HCS);

	if (PRINT_DETAIL)
		printk(BIOS_ERR, "check ready status: %x\n", val);

	if ((val & 0x07) == 0x07)
		return true;

	return false;
}

static int ufs_linkstartup_cmd(struct ufs_host *host)
{
	u32 loop = 0;
	u32 is_ulss_loop;
	int ret = 1;

	while (loop++ < 100) {
		UFS_WRITE(UFSHCI_UICCMDR, H_UIC_DME_LINKSTARTUP);
		ufshci_wait_complete(host, UFSHCI_IS_UCCS);
		if (UFS_READ(UFSHCI_UCMDARG2) != 0) {
			ret = -21;
			is_ulss_loop = 0;
			while (!(UFS_READ(UFSHCI_IS) & UFSHCI_IS_ULSS) &&
				is_ulss_loop++ < 1000)
				udelay(100);
			printk(BIOS_INFO, "\n\n\n%s: retried IS_ULSS %d : IS %d\n\n\n", __func__, is_ulss_loop, UFS_READ(UFSHCI_IS));
			UFS_WRITE(UFSHCI_UCMDARG2, 0);
			continue;
		}
		if (!ufshci_check_ready(host)) {
			ret = -22;
			continue;
		}
		if (read_unipro_linkstartup_cnf_result(host) != 0) {
			ret = -23;
			continue;
		}

		if (PRINT_LOG)
			printk(BIOS_INFO, "Succeed : link start-up cmd\n");
		host->link_done = true;
		ret = 0;
		break;
	}

	if (loop > 1)
		printk(BIOS_INFO, "\n\n\n%s: retried %d : result %d\n\n\n", __func__, loop, ret);

	if (ret)
		printk(BIOS_ERR, "ufs_linkstartup_cmd: failed with %d\n", ret);

	return ret;
}

static void ufs_init_descriptor(struct ufs_host *host)
{
	UFSHCI_HCE_t hce;
	UFSHCI_IE_t ie;
	UFSHCI_UTRIACR_t utriacr;
	UFSHCI_UTRLBA_t utrlba;
	UFSHCI_UTRLBAU_t utrlbau;

	hce.data = 0;
	ie.data = 0;
	utriacr.data = 0;
	utrlba.data = (uintptr_t) host->utrd;

	if (((u64)host->utrd >> 32) & UFS_UPPER_ADDRESS)
		utrlbau.data = (u64)host->utrd >> 32;
	else
		utrlbau.data = 0;

	utrlba.data = (u32)(uintptr_t)host->utrd;

	if (utrlba.b.reserved_1) {
		/* address should be 1K align */
		while (1)
			;
		utrlba.b.reserved_1 = 0;
	}

	UFS_WRITE(UFSHCI_UTRLBAU, utrlbau.data);
	UFS_WRITE(UFSHCI_UTRLBA, utrlba.data);
	UFS_WRITE(UFSHCI_UTMRLRSR, 1);

	UFS_WRITE(UFSHCI_UTRLRSR, 1);
	ufs_vs_data_reorder(host, 0x0A);
	ufs_vs_init_nexus_type(host);
	//v2p1_ctrl
	ufs_vs_set_v2p1_ctrl(host, 0x0);
	ufs_vs_set_prdt(host);
	ufs_vs_set_1us_to_cnt(host, 1000);
}

static int ufs_clear_device_unit_attention(struct ufs_host *host)
{
	int ret = 0;
	int i;
	//int rs[5] = { 0, 1, 2, 0xd0, 0xc4 };
	int rs[5] = { 0, -1, -2, 0xd0, 0xc4 }; /* trav only use one LUN for now */

	for (i = 0; i < 5; i++) {
		if (rs[i] < 0)
			continue;

		if (PRINT_LOG)
			printk(BIOS_INFO, "req_sense: LUN %d\n", rs[i]);
		if (ufs_test_request_sense_auto(host, rs[i])) {
			if (ufs_test_unit_ready_auto(host, rs[i])) {
				ret = -(26 + i);
				goto out;
			}
		}
	}

out:
	if (ret)
		printk(BIOS_ERR, "ufs_clear_device_unit_attention: LUN %d fail %d\n", rs[i], ret);

	return ret;
}

void ufs_configure_ufs_device_lun(void)
{
#if IS_ENABLED(CONFIG_TRAV_UFS_CONFIGURE_DEVICE_LUN)
	int ret;
	init_ufs_load_from_ufs();
//	printk(BIOS_INFO,"Going to configure device\n");
	ret = configure_ufs_device(ufs);
	if (ret)
		printk(BIOS_ERR,"Failed to write device configuration: %d\n\n", ret);
	ufs_device_get_info();
#endif
}

static int ufs_init_host(struct ufs_host *host)
{
	int j;
	int ret;

	ufs_init_reset(host);

	ufs_vs_set_dfes_ocs_enable(host, true);

	ufshic_set_interrupt(host);
	ufshic_set_vs_interrupt(host);

	ufs_interrupt_enable(host, host->int_enable | UFSHCI_IS_UTRCS);
	ufs_vs_interrupt_enable(host, host->int_vs_enable);

	ufs_init_unipro_phy(host);

	ret = ufs_linkstartup_cmd(host);
	if (ret) {
		ret = -31;
		goto out;
	}

	if (PRINT_LOG)
		printk(BIOS_ERR, " %s:\n", "UFS Link Successful");

	// link connection
	ufs_post_link(host);

	ufs_init_descriptor(host);

	if (PRINT_LOG)
		printk(BIOS_ERR, "ufs_init_descriptor_passed....\n");

	if (ufshci_check_ready(host) == false) {
		ret = -32;
		goto out;
	}

	if (PRINT_LOG)
		printk(BIOS_ERR, "Sending NOP command...\n");

	for (j = 0; j < 3; j++) {
		ret = ufs_nop_auto(host);
		if (!ret)
			break;
	}

	if (ret) {
		ret = -33;
		goto out;
	}

	if (PRINT_LOG)
		printk(BIOS_ERR, "changing power mode to HSA series\n");
	ufs_power_mode_change(host, 1, 3, 2);
	if (PRINT_LOG)
		printk(BIOS_INFO, "sending FDevice request...\n");

	ret = ufs_fdevice_init_auto(host);
	if (ret) {
		ret = -34;
		goto out;
	}

	if (PRINT_LOG)
		printk(BIOS_ERR, "Sending Testready command...\n");

	ret = ufs_clear_device_unit_attention(host);
	if (ret) {
		ret = -35;
		goto out;
	}

out:
	if (ret)
		printk(BIOS_ERR, "ufs_init_host(): fail %d\n", ret);

	return ret;
}

static int ufs_test_unit_ready_auto(struct ufs_host *host, u32 nlun)
{
	ufs_set_doorbell_num(host, 0x1);
	ufs_set_lun_num(host, nlun);
	ufs_set_tag_num(host, 0x1);

	ufs_set_utr_nexus_type(host);
	ufs_settestunitreadycommand(host, 0);

	return ufs_issue_cmd(host);
}

static int ufs_test_request_sense_auto(struct ufs_host *host, u32 nlun)
{
	int ret;
	u32 db;
	u32 *buf;
	//u32 size = 4096;
	u32 alloc_len;

	ufs_set_doorbell_num(host, 0x1);
	ufs_set_tag_num(host, 0x1);
	ufs_set_lun_num(host, nlun);

	buf = (u32 *) UFS_SENSE_AUTO_ADDRESS;//malloc(size);
	address_check("sense buf", UPIU_MAX_SIZE, UFS_SENSE_AUTO_ADDRESS, UFS_HOST_CONFIG_ADDRESS);
	memset((u32 *)buf, 0x00, UPIU_MAX_SIZE);

	ufs_set_utr_nexus_type(host);
	alloc_len = 18;
	ufs_setrequestsensecommand(host, alloc_len, buf);

	ret = ufs_issue_cmd(host);
	if (ret)
		return ret;

	db = ufs_get_doorbell_num(host);
	ufs_sync_desc_db(host, db);
	free(buf);

	return 0;
}

static int ufs_fdevice_init_auto(struct ufs_host *host)
{
	int ret;
	u32 i;
	//u32 flag_value;
	//u64 elapsed_time;

	//ufs_start_time();

	ufs_set_doorbell_num(host, 0x2);
	//ufs_set_lun_num(host, 0x0);
	ufs_set_tag_num(host, 0x3);
	ufs_set_qr_flag_num(host, qr_flag_idn_bdeviceinit);

	ufs_clear_utr_nexus_type(host);
	ufs_query_setflag(host);

	ret = ufs_issue_query(host);
	if (ret) {
		ret = -61;
		goto out;
	}

	for (i = 0; i < UFS_FDEVINIT_MAX; i++) {

		ufs_set_doorbell_num(host, 0x4);
		ufs_set_tag_num(host, 0x4+i);
		ufs_set_qr_flag_num(host, qr_flag_idn_bdeviceinit);

		ufs_clear_utr_nexus_type(host);
		ufs_query_readflag(host);

		ret = ufs_issue_query(host);
		if (ret) {
			ret = -62;
			goto out;
		}

		ret = ufs_query_get_flag_value_of_responseflag(host);
		if (ret == 0) {
			/* fDeviceInit done */
			break;
		} else if (ret == 1) {
			/* fDeviceInit in progress */
			mdelay(1);
		} else if (ret < 0) {
			ret = -63;
			goto out;
		}
	}


	//elapsed_time = ufs_get_elapsed_time();

	//printk(BIOS_ERR, "fDeviceInit (retry:%d, time:%dus)", i, elapsed_time);

	if (i >= UFS_FDEVINIT_MAX)
		ret = -631;

out:
	if (ret)
		printk(BIOS_ERR, "ufs_fdevice_init_auto fail %d\n", ret);

	return ret;
}

int ufs_query_read_descriptor(struct ufs_host *host, u8 id, u32 index,
				u8 *resp, u16 respsize)
{
	int ret = 0;

	ufs_set_lun_num(host, 0);
	ufs_set_doorbell_num(host, 3);
	ufs_set_tag_num(host, 3);

	ufs_clear_utr_nexus_type(host);

	if (!ufs_query_set_read_descriptor(host, id, respsize, index)) {
		ret = -1000;
		goto out;
	}

	if (ufs_issue_query(host)) {
		ret = -1001;
		goto out;
	}

	if (ufs_query_response(host, resp, respsize)) {
		ret = -1002;
		goto out;
	}

out:
	if (ret)
		printk(BIOS_ERR, "%s: error: %d\n", __func__, ret);

	return ret;
}

int ufs_query_write_descriptor(struct ufs_host *host, u8 id, u32 index,
				u8 *descr, u16 size)
{
	int ret = 0;

	ufs_set_lun_num(host, 0);
	ufs_set_doorbell_num(host, 3);
	ufs_set_tag_num(host, 3);

	ufs_clear_utr_nexus_type(host);

	if (!ufs_query_set_write_descriptor(host, id, size,
					    descr, index, true)) {
		ret = -1003;
		goto out;
	}

	if (ufs_issue_query(host)) {
		ret = -1004;
		goto out;
	}

out:
	if (ret)
		printk(BIOS_ERR, "%s: error: %d\n", __func__, ret);

	return ret;
}

#if IS_ENABLED(CONFIG_TRAV_UFS_CONFIGURE_DEVICE_LUN)
int configure_ufs_device(struct ufs_host *host)
{
	int ret;
	u32 descr_size;
	u8 *descr;
	u8 alloc_unit_size;
	unsigned int segment_size;
	unsigned int capacity;
	ufs_config_descriptor conf_desc;

	descr_size = sizeof(ufs_config_descriptor);

	descr = (u8 *)(uintptr_t)UFS_DEVICE_DESC_BASE; //change it to malloc
	memset(descr, 0x00, descr_size);

	/* read geometry descriptor - up to bAllocationUnitSize */
	ret = ufs_query_read_descriptor(host, qr_descr_idn_geometry, 0, descr,
					0x12);
	if (ret) {
		printk(BIOS_ERR, "%s: read geometry failed : %d\n",
				__func__, ret);
		ret = -64;
		goto out;
	}

	/*	qTotalRawDeviceCapacity is in unit of 512 bytes
		dNumAllocUnits in Unit Descr is in unit of
		(bAllocationUnitSize × dSegmentSize × 512),
		which is 4 MB in most cases.
	 */

	segment_size =  (*(descr + 0x0d) << 24) |
			(*(descr + 0x0e) << 16) |
			(*(descr + 0x0f) <<  8) |
			 *(descr + 0x10);
	alloc_unit_size = *(descr + 0x11);

	capacity = be64_to_cpu(*(u64 *)(descr + 4)) /
		   (segment_size * alloc_unit_size);

	printk(BIOS_INFO, "%s: read capacity : %x, segment_size: %x, alloc_unit_size: %d\n",
			__func__, capacity, segment_size, alloc_unit_size);

	/* read config descriptor */
	ret = ufs_query_read_descriptor(host, qr_descr_idn_config, 0, descr,
					descr_size);
	if (ret) {
		ret = -65;
		goto out;
	}

	/* if LU's are already configured, let's skip it */
	ufs_setup_config_descriptor(&conf_desc, capacity);
	if (!memcmp(descr, &conf_desc, sizeof(conf_desc)))
		goto out;

	printk(BIOS_INFO, "%s: writing a new LU config\n", __func__);

	ret = ufs_query_write_descriptor(host, qr_descr_idn_config, 0,
					(u8 *)&conf_desc, descr_size);
	if (ret) {
		ret = -67;
		goto out;
	}

out:
	return ret;
}
#endif

void ufs_device_get_info(void)
{
	int ret = 0;
	struct ufs_host *host = ufs;
	u8 descr_id;
	u32 descr_size;
	int tag;
	int db;
	ufs_unit_descriptor pUnitDesc[8];
	u32 num, maxuintnum;
	u8 des[0x90];
	u32 uMB;

	maxuintnum = UFS_MAX_LU;
	tag = 0x2;

	ufs_set_lun_num(host, 0);
	ufs_set_tag_num(host, tag);

	for (num = 0; num < maxuintnum; num++) {
		db = num;
		ufs_set_doorbell_num(host, db);

		descr_id = qr_descr_idn_unit;
		descr_size = 0x90;

		memset(des, 0x00, descr_size);

		ufs_clear_utr_nexus_type(host);
		ufs_query_set_read_descriptor(host, descr_id, descr_size, num);

		ret = ufs_issue_cmd(host);
		if (ret) {
			ret = -68;
			goto out;
		}
		ret = ufs_query_response(host, des, descr_size);
		if (ret) {
			ret = -69;
			goto out;
		}

		if (!host->sysreg_enable) {
			clean_dcache_range((u64)(uintptr_t)des, sizeof(u64)*descr_size);
			inv_dcache_range((u64)(uintptr_t)des, sizeof(u64)*descr_size);
		}

		memcpy((u8 *)&pUnitDesc[num], (u8 *)des, sizeof(ufs_unit_descriptor));

	}

	memcpy(host->device_unitdesc, pUnitDesc, sizeof(ufs_unit_descriptor)*maxuintnum);
#if 1
	printk(BIOS_INFO, "+-------+--------+--------+--------+--------+--------+--------+--------+--------+\n");
	printk(BIOS_INFO, "|LU Num |");

	for (num = 0; num < maxuintnum; num++)
		printk(BIOS_INFO, "%7d |", pUnitDesc[num].bUnitIndex);

	printk(BIOS_INFO, "\n+-------+--------+--------+--------+--------+--------+--------+--------+--------+\n");

	printk(BIOS_INFO, "|LU en  |");

	for (num = 0; num < maxuintnum; num++)
		printk(BIOS_INFO, "%7s |", (pUnitDesc[num].bLUEnable ? "en" : ""));

	printk(BIOS_INFO, "\n");
	printk(BIOS_INFO, "|Boot   |");

	for (num = 0; num < maxuintnum; num++)
		printk(BIOS_INFO, "%7s |", (pUnitDesc[num].bBootLunID == 1 ? "Boot(A)" : (pUnitDesc[num].bBootLunID == 2 ? "Boot(B)" : "")));

/*Logical Unit Write Protect
 *00h: LU not write protected 01h: LU write protected when fPowerOnWPEn =1 02h: LU permanently write protected when
 *fPermanentWPEn =1 03h: Reserved (for UFS Security Extension specification) Others: Reserved
 */
	printk(BIOS_INFO, "\n");
	printk(BIOS_INFO, "|WP     |");
	for (num = 0; num < maxuintnum; num++)
		printk(BIOS_INFO, "%7d |", pUnitDesc[num].bLUWriteProtect);


/*Memory Type
 * bMemoryType defines logical unit memory type.
 *00h: Normal Memory 01h: System code memory type 02h: Non-Persistent memory type 03h: Enhanced memory type 1
 *04h: Enhanced memory type 2 05h: Enhanced memory type 3 06h: Enhanced memory type 4 Others: Reserved
 */

	printk(BIOS_INFO, "\n");
	printk(BIOS_INFO, "|Type   |");

	for (num = 0; num < maxuintnum; num++)
		printk(BIOS_INFO, "%7d |", pUnitDesc[num].bMemoryType);


	printk(BIOS_INFO, "\n");
	printk(BIOS_INFO, "|Capa   |");

	for (num = 0; num < maxuintnum; num++) {
		//uCount1 = (pUnitDesc[uTemp].qLogicalBlockCount >> 32) & 0xFFFFFFFF;
		//uCount2 = pUnitDesc[uTemp].qLogicalBlockCount & 0xFFFFFFFF;

#if 0
		uCount1 = ((pUnitDesc[num].qLogicalBlockCount_B3 << 24) | (pUnitDesc[num].qLogicalBlockCount_B2 << 16) |
				(pUnitDesc[num].qLogicalBlockCount_B1 << 8) | pUnitDesc[num].qLogicalBlockCount_B0);
		uCount2 = ((pUnitDesc[num].qLogicalBlockCount_B7 << 24) | (pUnitDesc[num].qLogicalBlockCount_B6 << 16) |
				(pUnitDesc[num].qLogicalBlockCount_B5 << 8) | pUnitDesc[num].qLogicalBlockCount_B4);


		uLower = uCount1;
		uUpper = uCount2;


		uKB = UFS_DIVIDE(ufs_pow(2, pUnitDesc[num].bLogicalBlockSize), 1024);
		uMB = UFS_DIVIDE((uLower * uKB), 1024);
#else
		uMB = ufs_cal_mb_size_from_qLogicalBlockCount(pUnitDesc[num]);
#endif
		if (uMB < 1024)
			printk(BIOS_INFO, "%5dMB |", uMB);
		else
			printk(BIOS_INFO, "%5dGB |", UFS_DIVIDE(uMB, 1024));

		//DebugUFS(D_DFT, "%5dMB |", UFS_DIVIDE((pUnitDesc[uTemp].qLogicalBlockCount * pUnitDesc[uTemp].bLogicalBlockSize), 1024));
	}

	printk(BIOS_INFO, "\n");
	printk(BIOS_INFO, "|BlSize |");

	for (num = 0; num < maxuintnum; num++)
		printk(BIOS_INFO, "%5dKB |", UFS_DIVIDE(ufs_pow(2, pUnitDesc[num].bLogicalBlockSize), 1024));

	printk(BIOS_INFO, "\n+-------+--------+--------+--------+--------+--------+--------+--------+--------+\n");
#endif

out:
	//if (ret)
	//	printk(BIOS_ERR, "ufs_device_get_info: fail %d\n", ret);
	;
}

u32 ufs_cal_mb_size_from_qLogicalBlockCount(ufs_unit_descriptor pUnitDesc)
{
	u32 uCount1, uCount2;
	u32 uLower, uUpper;
	u32 uKB, uMB;

	uCount1 = ((pUnitDesc.qLogicalBlockCount_B3 << 24) | (pUnitDesc.qLogicalBlockCount_B2 << 16) |
			(pUnitDesc.qLogicalBlockCount_B1 << 8) | pUnitDesc.qLogicalBlockCount_B0);
	uCount2 = ((pUnitDesc.qLogicalBlockCount_B7 << 24) | (pUnitDesc.qLogicalBlockCount_B6 << 16) |
			(pUnitDesc.qLogicalBlockCount_B5 << 8) | pUnitDesc.qLogicalBlockCount_B4);

	uLower = uCount1;
	uUpper = uCount2;

	uKB = UFS_DIVIDE(ufs_pow(2, pUnitDesc.bLogicalBlockSize), 1024);
	uMB = UFS_DIVIDE((uLower * uKB), 1024);

	return uMB;
}


void ufs_init(struct ufs_host *host)
{
	host->ucd = (ucd_t *)UFS_UCD_ADDRESS;
	address_check("ucd_t", sizeof(ucd_t)*MAX_UFS_UTRL, UFS_UCD_ADDRESS, UFS_DEVICE_DESC_BASE);
	memset(host->ucd, 0x00, sizeof(ucd_t)*MAX_UFS_UTRL);

	host->utrd = (utrd_t *)UFS_UTRD_ADDRESS;
	address_check("utrd_t", sizeof(utrd_t)*MAX_UFS_UTRL, UFS_UTRD_ADDRESS, UFS_UCD_ADDRESS);
	memset(host->utrd, 0x00, sizeof(utrd_t)*MAX_UFS_UTRL);

	if (PRINT_LOG)
		printk(BIOS_INFO, "ucd: %x utrd : %x\n", (u32)(uintptr_t)host->ucd, (u32)(uintptr_t)host->utrd);
	host->fpga = 0;

	dev_init_pmu(host);
	dev_init_gpio(host);
	dev_init_clock(host);

	host->link_done = false;
	/* set additional options */
	host->debug = false;
	host->auto_h8 = false;
	host->irq_mode = false;
	host->sysreg_enable = false;
	host->queue_depth = 16;
	/* PRDT type setting for FMP */
	host->fmp_type = 0;
	/* for command queuing */
	memset(host->utrd_ucd_map, 0xFF, sizeof(int) * 32);
	host->complete_req = 0;
	host->saved_db = 0;
	host->ucd_map = 0;
	host->next_db = 0;
	/* MIB address access : Indirect access(0, default), Direct access(1) */
	host->mib_direct_access = 0;	//
	/* UTRD, UPIU log print : enable(1), disable(0, default) */
	host->print_trans_data = 1;
	/* WLU enable select : enable(1, default), disable(0) */
	host->wlu_enable = 1;
	/* for performance check */
	memset(host->perf_mon, 0x00, sizeof(long long) * 17 * 5);
	host->async = false;
	if (PRINT_LOG)
		printk(BIOS_INFO, "calling ufs_init_host...\n");

	ufs_set_config_block_size(host, 4096);
	ufs_set_config_prdt_size(host,  4096);
	ufs_set_config_dma_blocking(host, true);

	ufs_init_host(host);
}

void ufs_init_unipro_phy(struct ufs_host *host)
{
	u32 AvailLane = 0;  //hardcoded

	ufs_check_num_of_lanes(host);
	host->config->hci_config_info.numoflane = 2;
	AvailLane = ufs_get_num_of_lanes(host);
	ufs_set_clock_period(host);
	ufs_pre_link(host);
	udelay(300);
}

void ufs_set_config_block_size(struct ufs_host *host, u32 block_size)
{
	struct ufs_host_config *config;

	config = ufs_get_host_config_info(host);
	config->block_size = block_size;
}

int ufs_get_config_block_size(struct ufs_host *host)
{
	struct ufs_host_config *config;

	config = ufs_get_host_config_info(host);
	return config->block_size;
}

void ufs_set_config_prdt_size(struct ufs_host *host, u32 prdt_size)
{
	struct ufs_host_config *config;

	config = ufs_get_host_config_info(host);
	config->prdt_size = prdt_size;
}

int ufs_get_config_prdt_size(struct ufs_host *host)
{
	struct ufs_host_config *config;

	config = ufs_get_host_config_info(host);
	return config->prdt_size;
}

/* in order to find zero, use zero's complement of bitmap */
int find_one_in_bitmap(unsigned int bitmap)
{
	int i;

	for (i = 0; i < 32; i++)
		if (bitmap & (1ul << i))
			return i;
	return -1;
}

void *ufs_get_host_config_info(struct ufs_host *host)
{
	return (void *) &(host->config->hci_config_info);
}

void *ufs_get_descriptor_info(struct ufs_host *host)
{
	return (void *) &(host->config->hci_des_info);
}
void ufs_set_config_dma_blocking(struct ufs_host *host, bool enable)
{
	struct ufs_host_config *config;

	config = ufs_get_host_config_info(host);
	config->dma_blocking = enable;
}
void *ufs_get_uic_pwr_mode(struct ufs_host *host)
{
	return (void *) &(host->config->hci_uic_pwr_mode_info);
}

void ufs_store_uic_pwr_mode(struct ufs_host *host, u32 gear, u32 pwr_mode, u32 hs_series)
{
	struct ufs_uic_pwr_mode *pInfo;

	pInfo = ufs_get_uic_pwr_mode(host);

	//backup status
	pInfo->prev_gear = pInfo->gear;
	pInfo->prev_pwr_mode = pInfo->pwr_mode;
	pInfo->prev_hs_series = pInfo->hs_series;

	pInfo->gear = gear;
	pInfo->pwr_mode = pwr_mode;
	pInfo->hs_series = hs_series;
}

int ufs_power_mode_change(struct ufs_host *host, u32 mode, u32 gear, u32 rate)
{
	u32 pwr_mode;
	u32 lane;
	bool hs_mode;
	bool auto_mode;
	int ret = 0;

	lane = ufs_get_num_of_lanes(host);

	/* (1:Fast_Mode, 2:Slow_Mode, 4:FastAuto_Mode, 5:SlowAuto_Mode, 7:UNCHANGED, Others:None) */

	if ((mode == 1) || (mode == 4))
		hs_mode = true;
	else if ((mode == 2) || (mode == 5))
		hs_mode = false;
	else {
		ret = -71;
		goto out;
	}

	if ((mode == 4) || (mode == 5))
		auto_mode = true;
	else
		auto_mode = false;

	set_TxRx_Gear(host, gear);

	if (hs_mode)
		set_HSSeries(host, rate);

	if (host->debug) {
		printk(BIOS_ERR, "mode : %d\n", mode);
		printk(BIOS_ERR, "gear : %d\n", gear);
		printk(BIOS_ERR, "rate : %d\n", rate);
	}

	pwr_mode = (mode & 0xF) << 4 | (mode & 0xF);
	ufs_store_uic_pwr_mode(host, gear, mode, rate);
	ufs_pre_pmc_cfg(host);

	if (!set_PA_PWRMode(host, pwr_mode)) {
		ret = -72;
		goto out;
	}

	UFS_WAIT_CLEAR_INT(host, UFSHCI_IS_UPMS);

	if (read_unipro_powermode_ind_result(host) != UFS_UPMCRS_PWR_LOCAL) {
		UFS_WAIT_CLEAR_INT(host, UFSHCI_IS_UPMS);
		if (read_unipro_powermode_ind_result(host) != UFS_UPMCRS_PWR_LOCAL)	{
			printk(BIOS_ERR, "1.POWERMODE Fail(DME_POWERMODE_IND_RESULT:0x%x)\n", read_unipro_powermode_ind_result(host));
			printk(BIOS_ERR, "2.POWERMODE Fail(DME_POWERMODE_IND_RESULT:0x%x)\n", read_unipro_powermode_ind_result(host));
			printk(BIOS_ERR, "3.POWERMODE Fail(DME_POWERMODE_IND_RESULT:0x%x)\n", read_unipro_powermode_ind_result(host));
			ret = -73;
			goto out;
		}
	}

	ufs_set_after_pmc_done(host, hs_mode, gear, rate);

out:

	if (ret)
		printk(BIOS_ERR, "ufs_power_mode_change: fail %d\n", ret);

	return ret;
}
u32 ufs_get_num_of_lanes(struct ufs_host *host)
{
	return host->config->hci_config_info.numoflane;
}

u32 ufs_get_num_of_active_tx_lanes(struct ufs_host *host)
{
	struct ufs_uic_pwr_mode *pinfo;

	pinfo = ufs_get_uic_pwr_mode(host);

	return pinfo->active_tx_data_lanes;
}

u32 ufs_get_num_of_active_rx_lanes(struct ufs_host *host)
{
	struct ufs_uic_pwr_mode *pinfo;

	pinfo = ufs_get_uic_pwr_mode(host);

	return pinfo->active_rx_data_lanes;
}

// UTRD functions

u32 swap_endian(u32 data)
{
	u32 d0, d1, d2, d3;

	d3 = (data & 0x000000FFul) << 24;
	d2 = (data & 0x0000FF00ul) << 8;
	d1 = (data & 0x00FF0000ul) >> 8;
	d0 = (data & 0xFF000000ul) >> 24;

	return (d3 | d2 | d1 | d0);
}

u16 swap_endian_word(u16 word)
{
	u16 d0, d1;

	d1 = (word & 0x0000FFul) << 8;
	d0 = (word & 0x00FF00ul) >> 8;

	return (d1 | d0);
}
u32 ufs_get_lba(struct ufs_host *host)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	return pdes->lba;
}

//static inline void ufs_set_lba(struct ufs_host *host, u32 lba)
void ufs_set_lba(struct ufs_host *host, u32 lba)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	pdes->lba = lba;
}


u32 ufs_get_num_lba(struct ufs_host *host)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	return pdes->num_lba;
}

//static inline void ufs_set_num_lba(struct ufs_host *host, u32 num_lba)
void ufs_set_num_lba(struct ufs_host *host, u32 num_lba)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	pdes->num_lba = num_lba;
}

//static inline void ufs_set_fua(struct ufs_host *host, bool fua)
void ufs_set_fua(struct ufs_host *host, bool fua)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	pdes->fua = fua;
}

bool ufs_get_fua(struct ufs_host *host)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	return pdes->fua;
}

void ufs_set_dpo(struct ufs_host *host, bool dpo)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	pdes->dpo = dpo;
}

bool ufs_get_dpo(struct ufs_host *host)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	return pdes->dpo;
}

static void ufshci_saved_doorbell(struct ufs_host *host)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	pdes->save_doorbell |= ((0x1ul << (pdes->doorbell)) & 0xFFFF);
}

static bool ufshci_check_utrlrsr(struct ufs_host *host)
{
	return GET_BITS(UFS_READ(UFSHCI_UTRLRSR), 0, 1) ? true : false;
}

static bool ufs_get_doorbellstatus(struct ufs_host *host)
{
	u32 db;

	db = ufs_get_doorbell_num(host);

	return GET_BITS(UFS_READ(UFSHCI_UTRLDBR), db, 1) ? true : false;
}
/**
 * UFS Commands
 */


bool UFSHCI_CheckOCS(struct ufs_host *host)
{
	struct ufs_des_info *pdes;
	utrd_t *putrd;
	ucd_t *pucd;
	u32 uIdx;

	pdes = ufs_get_descriptor_info(host);
	uIdx = pdes->doorbell;

	putrd = &(host->utrd[uIdx]);
	pucd = &(host->ucd[uIdx]);

	if (pucd->response_upiu.device_info) {
		printk(BIOS_ERR, "\n**********************************");
		printk(BIOS_ERR, "\n** [Caution] Check!!            **");
		printk(BIOS_ERR, "\n** Response UPIU                **");
		printk(BIOS_ERR, "\n** Device Information = 1       **");
		printk(BIOS_ERR, "\n**********************************");
	}
	if (putrd->dw2.ocs == 0x00)
		return true;

	u8 ocs;

	ocs = putrd->dw2.ocs;
	switch (ocs) {
	case 1:
		printk(BIOS_ERR, "\n[INVALID_COMMAND_TABLE_ATTRIBUTES]\n");
		break;

	case 2:
		printk(BIOS_ERR, "\n[INVALID_PRDT_ATTRIBUTES]\n");
		break;

	case 3:
		printk(BIOS_ERR, "\n[MISMATCH_DATA_BUFFER_SIZE]\n");
		break;

	case 4:
		printk(BIOS_ERR, "\n[MISMATCH_RESPONSE_UPIU_SIZE]\n");
		break;

	case 5:
		printk(BIOS_ERR, "\n[PEER_COMMUNICATION_FAILURE]\n");
		break;

	case 6:
		printk(BIOS_ERR, "\n[ABORTED]\n");
		break;

	case 7:
		//if (PRINT_LOG)  /*** WHY ****/
		printk(BIOS_ERR, "\n[FATAL_ERROR]\n");
		break;

	case 8:
		printk(BIOS_ERR, "\n[DEVICE FATAL ERROR]");
		break;

	case 9:
		printk(BIOS_ERR, "\n[INVALID CRYPTO CONFIGURATION]");
		break;

	case 10:
		printk(BIOS_ERR, "\n[GENERAL CRYPTO ERROR]");
		break;

	case 15:
		printk(BIOS_ERR, "\n[INVALID_OCS_VALUE]\n");
		break;

	default:
		printk(BIOS_ERR, "\n[RESERVED]\n");
		break;
	}

	/*TODO : add the checksensedata funtion*/
	//Check Sense Data
	//UFSHCI_CheckSenseData(&(pucd->TransferResponse));

	return false;
}

// Check Response Field value of RESPONSE/REJECT UPIU
static bool Check_ResponseUPIU(struct ufs_host *host)
{
	ucd_t *pucd;
	u32 uIdx;
	bool res1 = true, res2 = true;

	uIdx = ufs_get_doorbell_num(host);

	pucd = &(host->ucd[uIdx]);

	if (pucd->response_upiu.response) {
		printk(BIOS_ERR, "\n[Response/Reject UPIU(Response Field Error : 0x%x)]\n", pucd->response_upiu.response);
		res1 = false;
	}

	if (pucd->response_upiu.status) {
		printk(BIOS_ERR, "\n[Response/Reject UPIU(Status Field Error : 0x%x)]\n", pucd->response_upiu.status);
		res2 = false;
	}

	if ((res1 == false) || (res2 == false))
		return false;

	return true;
}

static int ufs_wait_done(struct ufs_host *host)
{
	int ret = 0;

	if (!ufshci_wait_complete(host, UFSHCI_IS_UTRCS)) {
		ret = -81;
		goto out;
	}

	ufshci_waitcomplete_doorbell(host);

	if (!UFSHCI_CheckOCS(host)) {
		if (host->debug) {
			printk(BIOS_ERR, "\nocs Error!!!");
			Check_ResponseUPIU(host);
		}
		ret = -82;
		goto out;
	}

	if (!Check_ResponseUPIU(host)) {
		ret = -83;
		goto out;
	}

out:
	if (ret)
		printk(BIOS_ERR, "ufs_wait_done: fail %d\n", ret);

	return ret;
}

void ufshci_waitcomplete_doorbell(struct ufs_host *host)
{
	u32 timeout = 10000;

	while (--timeout) {
		if (!ufs_get_doorbellstatus(host))
			break;
		udelay(10);
	}

	if (!timeout)
		printk(BIOS_ERR, "%s: timeout!!!\n", __func__);
}


static int ufs_nop_auto(struct ufs_host *host)
{
	ufs_set_doorbell_num(host, 0x0);
	ufs_set_tag_num(host, 0x0);
	ufs_clear_utr_nexus_type(host);
	ufs_nop_descriptor(host);

	return ufs_issue_cmd(host);
}

u32 ufs_get_doorbell_num(struct ufs_host *host)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);
	return pdes->doorbell;
}

void ufs_set_doorbell_num(struct ufs_host *host, u32 db)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	pdes->doorbell = db;
}

void ufs_set_lun_num(struct ufs_host *host, u32 nlun)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	pdes->lun = nlun;
}

u32 ufs_get_lun_num(struct ufs_host *host)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	return pdes->lun;
}
u32 ufs_get_tag_num(struct ufs_host *host)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	return pdes->tag;
}

void ufs_set_tag_num(struct ufs_host *host, u32 ntag)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	pdes->tag = ntag;
}

u32 ufs_get_qr_flag_num(struct ufs_host *host)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	return pdes->qr_flag;
}

void ufs_set_qr_flag_num(struct ufs_host *host, u32 nflag)
{
	struct ufs_des_info *pdes;

	pdes = ufs_get_descriptor_info(host);

	pdes->qr_flag = nflag;
}

/**
 *  four phase of UFS
 *  1. prepare UCD/UTRD/PRDT
 *  2. ring doorbel
 *  3. wait interrupt
 *  4. post process (check respsone and
 */
/* 0: success */
static int ufs_issue_cmd(struct ufs_host *host)
{
	int ret = 0;

	ret = ufshci_send_request(host);
	if (ret) {
		ret = -91;
		goto out;
	}

	ret = ufs_wait_done(host);
	if (ret) {
		ret = -92;
		goto out;
	}

out:
	if (ret)
		printk(BIOS_ERR, "ufs_issue_cmd: fail %d\n", ret);

	return ret;
}

/* 0: successfull : otherwise wrong */
static int ufs_issue_query(struct ufs_host *host)
{
	int ret = 0;

	ret = ufs_issue_cmd(host);
	if (ret) {
		ret = -93;
		goto out;
	}

out:
	if (ret)
		printk(BIOS_ERR, "ufs_issue_query: fail %d\n", ret);

	return ret;
}

static int ufshci_send_request(struct ufs_host *host)
{
	int ret = 0;
	struct ufs_host_config *config;
	u32 db;
	u32 bit_mask;

	config = ufs_get_host_config_info(host);
	db = ufs_get_doorbell_num(host);

	bit_mask = 1ul << db;

	if (!ufshci_check_utrlrsr(host)) {
		ret = -101;
		goto out;
	}

	if (UFS_READ(UFSHCI_UTRLDBR) & bit_mask) {
		//printk(BIOS_ERR, "\n Check the doorbells. Thedoorbell had already been set. :  %d ", bit_mask);
		ret = -102;
		goto out;
	}

	if (config->dma_blocking)
		ufs_vs_dma_blocking_enable(host, true);
	dsb();

	UFS_WRITE(UFSHCI_UTRLDBR, bit_mask);
	if (config->dma_blocking) {
		ufs_sync_desc_db(host, db);
#if 0 /* Don't do data and clean invalidate here till we find out the reason why PRDT is hot happy */
		ufshci_data_clean_and_invalidate(host, db);
#endif
		ufs_vs_dma_blocking_enable(host, false);
	}
	ufshci_saved_doorbell(host);

out:
	if (ret)
		printk(BIOS_ERR, "ufshci_send_request: fail %d\n", ret);

	return ret;
}

void ufshci_ucd_invalidatecache(struct ufs_host *host, u32 db)
{
	inv_dcache_range((u64)&(host->ucd[db]), sizeof(ucd_t));

}

void ufshci_ucd_clean_and_invalidate(struct ufs_host *host, u32 db)
{
	clean_dcache_range((u64)&(host->ucd[db]), sizeof(ucd_t));
	inv_dcache_range((u64)&(host->ucd[db]), sizeof(ucd_t));
}

#if 0
void ufshci_data_clean_and_invalidate(struct ufs_host *host, u32 db)
{
	prdt_fmp *prdt_3;
	prdt_t *prdt_0;
	int i;

	if (host->fmp_type == 3) {
		prdt_3 = (prdt_fmp *) host->ucd[db].prdt;
		for (i = 0; prdt_3[i].data_bytes && i < MAX_NUM_OF_PRDT; i++) {
			clean_dcache_range((u64) (prdt_3[i].base),
					prdt_3[i].data_bytes);
			inv_dcache_range((u64) (prdt_3[i].base),
					prdt_3[i].data_bytes);
		}

	} else {
		prdt_0 = (prdt_t *) host->ucd[db].prdt;
		for (i = 0; prdt_0[i].data_bytes && i < MAX_NUM_OF_PRDT; i++) {
			clean_dcache_range((u64)(prdt_0[i].base), prdt_0[i].data_bytes);
			inv_dcache_range((u64)(prdt_0[i].base), prdt_0[i].data_bytes);
		}
	}
}
#endif

void ufs_sync_desc_db(struct ufs_host *host, u32 db)
{
	ufshci_utrd_clean_and_invalidate(host, db);
	ufshci_ucd_clean_and_invalidate(host, db);
}

void ufs_sync_desc(struct ufs_host *host, UFS_DCACHE_CTRL eFlush, u32 db)
{
	switch (eFlush) {
	case UFS_UTRD_CLEAN_DCACHE:
		ufshci_utrd_cleancache(host, db);
		break;

	case UFS_UTRD_INVALIDATE_DCACHE:
		ufshci_utrd_invalidatecache(host, db);
		break;

	case UFS_UCD_CLEAN_DCACHE:
		ufshci_ucd_cleancache(host, db);
		break;

	case UFS_UCD_INVALIDATE_DCACHE:
		ufshci_ucd_invalidatecache(host, db);
		break;

	default:
		printk(BIOS_ERR, "\n DCACHE Control Select Error!");
		break;
	}
}


void ufshci_ucd_cleancache(struct ufs_host *host, u32 db)
{
	clean_dcache_range((u64)&(host->ucd[db]), sizeof(ucd_t));
}
void ufshci_utrd_cleancache(struct ufs_host *host, u32 db)
{
	clean_dcache_range((u64)&(host->utrd[db]), sizeof(utrd_t));
}

void ufshci_utrd_invalidatecache(struct ufs_host *host, u32 db)
{
	inv_dcache_range((u64)&(host->utrd[db]), sizeof(utrd_t));
}

void ufshci_utrd_clean_and_invalidate(struct ufs_host *host, u32 db)
{
	clean_dcache_range((u64)&(host->utrd[db]), sizeof(utrd_t));
	inv_dcache_range((u64)&(host->utrd[db]), sizeof(utrd_t));
}

#if IS_ENABLED(CONFIG_BOOTBLOCK_X)
static int ufs_write_auto(struct ufs_host *host, u32 lun, u32 lba, u32 num_lba, void *buf)
{
	ufs_set_doorbell_num(host, 0x6);
	ufs_set_lun_num(host, lun);
	ufs_set_tag_num(host, 0xA);

	ufs_set_lba(host, lba);
	ufs_set_num_lba(host, num_lba);
	ufs_set_dpo(host, true);
	ufs_set_fua(host, true);

	ufs_set_utr_nexus_type(host);
	ufs_setwrite10command(host, buf);

	return ufs_issue_cmd(host);
}
#endif

static int ufs_read_auto(struct ufs_host *host, u32 lun, u32 lba, u32 num_lba, void *buf)
{
	ufs_set_doorbell_num(host, 0x7);
	ufs_set_lun_num(host, lun);
	ufs_set_tag_num(host, 0xB);

	ufs_set_lba(host, lba);
	ufs_set_num_lba(host, num_lba);
	ufs_set_fua(host, false);

	ufs_set_utr_nexus_type(host);
	ufs_setread10command(host, buf);

	return ufs_issue_cmd(host);
}

#if 0
u32 ufs_tc_wr_seq(struct ufs_host *host)
{
	unsigned char *wr_buf;
	unsigned char *rd_buf;
	unsigned int size;
	int start_lba;
	int num_lba;
	int i;
	int lun = UFS_USER_LUN;
	bool res = true;

#ifdef ZEBU
	start_lba = 50;
#else
	start_lba = 0xc0000 + (rand() % 0x500000);
#endif

	num_lba = 512 / 4;
	size = 4096 * num_lba;

	wr_buf = (unsigned char *)UFS_WRITE_BUF_ADDR;
	rd_buf = (unsigned char *)UFS_READ_BUF_ADDR;

	for (i = 0; i < size; i++) {
		wr_buf[i++] = 0x4A;
		wr_buf[i++] = 0x4A;
		wr_buf[i++] = 0x49;
		wr_buf[i++] = 0x4A;
		wr_buf[i] = 0x4A;
	}

	if (!host->sysreg_enable) {
		clean_dcache_range((u64)(uintptr_t)wr_buf, sizeof(u64)*size);
		inv_dcache_range((u64)(uintptr_t)wr_buf, sizeof(u64)*size);
	}

	printk(BIOS_ERR, "write_auto\n");
	res = ufs_write_auto(host, lun, start_lba, num_lba, (u64 *)wr_buf);
	if (res == 0) {
		printk(BIOS_ERR, "error: write blocks\n");
		res = false;
		goto out;
	}

	printk(BIOS_ERR, "Going for Reading...\n ");
	if (!host->sysreg_enable) {
		clean_dcache_range((u64)(uintptr_t)rd_buf, sizeof(u64)*size);
		inv_dcache_range((u64)(uintptr_t)rd_buf, sizeof(u64)*size);
	}

	res = ufs_read_auto(host, lun, start_lba, num_lba, (u64 *)rd_buf);

	if (!host->sysreg_enable)
		inv_dcache_range((u64)(uintptr_t)rd_buf, sizeof(u64)*size);
	if (res == 0) {
		printk(BIOS_ERR, "error: read blocks\n");
		res = false;
		goto out;
	}

	if (memcmp(wr_buf, rd_buf, size)) {
		printk(BIOS_ERR, "error: compare error\n");
		res = false;
		goto out;
	}


out:
	free(wr_buf);
	free(rd_buf);
	printk(BIOS_ERR, "Write/Read\n");
	return res;
}
#endif

static int ufs_load_from_ufs(struct ufs_host *host, u32 lun, u32 pos_4k, unsigned char *pReadBuf, u32 size)
{
	u32 uFW_NumOfBlockInLoop;
	u32 uFW_NumOfBlock;
	u32 uFW_RemainBlock;
	u32 uFW_Write_Unit_Size;
	u32 uFW_Size;
	u32 uLoop;
	u32 uLUN_FW;
	//unsigned char * pReadBuf;
	u32 offset = pos_4k;
	int ret = 0;

	uLUN_FW = lun;// 0x0;						// User partition
	//uFW_Size = 2 * 1024 * 1024 * 1024;				// 2  GByte
	uFW_Size = size; //4 * 1024 * 1024;//0x175AE;//0x80000000;	// 2  GByte
	uFW_Write_Unit_Size = size; //4 * 1024 * 1024; 			// 4MB

	/* limit lba to MAX_NUM_OF_PRDT, an that is num_prdt later */
	uFW_Write_Unit_Size &= 0xfffff000;
	if (uFW_Write_Unit_Size > (MAX_NUM_OF_PRDT * 4096))  /* 4MiB or 128KiB */
		uFW_Write_Unit_Size = (MAX_NUM_OF_PRDT * 4096);

	uFW_NumOfBlockInLoop = UFS_DIVIDE(uFW_Write_Unit_Size, 4096);
	uFW_NumOfBlock = UFS_DIVIDE(uFW_Size, uFW_Write_Unit_Size);
	uFW_RemainBlock = UFS_REMAINDER(uFW_Size, uFW_Write_Unit_Size);
	if (uFW_RemainBlock)
		uFW_NumOfBlock++;

	memset((unsigned char *)(uintptr_t)pReadBuf/*UFS_CBFS_LOAD_ADDR*/, 0x00, uFW_Write_Unit_Size);
	for (uLoop = 0; uLoop < uFW_NumOfBlock; uLoop++) {


		if (!host->sysreg_enable) {
			clean_dcache_range((u64)(uintptr_t)pReadBuf, uFW_Write_Unit_Size);
			inv_dcache_range((u64)(uintptr_t)pReadBuf, uFW_Write_Unit_Size);
		}

		ret = ufs_read_auto(host, uLUN_FW, offset, uFW_NumOfBlockInLoop, pReadBuf);
		if (ret)
			return ret;

		if (!host->sysreg_enable)
			inv_dcache_range((u64)(uintptr_t)pReadBuf, uFW_Write_Unit_Size);

		//ufshci_print_buf_align((unsigned long int)pReadBuf, uTestSize, true);
		udelay(1000);

		offset += uFW_NumOfBlockInLoop;
		pReadBuf += uFW_Write_Unit_Size;
	}

	return ret;
}

void load_from_ufs(unsigned char *buffer, u32 lun, u32 pos_4k, u32 in_size)
{
	struct ufs_host *host = ufs;
	int ret;

#if !IS_ENABLED(CONFIG_BOOTBLOCK_X)
	printk(BIOS_INFO, "load_from_ufs: LUN %d [%lx,%lx] to [%lx, %lx] starting...\n", lun,
			((unsigned long)pos_4k) << 12, (((unsigned long)pos_4k) << 12) + in_size - 1,
			(unsigned long)buffer, (unsigned long)buffer + in_size - 1);
#endif

	ret = ufs_load_from_ufs(host, lun, pos_4k, buffer, in_size);

	printk(BIOS_INFO, "load_from_ufs: LUN %d [%lx,%lx] to [%lx, %lx] status=%d\n", lun,
			((unsigned long)pos_4k) << 12, (((unsigned long)pos_4k) << 12) + in_size - 1,
			(unsigned long)buffer, (unsigned long)buffer + in_size - 1, ret);
}

#if IS_ENABLED(CONFIG_BOOTBLOCK_X)
static int ufs_save_to_ufs(struct ufs_host *host, u32 lun, u32 pos_4k, unsigned char *pWriteBuf, u32 size)
{
	u32 uFW_NumOfBlockInLoop;
	u32 uFW_NumOfBlock;
	u32 uFW_RemainBlock;
	u32 uFW_Write_Unit_Size;
	u32 uFW_Size;
	u32 uLoop;
	u32 uLUN_FW;
	//unsigned char * pReadBuf;
	u32 offset = pos_4k;
	int ret = 0;

	uLUN_FW = lun;// 0x0;						// User partition
	//uFW_Size = 2 * 1024 * 1024 * 1024;				// 2  GByte
	uFW_Size = size; //4 * 1024 * 1024;//0x175AE;//0x80000000;	// 2  GByte
	uFW_Write_Unit_Size = size; //4 * 1024 * 1024;			// 4MB

	/* limit lba to MAX_NUM_OF_PRDT, an that is num_prdt later */
	uFW_Write_Unit_Size &= 0xfffff000;
	if (uFW_Write_Unit_Size > (MAX_NUM_OF_PRDT * 4096))  /* 4MiB or 128KiB */
		uFW_Write_Unit_Size = (MAX_NUM_OF_PRDT * 4096);

	uFW_NumOfBlockInLoop = UFS_DIVIDE(uFW_Write_Unit_Size, 4096);
	uFW_NumOfBlock = UFS_DIVIDE(uFW_Size, uFW_Write_Unit_Size);
	uFW_RemainBlock = UFS_REMAINDER(uFW_Size, uFW_Write_Unit_Size);
	if (uFW_RemainBlock)
		uFW_NumOfBlock++;

	for (uLoop = 0; uLoop < uFW_NumOfBlock; uLoop++) {

		if (!host->sysreg_enable) {
			clean_dcache_range((u64)(uintptr_t)pWriteBuf, uFW_Write_Unit_Size);
			inv_dcache_range((u64)(uintptr_t)pWriteBuf, uFW_Write_Unit_Size);
		}

		//ufshci_print_buf_align((unsigned long int)pWriteBuf, 512, true);
		ret = ufs_write_auto(host, uLUN_FW, offset, uFW_NumOfBlockInLoop, pWriteBuf);
		if (ret)
			return ret;
#if 0
		if (!host->sysreg_enable)
			inv_dcache_range((u64)(uintptr_t)pWriteBuf, uFW_Write_Unit_Size);
#endif

		mdelay(20);

		offset += uFW_NumOfBlockInLoop;
		pWriteBuf += uFW_Write_Unit_Size;
	}

	return ret;
}

void save_to_ufs(unsigned char *buffer, u32 lun, u32 pos_4k, u32 out_size)
{
	struct ufs_host *host = ufs;
	int ret;

//	printk(BIOS_INFO, "save_to_ufs: [%lx,%lx] to LUN %d [%lx, %lx] starting...\n",
//			(unsigned long)buffer, (unsigned long)buffer + out_size - 1,
//			lun, ((unsigned long)pos_4k) << 12, (((unsigned long)pos_4k) << 12) + out_size - 1);

	ret = ufs_save_to_ufs(host, lun, pos_4k, buffer, out_size);

	printk(BIOS_INFO, "save_to_ufs: [%lx,%lx] to LUN %d [%lx, %lx] status=%d\n",
			(unsigned long)buffer, (unsigned long)buffer + out_size - 1,
			lun, ((unsigned long)pos_4k) << 12, (((unsigned long)pos_4k) << 12) + out_size - 1, ret);
}
#endif
