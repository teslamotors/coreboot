/* SPDX-License-Identifier: GPL-2.0-only */

#include <stddef.h>
#include <stdint.h>
#include <boot/coreboot_tables.h>

void lb_ramoops(struct lb_header *header)
{
	struct lb_range *ramoops;
	ramoops= (struct lb_range *) lb_new_record(header);
	ramoops->tag = LB_TAG_RAM_OOPS;
	ramoops->size = sizeof(*ramoops);
	ramoops->range_start = CONFIG_PSP_PERSISTENT_MEM_BASE;
	ramoops->range_size = CONFIG_PSP_PERSISTENT_MEM_SIZE;
}
