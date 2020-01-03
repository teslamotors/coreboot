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

#ifndef _CPU_SAMSUNG_EXYNOS7_GIC_H_
#define _CPU_SAMSUNG_EXYNOS7_GIC_H_

#define sys_reg(op0, op1, crn, crm, op2)        \
	((((op0)-2)<<19)|((op1)<<16)|((crn)<<12)|((crm)<<8)|((op2)<<5))
#define ICC_SRE_EN_BIT		(1 << 3)
#define ICC_SRE_DIB_BIT		(1 << 2)
#define ICC_SRE_DFB_BIT		(1 << 1)
#define ICC_SRE_SRE_BIT		(1 << 0)

#define __stringyfy(x) #x

#define DEFAULT_PMR_VALUE	0xFF
#define MIN_SPI_ID              32

/* Constants to categorise priorities */
#define GIC_HIGHEST_NS_PRIORITY	128
#define GIC_LOWEST_NS_PRIORITY  254 /* 255 would disable an interrupt */

#define GICD_CTLR		0x0000
#define GICD_TYPER		0x0004
#define GICD_IGROUPR		0x0080
#define GICD_IPRIORITYR		0x0400

#define GICD_CTLR_RWP           (1U << 31)
#define GICD_CTLR_ARE_S		(1U << 4)
#define GICD_CTLR_ARE_NS        (1U << 5) /* Non-Secure with two security state */
#define GICD_CTLR_ENABLE_G1S    (1U << 2)
#define GICD_CTLR_ENABLE_G1NS   (1U << 1)
#define GICD_CTLR_ENABLE_G0     (1U << 0)

#define GICD_TYPER_IRQS(typer)	((((typer) & 0x1f) + 1) * 32)

/* Re-distributor register, offset from rd_base */
#define GICR_WAKER		0x0014
#define GICR_WAKER_ProcessorSleep       (1U << 1)
#define GICR_WAKER_ChildrenAsleep       (1U << 2)

/* Re-Distributor registers, offsets from SGI_base */
#define GICR_IGROUPR0		0x0080
#define GICR_ISENABLER0		0x0100
#define GICR_ICENABLER0		0x0180
#define GICR_IPRIORITYR0	0x0400

#define GICR_BASE		0x200000
#define GICR_SGI_BASE		0x10000

#define IPRIORITYR_SHIFT	2

/* Value used to initialize Group1 interrupt priorities four at a time */
#define GICD_IPRIORITYR_DEF_VAL                 \
	(GIC_HIGHEST_NS_PRIORITY        |       \
	(GIC_HIGHEST_NS_PRIORITY << 8)  |       \
	(GIC_HIGHEST_NS_PRIORITY << 16) |       \
	(GIC_HIGHEST_NS_PRIORITY << 24))

/* Value used to initialize Group0 interrupt priorites four at a time */

/* System register definitions */
#define ICC_GRPEN0_EL1		S3_0_C12_C12_6
#define ICC_GRPEN1_EL1		S3_0_C12_C12_7
#define ICC_GRPEN1_EL3		S3_6_C12_C12_7
#define ICC_PMR_EL1		S3_0_C4_C6_0
#define ICC_SRE_EL1		S3_0_C12_C12_5
#define ICC_SRE_EL3		S3_6_C12_C12_5

/* ICC_SRE bit definitions*/
#define ICC_SRE_EN_BIT		(1 << 3)
#define ICC_SRE_DIB_BIT		(1 << 2)
#define ICC_SRE_DFB_BIT		(1 << 1)
#define ICC_SRE_SRE_BIT		(1 << 0)

#define write_sysreg(v, reg) do {		\
	u64 __val = (u64)v;		\
	__asm__ __volatile__(		\
		"msr "__stringyfy(reg)", %0"	\
		:			\
		: "r"(__val)		\
		: "memory");		\
} while (0)

#define read_sysreg(reg) ({		\
	u64 __val;			\
	__asm__ __volatile__(		\
		"mrs     %0,"__stringyfy(reg)	\
		: "=r"(__val)		\
		:			\
		: "memory");		\
	__val;				\
})

/* Setting Generic Interrupt Controller */
void gic_init(void);
void irqchip_primary_init(void);
void gicr_write_ipriorityr(void *, unsigned int, unsigned int);
void gic_ppi_sgi_configure_defaults(void);
#endif /* _CPU_SAMSUNG_EXYNOS7_GIC_H_ */
