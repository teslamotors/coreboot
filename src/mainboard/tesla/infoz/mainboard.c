/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <string.h>
#include <console/console.h>
#include <cbfs.h>
#include <device/device.h>
#include <device/mmio.h>
#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/gpio_banks.h>
#include <amdblocks/lpc.h>
#include <infoz/variants.h>
#include <infoz/thermal.h>
#include <infoz/gpio.h>
#include <boardid.h>
#include <gpio.h>
#include <smbios.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/gpio.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <soc/platform_descriptors.h>
#include <soc/psb.h>
#include <soc/southbridge.h>
#include <soc/smi.h>
#include <soc/soc_util.h>
#include <amdblocks/acpimmio.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <commonlib/helpers.h>
#include <bootstate.h>
#include <delay.h>
#include <version.h>
#include <mainboard/tesla/infoz/acpi.h>
#include <amdblocks/psp.h>

/***********************************************************
 * These arrays set up the FCH PCI_INTR registers 0xC00/0xC01.
 * This table is responsible for physically routing the PIC and
 * IOAPIC IRQs to the different PCI devices on the system.  It
 * is read and written via registers 0xC00/0xC01 as an
 * Index/Data pair.  These values are chipset and mainboard
 * dependent and should be updated accordingly.
 */
static uint8_t fch_pic_routing[0x80];
static uint8_t fch_apic_routing[0x80];

_Static_assert(sizeof(fch_pic_routing) == sizeof(fch_apic_routing),
	"PIC and APIC FCH interrupt tables must be the same size");

/*
 * This controls the device -> IRQ routing.
 *
 * Hardcoded IRQs:
 *  0: timer <- soc/amd/common/acpi/lpc.asl
 *  2: cascade
 *  8: rtc0 <- soc/amd/common/acpi/lpc.asl
 *  9: acpi <- soc/amd/common/acpi/lpc.asl
 *
 */
static const struct fch_apic_routing {
	uint8_t intr_index;
	uint8_t pic_irq_num;
	uint8_t apic_irq_num;
} fch_pirq[] = {
	{ PIRQ_A,	6,		PIRQ_NC },
	{ PIRQ_B,	13,		PIRQ_NC },
	{ PIRQ_C,	14,		PIRQ_NC },
	{ PIRQ_D,	15,		PIRQ_NC },
	{ PIRQ_E,	15,		PIRQ_NC },
	{ PIRQ_F,	14,		PIRQ_NC },
	{ PIRQ_G,	13,		PIRQ_NC },
	{ PIRQ_H,	6,		PIRQ_NC },

	{ PIRQ_SCI,	9,		9 },
	{ PIRQ_GPIO,	7,		7 },
	{ PIRQ_I2C2,	10,		10 },
	{ PIRQ_I2C3,	11,		11 },
	{ PIRQ_UART0,	4,		4 },
	{ PIRQ_UART1,	3,		3 },
	{ PIRQ_UART3,	5,		5 },

	/* The MISC registers are not interrupt numbers */
	{ PIRQ_MISC,	0xfa,		0x00 },
	{ PIRQ_MISC0,	0xf1,		0x00 },
	{ PIRQ_MISC1,	0x00,		0x00 },
	{ PIRQ_MISC2,	0x00,		0x00 },
};

static void init_tables(void)
{
	const struct fch_apic_routing *entry;
	int i;

	memset(fch_pic_routing, PIRQ_NC, sizeof(fch_pic_routing));
	memset(fch_apic_routing, PIRQ_NC, sizeof(fch_apic_routing));

	for (i = 0; i < ARRAY_SIZE(fch_pirq); i++) {
		entry = fch_pirq + i;
		fch_pic_routing[entry->intr_index] = entry->pic_irq_num;
		fch_apic_routing[entry->intr_index] = entry->apic_irq_num;
	}
}

/* PIRQ Setup */
static void pirq_setup(void)
{
	init_tables();

	intr_data_ptr = fch_apic_routing;
	picr_data_ptr = fch_pic_routing;
}

static void mainboard_configure_gpios(void)
{
	size_t base_num_gpios, override_num_gpios;
	const struct soc_amd_gpio *base_gpios, *override_gpios;

	base_gpios = variant_base_gpio_table(&base_num_gpios);
	override_gpios = variant_override_gpio_table(&override_num_gpios);

	gpio_configure_pads_with_override(base_gpios, base_num_gpios, override_gpios,
					  override_num_gpios);
}

uint32_t board_id(void)
{
	MAYBE_STATIC_NONZERO uint32_t id = BOARD_ID_INIT;
	int flags;

	if (id == BOARD_ID_INIT) {
		flags = variant_get_flags();

		id = (gpio_get(GPIO_BOARD_ID_0) << 0) |
		     (gpio_get(GPIO_BOARD_ID_1) << 1) |
		     (gpio_get(GPIO_BOARD_ID_2) << 2) |
		     (gpio_get(GPIO_BOARD_ID_3) << 3);

		if (flags & INFOZ_BOARD_ID_6BIT)
			id |= (gpio_get(GPIO_BOARD_ID_4) << 4) |
			      (gpio_get(GPIO_BOARD_ID_5) << 5);
	}

	return id;
}

const char *smbios_mainboard_version(void)
{
	enum infoz_hw_rev hw_rev;

	hw_rev = variant_get_rev();
	switch (hw_rev) {
	case INFOZ_HW_REV_A:	return "A";
	case INFOZ_HW_REV_B:	return "B";
	case INFOZ_HW_REV_C:	return "C";
	case INFOZ_HW_REV_D:	return "D";
	case INFOZ_HW_REV_G:	return "G";
	case INFOZ_HW_REV_HGA:	return "HG-A";
	case INFOZ_HW_REV_HGD:	return "HG-D";
	case INFOZ_HW_UNKNOWN:
	default:
		return "Unknown";
	}
}

static void mainboard_init(void *chip_info)
{
	int boardid;

	boardid = board_id();
	printk(BIOS_NOTICE, "Board name: %s\n", smbios_mainboard_product_name());
	printk(BIOS_NOTICE, "Board ID: %d\n", boardid);
	printk(BIOS_NOTICE, "Board revision: %s\n", smbios_mainboard_version());

	/* IOMUX pad config alone is insufficient to disable some LPC functions */
	lpc_disable_ldrq0();
	lpc_disable_clk1(1, 1);

	mainboard_configure_gpios();

	/* Power on PCIe peripherals */
	variant_pcie_devs_power(1);

	/* Update DUT configuration */
	variant_devtree_update();
}

void mainboard_get_dxio_ddi_descriptors(const fsp_dxio_descriptor **dxio_descs,
					size_t *dxio_num,
					const fsp_ddi_descriptor **ddi_descs,
					size_t *ddi_num)
{
	variant_get_dxio_ddi_descriptors(dxio_descs, dxio_num, ddi_descs, ddi_num);
}

static void mainboard_dsdt_generator(const struct device *dev)
{
	/* acpigen_* functions here */
}

static void write_hsti_acpi_table(acpi_oem_hsti_t *hsti, u32 state)
{
	acpi_header_t *header = &(hsti->header);

	memset((void *)hsti, 0, sizeof(acpi_oem_hsti_t));

	memcpy(header->signature, "OEM1", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_oem_hsti_t);
	header->revision = 1;

	hsti->state = state;

	header->checksum = acpi_checksum((void *)hsti, header->length);
}

static void write_psb_acpi_table(acpi_oem_psb_t *psb,
					struct psb_fuse_config *cfg)
{
	acpi_header_t *header = &(psb->header);

	memset((void *)psb, 0, sizeof(acpi_oem_psb_t));

	memcpy(header->signature, "OEM2", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_oem_psb_t);
	header->revision = 1;

	psb->config = cfg->config;
	psb->bios_key_rev_id = cfg->bios_key_rev_id;
	memcpy(psb->root_key_hash, cfg->root_key_hash,
		sizeof(psb->root_key_hash));

	header->checksum = acpi_checksum((void *)psb, header->length);
}

static void write_oem_state_acpi_table(acpi_oem_state_t *oem, u8 state)
{
	acpi_header_t *header = &(oem->header);

	memset((void *)oem, 0, sizeof(acpi_oem_state_t));

	memcpy(header->signature, "OEM3", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_oem_state_t);
	header->revision = 1;

	oem->state = state;

	header->checksum = acpi_checksum((void *)oem, header->length);
}

static void write_psb_hsti1_acpi_table(acpi_psb_hsti1_t *status, u32 state)
{
	acpi_header_t *header = &(status->header);

	memset((void *)status, 0, sizeof(acpi_psb_hsti1_t));

	memcpy(header->signature, "OEM4", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_psb_hsti1_t);
	header->revision = 1;

	status->state = state;

	header->checksum = acpi_checksum((void *)status, header->length);
}

static unsigned long mainboard_write_acpi_tables(
	const struct device *device, unsigned long current, acpi_rsdp_t *rsdp)
{
	int ret;
	acpi_oem_hsti_t *hsti;
	acpi_oem_psb_t *psb;
	acpi_oem_state_t *oem;
	acpi_psb_hsti1_t *hsti1;
	uint32_t hsti_state;
	struct psb_fuse_config cfg;
	uint32_t hsti1_state;
	int i;

	/* assume customer key lock fuse is blown */
	uint32_t customer_key_lock = 1;
	uint8_t oem_state = 0;

	/* Write AGESA Tables */
	current = agesa_write_acpi_tables(device, current, rsdp);

	ret = psp_get_hsti_state(&hsti_state);
	if (ret == 0) {
		printk(BIOS_NOTICE, "HSTI: State = %x\n", hsti_state);
		hsti = (acpi_oem_hsti_t *)current;
		write_hsti_acpi_table(hsti, hsti_state);
		current += hsti->header.length;
		current = acpi_align_current(current);
		acpi_add_table(rsdp, hsti);
	} else {
		printk(BIOS_ERR, "HSTI: Failed to get state: %d\n", ret);
	}

	ret = psp_get_psb_fuse_config(&cfg);
	if (ret == 0) {
		printk(BIOS_NOTICE, "PSB: Config = %x\n", cfg.config);
		printk(BIOS_NOTICE, "PSB: BIOS key revision id = %x\n", cfg.bios_key_rev_id);
		printk(BIOS_NOTICE, "PSB: Root key hash = ");
		for (i = 0; i < sizeof(cfg.root_key_hash); i++)
			printk(BIOS_NOTICE, "%02x", cfg.root_key_hash[i]);
		printk(BIOS_NOTICE, "\n");
		psb = (acpi_oem_psb_t *)current;
		write_psb_acpi_table(psb, &cfg);
		current += psb->header.length;
		current = acpi_align_current(current);
		acpi_add_table(rsdp, psb);
		customer_key_lock = cfg.config & (1 << 28);
	} else {
		printk(BIOS_ERR, "PSB: Failed to get config: %d\n", ret);
	}

	/* if customer key lock not set, oem state is not queryable - default to 0 */
	if (customer_key_lock)
		ret = psp_get_oem_state(&oem_state);
	else
		ret = 0;

	if (ret == 0) {
		printk(BIOS_NOTICE, "OEM: State = %x\n", oem_state);
		oem = (acpi_oem_state_t *)current;
		write_oem_state_acpi_table(oem, oem_state);
		current += oem->header.length;
		current = acpi_align_current(current);
		acpi_add_table(rsdp, oem);
	} else {
		printk(BIOS_ERR, "OEM: Failed to get state: %d\n", ret);
	}

	ret = psb_get_hsti_status(&hsti1_state);
	if (ret == 0) {
		printk(BIOS_NOTICE, "HSTI1: State = %x\n", hsti1_state);
		hsti1 = (acpi_psb_hsti1_t *)current;
		write_psb_hsti1_acpi_table(hsti1, hsti1_state);
		current += hsti1->header.length;
		current = acpi_align_current(current);
		acpi_add_table(rsdp, hsti1);
	} else {
		printk(BIOS_ERR, "HSTI1: Failed to get state: %d\n", ret);
	}

	return current;
}

/*************************************************
 * Dedicated mainboard function
 *************************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	/* Initialize the PIRQ data structures for consumption */
	pirq_setup();

	dev->ops->acpi_inject_dsdt = mainboard_dsdt_generator;
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;

	/* Begin blinking, 1 second on, 3 seconds off */
	fch_blink_rate(FCH_BLINK_25);
}


#define USB_PD_PORT0_CONTROL     0xFEDC6600
#define USB_PD_PORT1_CONTROL     0xFEDC6610
#define USB_PD_PORTN_CONTROL_VAL 0x10c

static void mainboard_final(void *chip_info)
{
	struct global_nvs *gnvs;

	gnvs = acpi_get_gnvs();

	/*
	 * Final board-specific fixups for gnvs, used for dynamically
	 * populating ACPI tables
	 */
	if (gnvs) {
		/* Temperature where OS will shut down */
		gnvs->tcrt = CRITICAL_TEMPERATURE;

		/* Temperature where OS will throttle CPU */
		gnvs->tpsv = PASSIVE_TEMPERATURE;
	}
}

/* Control RF MUX to use DP2 instead of USBC */
static void rfmux_enable_dp2(void)
{
	write32((u32 *) USB_PD_PORT0_CONTROL, USB_PD_PORTN_CONTROL_VAL);
	write32((u32 *) USB_PD_PORT1_CONTROL, USB_PD_PORTN_CONTROL_VAL);
}

void mainboard_pre_soc_init(void)
{
	/* FSP-S doesn't do this critical initialization */
	rfmux_enable_dp2();
}

void mainboard_fsp_silicon_fixup(void)
{
	/* To be safe, make sure FSP-S didn't undo our init */
	rfmux_enable_dp2();

	/* Fix up GPIOs that get clobbered back into dedicated function modes in FSP */
	/* These should match the settings in infoz_gpio_set_stage_ram[] */
	iomux_write8(GPIO_21, GPIO_21_IOMUX_GPIOxx); /* LPC_PD */
	iomux_write8(GPIO_31, GPIO_31_IOMUX_GPIOxx); /* SPI_CS3 */
	gpio_output(GPIO_75, 1);	/* LPCCLK1 / SER2-ICU-TOUCH-NRESET */
	iomux_write8(GPIO_75, GPIO_75_IOMUX_GPIOxx); /* LPCCLK1 */
	iomux_write8(GPIO_87, GPIO_87_IOMUX_GPIOxx); /* SERIRQ */
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};

void __weak variant_devtree_update(void)
{
}

const char * __weak variant_board_sku(void)
{
	return "Unknown";
}

const char *smbios_system_sku(void)
{
	return variant_board_sku();
}
