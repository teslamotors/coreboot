/*
 * Early initialization code for aarch64 (a.k.a. armv8)
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/asm.h>
#include <soc/power.h>
#include <soc/netspeed.h>

#define L2CTLR_DATA_RAM_LATENCY_SHIFT	0
#define L2CTLR_TAG_RAM_LATENCY_SHIFT	6
#define L2_DATA_RAM_LATENCY_3_CYCLES	0x2
#define L2_DATA_RAM_LATENCY_4_CYCLES	0x3
#define L2_TAG_RAM_LATENCY_2_CYCLES	0x1
#define L2_TAG_RAM_LATENCY_3_CYCLES	0x2
#define ECC_ENABLE_SHIFT		21

/* NOTE: When making changes to general ARM64 initialization, keep in mind that
 * there are other CPU entry points, using BOOTBLOCK_CUSTOM or entering the CPU
 * in a later stage (like Tegra). Changes should generally be put into
 * arm64_cpu_init so they can be shared between those instances. */

ENTRY(_start)

	mrs	x0, S3_1_C11_C0_2
	orr     x0, x0, #((L2_DATA_RAM_LATENCY_4_CYCLES << L2CTLR_DATA_RAM_LATENCY_SHIFT))
	orr     x0, x0, #((L2_TAG_RAM_LATENCY_3_CYCLES << L2CTLR_TAG_RAM_LATENCY_SHIFT))
	orr	x0, x0, #(1 << ECC_ENABLE_SHIFT)
	msr     S3_1_C11_C0_2, x0
	isb

	mrs	x0, s3_1_c15_c2_1
	orr	x0, x0, #(1 << 6)
	msr	s3_1_c15_c2_1, x0
	isb

	ldr	x2, =BOOTUP_FINISH
	ldr	x1, =PMU_INFORM0
	ldr	w3, [x1]
	cmp	x3, x2
	beq	1f

	ldr	w2, =BOOTUP_FINISH
	str	w2, [x1]

	bl      netspeed_init

	/* Initialize PSTATE, SCTLR and caches to clean state, set up stack. */
	bl	arm64_init_cpu

	bl	main
1:
	mrs	x1, mpidr_el1
	ubfx	x0, x1, #6, #3
	bfi	x0, x1, #0, #2

2:
	/* X3 = cpu_num * 0x8 + CORE0_BOOTPC_ADDR */
	mov	x2, #8
	ldr	x3, =SHMEM_BASE
	madd	x4, x2, x0, x3

	/* Wait until BOOTPC_ADDR is non-zero */
3:
	ldr	x5, [x4]
	cmp	x5, #0
	b.eq	3b

	/* Jump to saved address */
	br	x5
	b	.


ENDPROC(_start)
