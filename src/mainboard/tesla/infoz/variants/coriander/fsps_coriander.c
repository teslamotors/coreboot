/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <infoz/gpio.h>
#include <infoz/variants.h>
#include <commonlib/bsd/compiler.h>
#include <soc/soc_util.h>

void __weak variant_get_dxio_ddi_descriptors(const fsp_dxio_descriptor **dxio_descs,
					     size_t *dxio_num,
					     const fsp_ddi_descriptor **ddi_descs,
					     size_t *ddi_num)
{
	*dxio_descs = variant_get_dxio_descriptors(dxio_num);
	*ddi_descs = variant_get_ddi_descriptors(ddi_num);
}

static const fsp_dxio_descriptor coriander_nogpu_dxio_descriptors[] = {
	{
		// MXM GFX lanes - not connected
		.port_present = false,
		.engine_type = UNUSED_ENGINE,
		.start_logical_lane = 8,
		.end_logical_lane = 15,
		.device_number = 1,
		.function_number = 1,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ5,
		.link_hotplug = HOTPLUG_ENHANCED,
	},
	{
		// NVMe SSD: Physical lanes 0:3, logical 4:7
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 4,
		.end_logical_lane = 7,
		.device_number = 1,
		.function_number = 7,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ4,
		.link_hotplug = HOTPLUG_ENHANCED,
	},
	{
		// Not used
		.port_present = false,
		.engine_type = UNUSED_ENGINE,
		.start_logical_lane = 3,
		.end_logical_lane = 3,
		.device_number = 1,
		.function_number = 6,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ6,
		.clk_pm_support = false,
	},
	{
		// WiFI: Physical lanes 6:6, logical 2:2
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 2,
		.end_logical_lane = 2,
		.device_number = 1,
		.function_number = 5,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ6,
		.clk_pm_support = false,
		.link_hotplug = HOTPLUG_ENHANCED,
	},
	{
		// XGBE SGMII interface: Physical lane 4, Logical lane 0
		// NOTE: Ancillary data not yet captured here due to FSP
		// limitations
		.port_present = true,
		.engine_type = ETHERNET_ENGINE,
		.start_logical_lane = 0,
		.end_logical_lane = 0,
	},
};

static const fsp_dxio_descriptor coriander_dxio_descriptors[] = {
	{
		// MXM GFX lanes - dGPU
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 8,
		.end_logical_lane = 15,
		.device_number = 1,
		.function_number = 1,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ3,
	},
	{
		// NVMe SSD: Physical lanes 0:3, logical 4:7
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 4,
		.end_logical_lane = 7,
		.device_number = 1,
		.function_number = 7,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ4,
	},
	{
		// Not used
		.port_present = false,
		.engine_type = UNUSED_ENGINE,
		.start_logical_lane = 3,
		.end_logical_lane = 3,
		.device_number = 1,
		.function_number = 6,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ6,
		.clk_pm_support = false,
	},
	{
		// WiFI: Physical lanes 6:6, logical 2:2
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 2,
		.end_logical_lane = 2,
		.device_number = 1,
		.function_number = 5,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ6,
		.clk_pm_support = false,
	},
	{
		// XGBE SGMII interface: Physical lane 4, Logical lane 0
		// NOTE: Ancillary data not yet captured here due to FSP
		// limitations
		.port_present = true,
		.engine_type = ETHERNET_ENGINE,
		.start_logical_lane = 0,
		.end_logical_lane = 0,
	},
};

const fsp_dxio_descriptor *variant_get_dxio_descriptors(size_t *num)
{
	int flags;

	flags = variant_get_flags();
	if (flags & INFOZ_HAS_DGPU) {
		*num = ARRAY_SIZE(coriander_dxio_descriptors);
		return coriander_dxio_descriptors;
	}

	*num = ARRAY_SIZE(coriander_nogpu_dxio_descriptors);
	return coriander_nogpu_dxio_descriptors;
}

static const fsp_ddi_descriptor coriander_ddi_descriptors[] = {
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
	*num = ARRAY_SIZE(coriander_ddi_descriptors);
	return coriander_ddi_descriptors;
}
