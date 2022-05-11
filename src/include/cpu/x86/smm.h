/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_X86_SMM_H
#define CPU_X86_SMM_H

#include <arch/cpu.h>
#include <commonlib/region.h>
#include <types.h>

#define SMM_DEFAULT_BASE 0x30000
#define SMM_DEFAULT_SIZE 0x10000

/* used only by C programs so far */
#define SMM_BASE 0xa0000

#define SMM_ENTRY_OFFSET 0x8000
#define SMM_SAVE_STATE_BEGIN(x) (SMM_ENTRY_OFFSET + (x))

#define APM_CNT		0xb2
#define APM_CNT_NOOP_SMI	0x00
#define APM_CNT_CST_CONTROL	0x85
#define APM_CNT_PST_CONTROL	0x80
#define APM_CNT_ACPI_DISABLE	0x1e
#define APM_CNT_ACPI_ENABLE	0xe1
#define APM_CNT_MBI_UPDATE	0xeb
#define APM_CNT_GNVS_UPDATE	0xea
#define APM_CNT_ROUTE_ALL_XHCI	0xca
#define APM_CNT_FINALIZE	0xcb
#define APM_CNT_LEGACY		0xcc
#define APM_CNT_PSPSMI		0xe9
#define APM_CNT_SMMINFO		0xec
#define APM_CNT_SMMSTORE	0xed
#define APM_CNT_ELOG_GSMI	0xef
#define APM_STS		0xb3

/* Send cmd to APM_CNT with HAVE_SMI_HANDLER checking. */
int apm_control(u8 cmd);

void io_trap_handler(int smif);
int southbridge_io_trap_handler(int smif);
int mainboard_io_trap_handler(int smif);

void southbridge_smi_set_eos(void);

void global_smi_enable(void);
void global_smi_enable_no_pwrbtn(void);

void cpu_smi_handler(void);
void northbridge_smi_handler(void);
void southbridge_smi_handler(void);

void mainboard_smi_gpi(u32 gpi_sts);
int  mainboard_smi_apmc(u8 data);
void mainboard_smi_sleep(u8 slp_typ);

/* This is the SMM handler. */
extern unsigned char _binary_smm_start[];
extern unsigned char _binary_smm_end[];

struct smm_runtime {
	u32 smbase;
	u32 smm_size;
	u32 save_state_size;
	u32 num_cpus;
	u32 gnvs_ptr;
	/* STM's 32bit entry into SMI handler */
	u32 start32_offset;
	/* The apic_id_to_cpu provides a mapping from APIC id to CPU number.
	 * The CPU number is indicated by the index into the array by matching
	 * the default APIC id and value at the index. The stub loader
	 * initializes this array with a 1:1 mapping. If the APIC ids are not
	 * contiguous like the 1:1 mapping it is up to the caller of the stub
	 * loader to adjust this mapping. */
	u8 apic_id_to_cpu[CONFIG_MAX_CPUS];
} __packed;

struct smm_module_params {
	void *arg;
	size_t cpu;
	const struct smm_runtime *runtime;
	/* A canary value that has been placed at the end of the stack.
	 * If (uintptr_t)canary != *canary then a stack overflow has occurred.
	 */
	const uintptr_t *canary;
};

/* smm_handler_t is called with arg of smm_module_params pointer. */
typedef asmlinkage void (*smm_handler_t)(void *);

/* SMM Runtime helpers. */
#if ENV_SMM
extern struct global_nvs *gnvs;
#endif

/* Entry point for SMM modules. */
asmlinkage void smm_handler_start(void *params);

/* Retrieve SMM save state for a given CPU. WARNING: This does not take into
 * account CPUs which are configured to not save their state to RAM. */
void *smm_get_save_state(int cpu);

/* Returns true if the region overlaps with the SMM */
bool smm_region_overlaps_handler(const struct region *r);

/* Returns true if the memory pointed to overlaps with SMM reserved memory. */
static inline bool smm_points_to_smram(const void *ptr, const size_t len)
{
	const struct region r = {(uintptr_t)ptr, len};

	return smm_region_overlaps_handler(&r);
}

/* SMM Module Loading API */

/* The smm_loader_params structure provides direction to the SMM loader:
 * - stack_top - optional external stack provided to loader. It must be at
 *               least per_cpu_stack_size * num_concurrent_stacks in size.
 * - per_cpu_stack_size - stack size per CPU for smm modules.
 * - num_concurrent_stacks - number of concurrent cpus in handler needing stack
 *                           optional for setting up relocation handler.
 * - per_cpu_save_state_size - the SMM save state size per cpu
 * - num_concurrent_save_states - number of concurrent cpus needing save state
 *                                space
 * - handler - optional handler to call. Only used during SMM relocation setup.
 * - handler_arg - optional argument to handler for SMM relocation setup. For
 *                 loading the SMM module, the handler_arg is filled in with
 *                 the address of the module's parameters (if present).
 * - runtime - this field is a result only. The SMM runtime location is filled
 *             into this field so the code doing the loading can manipulate the
 *             runtime's assumptions. e.g. updating the APIC id to CPU map to
 *             handle sparse APIC id space.
 * The following parameters are only used when X86_SMM_LOADER_VERSION2 is enabled.
 * - smm_entry - entry address of first CPU thread, all others will be tiled
 *               below this address.
 * - smm_main_entry_offset - default entry offset (e.g 0x8000)
 * - smram_start - smaram starting address
 * - smram_end - smram ending address
 */
struct smm_loader_params {
	void *stack_top;
	size_t per_cpu_stack_size;
	size_t num_concurrent_stacks;

	size_t per_cpu_save_state_size;
	size_t num_concurrent_save_states;

	smm_handler_t handler;
	void *handler_arg;

	struct smm_runtime *runtime;

	/* The following are only used by X86_SMM_LOADER_VERSION2 */
#if CONFIG(X86_SMM_LOADER_VERSION2)
	unsigned int smm_entry;
	unsigned int smm_main_entry_offset;
	unsigned int smram_start;
	unsigned int smram_end;
#endif
};

/* Both of these return 0 on success, < 0 on failure. */
int smm_setup_relocation_handler(struct smm_loader_params *params);
int smm_load_module(void *smram, size_t size, struct smm_loader_params *params);

#if CONFIG(X86_SMM_LOADER_VERSION2)
u32 smm_get_cpu_smbase(unsigned int cpu_num);
#endif

/* Backup and restore default SMM region. */
void *backup_default_smm_area(void);
void restore_default_smm_area(void *smm_save_area);

/*
 * Fills in the arguments for the entire SMM region covered by chipset
 * protections. e.g. TSEG.
 */
void smm_region(uintptr_t *start, size_t *size);

enum {
	/* SMM handler area. */
	SMM_SUBREGION_HANDLER,
	/* SMM cache region. */
	SMM_SUBREGION_CACHE,
	/* Chipset specific area. */
	SMM_SUBREGION_CHIPSET,
	/* Total sub regions supported. */
	SMM_SUBREGION_NUM,
};

/* Fills in the start and size for the requested SMM subregion. Returns
 * 0 on success, < 0 on failure. */
int smm_subregion(int sub, uintptr_t *start, size_t *size);

/* Print the SMM memory layout on console. */
void smm_list_regions(void);

#define SMM_REVISION_OFFSET_FROM_TOP (0x8000 - 0x7efc)
/* Return the SMM save state revision. The revision can be fetched from the smm savestate
   which is always at the same offset downward from the top of the save state. */
uint32_t smm_revision(void);

#endif /* CPU_X86_SMM_H */
