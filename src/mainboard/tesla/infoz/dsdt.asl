/* SPDX-License-Identifier: GPL-2.0-or-later */

/* DefinitionBlock Statement */
#include <acpi/acpi.h>

DefinitionBlock (
	"DSDT.AML",	/* Output filename */
	"DSDT",		/* Signature */
	0x02,		/* DSDT Revision, needs to be 2 for 64bit */
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x00010001	/* OEM Revision */
	)
{	/* Start of ASL file */
	/* #include <arch/x86/acpi/debug.asl> */	/* as needed */

	/* global NVS and variables */
	#include <globalnvs.asl>

	/* PCI IRQ mapping for the Southbridge */
	#include <pcie.asl>

	/* Describe the processor tree (\_PR) */
	#include <cpu.asl>

	/* Contains the supported sleep states for this chipset */
	#include <sleepstates.asl>

	/* Contains _SWS methods */
	#include <soc/amd/common/acpi/acpi_wake_source.asl>

	/* System Bus */
	Scope(\_SB) { /* Start \_SB scope */
		/* global utility methods expected within the \_SB scope */
		#include <arch/x86/acpi/globutil.asl>

		/* Describe the SOC */
		#include <soc.asl>

		/* Per board variant specific definitions. */
		#include <variant/acpi/display.asl>
	} /* End \_SB scope */
}
/* End of ASL file */
