#include <cpu/amd/msr.h>
#include <cpu/x86/msr.h>
#include "cc6.h"

void cc6_enable(void)
{
	msr_t cstate_cfg;

	cstate_cfg = rdmsr(CSTATE_CFG_MSR);

	cstate_cfg.lo |= CSTATE_CCR0_CC6EN;
	cstate_cfg.lo |= CSTATE_CCR1_CC6EN;
	cstate_cfg.lo |= CSTATE_CCR2_CC6EN;

	wrmsr(CSTATE_CFG_MSR, cstate_cfg);
}

void cc6_disable(void)
{
	msr_t cstate_cfg;

	cstate_cfg = rdmsr(CSTATE_CFG_MSR);

	cstate_cfg.lo &= ~CSTATE_CCR0_CC6EN;
	cstate_cfg.lo &= ~CSTATE_CCR1_CC6EN;
	cstate_cfg.lo &= ~CSTATE_CCR2_CC6EN;

	wrmsr(CSTATE_CFG_MSR, cstate_cfg);
}
