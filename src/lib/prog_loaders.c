/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <stdlib.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <fallback.h>
#include <halt.h>
#include <lib.h>
#include <program_loading.h>
#include <reset.h>
#include <romstage_handoff.h>
#include <rmodule.h>
#include <rules.h>
#include <stage_cache.h>
#include <symbols.h>
#include <timestamp.h>

/* Only can represent up to 1 byte less than size_t. */
const struct mem_region_device addrspace_32bit =
	MEM_REGION_DEV_RO_INIT(0, ~0UL);

int prog_locate(struct prog *prog)
{
	struct cbfsf file;

	cbfs_prepare_program_locate();

	if (cbfs_boot_locate(&file, prog_name(prog), NULL))
		return -1;

	cbfs_file_data(prog_rdev(prog), &file);

	return 0;
}

void run_romstage(void)
{
	struct prog romstage =
		PROG_INIT(PROG_ROMSTAGE, CONFIG_CBFS_PREFIX "/romstage");

	if (prog_locate(&romstage))
		goto fail;

	timestamp_add_now(TS_START_COPYROM);

	if (cbfs_prog_stage_load(&romstage))
		goto fail;

	timestamp_add_now(TS_END_COPYROM);

	prog_run(&romstage);

fail:
	if (IS_ENABLED(CONFIG_BOOTBLOCK_CONSOLE))
		die("Couldn't load romstage.\n");
	halt();
}

void __attribute__((weak)) stage_cache_add(int stage_id,
						const struct prog *stage) {}
void __attribute__((weak)) stage_cache_load_stage(int stage_id,
							struct prog *stage) {}

static void ramstage_cache_invalid(void)
{
	printk(BIOS_ERR, "ramstage cache invalid.\n");
	if (IS_ENABLED(CONFIG_RESET_ON_INVALID_RAMSTAGE_CACHE)) {
		hard_reset();
		halt();
	}
}

static void run_ramstage_from_resume(struct prog *ramstage)
{
	if (!romstage_handoff_is_resume())
		return;

	/* Load the cached ramstage to runtime location. */
	stage_cache_load_stage(STAGE_RAMSTAGE, ramstage);

	if (prog_entry(ramstage) != NULL) {
		printk(BIOS_DEBUG, "Jumping to image.\n");
		prog_run(ramstage);
	}
	ramstage_cache_invalid();
}

static int load_relocatable_ramstage(struct prog *ramstage)
{
	struct rmod_stage_load rmod_ram = {
		.cbmem_id = CBMEM_ID_RAMSTAGE,
		.prog = ramstage,
	};

	return rmodule_stage_load(&rmod_ram);
}

static int load_nonrelocatable_ramstage(struct prog *ramstage)
{
	if (IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)) {
		uintptr_t base = 0;
		size_t size = cbfs_prog_stage_section(ramstage, &base);
		if (size)
			backup_ramstage_section(base, size);
	}

	return cbfs_prog_stage_load(ramstage);
}

void run_ramstage(void)
{
	struct prog ramstage =
		PROG_INIT(PROG_RAMSTAGE, CONFIG_CBFS_PREFIX "/ramstage");

	timestamp_add_now(TS_END_ROMSTAGE);

	/*
	 * Only x86 systems using ramstage stage cache currently take the same
	 * firmware path on resume.
	 */
	if (IS_ENABLED(CONFIG_ARCH_X86) &&
	    !IS_ENABLED(CONFIG_NO_STAGE_CACHE) &&
	    IS_ENABLED(CONFIG_EARLY_CBMEM_INIT))
		run_ramstage_from_resume(&ramstage);

	if (prog_locate(&ramstage))
		goto fail;

	timestamp_add_now(TS_START_COPYRAM);

	if (IS_ENABLED(CONFIG_RELOCATABLE_RAMSTAGE)) {
		if (load_relocatable_ramstage(&ramstage))
			goto fail;
	} else if (load_nonrelocatable_ramstage(&ramstage))
		goto fail;

	stage_cache_add(STAGE_RAMSTAGE, &ramstage);

	timestamp_add_now(TS_END_COPYRAM);

	prog_run(&ramstage);

fail:
	die("Ramstage was not loaded!\n");
}

#ifdef __RAMSTAGE__ // gc-sections should take care of this

void __attribute__((weak)) update_dtb(unsigned char *load, int size) {}

int __attribute__((weak)) use_dtb2(void)
{
	return 0;
}

static void linux_dtb_initrd_load(void)
{
	struct prog linux2 =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/linux");
	struct prog dtb =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/dtb");
	struct prog dtb2 =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/dtb2");
	struct prog initrd =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/initrd");

	struct prog *p_dtb = use_dtb2() ? &dtb2 : &dtb;

	if (!IS_ENABLED(CONFIG_JUMP_TO_KERNEL))
		return;

	if (!prog_locate(&linux2)) {
		timestamp_add_now(TS_START_COPYLINUX);
		if (cbfs_prog_stage_load(&linux2))
			die("linux was not loaded!.\n");

		timestamp_add_now(TS_END_COPYLINUX);
	} else
		printk(BIOS_ERR, "linux was not found.\n");

	if (!prog_locate(p_dtb)) {
		unsigned char *load;
		int size;

		timestamp_add_now(TS_START_COPYDTB);
		if (cbfs_prog_stage_load_ex(p_dtb, &load, &size))
			die("dtb was not loaded.\n");

		update_dtb(load, size);

		timestamp_add_now(TS_END_COPYDTB);
	} else
		printk(BIOS_ERR, "dtb was not found.\n");

	if (!prog_locate(&initrd)) {
		timestamp_add_now(TS_START_COPYINITRD);
		if (cbfs_prog_stage_load(&initrd))
			die("initrd was not loaded.\n");

		timestamp_add_now(TS_END_COPYINITRD);
	} else
		printk(BIOS_INFO, "initrd was not found.\n");
}

static struct prog global_payload =
	PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/payload");

void __attribute__((weak)) mirror_payload(struct prog *payload)
{
}

void payload_load(void)
{
#if !IS_ENABLED(CONFIG_PAYLOAD_NONE)
	struct prog *payload = &global_payload;
#endif

	linux_dtb_initrd_load();

#if !IS_ENABLED(CONFIG_PAYLOAD_NONE)
	timestamp_add_now(TS_LOAD_PAYLOAD);

	if (prog_locate(payload))
		goto out;

	mirror_payload(payload);

	/* Pass cbtables to payload if architecture desires it. */
	prog_set_entry(payload, selfload(payload, true),
			cbmem_find(CBMEM_ID_CBTABLE));

out:
	if (prog_entry(payload) == NULL)
		die("Payload not loaded.\n");
#endif
}

void payload_run(void)
{
	struct prog *payload = &global_payload;

	/* Reset to booting from this image as late as possible */
	boot_successful();

#if !IS_ENABLED(CONFIG_PAYLOAD_NONE)
	printk(BIOS_ERR, "Jumping to boot code at %p(%p)\n",
		prog_entry(payload), prog_entry_arg(payload));

	post_code(POST_ENTER_ELF_BOOT);

	timestamp_add_now(TS_SELFBOOT_JUMP);

	/* Before we go off to run the payload, see if
	 * we stayed within our bounds.
	 */
	checkstack(_estack, 0);
#endif

	prog_run(payload);
}

#endif
