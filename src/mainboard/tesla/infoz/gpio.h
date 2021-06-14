/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __INFOZ_GPIO_H__
#define __INFOZ_GPIO_H__

#ifndef __ACPI__
#include <soc/gpio.h>
#include <platform_descriptors.h>

#endif	/* __ACPI__ */

/* External interrupt lines going into SoC */
#define GPIO_EXP3_INT		GPIO_10
#define CENTER_TOUCH_INT	GPIO_14
#define GTW_SOC_INT		GPIO_17
#define SER2_ICU_INT		GPIO_23
#define SER2_ICU_REM_INT	GPIO_24
#define GPIO_EXP2_INT		GPIO_29
#define GPIO_EXP1_INT		GPIO_31
#define SER1_REAR_INT		GPIO_85
#define LPC_SERIRQ		GPIO_87
#define SER1_REAR_REM_INT	GPIO_89
#define SER1_REAR_TOUCH_INT	GPIO_139

/* Reset peripheral devices */
#define ETH_WLAN_PCIE_RST	GPIO_26
#define PCIE_RST1_L		GPIO_27
#define NVME_PCIE_RST		PCIE_RST1_L
#define SER1_REAR_TOUCH_RST	GPIO_144
#define GPIO_GPU_RST		GPIO_120

/* PCIe bifurcation for GPP[0:7] */
#define NVME_START_LANE 0
#define NVME_END_LANE 3
#define WLAN_START_LANE 6
#define WLAN_END_LANE 6

/* PCIe reset pins */
#define PCIE_0_RST		ETH_WLAN_PCIE_RST
#define PCIE_1_RST		NVME_PCIE_RST
#define PCIE_2_RST		0
#define PCIE_3_RST		0
#define PCIE_4_RST		0

/* Power enable pins */
#define SOC_NVME_EN		GPIO_8
#define SOC_WIFI_EN		GPIO_42
#define SOC_GPU_EN		GPIO_108

/* Board ID pins */
#define GPIO_BOARD_ID_0		GPIO_130
#define GPIO_BOARD_ID_1		GPIO_69
#define GPIO_BOARD_ID_2		GPIO_70
#define GPIO_BOARD_ID_3		GPIO_116

#define GPIO_BOOT_BANK_SEL	GPIO_115
#define GPIO_BOOT_RECOVERY	GPIO_84

#endif /* __INFOZ_GPIO_H__ */
