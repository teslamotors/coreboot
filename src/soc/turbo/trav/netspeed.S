#include <arch/asm.h>
#include <soc/netspeed.h>

ENTRY(netspeed_init)
	mov		x0, #0
	ldr		x1, =TRIP0_TCU_PORT
	str		w0, [x1]
	ldr		x1, =TRIP1_TCU_PORT
	str		w0, [x1]
	ldr		x1, =CA72_CL1_PORT
	str		w0, [x1]
	ldr		x1, =CA72_CL2_PORT
	str		w0, [x1]
	ldr		x1, =GPU_M0_PORT
	str		w0, [x1]
	ldr		x1, =GPU_M1_PORT
	str		w0, [x1]
	ldr		x1, =GPU_M2_PORT
	str		w0, [x1]
	ldr		x1, =GPU_M3_PORT
	str		w0, [x1]
	ldr		x1, =IMEM_TCU_PORT
	str		w0, [x1]
	ldr		x1, =ISP_TCU_PORT
	str		w0, [x1]
	ldr		x1, =PERIC_TCU_PORT
	str		w0, [x1]
	ldr		x1, =FSYS0_TCU_PORT
	str		w0, [x1]
	ret
ENDPROC(netspeed_init)
