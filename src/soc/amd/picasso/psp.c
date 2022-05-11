/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <device/mmio.h>
#include <soc/psp.h>
#include <soc/smi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/psp.h>
#include <string.h>

#define PSP_DEBUG_OFFSET		0x10500
#define PSP_MAILBOX_OFFSET		0x10570
#define MSR_CU_CBBCFG			0xc00110a2

void *soc_get_mbox_address(void)
{
	uintptr_t psp_mmio;

	psp_mmio = rdmsr(MSR_CU_CBBCFG).lo;
	if (psp_mmio == 0xffffffff) {
		printk(BIOS_WARNING, "PSP: MSR_CU_CBBCFG uninitialized\n");
		return 0;
	}

	return (void *)(psp_mmio + PSP_MAILBOX_OFFSET);
}

int soc_get_debug_info(struct soc_debug_info *info)
{
	uintptr_t psp_mmio;
	u32 *c2pmsg;

	psp_mmio = rdmsr(MSR_CU_CBBCFG).lo;
	if (psp_mmio == 0xffffffff) {
		printk(BIOS_WARNING, "PSP: MSR_CU_CBBCFG uninitialized\n");
		return -1;
	}

	c2pmsg = (u32 *)(psp_mmio + PSP_DEBUG_OFFSET);

	// Increment if new fields are added.
	info->version = 1;

	info->mp0_c2pmsg_0 = read32(c2pmsg);
	info->mp0_c2pmsg_1 = read32(c2pmsg + 1);
	info->mp0_c2pmsg_2 = read32(c2pmsg + 2);
	info->mp0_c2pmsg_3 = read32(c2pmsg + 3);
	info->mp0_c2pmsg_4 = read32(c2pmsg + 4);
	info->mp0_c2pmsg_5 = read32(c2pmsg + 5);
	info->mp0_c2pmsg_6 = read32(c2pmsg + 6);
	info->mp0_c2pmsg_7 = read32(c2pmsg + 7);
	info->mp0_c2pmsg_8 = read32(c2pmsg + 8);
	info->mp0_c2pmsg_9 = read32(c2pmsg + 9);
	info->mp0_c2pmsg_10 = read32(c2pmsg + 10);
	info->mp0_c2pmsg_11 = read32(c2pmsg + 11);
	info->mp0_c2pmsg_12 = read32(c2pmsg + 12);
	info->mp0_c2pmsg_13 = read32(c2pmsg + 13);
	info->mp0_c2pmsg_14 = read32(c2pmsg + 14);
	info->mp0_c2pmsg_15 = read32(c2pmsg + 15);

	return 0;
}

void soc_fill_smm_trig_info(struct smm_trigger_info *trig)
{
	if (!trig)
		return;

	trig->address = (uintptr_t)acpimmio_smi + SMI_REG_SMITRIG0;
	trig->address_type = SMM_TRIGGER_MEM;
	trig->value_width = SMM_TRIGGER_DWORD;
	trig->value_and_mask = ~SMITRG0_FAKESTS0;
	trig->value_or_mask = SMITRG0_FAKESTS0;
}

void soc_fill_smm_reg_info(struct smm_register_info *reg)
{
	if (!reg)
		return;

	reg->smi_enb.address = (uintptr_t)acpimmio_smi + SMI_REG_SMITRIG0;
	reg->smi_enb.address_type = SMM_TRIGGER_MEM;
	reg->smi_enb.value_width = SMM_TRIGGER_DWORD;
	reg->smi_enb.reg_bit_mask = SMITRG0_SMIENB;
	reg->smi_enb.expect_value = 0;

	reg->eos.address = (uintptr_t)acpimmio_smi + SMI_REG_SMITRIG0;
	reg->eos.address_type = SMM_TRIGGER_MEM;
	reg->eos.value_width = SMM_TRIGGER_DWORD;
	reg->eos.reg_bit_mask = SMITRG0_EOS;
	reg->eos.expect_value = SMITRG0_EOS;

	reg->psp_smi_en.address = (uintptr_t)acpimmio_smi + SMI_REG_CONTROL0;
	reg->psp_smi_en.address += sizeof(uint32_t) * SMITYPE_PSP / 16;
	reg->psp_smi_en.address_type = SMM_TRIGGER_MEM;
	reg->psp_smi_en.value_width = SMM_TRIGGER_DWORD;
	reg->psp_smi_en.reg_bit_mask = SMI_MODE_MASK << (2 * SMITYPE_PSP % 16);
	reg->psp_smi_en.expect_value = SMI_MODE_SMI << (2 * SMITYPE_PSP % 16);
}
