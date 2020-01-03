#ifndef CPU_TURBO_TRAV_NETSPEED_C_H
#define CPU_TURBO_TRAV_NETSPEED_C_H

#define bridge_blk_cam_csi_bus_d_cam_csi_22_49_base     0x1180b000
#define bridge_blk_cam_dprx0_bus_d0_cam_dprx_23_54_base 0x11813000
#define bridge_blk_cam_dprx1_bus_d1_cam_dprx_24_55_base 0x11817000
#define bridge_blk_cpucl_0_ca72_m0_0_104_base           0x11827000
#define bridge_blk_cpucl_1_ca72_m1_1_170_base           0x1182f000
#define bridge_blk_cpucl_1_ca72_m2_2_236_base           0x11833000
#define bridge_blk_fsys0_bus_d_fsys0_18_284_base        0x11837000
#define bridge_blk_fsys0_tcu_12_284_base                0x11843000
#define bridge_blk_fsys1_bus_d0_fsys1_19_60_base        0x11847000
#define bridge_blk_gpu_gpu_m0_3_140_base                0x11853000
#define bridge_blk_gpu_gpu_m1_4_140_base                0x11857000
#define bridge_blk_gpu_gpu_m2_5_141_base                0x1185b000
#define bridge_blk_gpu_gpu_m3_6_141_base                0x1185f000
#define bridge_blk_imem_bus_d0_imem_25_62_base          0x11863000
#define bridge_blk_imem_tcu_13_62_base                  0x1186b000
#define bridge_blk_isp_isp_m0_26_47_base                0x11873000
#define bridge_blk_isp_isp_m1_27_47_base                0x11877000
#define bridge_blk_isp_tcu_14_47_base                   0x1187b000
#define bridge_blk_mfc_mfc_m0_28_69_base                0x11883000
#define bridge_blk_mfc_mfc_m1_29_69_base                0x11887000
#define bridge_blk_peric_bus_d_peric_15_266_base        0x118cb000
#define bridge_blk_pmu_cssys0_30_86_base                0x118db000
#define bridge_blk_pmu_cssys1_31_86_base                0x118df000
#define bridge_blk_scs_bus_d_scs_32_64_base             0x118e3000
#define bridge_blk_sms_bus_d_sms_33_64_base             0x118e7000
#define bridge_blk_trip_mmu0_trip_m_20_135_base         0x11903000
#define bridge_blk_trip_mmu0_triptcu_16_135_base        0x11907000
#define bridge_blk_trip_mmu1_trip_m_21_142_base         0x1190f000
#define bridge_blk_trip_mmu1_triptcu_17_142_base        0x11913000
#define bridge_ccc0_ccc_m_34_258_base                   0x1192f000
#define bridge_ccc1_ccc_m_35_236_base                   0x11937000
#define bridge_ccc2_ccc_m_36_148_base                   0x1193f000
#define bridge_ccc3_ccc_m_37_104_base                   0x11947000
#define bridge_io_0_iocb_m_7_218_base                   0x11953000
#define bridge_io_1_iocb_m_8_52_base                    0x1195b000
#define bridge_io_2_iocb_m_9_266_base                   0x11963000
#define bridge_trip0_iocb_m_10_139_base                 0x1196b000
#define bridge_trip1_iocb_m_11_146_base                 0x11973000

#define DMC_REG0    0x0110

void noc_address_stride_update (void);

#endif
