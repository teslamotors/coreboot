/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMDBLOCKS_BIOSRAM_H__
#define __AMDBLOCKS_BIOSRAM_H__

#include <stdint.h>

/* Any new chipset and board must implement cbmem_top() for both
 * romstage and ramstage to support early features like COLLECT_TIMESTAMPS
 * and CBMEM_CONSOLE. With AMD it is necessary to have cbmem_top()
 * value stored in nvram to enable early recovery on S3 path.
 */
void backup_top_of_low_cacheable(uintptr_t ramtop);
uintptr_t restore_top_of_low_cacheable(void);

/* Returns the bootblock C entry point for APs */
void *get_ap_entry_ptr(void);
/* Used by BSP to store the bootblock entry point for APs */
void set_ap_entry_ptr(void *entry);
/* Saves the UMA size returned by AGESA */
void save_uma_size(uint32_t size);
/* Saves the UMA base address returned by AGESA */
void save_uma_base(uint64_t base);
/* Returns the saved UMA size */
uint32_t get_uma_size(void);
/* Returns the saved UMA base */
uint64_t get_uma_base(void);

#if CONFIG(SOC_AMD_BIOSRAM_PANIC_MSG)
int panic_detected(void);
void panic_msg_copy(char *dest, int max_len);
void panic_msg_print(int loglevel);
void panic_msg_clear(void);
#else
static inline int panic_detected(void) { return 0; }
static inline void panic_msg_copy(char *dest, int max_len) {};
static inline void panic_msg_print(int loglevel) {};
static inline void panic_msg_clear(void) {};
#endif

#endif
