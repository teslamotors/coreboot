/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <arch/ioapic.h>
#include <assert.h>
#include <amdblocks/amd_pci_util.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <soc/pci.h>
#include <soc/pci_devs.h>
#include <stdio.h>

/**
 * Each PCI bridge has its INTx lines routed to one of the 8 GNB IOAPIC PCI
 * groups. Each group has 4 interrupts. The INTx lines can be swizzled before
 * being routed to the IOAPIC. If the IOAPIC redirection entry is masked, the
 * interrupt is reduced modulo 8 onto INT[A-H] and forwarded to the FCH IOAPIC.
 **/
struct pci_routing {
	unsigned int devfn;
	unsigned int group;
	const char intx[5];
};

/* See AMD PPR 55570 - IOAPIC Initialization for the table that AGESA sets up */
static const struct pci_routing pci_routing_table[] = {
	{PCIE_GPP_0_DEVFN, 0, "ABCD"},
	{PCIE_GPP_1_DEVFN, 1, "ABCD"},
	{PCIE_GPP_2_DEVFN, 2, "ABCD"},
	{PCIE_GPP_3_DEVFN, 3, "ABCD"},
	{PCIE_GPP_4_DEVFN, 4, "ABCD"},
	{PCIE_GPP_5_DEVFN, 5, "ABCD"},
	{PCIE_GPP_6_DEVFN, 6, "ABCD"},
	{PCIE_GPP_A_DEVFN, 7, "ABCD"},
	{PCIE_GPP_B_DEVFN, 7, "CDAB"},
};

/*
 * This data structure is populated from the raw data above. It is used
 * by amd/common/block/pci/amd_pci_util to write the PCI_INT_LINE register
 * to each PCI device.
 */
static struct pirq_struct pirq_data[] = {
	{ PCIE_GPP_0_DEVFN },
	{ PCIE_GPP_1_DEVFN },
	{ PCIE_GPP_2_DEVFN },
	{ PCIE_GPP_3_DEVFN },
	{ PCIE_GPP_4_DEVFN },
	{ PCIE_GPP_5_DEVFN },
	{ PCIE_GPP_6_DEVFN },
	{ PCIE_GPP_A_DEVFN },
	{ PCIE_GPP_B_DEVFN },
};

_Static_assert(ARRAY_SIZE(pci_routing_table) == ARRAY_SIZE(pirq_data),
	"PCI and PIRQ tables must be the same size");

static const struct pci_routing *get_pci_routing(unsigned int devfn)
{
	for (size_t i = 0; i < ARRAY_SIZE(pci_routing_table); ++i) {
		if (devfn == pci_routing_table[i].devfn)
			return &pci_routing_table[i];
	}

	return NULL;
}

static unsigned int calculate_irq(const struct pci_routing *pci_routing, unsigned int i)
{
	unsigned int irq_index;
	irq_index = pci_routing->group * 4;
	irq_index += pci_routing->intx[i] - 'A';

	return irq_index;
}

void populate_pirq_data(void)
{
	const struct pci_routing *pci_routing;
	struct pirq_struct *pirq;
	unsigned int irq_index;

	for (size_t i = 0; i < ARRAY_SIZE(pirq_data); ++i) {
		pirq = &pirq_data[i];
		pci_routing = get_pci_routing(pirq->devfn);
		if (!pci_routing)
			die("%s: devfn %u not found\n", __func__, pirq->devfn);

		for (size_t j = 0; j < 4; ++j) {
			irq_index = calculate_irq(pci_routing, j);

			pirq->PIN[j] = irq_index % 8;
		}
	}

	pirq_data_ptr = pirq_data;
	pirq_data_size = ARRAY_SIZE(pirq_data);
}

static const char *pcie_gpp_acpi_name(const struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	switch (dev->path.pci.devfn) {
	case PCIE_GPP_0_DEVFN:
		return "PBR0";
	case PCIE_GPP_1_DEVFN:
		return "PBR1";
	case PCIE_GPP_2_DEVFN:
		return "PBR2";
	case PCIE_GPP_3_DEVFN:
		return "PBR3";
	case PCIE_GPP_4_DEVFN:
		return "PBR4";
	case PCIE_GPP_5_DEVFN:
		return "PBR5";
	case PCIE_GPP_6_DEVFN:
		return "PBR6";
	case PCIE_GPP_A_DEVFN:
		return "PBRA";
	case PCIE_GPP_B_DEVFN:
		return "PBRB";
	}

	return NULL;
}

static int pcie_gpp_is_external(const struct device *dev)
{
	return (dev->device == PCI_DEVICE_ID_AMD_FAM17H_PCIE_GPP);
}

static void acpigen_write_PRT(const struct device *dev)
{
	char link_template[] = "\\_SB.INTX";
	unsigned int irq_index;

	const struct pci_routing *pci_routing = get_pci_routing(dev->path.pci.devfn);
	if (!pci_routing) {
		printk(BIOS_ERR, "PCI routing table not found for %s\n", dev_path(dev));
		return;
	}

	acpigen_write_method("_PRT", 0);

	/* If (PMOD) */
	acpigen_write_if();
	acpigen_emit_namestring("PMOD");

	/* Return (Package{...}) */
	acpigen_emit_byte(RETURN_OP);

	acpigen_write_package(4); /* Package - APIC Routing */
	for (unsigned int i = 0; i < 4; ++i) {
		irq_index = calculate_irq(pci_routing, i);

		acpigen_write_package(4);
		acpigen_write_dword(0x0000FFFF);
		acpigen_write_byte(i);
		acpigen_write_byte(0); /* Source: GSI  */
		/* GNB IO-APIC is located after the FCH IO-APIC */
		acpigen_write_dword(IO_APIC_INTERRUPTS + irq_index);
		acpigen_pop_len();
	}
	acpigen_pop_len(); /* Package - APIC Routing */
	acpigen_pop_len(); /* End If */

	/* Else */
	acpigen_write_else();

	/* Return (Package{...}) */
	acpigen_emit_byte(RETURN_OP);

	acpigen_write_package(4); /* Package - PIC Routing */
	for (unsigned int i = 0; i < 4; ++i) {
		irq_index = calculate_irq(pci_routing, i);

		link_template[8] = 'A' + (irq_index % 8);

		acpigen_write_package(4);
		acpigen_write_dword(0x0000FFFF);
		acpigen_write_byte(i);
		acpigen_emit_namestring(link_template);
		acpigen_write_dword(0);
		acpigen_pop_len();
	}
	acpigen_pop_len(); /* Package - PIC Routing */

	acpigen_pop_len(); /* End Else */

	acpigen_pop_len(); /* Method */
}

/*
 * Create ACPI Fields for accessing registers of interest from PCI config
 * space (e.g., for hotplug use elsewhere in SSDT).
 */
static void acpigen_write_pci_cfg_fields(const struct device *dev)
{
	static const struct fieldlist list[] = {
		FIELDLIST_OFFSET(0x04),
		FIELDLIST_NAMESTR("PC04", 16),	/* COMMAND */
		FIELDLIST_OFFSET(0x18),
		FIELDLIST_NAMESTR("PC18", 32),	/* SUB_BUS_NUMBER_LATENCY */
		FIELDLIST_NAMESTR("PC1C", 16),	/* IO_BASE_LIMIT */
		FIELDLIST_OFFSET(0x20),
		FIELDLIST_NAMESTR("PC20", 32),	/* MEM_BASE_LIMIT */
		FIELDLIST_NAMESTR("PC24", 32),	/* PREF_BASE_LIMIT */
		FIELDLIST_NAMESTR("PC28", 32),	/* PREF_BASE_UPPER */
		FIELDLIST_NAMESTR("PC2C", 32),	/* PREF_LIMIT_UPPER */
		FIELDLIST_OFFSET(0x54),
		FIELDLIST_NAMESTR("PC54", 32),	/* PMI_STATUS_CNTL */
		FIELDLIST_OFFSET(0x68),
		FIELDLIST_NAMESTR("PC68", 16),	/* LINK_CNTL */
		FIELDLIST_NAMESTR("PC6A", 16),	/* LINK_STATUS */
		FIELDLIST_NAMESTR("PC6C", 32),	/* SLOT_CAP */
		FIELDLIST_NAMESTR("PC70", 16),	/* SLOT_CNTL */
		FIELDLIST_NAMESTR("PC72", 16),	/* SLOT_STATUS */
	};
	struct opregion opreg = OPREGION("PCFG", PCI_CONFIG, 0x0, 0x100);

	acpigen_write_opregion(&opreg);
	acpigen_write_field(opreg.name, list, ARRAY_SIZE(list),
			    FIELD_ANYACC | FIELD_NOLOCK | FIELD_PRESERVE);
}

/*
 * This function writes a PCI device with _ADR, _STA, and _PRT objects:
 * Example:
 * Scope (\_SB.PCI0)
 * {
 *     Device (PBRA)
 *     {
 *         Name (_ADR, 0x0000000000080001)  // _ADR: Address
 *         Method (_STA, 0, NotSerialized)  // _STA: Status
 *         {
 *             Return (0x0F)
 *         }
 *
 *         Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
 *         {
 *             If (PMOD)
 *             {
 *                 Return (Package (0x04)
 *                 {
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x00,
 *                         0x00,
 *                         0x00000034
 *                     },
 *
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x01,
 *                         0x00,
 *                         0x00000035
 *                     },
 *
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x02,
 *                         0x00,
 *                         0x00000036
 *                     },
 *
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x03,
 *                         0x00,
 *                         0x00000037
 *                     }
 *                 })
 *             }
 *             Else
 *             {
 *                 Return (Package (0x04)
 *                 {
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x00,
 *                         \_SB.INTE,
 *                         0x00000000
 *                     },
 *
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x01,
 *                         \_SB.INTF,
 *                         0x00000000
 *                     },
 *
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x02,
 *                         \_SB.INTG,
 *                         0x00000000
 *                     },
 *
 *                     Package (0x04)
 *                     {
 *                         0x0000FFFF,
 *                         0x03,
 *                         \_SB.INTH,
 *                         0x00000000
 *                     }
 *                 })
 *             }
 *         }
 *     }
 * }
 */
static void acpi_device_write_gpp_pci_dev(const struct device *dev)
{
	const char *scope = acpi_device_scope(dev);
	const char *name = acpi_device_name(dev);

	assert(dev->path.type == DEVICE_PATH_PCI);
	assert(name);
	assert(scope);

	acpigen_write_scope(scope);
	acpigen_write_device(name);

	acpigen_write_ADR_pci_device(dev);
	acpigen_write_STA(acpi_device_status(dev));

	acpigen_write_PRT(dev);


	if (pcie_gpp_is_external(dev)) {
		/* Create useful ACPI Fields for PCI config space access */
		acpigen_write_pci_cfg_fields(dev);
	}

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}


static struct device_operations internal_pcie_gpp_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= pci_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.acpi_name		= pcie_gpp_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_gpp_pci_dev,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_AMD_FAM17H_PCIE_GPP_BUSA,
	PCI_DEVICE_ID_AMD_FAM17H_PCIE_GPP_BUSB,
	0
};

static const struct pci_driver internal_pcie_gpp_driver __pci_driver = {
	.ops			= &internal_pcie_gpp_ops,
	.vendor			= PCI_VENDOR_ID_AMD,
	.devices		= pci_device_ids,
};

static struct device_operations external_pcie_gpp_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= pciexp_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.acpi_name		= pcie_gpp_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_gpp_pci_dev,
};

static const struct pci_driver external_pcie_gpp_driver __pci_driver = {
	.ops			= &external_pcie_gpp_ops,
	.vendor			= PCI_VENDOR_ID_AMD,
	.device			= PCI_DEVICE_ID_AMD_FAM17H_PCIE_GPP,
};
