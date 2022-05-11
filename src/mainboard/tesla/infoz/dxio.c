/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <infoz/gpio.h>
#include <infoz/variants.h>
#include <commonlib/bsd/compiler.h>
#include <soc/soc_util.h>
#include "dxio.h"

/*
 * Common DXIO descriptor tables shared across all InfoZ variants
 * (unless overridden)
 */

static const fsp_dxio_descriptor infoz_nogpu_dxio_descriptors[] = {
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

static const fsp_dxio_descriptor infoz_dxio_descriptors[] = {
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

const fsp_dxio_descriptor *infoz_get_dxio_descriptors(size_t *num)
{
	int flags;

	flags = variant_get_flags();
	if (flags & INFOZ_HAS_DGPU) {
		*num = ARRAY_SIZE(infoz_dxio_descriptors);
		return infoz_dxio_descriptors;
	}

	*num = ARRAY_SIZE(infoz_nogpu_dxio_descriptors);
	return infoz_nogpu_dxio_descriptors;
}
