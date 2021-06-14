/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <rmodule.h>
#include <security/vboot/misc.h>
#include <security/vboot/symbols.h>
#include <security/vboot/vboot_common.h>

/* Ensure vboot configuration is valid: */
_Static_assert(CONFIG(VBOOT_STARTS_IN_BOOTBLOCK) +
	       CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK) +
	       CONFIG(VBOOT_STARTS_IN_ROMSTAGE) == 1,
	       "vboot must start in bootblock, PSP or romstage (but only one!)");
_Static_assert(!CONFIG(VBOOT_SEPARATE_VERSTAGE) || CONFIG(VBOOT_STARTS_IN_BOOTBLOCK) ||
	       CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK),
	       "stand-alone verstage must start in or before bootblock ");
_Static_assert(!CONFIG(VBOOT_RETURN_FROM_VERSTAGE) ||
	       CONFIG(VBOOT_SEPARATE_VERSTAGE),
	       "return from verstage only makes sense for separate verstages");

int vboot_executed;

void vboot_run_logic(void)
{
	if (verification_should_run()) {
		/* Note: this path is not used for VBOOT_RETURN_FROM_VERSTAGE */
		verstage_main();
		vboot_executed = 1;
	} else if (verstage_should_load()) {
		struct cbfsf file;
		struct prog verstage =
			PROG_INIT(PROG_VERSTAGE,
				CONFIG_CBFS_PREFIX "/verstage");

		printk(BIOS_DEBUG, "VBOOT: Loading verstage.\n");

		/* load verstage from RO */
		if (cbfs_boot_locate(&file, prog_name(&verstage), NULL))
			die("failed to load verstage");

		cbfs_file_data(prog_rdev(&verstage), &file);

		if (cbfs_prog_stage_load(&verstage))
			die("failed to load verstage");

		/* verify and select a slot */
		prog_run(&verstage);

		/* This is not actually possible to hit this condition at
		 * runtime, but this provides a hint to the compiler for dead
		 * code elimination below. */
		if (!CONFIG(VBOOT_RETURN_FROM_VERSTAGE))
			return;

		vboot_executed = 1;
	}
}

int vboot_locate_cbfs(struct region_device *rdev)
{
	struct vb2_context *ctx;
	int is_recovery;

	/* Don't honor vboot results until the vboot logic has run. */
	if (!vboot_logic_executed())
		return -1;

	ctx = vboot_get_context();
	is_recovery = ctx->flags & VB2_CONTEXT_RECOVERY_MODE;

	if (CONFIG(VBOOT_REFUSE_RECOVERY)) {
		if (!is_recovery && !vboot_locate_firmware(ctx, rdev))
			return 0;

		/* die instead of informing caller to look in COREBOOT */
		vb2ex_abort();
	}

	if (is_recovery)
		return -1;

	return vboot_locate_firmware(ctx, rdev);
}
