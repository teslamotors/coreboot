/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __INFOZ_DXIO_H__
#define __INFOZ_DXIO_H__

#include <stddef.h>
#include <boardid.h>
#include <soc/platform_descriptors.h>

const fsp_dxio_descriptor *infoz_get_dxio_descriptors(size_t *num);

#endif /* __INFOZ_DXIO_H__ */
