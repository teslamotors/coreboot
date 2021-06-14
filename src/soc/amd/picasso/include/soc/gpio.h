/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PICASSO_GPIO_H__
#define __PICASSO_GPIO_H__

#define GPIO_DEVICE_NAME	"AMD0030"
#define GPIO_DEVICE_DESC	"GPIO Controller"

#ifndef __ACPI__
#include <soc/iomap.h>
#include <amdblocks/gpio_banks.h>

/* The following sections describe only the GPIOs defined for this SOC */

#define SOC_GPIO_TOTAL_PINS		145

/* Bank 0: GPIO_0 - GPIO_62 */
#define GPIO_0				0
#define GPIO_1				1
#define GPIO_2				2
#define GPIO_3				3
#define GPIO_4				4
#define GPIO_5				5
#define GPIO_6				6
#define GPIO_7				7
#define GPIO_8				8
#define GPIO_9				9
#define GPIO_10				10
#define GPIO_11				11
#define GPIO_12				12
#define GPIO_13				13
#define GPIO_14				14
#define GPIO_16				16
#define GPIO_17				17
#define GPIO_18				18
#define GPIO_19				19
#define GPIO_20				20
#define GPIO_21				21
#define GPIO_22				22
#define GPIO_23				23
#define GPIO_24				24
#define GPIO_26				26
#define GPIO_27				27
#define GPIO_29				29
#define GPIO_30				30
#define GPIO_31				31
#define GPIO_32				32
#define GPIO_40				40
#define GPIO_42				42

/* Bank 1: GPIO_64 - GPIO_127 */
#define GPIO_67				67
#define GPIO_68				68
#define GPIO_69				69
#define GPIO_70				70
#define GPIO_74				74
#define GPIO_75				75
#define GPIO_76				76
#define GPIO_84				84
#define GPIO_85				85
#define GPIO_86				86
#define GPIO_87				87
#define GPIO_88				88
#define GPIO_89				89
#define GPIO_90				90
#define GPIO_91				91
#define GPIO_92				92
#define GPIO_103			103
#define GPIO_104			104
#define GPIO_105			105
#define GPIO_106			106
#define GPIO_107			107
#define GPIO_108			108
#define GPIO_109			109
#define GPIO_113			113
#define GPIO_114			114
#define GPIO_115			115
#define GPIO_116			116
#define GPIO_120			120
#define GPIO_121			121

/* Bank 2: GPIO_128 - GPIO_183 */
#define GPIO_129			129
#define GPIO_130			130
#define GPIO_131			131
#define GPIO_132			132
#define GPIO_135			135
#define GPIO_136			136
#define GPIO_137			137
#define GPIO_138			138
#define GPIO_139			139
#define GPIO_140			140
#define GPIO_141			141
#define GPIO_142			142
#define GPIO_143			143
#define GPIO_144			144

#define GPIO_SCL_HIGH			0
#define GPIO_SCL_LOW			GPIO_OUTPUT_ENABLE

/* IOMUX function names and values generated from PPR. */
#define GPIO_0_IOMUX_PWR_BTN_L 0
#define GPIO_0_IOMUX_GPIOxx 1
#define GPIO_1_IOMUX_SYS_RESET_L 0
#define GPIO_1_IOMUX_GPIOxx 1
#define GPIO_2_IOMUX_WAKE_L 0
#define GPIO_2_IOMUX_GPIOxx 1
#define GPIO_3_IOMUX_GPIOxx 0
#define GPIO_4_IOMUX_GPIOxx 0
#define GPIO_5_IOMUX_GPIOxx 0
#define GPIO_5_IOMUX_DEVSLP0 1
#define GPIO_6_IOMUX_GPIOxx 0
#define GPIO_6_IOMUX_DEVSLP1 1
#define GPIO_7_IOMUX_GPIOxx 0
#define GPIO_7_IOMUX_ACP_I2S_SDIN 1
#define GPIO_8_IOMUX_GPIOxx 0
#define GPIO_8_IOMUX_ACP_I2S_LRCLK 1
#define GPIO_9_IOMUX_GPIOxx 0
#define GPIO_9_IOMUX_SGPIO_DATAOUT 1
#define GPIO_9_IOMUX_MDIO1_SCL 2
#define GPIO_10_IOMUX_GPIOxx 0
#define GPIO_10_IOMUX_S0A3_GPIO 1
#define GPIO_10_IOMUX_SGPIO_CLK 2
#define GPIO_10_IOMUX_MDIO0_SCL 3
#define GPIO_11_IOMUX_GPIOxx 0
#define GPIO_11_IOMUX_BLINK 1
#define GPIO_12_IOMUX_LLB_L 0
#define GPIO_12_IOMUX_GPIOxx 1
#define GPIO_13_IOMUX_USB_OC5_L 0
#define GPIO_13_IOMUX_GPIOxx 1
#define GPIO_14_IOMUX_USB_OC4_L 0
#define GPIO_14_IOMUX_GPIOxx 1
#define GPIO_16_IOMUX_USB_OC0_L 0
#define GPIO_16_IOMUX_GPIOxx 1
#define GPIO_17_IOMUX_USB_OC1_L 0
#define GPIO_17_IOMUX_GPIOxx 1
#define GPIO_18_IOMUX_USB_OC2_L 0
#define GPIO_18_IOMUX_GPIOxx 1
#define GPIO_19_IOMUX_SCL1 0
#define GPIO_19_IOMUX_I2C3_SCL 1
#define GPIO_19_IOMUX_GPIOxx 2
#define GPIO_20_IOMUX_SDA1 0
#define GPIO_20_IOMUX_I2C3_SDA 1
#define GPIO_20_IOMUX_GPIOxx 2
#define GPIO_21_IOMUX_LPC_PD_L 0
#define GPIO_21_IOMUX_EMMC_CMD 1
#define GPIO_21_IOMUX_GPIOxx 2
#define GPIO_22_IOMUX_LPC_PME_L 0
#define GPIO_22_IOMUX_EMMC_PRW_CTRL 1
#define GPIO_22_IOMUX_GPIOxx 2
#define GPIO_23_IOMUX_AC_PRES 0
#define GPIO_23_IOMUX_SGPIO_LOAD 1
#define GPIO_23_IOMUX_MDIO1_SDA 2
#define GPIO_23_IOMUX_GPIOxx 3
#define GPIO_24_IOMUX_USB_OC3_L 0
#define GPIO_24_IOMUX_GPIOxx 1
#define GPIO_26_IOMUX_PCIE_RST_L 0
#define GPIO_26_IOMUX_GPIOxx 1
#define GPIO_27_IOMUX_GPIOxx 0
#define GPIO_27_IOMUX_PCIE_RST1_L 1
#define GPIO_29_IOMUX_SPI_TPM_CS_L 0
#define GPIO_29_IOMUX_GPIOxx 1
#define GPIO_29_IOMUX_USB_OC6_L 2
#define GPIO_30_IOMUX_SPI_CS2_L 0
#define GPIO_30_IOMUX_ESPI_CS_L 1
#define GPIO_30_IOMUX_GPIOxx 2
#define GPIO_31_IOMUX_SPI_CS3_L 0
#define GPIO_31_IOMUX_ESPI_CS_L 1
#define GPIO_31_IOMUX_GPIOxx 2
#define GPIO_32_IOMUX_LPC_RST_L 0
#define GPIO_32_IOMUX_SD_WP_L 1
#define GPIO_32_IOMUX_GPIOxx 2
#define GPIO_40_IOMUX_GPIOxx 0
#define GPIO_40_IOMUX_SGPIO_DATAIN 1
#define GPIO_40_IOMUX_MDIO0_SDA 2
#define GPIO_42_IOMUX_GPIOxx 0
#define GPIO_67_IOMUX_SPI_ROM_REQ 0
#define GPIO_67_IOMUX_GPIOxx 1
#define GPIO_68_IOMUX_GPIOxx 0
#define GPIO_68_IOMUX_EMMC_CD 1
#define GPIO_69_IOMUX_GPIOxx 0
#define GPIO_70_IOMUX_GPIOxx 0
#define GPIO_70_IOMUX_EMMC_CLK 1
#define GPIO_70_IOMUX_SD_CLK 2
#define GPIO_74_IOMUX_LPCCLK0 0
#define GPIO_74_IOMUX_EMMC_DATA4 1
#define GPIO_74_IOMUX_GPIOxx 2
#define GPIO_75_IOMUX_LPCCLK1 0
#define GPIO_75_IOMUX_EMMC_DATA6 1
#define GPIO_75_IOMUX_GPIOxx 2
#define GPIO_76_IOMUX_SPI_ROM_GNT 0
#define GPIO_76_IOMUX_GPIOxx 1
#define GPIO_84_IOMUX_FANIN0 0
#define GPIO_84_IOMUX_GPIOxx 1
#define GPIO_85_IOMUX_FANOUT0 0
#define GPIO_85_IOMUX_GPIOxx 1
#define GPIO_86_IOMUX_LPC_SMI_L 0
#define GPIO_86_IOMUX_GPIOxx 1
#define GPIO_87_IOMUX_SERIRQ 0
#define GPIO_87_IOMUX_EMMC_DATA7 1
#define GPIO_87_IOMUX_GPIOxx 2
#define GPIO_88_IOMUX_LPC_CLKRUN_L 0
#define GPIO_88_IOMUX_EMMC_DATA5 1
#define GPIO_88_IOMUX_GPIOxx 2
#define GPIO_89_IOMUX_GENINT1_L 0
#define GPIO_89_IOMUX_PSP_INTR0 1
#define GPIO_89_IOMUX_GPIOxx 2
#define GPIO_90_IOMUX_GENINT2_L 0
#define GPIO_90_IOMUX_PSP_INTR1 1
#define GPIO_90_IOMUX_GPIOxx 2
#define GPIO_91_IOMUX_SPKR 0
#define GPIO_91_IOMUX_GPIOxx 1
#define GPIO_92_IOMUX_CLK_REQ0_L 0
#define GPIO_92_IOMUX_SATA_IS0_L 1
#define GPIO_92_IOMUX_SATA_ZP0_L 2
#define GPIO_92_IOMUX_GPIOxx 3
#define GPIO_95_IOMUX_GPIOxx 0
#define GPIO_95_IOMUX_SD0_CLK 1
#define GPIO_96_IOMUX_GPIOxx 0
#define GPIO_96_IOMUX_SD0_CMD 1
#define GPIO_97_IOMUX_GPIOxx 0
#define GPIO_97_IOMUX_SD0_DATA0 1
#define GPIO_98_IOMUX_GPIOxx 0
#define GPIO_98_IOMUX_SD0_DATA1 1
#define GPIO_99_IOMUX_GPIOxx 0
#define GPIO_99_IOMUX_SD0_DATA2 1
#define GPIO_100_IOMUX_GPIOxx 0
#define GPIO_100_IOMUX_SD0_DATA3 1
#define GPIO_104_IOMUX_LAD0 0
#define GPIO_104_IOMUX_EMMC_DATA0 1
#define GPIO_104_IOMUX_GPIOxx 3
#define GPIO_105_IOMUX_LAD1 0
#define GPIO_105_IOMUX_EMMC_DATA1 1
#define GPIO_105_IOMUX_GPIOxx 3
#define GPIO_106_IOMUX_LAD2 0
#define GPIO_106_IOMUX_EMMC_DATA2 1
#define GPIO_106_IOMUX_GPIOxx 3
#define GPIO_107_IOMUX_LAD3 0
#define GPIO_107_IOMUX_EMMC_DATA3 1
#define GPIO_107_IOMUX_GPIOxx 3
#define GPIO_108_IOMUX_LDRQ0_L 0
#define GPIO_108_IOMUX_ESPI_ALERT_L 1
#define GPIO_108_IOMUX_GPIOxx 2
#define GPIO_109_IOMUX_LFRAME_L 0
#define GPIO_109_IOMUX_EMMC_DS 1
#define GPIO_109_IOMUX_GPIOxx 2
#define GPIO_113_IOMUX_SCL0 0
#define GPIO_113_IOMUX_I2C2_SCL 1
#define GPIO_113_IOMUX_GPIOxx 2
#define GPIO_114_IOMUX_SDA0 0
#define GPIO_114_IOMUX_I2C2_SDA 1
#define GPIO_114_IOMUX_GPIOxx 2
#define GPIO_115_IOMUX_CLK_REQ1_L 0
#define GPIO_115_IOMUX_GPIOxx 1
#define GPIO_116_IOMUX_CLK_REQ2_L 0
#define GPIO_116_IOMUX_GPIOxx 1
#define GPIO_120_IOMUX_CLK_REQ5_L 0
#define GPIO_120_IOMUX_GPIOxx 1
#define GPIO_121_IOMUX_CLK_REQ6_L 0
#define GPIO_121_IOMUX_GPIOxx 1
#define GPIO_129_IOMUX_KBRST_L 0
#define GPIO_129_IOMUX_GPIOxx 1
#define GPIO_130_IOMUX_SATA_ACT_L 0
#define GPIO_130_IOMUX_GPIOxx 1
#define GPIO_131_IOMUX_CLK_REQ3_L 0
#define GPIO_131_IOMUX_SATA_IS1_L 1
#define GPIO_131_IOMUX_SATA_ZP1_L 2
#define GPIO_131_IOMUX_GPIOxx 3
#define GPIO_132_IOMUX_CLK_REQ4_L 0
#define GPIO_132_IOMUX_OSCIN 1
#define GPIO_132_IOMUX_GPIOxx 2
#define GPIO_135_IOMUX_UART0_CTS_L 0
#define GPIO_135_IOMUX_UART2_RXD 1
#define GPIO_135_IOMUX_GPIOxx 2
#define GPIO_136_IOMUX_UART0_RXD 0
#define GPIO_136_IOMUX_GPIOxx 1
#define GPIO_137_IOMUX_UART0_RTS_L 0
#define GPIO_137_IOMUX_UART2_TXD 1
#define GPIO_137_IOMUX_GPIOxx 2
#define GPIO_138_IOMUX_UART0_TXD 0
#define GPIO_138_IOMUX_GPIOxx 1
#define GPIO_139_IOMUX_UART0_INTR 0
#define GPIO_139_IOMUX_GPIOxx 1
#define GPIO_140_IOMUX_UART1_CTS_L 0
#define GPIO_140_IOMUX_UART3_TXD 1
#define GPIO_140_IOMUX_GPIOxx 2
#define GPIO_141_IOMUX_UART1_RXD 0
#define GPIO_141_IOMUX_GPIOxx 1
#define GPIO_142_IOMUX_UART1_RTS_L 0
#define GPIO_142_IOMUX_UART3_RXD 1
#define GPIO_142_IOMUX_GPIOxx 2
#define GPIO_143_IOMUX_UART1_TXD 0
#define GPIO_143_IOMUX_GPIOxx 1
#define GPIO_144_IOMUX_UART1_INTR 0
#define GPIO_144_IOMUX_GPIOxx 1

#define GPIO_2_EVENT GEVENT_8

/* Array of all GPIO pin numbers used */
#define SOC_GPIO_ALL_PINS	\
{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,16,17,18,19,20,21,22,23,24,26,27,29, \
30,31,32,40,42,67,68,69,70,74,75,76,84,85,86,87,88,89,90,91,92,103,104,  \
105,106,107,108,109,113,114,115,116,120,121,129,130,131,132,135,136,137, \
138,139,140,141,142,143,144,}
/*
 * Indicate which IOMUX function is for GPIO mode, indexed by GPIO number
 * e.g., the first element value of '1' indicates that for GPIO0, an IOMUX
 * function value of '1' would be used for using GPIO0 in GPIO mode.
 */
#define SOC_IOMUX_GPIO_FUNC_MAP	{ \
	1, 1, 1, 0, 0, 0, 0, 0, 0, 0, \
	0, 0, 1, 1, 1, 0, 1, 1, 1, 2, \
	2, 2, 2, 3, 1, 0, 1, 0, 0, 1, \
	2, 2, 2, 0, 0, 0, 0, 0, 0, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
	0, 0, 0, 0, 0, 0, 0, 1, 0, 0, \
	0, 0, 0, 0, 2, 2, 1, 0, 0, 0, \
	0, 0, 0, 0, 1, 1, 1, 2, 2, 2, \
	2, 1, 3, 0, 0, 0, 0, 0, 0, 0, \
	0, 0, 0, 0, 3, 3, 3, 3, 2, 2, \
	0, 0, 0, 2, 2, 1, 1, 0, 0, 0, \
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, \
	1, 3, 2, 0, 0, 2, 1, 2, 1, 1, \
	2, 1, 2, 1, 1,}

#endif /* __ACPI__ */
#endif /* __PICASSO_GPIO_H__ */
