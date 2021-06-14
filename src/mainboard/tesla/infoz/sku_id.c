/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boardid.h>
#include <infoz/variants.h>

uint32_t sku_id(void)
{
/* XXX: FIXME */
	return 3;
#if 0
	return google_chromeec_get_board_sku();
#endif
}

/* An unprovisioned SKU ID indicates we're in the factory booting prior to
   proper SKU ID provisioning. */
int boot_is_factory_unprovisioned(void)
{
/* XXX: FIXME */
#if 0
	return sku_id() == CROS_SKU_UNPROVISIONED;
#else
	return 0;
#endif
}
