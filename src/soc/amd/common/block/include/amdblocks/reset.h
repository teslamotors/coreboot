/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMD_RESET_H__
#define __AMD_RESET_H__

#include <console/console.h>
#include <arch/cache.h>
#include <halt.h>

void do_warm_reset(void);
void do_cold_reset(void);

static inline __noreturn void warm_reset(void)
{
	printk(BIOS_NOTICE, "%s() called!\n", __func__);
	dcache_clean_all();
	do_warm_reset();
	halt();
}

static inline __noreturn void cold_reset(void)
{
	printk(BIOS_NOTICE, "%s() called!\n", __func__);
	dcache_clean_all();
	do_cold_reset();
	halt();
}

#endif /* __AMD_RESET_H__ */
