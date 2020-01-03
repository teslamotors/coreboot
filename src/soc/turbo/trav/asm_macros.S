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
#ifndef __ASM_MACROS_S__
#define __ASM_MACROS_S__

	/*
	 * This macro is used to create a function label and place the
	 * code into a separate text section based on the function name
	 * to enable elimination of unused code during linking. It also adds
	 * basic debug information to enable call stack printing most of the
	 * time.
	 */
	.macro func _name
	/*
	 * Add Call Frame Information entry in the .debug_frame section for
	 * debugger consumption. This enables callstack printing in debuggers.
	 * This does not use any space in the final loaded binary, only in the
	 * ELF file.
	 * Note that a function manipulating the CFA pointer location (i.e. the
	 * x29 frame pointer on AArch64) should declare it using the
	 * appropriate .cfi* directives, or be prepared to have a degraded
	 * debugging experience.
	 */
	.cfi_sections .debug_frame
	.section .text.\_name, "ax"
	.type \_name, %function
	.func \_name
	/*
	 * .cfi_startproc and .cfi_endproc are needed to output entries in
	 * .debug_frame
	 */
	.cfi_startproc
	\_name:
	.endm

	/*
	 * This macro is used to mark the end of a function.
	 */
	.macro endfunc _name
	.endfunc
	.cfi_endproc
	.size \_name, . - \_name
	.endm

	.macro	func_prologue
	stp	x29, x30, [sp, #-0x10]!
	mov	x29,sp
	.endm

	.macro	func_epilogue
	ldp	x29, x30, [sp], #0x10
	.endm


	.macro	dcache_line_size  reg, tmp
	mrs	\tmp, ctr_el0
	ubfx	\tmp, \tmp, #16, #4
	mov	\reg, #4
	lsl	\reg, \reg, \tmp
	.endm


	.macro	icache_line_size  reg, tmp
	mrs	\tmp, ctr_el0
	and	\tmp, \tmp, #0xf
	mov	\reg, #4
	lsl	\reg, \reg, \tmp
	.endm


	.macro	smc_check  label
	mrs	x0, esr_el3
	ubfx	x0, x0, #ESR_EC_SHIFT, #ESR_EC_LENGTH
	cmp	x0, #EC_AARCH64_SMC
	b.ne	$label
	.endm

	/*
	 * Declare the exception vector table, enforcing it is aligned on a
	 * 2KB boundary, as required by the ARMv8 architecture.
	 * Use zero bytes as the fill value to be stored in the padding bytes
	 * so that it inserts illegal AArch64 instructions. This increases
	 * security, robustness and potentially facilitates debugging.
	 */
	.macro vector_base  label
	.section .vectors, "ax"
	.align 11, 0
	\label:
	.endm

	/*
	 * Create an entry in the exception vector table, enforcing it is
	 * aligned on a 128-byte boundary, as required by the ARMv8 architecture.
	 * Use zero bytes as the fill value to be stored in the padding bytes
	 * so that it inserts illegal AArch64 instructions. This increases
	 * security, robustness and potentially facilitates debugging.
	 */
	.macro vector_entry  label
	.section .vectors, "ax"
	.align 7, 0
	\label:
	.endm

	/*
	 * This macro verifies that the given vector doesn't exceed the
	 * architectural limit of 32 instructions. This is meant to be placed
	 * immediately after the last instruction in the vector. It takes the
	 * vector entry as the parameter
	 */
	.macro check_vector_size since
	  .if (. - \since) > (32 * 4)
	    .error "Vector exceeds 32 instructions"
	  .endif
	.endm



	/*
	 * This macro calculates the base address of the current CPU's MP stack
	 * using the plat_my_core_pos() index, the name of the stack storage
	 * and the size of each stack
	 * Out: X0 = physical address of stack base
	 * Clobber: X30, X1, X2
	 */
	.macro get_my_mp_stack _name, _size
	bl  plat_my_core_pos
	ldr x2, =(\_name + \_size)
	mov x1, #\_size
	madd x0, x0, x1, x2
	.endm

	/*
	 * This macro calculates the base address of a UP stack using the
	 * name of the stack storage and the size of the stack
	 * Out: X0 = physical address of stack base
	 */
	.macro get_up_stack _name, _size
	ldr x0, =(\_name + \_size)
	.endm

	/*
	 * Helper macro to generate the best mov/movk combinations according
	 * the value to be moved. The 16 bits from '_shift' are tested and
	 * if not zero, they are moved into '_reg' without affecting
	 * other bits.
	 */
	.macro _mov_imm16 _reg, _val, _shift
		.if (\_val >> \_shift) & 0xffff
			.if (\_val & (1 << \_shift - 1))
				movk	\_reg, (\_val >> \_shift) & 0xffff, LSL \_shift
			.else
				mov	\_reg, \_val & (0xffff << \_shift)
			.endif
		.endif
	.endm

	/*
	 * Helper macro to load arbitrary values into 32 or 64-bit registers
	 * which generates the best mov/movk combinations. Many base addresses
	 * are 64KB aligned the macro will eliminate updating bits 15:0 in
	 * that case
	 */
	.macro mov_imm _reg, _val
		.if (\_val) == 0
			mov	\_reg, #0
		.else
			_mov_imm16	\_reg, (\_val), 0
			_mov_imm16	\_reg, (\_val), 16
			_mov_imm16	\_reg, (\_val), 32
			_mov_imm16	\_reg, (\_val), 48
		.endif
	.endm

	/*
	 * Macro to mark instances where we're jumping to a function and don't
	 * expect a return. To provide the function being jumped to with
	 * additional information, we use 'bl' instruction to jump rather than
	 * 'b'.
         *
	 * Debuggers infer the location of a call from where LR points to, which
	 * is usually the instruction after 'bl'. If this macro expansion
	 * happens to be the last location in a function, that'll cause the LR
	 * to point a location beyond the function, thereby misleading debugger
	 * back trace. We therefore insert a 'nop' after the function call for
	 * debug builds, unless 'skip_nop' parameter is non-zero.
	 */
	.macro no_ret _func:req, skip_nop=0
	bl	\_func

	.endm

#endif /* __ASM_MACROS_S__ */
