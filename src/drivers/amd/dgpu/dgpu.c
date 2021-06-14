/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * coreboot driver for satisfying OS ACPI requirements for an AMD
 * Discrete GPU card.  Specifically, this includes:
 *
 *	* ATPX hybrid graphics interface support (note: only
 *	partially implemented to make amdgpu driver recognize BOCO support)
 *
 *	* Power Resource creation
 *
 *	* _ON / _OFF methods for BOCO / D3Cold support (Bus Off, Chip Off)
 *
 * This driver imposes certain ACPI requirements on the root complex
 * side (existence of PCnn OperationRegion fields.  see picasso
 * pcie_gpp.c driver for an example).
 *
 * Limitations:
 *
 *	* Assumes dGPU power rails controlled by SoC GPIO
 *	* Assumes dedicated PERST# line for dGPU controlled by SoC GPIO
 *	* PMETurnOffSupport not used
 *	* Requires AMD APU host Raven/Picasso or newer
 *	* Only HotPlugEnhanced mode supported
 *	* SSID not programmed
 *
 * Copyright (c) 2021, Tesla, Inc
 */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <assert.h>
#include <gpio.h>
#include "chip.h"

/* ATPX interface */
#define ATPX_GET_PX_PARAMETERS_SUPPORTED	(1 << 0)
#define ATPX_MS_HYBRID_GFX_SUPPORTED		(1 << 14)

#define DGPU_PWR_CTRL_METHOD			"GPWR"

/* Time to wait after switching device to D0 state */
#define DELAY_D0_SETTLE_MS			1
/* Time to wait after enabling/disabling PCIe link */
#define DELAY_LINK_DISABLE_MS			24
#define DELAY_LINK_ENABLE_MS			24
/* Time to wait after doing a dummy read to PCI config space */
#define DELAY_DUMMY_READ_MS			10
/* Max time to allow for PCIe link training */
#define LINK_TRAINING_TIMEOUT_MS		500
/* Max time for PCI vendor/device ID to be valid after link up */
#define PCI_ID_READY_TIMEOUT_MS			1000

struct atpx_verify_interface_output {
	uint16_t size; /* Size of this object, including size field */
	uint16_t version;
	uint32_t supported_functions;
} __packed;

struct atpx_px_params {
	uint16_t size; /* Size of this object, including size field */
	uint32_t valid_flags_mask;
	uint32_t flags;
} __packed;

/*
 * AMD dGPU has an integrated PCIe switch presenting different device
 * types to the root port:
 *
 *	Root port
 *		-> Switch Upstream Port (bridge)
 *			-> Switch Downstream Port (bridge)
 *					-> GPU
 *					-> Audio
 *
 * This driver defines "chip" operations only for the upstream port
 * device.  The others devices are also handled in this driver, but
 * strictly through the pci_driver without an associated chip_info.
 */

/*
 * Generate an ACPI PowerResource with a flag tracking ON/OFF state.
 * Provide an optional @ctrl_method_name to call an ACPI method by
 * this name with arg One or Zero to control power state.
 */
static void generate_power_res(const char *ctrl_method_name)
{
	const char *power_res_dev_states[] = { "_PR0", "_PR2", "_PR3" };
	const char *vcc_en_flag = "FLAG";

	/* PowerResource (PRIC, 0, 0) { */
	acpigen_write_power_res("PRIC", 0, 0, power_res_dev_states,
				ARRAY_SIZE(power_res_dev_states));

	/* Name (FLAG, One) */
	acpigen_write_name(vcc_en_flag);
	acpigen_write_one();

	/* Method (_STA, ... ) { Return(FLAG) } */
	acpigen_write_STA_ext(vcc_en_flag);

	/* _ON method */
	acpigen_write_method_serialized("_ON", 0);
	if (ctrl_method_name != NULL) {
		/* If (FLAG == Zero) { ctrl_method(One) } */
		acpigen_write_if();
		acpigen_emit_byte(LEQUAL_OP);
		acpigen_emit_namestring(vcc_en_flag);
		acpigen_emit_byte(ZERO_OP);
		acpigen_emit_namestring(ctrl_method_name);
		acpigen_emit_byte(ONE_OP);
		acpigen_pop_len();	/* if */
	}
	/* Store (One, FLAG) */
	acpigen_write_store_op_to_namestr(ONE_OP, vcc_en_flag);
	acpigen_pop_len();	/* _ON method */

	/* _OFF method */
	acpigen_write_method_serialized("_OFF", 0);
	if (ctrl_method_name != NULL) {
		/* If (FLAG == One) { ctrl_method(Zero) } */
		acpigen_write_if();
		acpigen_emit_byte(LEQUAL_OP);
		acpigen_emit_namestring(vcc_en_flag);
		acpigen_emit_byte(ONE_OP);
		acpigen_emit_namestring(ctrl_method_name);
		acpigen_emit_byte(ZERO_OP);
		acpigen_pop_len();	/* if */
	}
	/* Store (Zero, FLAG) */
	acpigen_write_store_op_to_namestr(ZERO_OP, vcc_en_flag);
	acpigen_pop_len();	/* _OFF method */

	acpigen_pop_len();	/* PowerResource */
}

/*
 * ATPX_VERIFY_INTERFACE function - no input args
 **/
static void generate_atpx_px00_method(void)
{
	struct atpx_verify_interface_output verify_output = {
		.size = sizeof(verify_output),
		.version = 1,
		.supported_functions = ATPX_GET_PX_PARAMETERS_SUPPORTED,
	};

	acpigen_write_method("PX00", 0);

	/* Return buffer */
	acpigen_write_return_byte_buffer((uint8_t *)(void *)&verify_output,
		sizeof(verify_output));

	acpigen_pop_len(); /* method */
}

/*
 * GET_PX_PARAMETERS function - no input args
 **/
static void generate_atpx_px01_method(int boco_support)
{
	struct atpx_px_params px_params = {
		.size = sizeof(px_params),
		.valid_flags_mask = 0x0,
		.flags = 0x0,
	};

	acpigen_write_method("PX01", 0);

	if (boco_support)
		px_params.flags |= ATPX_MS_HYBRID_GFX_SUPPORTED;

	px_params.valid_flags_mask = px_params.flags;

	/* Return buffer */
	acpigen_write_return_byte_buffer((uint8_t *)(void *)&px_params,
		sizeof(px_params));

	acpigen_pop_len(); /* method */
}

/*
 * Minimal ATPX interface to advertise BOCO support to OS
 * ATPX power control methods omitted (handled through ACPI Power Resources)
 *
 * @boco_support: Indicate to OS that we support hybrid graphics on a system
 * with D3 power control for the dGPU
 */
static void generate_atpx(const struct device *dev, int boco_support)
{
	struct atpx_verify_interface_output verify_output = { 0 };

	/* Method (ATPX, 2, Serialized) */
	acpigen_write_method_serialized("ATPX", 2);

	/* arg0 == 0: return PX00 */
	acpigen_write_if_lequal_op_int(ARG0_OP, 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring("PX00");
	acpigen_pop_len();	/* if */

	/* arg0 == 1: return PX01 */
	acpigen_write_if_lequal_op_int(ARG0_OP, 1);
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring("PX01");
	acpigen_pop_len();	/* if */

	/* Return blank buffer */
	acpigen_write_return_byte_buffer((uint8_t *)(void *)&verify_output,
		sizeof(verify_output));

	acpigen_pop_len(); /* ATPX method */

	/* ATPX functions */
	generate_atpx_px00_method();
	generate_atpx_px01_method(boco_support);
}

/*
 * Specify PCI configuration registers we may need to touch from ACPI
 * methods.  Registers are explicitly defined to eliminate the need
 * for a general accessor method with broad access in ACPI space.
 */
static const struct fieldlist dgpu_ep_regs[] = {
	FIELDLIST_OFFSET(0x00),
	FIELDLIST_NAMESTR("PC00", 16),	/* VENDOR ID */
	FIELDLIST_NAMESTR("PC02", 16),	/* DEVICE ID */
	FIELDLIST_NAMESTR("PC04", 16),	/* COMMAND */
	FIELDLIST_OFFSET(0x10),
	FIELDLIST_NAMESTR("PC10", 32),	/* BAR0 */
	FIELDLIST_NAMESTR("PC14", 32),	/* BAR1 */
	FIELDLIST_NAMESTR("PC18", 32),	/* BAR2 */
	FIELDLIST_NAMESTR("PC1C", 32),	/* BAR3 */
	FIELDLIST_NAMESTR("PC20", 32),	/* BAR4 */
	FIELDLIST_NAMESTR("PC24", 32),	/* BAR5 */
};

static const struct fieldlist dgpu_bridge_regs[] = {
	FIELDLIST_OFFSET(0x00),
	FIELDLIST_NAMESTR("PC00", 16),	/* VENDOR ID */
	FIELDLIST_NAMESTR("PC02", 16),	/* DEVICE ID */
	FIELDLIST_NAMESTR("PC04", 16),	/* COMMAND */
	FIELDLIST_OFFSET(0x18),
	FIELDLIST_NAMESTR("PC18", 8),	/* Primary Bus */
	FIELDLIST_NAMESTR("PC19", 8),	/* Secondary Bus */
	FIELDLIST_NAMESTR("PC1A", 8),	/* Subordinate Bus */
	FIELDLIST_NAMESTR("PC1B", 8),	/* Sec. Latency Timer */
	FIELDLIST_OFFSET(0x20),
	FIELDLIST_NAMESTR("PC20", 32),	/* Memory Base/Limit */
	FIELDLIST_NAMESTR("PC24", 32),	/* Prefetchable Mem */
	FIELDLIST_NAMESTR("PC28", 32),	/* Prefetchable Base Upper */
	FIELDLIST_NAMESTR("PC2C", 32),	/* Prefetchable Base Lower */
};

/*
 * Create ACPI Fields for accessing registers of interest from PCI config
 * space (e.g., for hotplug use elsewhere in SSDT).
 */
static void acpigen_write_pci_cfg_fields(const struct device *dev,
					 const struct fieldlist *reg_list,
					 int reg_count)
{
	unsigned int cap;
	struct fieldlist cap_regs[2] = {
		FIELDLIST_OFFSET(0x00),
		FIELDLIST_NAMESTR("", 16),
	};
	struct opregion opreg = OPREGION("PCFG", PCI_CONFIG, 0x0, 0x100);

	acpigen_write_opregion(&opreg);
	acpigen_write_field(opreg.name, reg_list, reg_count,
			    FIELD_ANYACC | FIELD_NOLOCK | FIELD_PRESERVE);

	/*
	 * Registers in PCIe capability area, offsets determined at
	 * runtime in coreboot and written to SSDT
	 */
	cap = pci_find_capability(dev, PCI_CAP_ID_PCIE);
	if (!cap) {
		printk(BIOS_ERR, "dGPU: ERROR finding PCIe cap\n");
		return;
	}
	cap_regs[0].bits = (cap + PCI_EXP_LNKCTL) * 8;
	cap_regs[1].name = "LCTL";
	acpigen_write_field(opreg.name, cap_regs, 2,
			    FIELD_ANYACC | FIELD_NOLOCK | FIELD_PRESERVE);
	cap_regs[0].bits = (cap + PCI_EXP_LNKSTA) * 8;
	cap_regs[1].name = "LSTA";
	acpigen_write_field(opreg.name, cap_regs, 2,
			    FIELD_ANYACC | FIELD_NOLOCK | FIELD_PRESERVE);
}

/*
 * List of Field values to save before powering off and restore after power-on
 * Field names listed here should be in the appropriate fieldlist above.
 */
static const char *const dgpu_saved_regs[] = {
	"PC04", "PC10", "PC14", "PC18", "PC1C", "PC20", "PC24",
};

static const char *const dgpu_bridge_saved_regs[] = {
	"PC04", "PC18", "PC19", "PC1A", "PC1B", "PC20", "PC24", "PC28", "PC2C",
};

static void acpigen_pci_cfg_save_restore(const struct device *dev,
					 const char * const pcfg_reg_names[],
					 int pcfg_reg_count)
{
	int i;
	char tmp[5] = { 'S', 'V', 'x', 'x', '\0' };

	/* 'Name (SVnn, 0)' for each PCI config register to save */
	for (i = 0; i < pcfg_reg_count; i++) {
		/* ACPI Name 'SVnn' saves contents of Field 'PCnn' */
		tmp[2] = pcfg_reg_names[i][2];
		tmp[3] = pcfg_reg_names[i][3];
		acpigen_write_name(tmp);
		acpigen_write_zero();
	}

	/* Method (SAVE) - Save dGPU PCI config registers */
	acpigen_write_method_serialized("SAVE", 0);

	for (i = 0; i < pcfg_reg_count; i++) {
		/* SVnn = PCnn */
		tmp[2] = pcfg_reg_names[i][2];
		tmp[3] = pcfg_reg_names[i][3];
		acpigen_write_store();
		acpigen_emit_namestring(pcfg_reg_names[i]);
		acpigen_emit_namestring(tmp);
	}

	acpigen_pop_len(); /* Method */

	/* Method (RSTR) - Restore */
	acpigen_write_method_serialized("RSTR", 0);

	for (i = 0; i < pcfg_reg_count; i++) {
		/* PCnn = SVnn */
		tmp[2] = pcfg_reg_names[i][2];
		tmp[3] = pcfg_reg_names[i][3];
		acpigen_write_store();
		acpigen_emit_namestring(tmp);
		acpigen_emit_namestring(pcfg_reg_names[i]);
	}

	acpigen_pop_len(); /* Method */
}

/*
 * Generate ACPI call to @method_name in the scope of @dev.  Method name
 * can be an immediate method within the Device() or it can reference
 * methods in child devices (e.g., "CHLD.METH").
 */
static void acpigen_call_dev_method(const struct device *dev, const char *method_name)
{
	const char *s;

	s = acpi_device_path_join(dev, method_name);
	if (s == NULL) {
		printk(BIOS_ERR, "Error building ACPI method path\n");
		return;
	}

	acpigen_emit_namestring(s);
}

/*
 * Store (path.to.dev.@field_name, @dest_op)
 */
static void acpigen_get_dev_field(const struct device *dev, const char *field_name, uint8_t dest_op)
{
	const char *s;

	s = acpi_device_path_join(dev, field_name);
	if (s == NULL) {
		printk(BIOS_ERR, "Error building ACPI field path\n");
		return;
	}
	acpigen_write_store();
	acpigen_emit_namestring(s);
	acpigen_emit_byte(dest_op);
}

/*
 * Store (@src_op, path.to.dev.@field_name)
 */
static void acpigen_set_dev_field(const struct device *dev, uint8_t src_op, const char *field_name)
{
	const char *s;

	s = acpi_device_path_join(dev, field_name);
	if (s == NULL) {
		printk(BIOS_ERR, "Error building ACPI field path\n");
		return;
	}
	acpigen_write_store();
	acpigen_emit_byte(src_op);
	acpigen_emit_namestring(s);
}

static void acpigen_dev_field_clear32(const struct device *dev, const char *field_name, uint32_t clear_bits)
{
	const char *s;

	s = acpi_device_path_join(dev, field_name);
	if (s == NULL) {
		printk(BIOS_ERR, "Error building ACPI field path\n");
		return;
	}

	acpigen_emit_byte(AND_OP);
	acpigen_emit_namestring(s);
	acpigen_write_dword(~clear_bits);
	acpigen_emit_namestring(s);
}

static void acpigen_dev_field_clear16(const struct device *dev, const char *field_name, uint32_t clear_bits)
{
	const char *s;

	s = acpi_device_path_join(dev, field_name);
	if (s == NULL) {
		printk(BIOS_ERR, "Error building ACPI field path\n");
		return;
	}

	acpigen_emit_byte(AND_OP);
	acpigen_emit_namestring(s);
	acpigen_write_word(~clear_bits);
	acpigen_emit_namestring(s);
}

static void acpigen_dev_field_set16(const struct device *dev, const char *field_name, uint32_t set_bits)
{
	const char *s;

	s = acpi_device_path_join(dev, field_name);
	if (s == NULL) {
		printk(BIOS_ERR, "Error building ACPI field path\n");
		return;
	}

	acpigen_emit_byte(OR_OP);
	acpigen_emit_namestring(s);
	acpigen_write_word(set_bits);
	acpigen_emit_namestring(s);
}

/*
 * Clear and disable slot presence detection for a slot.
 * Should only be called for @dev == upstream port dev (SWUS), since
 * we assume its parent is the root complex.
 */
static void generate_clear_slot_presence_detect(const struct device *dev)
{
	/* Clear and disable slot presence detection */
	acpigen_dev_field_clear16(dev->bus->dev, "PC70", PCI_EXP_SLTCTL_PDCE);
	acpigen_dev_field_set16(dev->bus->dev, "PC72", PCI_EXP_SLTSTA_PDC);
}

/*
 * Retrain link for @dev on its parent root complex
 * Should only be called for @dev == upstream port dev (SWUS), since
 * we assume its parent is the root complex.
 *
 * Clobbers Local0/Local1
 */
static void generate_retrain_pcie_link(const struct device *dev)
{
	const unsigned int check_period_us = ACPI_STALL_MAX_US;
	unsigned int max_tries;
	/* Force 0 -> 1 transition on RETRAIN_LINK register */
	acpigen_dev_field_clear16(dev->bus->dev, "PC68", PCI_EXP_LNKCTL_RL);
	acpigen_dev_field_set16(dev->bus->dev, "PC68", PCI_EXP_LNKCTL_RL);

	/* Wait until LINK_TRAINING status == 0 or timeout */

	/* Local1 = max_tries */
	max_tries = (LINK_TRAINING_TIMEOUT_MS*1000) / check_period_us;
	acpigen_write_store_int_to_op(max_tries, LOCAL1_OP);

	/* While (Local1 > 0) { */
	acpigen_emit_byte(WHILE_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_emit_byte(ZERO_OP);

	/* Local0 = pci_read_config(0x6a) */
	acpigen_get_dev_field(dev->bus->dev, "PC6A", LOCAL0_OP);

	/* Local0 &= (LNKSTA_DLLLA | LNKSTA_LT) */
	acpigen_emit_byte(AND_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_integer(PCI_EXP_LNKSTA_DLLLA | PCI_EXP_LNKSTA_LT);
	acpigen_emit_byte(LOCAL0_OP);

	/*
	 * If (Local0 == LNKSTA_DLLLA) {
	 *	break
	 * }
	 */
	acpigen_write_if_lequal_op_int(LOCAL0_OP, PCI_EXP_LNKSTA_DLLLA);
	acpigen_emit_byte(BREAK_OP);
	acpigen_pop_len();	/* If */

	/* Stall() */
	acpigen_write_stall(check_period_us);

	/* Local1-- */
	acpigen_emit_byte(DECREMENT_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_pop_len();	/* While */
}

/*
 * Wait for dGPU upstream port to return a valid vendor ID
 * Clobbers Local0/Local1
 */
static void generate_wait_pci_id_ok(const struct device *dev)
{
	const unsigned int check_period_ms = 25;
	unsigned int max_tries;

	/* Local1 = max_tries */
	max_tries = PCI_ID_READY_TIMEOUT_MS / check_period_ms;
	acpigen_write_store_int_to_op(max_tries, LOCAL1_OP);

	/* While (Local1 > 0) { */
	acpigen_emit_byte(WHILE_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_emit_byte(ZERO_OP);

	/* Local0 = pci_read_config(0x00) */
	acpigen_get_dev_field(dev, "PC00", LOCAL0_OP);

	/*
	 * If (Local0 == expected_vendor_ID) {
	 *	break
	 * }
	 */
	acpigen_write_if_lequal_op_int(LOCAL0_OP, dev->vendor);
	acpigen_emit_byte(BREAK_OP);
	acpigen_pop_len();	/* If */

	/* Sleep() */
	acpigen_write_sleep(check_period_ms);

	/* Local1-- */
	acpigen_emit_byte(DECREMENT_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_pop_len();	/* While */
}

/*
 * ACPI sequence to turn off dGPU / enter BOCO
 */
static void generate_dgpu_off(const struct device *dev)
{
	struct drivers_amd_dgpu_config *config = dev->chip_info;

	/* Call save methods for each device */
	acpigen_call_dev_method(dev, "SAVE");
	acpigen_call_dev_method(dev, "SWDS.SAVE");
	acpigen_call_dev_method(dev, "SWDS.DGPU.SAVE");

	/* XXX: SB.ALIB(0x0C) DPTCi call omitted due to PeApmEnable=0 */

	/*
	 * Save host-side PMI_STATUS_CNTL (0x54), for power state (D0-D3)
	 */
	acpigen_get_dev_field(dev->bus->dev, "PC54", LOCAL0_OP);

	/* Set PMI_STATUS_CNTL (0x54) to set D0 state on host / root port */
	acpigen_dev_field_clear32(dev->bus->dev, "PC54", PCI_PM_CTRL_PME_STATUS | PCI_PM_CTRL_STATE_MASK);
	acpigen_write_sleep(DELAY_D0_SETTLE_MS);

	/*
	 * Set PM_CONTROL on dGPU's Upstream bridge's LINK_CNTL
	 * This disables ASPM
	 */
	acpigen_dev_field_clear16(dev, "LCTL", PCI_EXP_LNKCTL_ASPMC);

	/* Restore power state */
	acpigen_emit_byte(AND_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_dword(~PCI_PM_CTRL_PME_STATUS);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_set_dev_field(dev->bus->dev, LOCAL0_OP, "PC54");

	/* Disable link */
	/* Note: Should only be done for case without PME turn-off support */
	acpigen_dev_field_set16(dev->bus->dev, "PC68", PCI_EXP_LNKCTL_LD);
	acpigen_write_sleep(DELAY_LINK_DISABLE_MS);

	/* Assert reset */
	acpigen_enable_tx_gpio(&config->reset_gpio);
	/* Delay */
	acpigen_write_sleep(config->reset_off_delay_ms);
	/* Power off */
	acpigen_disable_tx_gpio(&config->enable_gpio);
	/* Delay */
	acpigen_write_sleep(config->enable_off_delay_ms);

	generate_clear_slot_presence_detect(dev);

	/* Dummy PCI config read (w/out PmeTurnOff support) */
	acpigen_get_dev_field(dev->bus->dev, "PC04", LOCAL0_OP);
	acpigen_write_sleep(DELAY_DUMMY_READ_MS);
}

/*
 * ACPI sequence to turn on dGPU / exit BOCO
 */
static void generate_dgpu_on(const struct device *dev)
{
	struct drivers_amd_dgpu_config *config = dev->chip_info;

	/* Assert dGPU reset */
	acpigen_enable_tx_gpio(&config->reset_gpio);
	/* Power on dGPU */
	acpigen_enable_tx_gpio(&config->enable_gpio);
	/* Delay */
	acpigen_write_sleep(config->enable_delay_ms);
	/* Release reset */
	acpigen_disable_tx_gpio(&config->reset_gpio);
	/* Delay */
	acpigen_write_sleep(config->reset_delay_ms);

	/* Enable link */
	acpigen_dev_field_clear16(dev->bus->dev, "PC68", PCI_EXP_LNKCTL_LD);
	acpigen_write_sleep(DELAY_LINK_ENABLE_MS);

	generate_retrain_pcie_link(dev);

	/*
	 * Save host-side PMI_STATUS_CNTL (0x54), for power state (D0-D3)
	 */
	acpigen_get_dev_field(dev->bus->dev, "PC54", LOCAL7_OP);

	/* Set to D0 power state */
	acpigen_dev_field_clear32(dev->bus->dev, "PC54", PCI_PM_CTRL_PME_STATUS | PCI_PM_CTRL_STATE_MASK);
	acpigen_write_sleep(DELAY_D0_SETTLE_MS);

	/* Wait for device to report expected vendor ID */
	generate_wait_pci_id_ok(dev);

	/* Call restore methods for each device */
	acpigen_call_dev_method(dev, "RSTR");
	acpigen_call_dev_method(dev, "SWDS.RSTR");
	acpigen_call_dev_method(dev, "SWDS.DGPU.RSTR");

	/* Restore power state */
	acpigen_emit_byte(AND_OP);
	acpigen_emit_byte(LOCAL7_OP);
	acpigen_write_dword(~PCI_PM_CTRL_PME_STATUS);
	acpigen_emit_byte(LOCAL7_OP);
	acpigen_set_dev_field(dev->bus->dev, LOCAL7_OP, "PC54");
}

/*
 * ACPI GPWR() method for dGPU power control (D3Cold)
 * Only HotPlugEnhanced mode supported
 * Called for upstream port only.
 */
static void generate_gpwr_method(const struct device *dev)
{
	/* Method() */
	acpigen_write_method_serialized(DGPU_PWR_CTRL_METHOD, 1);

	generate_clear_slot_presence_detect(dev);

	/*
	 * XXX: Reference code had a guard check here on the reset line
	 * state.  Not done here.
	 */

	/* XXX: Skipping MACO mode check - not supported */

	/* If (Arg0 == One) { enable_gpio() } */
	acpigen_write_if_lequal_op_op(ARG0_OP, ONE_OP);

	generate_dgpu_on(dev);

	acpigen_pop_len();	/* if */
	/* else */
	acpigen_write_else();

	generate_dgpu_off(dev);

	acpigen_pop_len();	/* else */
	acpigen_pop_len(); /* method */
}

static void dgpu_fill_ssdt(const struct device *dev)
{
	/* Scope() */
	acpigen_write_scope(acpi_device_scope(dev));
	/* Device(DGPU) */
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_ADR_pci_device(dev);

	if (CONFIG(AMD_DGPU_BOCO_SUPPORT)) {
		acpigen_write_STA(acpi_device_status(dev));

		acpigen_write_pci_cfg_fields(dev, dgpu_ep_regs,
					     ARRAY_SIZE(dgpu_ep_regs));
		acpigen_pci_cfg_save_restore(dev, dgpu_saved_regs,
					     ARRAY_SIZE(dgpu_saved_regs));
	}

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	/* Scope (\_SB.PCI0.PBR0.SWUS.SWDS.DGPU) */
	acpigen_write_scope(acpi_device_path(dev));

	if (CONFIG(AMD_DGPU_ATPX)) {
		generate_atpx(dev, CONFIG(AMD_DGPU_BOCO_SUPPORT));
	}

	acpigen_pop_len(); /* Scope */
}

static void dgpu_bridge_fill_ssdt(const struct device *dev)
{
	struct drivers_amd_dgpu_config *config = dev->chip_info;
	const char *parent_scope = acpi_device_scope(dev);

	/* Scope (\_SB.PCI0.PBR0) or (\_SB.PCI0.PBR0.SWUS) */
	acpigen_write_scope(parent_scope);

	/* Create power resource that controls HW at root port level */
	if (dev->device == PCI_DEVICE_ID_AMD_NV_SWUS &&
	    CONFIG(AMD_DGPU_BOCO_SUPPORT)) {
		/* has_power_resource required in devicetree */
		assert(config->has_power_resource == true);
		generate_gpwr_method(dev);
		generate_power_res(DGPU_PWR_CTRL_METHOD);
		acpigen_write_name_integer("_S0W", ACPI_DEVICE_SLEEP_D3_COLD);
	}

	/* Device(SWUS or SWDS) */
	acpigen_write_device(acpi_device_name(dev));

	acpigen_write_ADR_pci_device(dev);

	if (CONFIG(AMD_DGPU_BOCO_SUPPORT)) {
		acpigen_write_STA(acpi_device_status(dev));

		acpigen_write_pci_cfg_fields(dev, dgpu_bridge_regs,
					     ARRAY_SIZE(dgpu_bridge_regs));
		acpigen_pci_cfg_save_restore(dev, dgpu_bridge_saved_regs,
					     ARRAY_SIZE(dgpu_bridge_saved_regs));

		generate_power_res(NULL);
		acpigen_write_name_integer("_S0W", ACPI_DEVICE_SLEEP_D3_COLD);
	}

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static const char *dgpu_acpi_name(const struct device *dev)
{
	return "DGPU";
}

static const char *dgpu_bridge_acpi_name(const struct device *dev)
{
	switch (dev->device) {
	case PCI_DEVICE_ID_AMD_NV_SWUS:
		return "SWUS";
	case PCI_DEVICE_ID_AMD_NV_SWDS:
		return "SWDS";
	}
	return "SWXX";
}

static void dgpu_bridge_enable(struct device *dev)
{
	struct drivers_amd_dgpu_config *config = dev->chip_info;
	int pgood;

	if (pciexp_type(dev) != PCI_EXP_TYPE_UPSTREAM)
		/* Only run enable on the dGPU's upstream dev */
		return;

	printk(BIOS_NOTICE, "AMD dGPU is present: Upstream port ID %04x:%04x\n",
	       dev->vendor, dev->device);

	if (config->pgood_gpio.pin_count == 1) {
		pgood = gpio_get(config->pgood_gpio.pins[0]);
		printk(BIOS_INFO, "AMD dGPU power rails: %s\n",
		       pgood ? "OK" : "DOWN");
		if (!pgood)
			printk(BIOS_WARNING, "WARNING: dGPU detected over PCI but PGOOD signal is low\n");
	}
}

static struct device_operations dgpu_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= pci_dev_init,
	.ops_pci		= &pci_dev_ops_pci,
	.write_acpi_tables	= pci_rom_write_acpi_tables,
	.acpi_fill_ssdt		= dgpu_fill_ssdt,
	.acpi_name		= dgpu_acpi_name,
};

static struct pci_operations dgpu_bridge_bus_ops_pci = {
	.set_subsystem = 0,
};

static struct device_operations dgpu_bridge_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= pciexp_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.enable			= dgpu_bridge_enable,
	.ops_pci		= &dgpu_bridge_bus_ops_pci,
	.acpi_fill_ssdt		= dgpu_bridge_fill_ssdt,
	.acpi_name		= dgpu_bridge_acpi_name,
};

static const unsigned short dgpu_device_ids[] = {
	PCI_DEVICE_ID_AMD_NV_GPU_0,
	PCI_DEVICE_ID_AMD_NV_GPU_1,
	PCI_DEVICE_ID_AMD_NV_GPU_2,
	PCI_DEVICE_ID_AMD_NV_GPU_3,
	0,
};

static const unsigned short dgpu_bridge_device_ids[] = {
	/* Upstream port */
	PCI_DEVICE_ID_AMD_NV_SWUS,
	/* Downstream port */
	PCI_DEVICE_ID_AMD_NV_SWDS,
	0,
};

static const struct pci_driver dgpu_driver __pci_driver = {
	.ops		= &dgpu_ops,
	.vendor		= PCI_VENDOR_ID_ATI,
	.devices	= dgpu_device_ids,
};

static const struct pci_driver dgpu_bridge_driver __pci_driver = {
	.ops		= &dgpu_bridge_ops,
	.vendor		= PCI_VENDOR_ID_ATI,
	.devices	= dgpu_bridge_device_ids,
};

static void dgpu_enable(struct device *dev)
{
	dev->ops = &dgpu_bridge_ops;
}

struct chip_operations drivers_amd_dgpu_ops = {
	CHIP_NAME("AMD Discrete GPU")
	.enable_dev = dgpu_enable
};
