/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <bootstate.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/smm.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <acpi/acpi_gnvs.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/reset.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/espi.h>
#include <amdblocks/lpc.h>
#include <amdblocks/acpi.h>
#include <amdblocks/spi.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/i2c.h>
#include <soc/southbridge.h>
#include <soc/smi.h>
#include <soc/uart.h>
#include <soc/amd_pci_int_defs.h>
#include <soc/pci.h>
#include <soc/pci_devs.h>
#include <soc/nvs.h>
#include <types.h>
#include "chip.h"

/*
 * Table of APIC register index and associated IRQ name. Using IDX_XXX_NAME
 * provides a visible association with the index, therefore helping
 * maintainability of table. If a new index/name is defined in
 * amd_pci_int_defs.h, just add the pair at the end of this table.
 * Order is not important.
 */
const static struct irq_idx_name irq_association[] = {
	{ PIRQ_A,	"INTA#" },
	{ PIRQ_B,	"INTB#" },
	{ PIRQ_C,	"INTC#" },
	{ PIRQ_D,	"INTD#" },
	{ PIRQ_E,	"INTE#" },
	{ PIRQ_F,	"INTF#/GENINT2" },
	{ PIRQ_G,	"INTG#" },
	{ PIRQ_H,	"INTH#" },
	{ PIRQ_MISC,	"Misc" },
	{ PIRQ_MISC0,	"Misc0" },
	{ PIRQ_MISC1,	"Misc1" },
	{ PIRQ_MISC2,	"Misc2" },
	{ PIRQ_SIRQA,	"Ser IRQ INTA" },
	{ PIRQ_SIRQB,	"Ser IRQ INTB" },
	{ PIRQ_SIRQC,	"Ser IRQ INTC" },
	{ PIRQ_SIRQD,	"Ser IRQ INTD" },
	{ PIRQ_SCI,	"SCI" },
	{ PIRQ_SMBUS,	"SMBUS" },
	{ PIRQ_ASF,	"ASF" },
	{ PIRQ_PMON,	"PerMon" },
	{ PIRQ_SD,	"SD" },
	{ PIRQ_SDIO,	"SDIO" },
	{ PIRQ_CIR,	"CIR" },
	{ PIRQ_GPIOA,	"GPIOa" },
	{ PIRQ_GPIOB,	"GPIOb" },
	{ PIRQ_GPIOC,	"GPIOc" },
	{ PIRQ_SATA,	"SATA" },
	{ PIRQ_EMMC,	"eMMC" },
	{ PIRQ_GPP0,	"GPP0" },
	{ PIRQ_GPP1,	"GPP1" },
	{ PIRQ_GPP2,	"GPP2" },
	{ PIRQ_GPP3,	"GPP3" },
	{ PIRQ_GPIO,	"GPIO" },
	{ PIRQ_I2C0,	"I2C0" },
	{ PIRQ_I2C1,	"I2C1" },
	{ PIRQ_I2C2,	"I2C2" },
	{ PIRQ_I2C3,	"I2C3" },
	{ PIRQ_UART0,	"UART0" },
	{ PIRQ_UART1,	"UART1" },
	{ PIRQ_I2C4,	"I2C4" },
	{ PIRQ_I2C5,	"I2C5" },
	{ PIRQ_UART2,	"UART2" },
	{ PIRQ_UART3,	"UART3" },
};

const struct irq_idx_name *sb_get_apic_reg_association(size_t *size)
{
	*size = ARRAY_SIZE(irq_association);
	return irq_association;
}

static void sb_enable_cf9_io(void)
{
	uint32_t reg = pm_read32(PM_DECODE_EN);

	pm_write32(PM_DECODE_EN, reg | CF9_IO_EN);
}

static void sb_enable_legacy_io(void)
{
	uint32_t reg = pm_read32(PM_DECODE_EN);

	pm_write32(PM_DECODE_EN, reg | LEGACY_IO_EN);
}

void sb_clk_output_48Mhz(void)
{
	u32 ctrl;

	ctrl = misc_read32(MISC_CLK_CNTL1);
	ctrl |= BP_X48M0_OUTPUT_EN;
	misc_write32(MISC_CLK_CNTL1, ctrl);
}

static void fch_smbus_init(void)
{
	/* 400 kHz smbus speed. */
	const uint8_t smbus_speed = (66000000 / (400000 * 4));

	pm_write8(SMB_ASF_IO_BASE, SMB_BASE_ADDR >> 8);
	smbus_write8(SMBTIMING, smbus_speed);
	/* Clear all SMBUS status bits */
	smbus_write8(SMBHSTSTAT, SMBHST_STAT_CLEAR);
	smbus_write8(SMBSLVSTAT, SMBSLV_STAT_CLEAR);
	asf_write8(SMBHSTSTAT, SMBHST_STAT_CLEAR);
	asf_write8(SMBSLVSTAT, SMBSLV_STAT_CLEAR);
}

static void lpc_configure_decodes(void)
{
	if (CONFIG(POST_IO) && (CONFIG_POST_IO_PORT == 0x80))
		lpc_enable_port80();
}

/* Before console init */
void fch_pre_init(void)
{
	lpc_early_init();

	if (!CONFIG(SOC_AMD_COMMON_BLOCK_USE_ESPI))
		lpc_configure_decodes();

	fch_spi_early_init();
	enable_acpimmio_decode_pm04();
	fch_smbus_init();
	sb_enable_cf9_io();
	sb_enable_legacy_io();
	enable_aoac_devices();
	sb_reset_i2c_slaves();

	/*
	 * On reset Range_0 defaults to enabled. We want to start with a clean
	 * slate to not have things unexpectedly enabled.
	 */
	clear_uart_legacy_config();

	if (CONFIG(PICASSO_CONSOLE_UART))
		set_uart_config(CONFIG_UART_FOR_CONSOLE);
}

static void print_num_status_bits(int loglevel, int num_bits, uint32_t status,
				  const char *const bit_names[])
{
	int i;

	if (!status)
		return;

	for (i = num_bits - 1; i >= 0; i--) {
		if (status & (1 << i)) {
			if (bit_names[i])
				printk(loglevel, "%s ", bit_names[i]);
			else
				printk(loglevel, "BIT%d ", i);
		}
	}
}

static void sb_print_pmxc0_status(void)
{
	/* PMxC0 S5/Reset Status shows the source of previous reset. */
	uint32_t pmxc0_status = pm_read32(PM_RST_STATUS);

	static const char *const pmxc0_status_bits[32] = {
		[0] = "ThermalTrip",
		[1] = "FourSecondPwrBtn",
		[2] = "Shutdown",
		[3] = "ThermalTripFromTemp",
		[4] = "RemotePowerDownFromASF",
		[5] = "ShutDownFan0",
		[16] = "UserRst",
		[17] = "SoftPciRst",
		[18] = "DoInit",
		[19] = "DoReset",
		[20] = "DoFullReset",
		[21] = "SleepReset",
		[22] = "KbReset",
		[23] = "LtReset",
		[24] = "FailBootRst",
		[25] = "WatchdogIssueReset",
		[26] = "RemoteResetFromASF",
		[27] = "SyncFlood",
		[28] = "HangReset",
		[29] = "EcWatchdogRst",
	};

	printk(BIOS_NOTICE, "PMxC0 STATUS: 0x%x ", pmxc0_status);
	print_num_status_bits(BIOS_NOTICE,
			      ARRAY_SIZE(pmxc0_status_bits), pmxc0_status,
			      pmxc0_status_bits);
	printk(BIOS_NOTICE, "\n");
}

/* After console init */
void fch_early_init(void)
{
	sb_print_pmxc0_status();
	i2c_soc_early_init();

	if (CONFIG(DISABLE_SPI_FLASH_ROM_SHARING))
		lpc_disable_spi_rom_sharing();

	if (CONFIG(SOC_AMD_COMMON_BLOCK_USE_ESPI)) {
		espi_setup();
		espi_configure_decodes();
	}
}

/* Set BLINK pin (AGPIO11) rate. */
void fch_blink_rate(enum fch_blink_setting rate)
{
	uint32_t val;

	val = pm_read32(PM_MISC_CONTROL);
	val &= ~PM_MISC_CONTROL_BLINK_BITS;
	val |= (uint32_t) rate;

	pm_write32(PM_MISC_CONTROL, val);
}

void sb_enable(struct device *dev)
{
	printk(BIOS_DEBUG, "%s\n", __func__);
}

static void sb_init_acpi_ports(void)
{
	u32 reg;

	/* We use some of these ports in SMM regardless of whether or not
	 * ACPI tables are generated. Enable these ports indiscriminately.
	 */

	pm_write16(PM_EVT_BLK, ACPI_PM_EVT_BLK);
	pm_write16(PM1_CNT_BLK, ACPI_PM1_CNT_BLK);
	pm_write16(PM_TMR_BLK, ACPI_PM_TMR_BLK);
	pm_write16(PM_GPE0_BLK, ACPI_GPE0_BLK);

	if (CONFIG(HAVE_SMI_HANDLER)) {
		/* APMC - SMI Command Port */
		pm_write16(PM_ACPI_SMI_CMD, APM_CNT);
		configure_smi(SMITYPE_SMI_CMD_PORT, SMI_MODE_SMI);

		/* SMI on SlpTyp requires sending SMI before completion
		 * response of the I/O write.  The BKDG also specifies
		 * clearing ForceStpClkRetry for SMI trapping.
		 */
		reg = pm_read32(PM_PCI_CTRL);
		reg |= FORCE_SLPSTATE_RETRY;
		pm_write32(PM_PCI_CTRL, reg);

		/* Disable SlpTyp feature */
		reg = pm_read8(PM_RST_CTRL1);
		reg &= ~SLPTYPE_CONTROL_EN;
		pm_write8(PM_RST_CTRL1, reg);

		configure_smi(SMITYPE_SLP_TYP, SMI_MODE_SMI);
	} else {
		pm_write16(PM_ACPI_SMI_CMD, 0);
	}

	/* Decode ACPI registers and enable standard features */
	pm_write8(PM_ACPI_CONF, PM_ACPI_DECODE_STD |
				PM_ACPI_GLOBAL_EN |
				PM_ACPI_RTC_EN_EN |
				PM_ACPI_TIMER_EN_EN);
}

static void set_nvs_sws(void *unused)
{
	struct chipset_state *state;
	struct global_nvs *gnvs;

	state = cbmem_find(CBMEM_ID_POWER_STATE);
	if (state == NULL)
		return;
	gnvs = acpi_get_gnvs();
	if (gnvs == NULL)
		return;

	acpi_fill_gnvs(gnvs, &state->gpe_state);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, set_nvs_sws, NULL);

/*
 * A-Link to AHB bridge, part of the AMBA fabric. These are internal clocks
 * and unneeded for Raven/Picasso so gate them to save power.
 */
static void al2ahb_clock_gate(void)
{
	uint8_t al2ahb_val;
	uintptr_t al2ahb_base = ALINK_AHB_ADDRESS;

	al2ahb_val = read8((void *)(al2ahb_base + AL2AHB_CONTROL_CLK_OFFSET));
	al2ahb_val |= AL2AHB_CLK_GATE_EN;
	write8((void *)(al2ahb_base + AL2AHB_CONTROL_CLK_OFFSET), al2ahb_val);
	al2ahb_val = read8((void *)(al2ahb_base + AL2AHB_CONTROL_HCLK_OFFSET));
	al2ahb_val |= AL2AHB_HCLK_GATE_EN;
	write8((void *)(al2ahb_base + AL2AHB_CONTROL_HCLK_OFFSET), al2ahb_val);
}

/* configure the genral purpose PCIe clock outputs according to the devicetree settings */
static void gpp_clk_setup(void)
{
	const struct soc_amd_picasso_config *cfg = config_of_soc();

	/* look-up table to be able to iterate over the PCIe clock output settings */
	const uint8_t gpp_clk_shift_lut[GPP_CLK_OUTPUT_COUNT] = {
		GPP_CLK0_REQ_SHIFT,
		GPP_CLK1_REQ_SHIFT,
		GPP_CLK2_REQ_SHIFT,
		GPP_CLK3_REQ_SHIFT,
		GPP_CLK4_REQ_SHIFT,
		GPP_CLK5_REQ_SHIFT,
		GPP_CLK6_REQ_SHIFT,
	};

	uint32_t gpp_clk_ctl = misc_read32(GPP_CLK_CNTRL);

	for (int i = 0; i < GPP_CLK_OUTPUT_COUNT; i++) {
		gpp_clk_ctl &= ~GPP_CLK_REQ_MASK(gpp_clk_shift_lut[i]);
		/*
		 * The remapping of values is done so that the default of the enum used for the
		 * devicetree settings is the clock being enabled, so that a missing devicetree
		 * configuration for this will result in an always active clock and not an
		 * inactive PCIe clock output.
		 */
		switch (cfg->gpp_clk_config[i]) {
		case GPP_CLK_REQ:
			gpp_clk_ctl |= GPP_CLK_REQ_EXT(gpp_clk_shift_lut[i]);
			break;
		case GPP_CLK_OFF:
			gpp_clk_ctl |= GPP_CLK_REQ_OFF(gpp_clk_shift_lut[i]);
			break;
		case GPP_CLK_ON:
		default:
			gpp_clk_ctl |= GPP_CLK_REQ_ON(gpp_clk_shift_lut[i]);
		}
	}

	misc_write32(GPP_CLK_CNTRL, gpp_clk_ctl);
}

void southbridge_init(void *chip_info)
{
	struct chipset_state *state;

	i2c_soc_init();
	sb_init_acpi_ports();

	state = cbmem_find(CBMEM_ID_POWER_STATE);
	if (state) {
		acpi_pm_gpe_add_events_print_events(&state->gpe_state);
		gpio_add_events(&state->gpio_state);
	}
	acpi_clear_pm_gpe_status();

	al2ahb_clock_gate();

	gpp_clk_setup();
}

void southbridge_final(void *chip_info)
{
	uint8_t restored_power = PM_S5_AT_POWER_RECOVERY;

	if (CONFIG(MAINBOARD_POWER_RESTORE))
		restored_power = PM_RESTORE_S0_IF_PREV_S0;
	pm_write8(PM_RTC_SHADOW, restored_power);
}

/*
 * Update the PCI devices with a valid IRQ number
 * that is set in the mainboard PCI_IRQ structures.
 */
static void set_pci_irqs(void *unused)
{
	/* Write PCI_INTR regs 0xC00/0xC01 */
	write_pci_int_table();

	/* pirq_data is consumed by `write_pci_cfg_irqs` */
	populate_pirq_data();

	/* Write IRQs for all devicetree enabled devices */
	write_pci_cfg_irqs();
}

/*
 * Hook this function into the PCI state machine
 * on entry into BS_DEV_ENABLE.
 */
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, set_pci_irqs, NULL);
