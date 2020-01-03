/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/cache.h>
#include <arch/lib_helpers.h>
#include <arch/transition.h>
#include <arm_tf.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <symbols.h>

/*Enable this setting for booting Excite.
This will set the SPSR to EL1
*/
#if IS_ENABLED(CONFIG_JUMP_TO_EXCITE)
#define BOOT_EXCITE_SETTING         1
#else
#define BOOT_EXCITE_SETTING         0
#endif

typedef struct bl31_plat_params {
        uint32_t        type;
        uint64_t        spsr;
} bl31_plat_params_t;

static bl31_plat_params_t trav_plat_params;

void *soc_get_bl31_plat_params(bl31_params_t *params)
{

	trav_plat_params.type = BOOT_EXCITE_SETTING;
	return &trav_plat_params;
}

