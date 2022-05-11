/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __VBOOT_FIRMWARE_STASH_H__
#define __VBOOT_FIRMWARE_STASH_H__

#include <commonlib/region.h>
#include <stdlib.h>

uint8_t *platform_get_firmware_stash(void);

size_t platform_firmware_stash_size(void);

int cbfs_get_boot_device_stash(struct region_device **rdev);

#endif /* __VBOOT_FIRMWARE_STASH_H__ */
