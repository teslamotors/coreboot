/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __SOC_AMD_PICASSO_MEMMAP_H__
#define __SOC_AMD_PICASSO_MEMMAP_H__

#include <stdint.h>
#include <symbols.h>

DECLARE_REGION(early_reserved_dram)

#if CONFIG(VBOOT_VERIFY_RW_ON_LOAD)
DECLARE_REGION(vboot_fw_stash)
#endif

struct memmap_early_dram {
	uint32_t base;
	uint32_t size;
};

void memmap_stash_early_dram_usage(void);
const struct memmap_early_dram *memmap_get_early_dram_usage(void);

#endif /* __SOC_AMD_PICASSO_MEMMAP_H__ */
