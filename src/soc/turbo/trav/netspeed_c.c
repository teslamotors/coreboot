/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Tesla Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <delay.h>
#include <soc/common_definition.h>
#include <soc/netspeed.h>
#include <soc/cpu.h>
#include <soc/netspeed_c.h>

static void trav_write_all_noc_bridges (uint64_t offset, uint64_t value)
{
    writell(value, bridge_blk_cam_csi_bus_d_cam_csi_22_49_base + offset);
    writell(value, bridge_blk_cam_dprx0_bus_d0_cam_dprx_23_54_base + offset);
    writell(value, bridge_blk_cam_dprx1_bus_d1_cam_dprx_24_55_base + offset);
    writell(value, bridge_blk_cpucl_0_ca72_m0_0_104_base + offset);
    writell(value, bridge_blk_cpucl_1_ca72_m1_1_170_base + offset);
    writell(value, bridge_blk_cpucl_1_ca72_m2_2_236_base + offset);
    writell(value, bridge_blk_fsys0_bus_d_fsys0_18_284_base + offset);
    writell(value, bridge_blk_fsys0_tcu_12_284_base + offset);
    writell(value, bridge_blk_fsys1_bus_d0_fsys1_19_60_base + offset);
    writell(value, bridge_blk_gpu_gpu_m0_3_140_base + offset);
    writell(value, bridge_blk_gpu_gpu_m1_4_140_base + offset);
    writell(value, bridge_blk_gpu_gpu_m2_5_141_base + offset);
    writell(value, bridge_blk_gpu_gpu_m3_6_141_base + offset);
    writell(value, bridge_blk_imem_bus_d0_imem_25_62_base + offset);
    writell(value, bridge_blk_imem_tcu_13_62_base + offset);
    writell(value, bridge_blk_isp_isp_m0_26_47_base + offset);
    writell(value, bridge_blk_isp_isp_m1_27_47_base + offset);
    writell(value, bridge_blk_isp_tcu_14_47_base + offset);
    writell(value, bridge_blk_mfc_mfc_m0_28_69_base + offset);
    writell(value, bridge_blk_mfc_mfc_m1_29_69_base + offset);
    writell(value, bridge_blk_peric_bus_d_peric_15_266_base + offset);
    writell(value, bridge_blk_pmu_cssys0_30_86_base + offset);
    writell(value, bridge_blk_pmu_cssys1_31_86_base + offset);
    writell(value, bridge_blk_scs_bus_d_scs_32_64_base + offset);
    writell(value, bridge_blk_sms_bus_d_sms_33_64_base + offset);
    writell(value, bridge_blk_trip_mmu0_trip_m_20_135_base + offset);
    writell(value, bridge_blk_trip_mmu0_triptcu_16_135_base + offset);
    writell(value, bridge_blk_trip_mmu1_trip_m_21_142_base + offset);
    writell(value, bridge_blk_trip_mmu1_triptcu_17_142_base + offset);
    writell(value, bridge_ccc0_ccc_m_34_258_base + offset);
    writell(value, bridge_ccc1_ccc_m_35_236_base + offset);
    writell(value, bridge_ccc2_ccc_m_36_148_base + offset);
    writell(value, bridge_ccc3_ccc_m_37_104_base + offset);
    writell(value, bridge_io_0_iocb_m_7_218_base + offset);
    writell(value, bridge_io_1_iocb_m_8_52_base + offset);
    writell(value, bridge_io_2_iocb_m_9_266_base + offset);
    writell(value, bridge_trip0_iocb_m_10_139_base + offset);
    writell(value, bridge_trip1_iocb_m_11_146_base + offset);
}


static void trav_set_mif_address_compaction (uint32_t num_channels, uint32_t stride_size)
{
    uint32_t value = 0; //also the reset value

    if (num_channels == 8) {
        if (stride_size == 256) {
            value = 0x2;
        } else if (stride_size == 1024) {
            value = 0x1;
        } //512 is default
    } else if (num_channels == 4) {
        if (stride_size == 256) {
            value = 0x6;
        } else if (stride_size == 1024) {
            value = 0x5;
        } //512 is default
    }
    if (value != 0) {
        write32((void *)TRAV_SYSREG_MIF0 + DMC_REG0, value);
        write32((void *)TRAV_SYSREG_MIF1 + DMC_REG0, value);
        write32((void *)TRAV_SYSREG_MIF2 + DMC_REG0, value);
        write32((void *)TRAV_SYSREG_MIF3 + DMC_REG0, value);
        write32((void *)TRAV_SYSREG_MIF4 + DMC_REG0, value);
        write32((void *)TRAV_SYSREG_MIF5 + DMC_REG0, value);
        write32((void *)TRAV_SYSREG_MIF6 + DMC_REG0, value);
        write32((void *)TRAV_SYSREG_MIF7 + DMC_REG0, value);
    }
}

static void trav_noc_disable_override_MIF_AR_AW (void)
{
    //enabled at reset

    writell(0, 0x1188df20); // MIF0 as_awovrd
    writell(0, 0x1188df18); // MIF0 as_arovrd

    writell(0, 0x11895f20); // MIF1 as_awovrd
    writell(0, 0x11895f18); // MIF1 as_arovrd

    writell(0, 0x1189df20); // MIF2 as_awovrd
    writell(0, 0x1189df18); // MIF2 as_arovrd

    writell(0, 0x118a5f20); // MIF3 as_awovrd
    writell(0, 0x118a5f18); // MIF3 as_arovrd

    writell(0, 0x118adf20); // MIF4 as_awovrd
    writell(0, 0x118adf18); // MIF4 as_arovrd

    writell(0, 0x118b5f20); // MIF5 as_awovrd
    writell(0, 0x118b5f18); // MIF5 as_arovrd

    writell(0, 0x118bdf20); // MIF6 as_awovrd
    writell(0, 0x118bdf18); // MIF6 as_arovrd

    writell(0, 0x118c5f20); // MIF7 as_awovrd
    writell(0, 0x118c5f18); // MIF7 as_arovrd
}

static void trav_master_bridge_set_AR_AW (void)
{

    //define RT_CLASS         3
    //define COH_CLASS        0
    //define TP_CLASS         1
    //define IO_CLASS         2

    //[20-23] --> 1 means value is overridden, 0 means value unchanged
    //[16-19] --> Value to override QOS
    //CCC to take same QOS value as CPU

    // ISP, MFC, GPU should be fine and set to 1 (to be confirmed)

    writell(0xf30000, 0x11817f68); // DPRX0 AM_awovrd
    writell(0xf30000, 0x11813f68); // DPRX2 AM_awovrd
    writell(0xf30000, 0x1180bf68); // CSI AM_awovrd

    writell(0xf00000, 0x11827f60); // CL0 AM_arovrd
    writell(0xf00000, 0x11827f68); // CL0 AM_awovrd

    writell(0xf00000, 0x1182ff60); // CL1 AM_arovrd
    writell(0xf00000, 0x1182ff68); // CL1 AM_awovrd

    writell(0xf00000, 0x11833f60); // CL2 AM_arovrd
    writell(0xf00000, 0x11833f68); // CL2 AM_awovrd

    writell(0xf00000, 0x1192ff60); //CCC0 AM_arovrd
    writell(0xf00000, 0x1192ff68); //CCC0 AM_awovrd
    writell(0xf00000, 0x11937f60); //CCC1 AM_arovrd
    writell(0xf00000, 0x11937f68); //CCC1 AM_awovrd
    writell(0xf00000, 0x1193ff60); //CCC2 AM_arovrd
    writell(0xf00000, 0x1193ff68); //CCC2 AM_awovrd
    writell(0xf00000, 0x11947f60); //CCC3 AM_arovrd
    writell(0xf00000, 0x11947f68); //CCC3 AM_awovrd

    writell(0xf10000, 0x1196bf60); // TRIP0 IOCB AM_arovrd
    writell(0xf10000, 0x1196bf68); // TRIP0 IOCB AM_awovrd

    writell(0xf10000, 0x11973f60); // TRIP1 IOCB AM_arovrd
    writell(0xf10000, 0x11973f68); // TRIP1 IOCB AM_awovrd

    writell(0xf10000, 0x11903f60); // TRIP0 AM_arovrd
    writell(0xf10000, 0x11903f68); // TRIP0 AM_awovrd

    writell(0xf10000, 0x1190ff60); // TRIP1 AM_arovrd
    writell(0xf10000, 0x1190ff68); // TRIP1 AM_awovrd

    writell(0xf10000, 0x11907f60); // TRIP0 TCU0 AM_arovrd
    writell(0xf10000, 0x11913f60); // TRIP0 TCU1 AM_arovrd
    writell(0xf10000, 0x11907f68); // TRIP0 TCU0 AM_arovrd
    writell(0xf10000, 0x11913f68); // TRIP0 TCU1 AM_arovrd

    writell (0xf20000, 0x11847f60); // PCIE AM_arovrd
    writell(0xf20000, 0x11847f68); // PCIE AM_awovrd
}

void noc_address_stride_update(void)
{
    // hashing enables full use of CCC, avoid hot-spotting of one MCT
    // bit0 = 0001 1000 1010 0001 0000 0000
    // bit1 = 0100 0101 0001 0100 0000 0000
    // bit2 = 0010 0010 0100 1010 0000 0000
    // 256B stride
    uint64_t mem_hash_reg0 = 0x18A100;
    uint64_t mem_hash_reg1 = 0x451400;
    uint64_t mem_hash_reg2 = 0x224A00;
    uint64_t stride_size = 0x100;

    trav_write_all_noc_bridges(0x410, mem_hash_reg0);
    trav_write_all_noc_bridges(0x408, mem_hash_reg1);
    trav_write_all_noc_bridges(0x400, mem_hash_reg2);
    trav_set_mif_address_compaction(8, (uint32_t)stride_size);

    trav_noc_disable_override_MIF_AR_AW();
    trav_master_bridge_set_AR_AW();

    udelay(100); //FIXME, dont want traffic going out till system is stable
}
