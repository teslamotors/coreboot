/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <infoz/gpio.h>
#include <infoz/variants.h>
#include <commonlib/bsd/compiler.h>
#include <soc/soc_util.h>
#include <infoz/dxio.h>

void __weak variant_get_dxio_ddi_descriptors(const fsp_dxio_descriptor **dxio_descs,
					     size_t *dxio_num,
					     const fsp_ddi_descriptor **ddi_descs,
					     size_t *ddi_num)
{
	*dxio_descs = infoz_get_dxio_descriptors(dxio_num);
	*ddi_descs = variant_get_ddi_descriptors(ddi_num);
}

static const fsp_ddi_descriptor semi_ddi_descriptors[] = {
	{
		// DDI0, HDMI
		.connector_type = HDMI,
		.aux_index = AUX1,
		.hdp_index = HDP1
	},
	{
		// DDI1, HDMI
		.connector_type = HDMI,
		.aux_index = AUX2,
		.hdp_index = HDP2
	},
	{
		// DDI2, DP
		.connector_type = DP,
		.aux_index = AUX3,
		.hdp_index = HDP3,
	},
	{
		// DDI3, DP
		.connector_type = DP,
		.aux_index = AUX4,
		.hdp_index = HDP4,
	}
};

const fsp_ddi_descriptor *variant_get_ddi_descriptors(size_t *num)
{
	*num = ARRAY_SIZE(semi_ddi_descriptors);
	return semi_ddi_descriptors;
}
