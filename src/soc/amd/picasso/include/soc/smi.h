/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __SOUTHBRIDGE_AMD_PI_PICASSO_SMI_H__
#define __SOUTHBRIDGE_AMD_PI_PICASSO_SMI_H__

#include <types.h>

#define SMI_GEVENTS			24
#define SCIMAPS				58
#define SCI_GPES			32

#define SMI_EVENT_STATUS		0x0
#define SMI_EVENT_ENABLE		0x04
#define SMI_SCI_TRIG			0x08
#define SMI_SCI_LEVEL			0x0c
#define SMI_SCI_STATUS			0x10
#define SMI_SCI_EN			0x14
#define SMI_SCI_MAP0			0x40
# define SMI_SCI_MAP(X)			(SMI_SCI_MAP0 + (X))

/* SMI source and status */
#define SMITYPE_G_GENINT1_L		0
#define SMITYPE_G_GENINT2_L		1
#define SMITYPE_G_AGPIO3		2
#define SMITYPE_G_LPCPME		3
#define SMITYPE_G_AGPIO4		4
#define SMITYPE_G_LPCPD			5
#define SMITYPE_G_SPKR			6
#define SMITYPE_G_AGPIO5		7
#define SMITYPE_G_WAKE_L		8
#define SMITYPE_G_LPC_SMI_L		9
#define SMITYPE_G_AGPIO6		10
#define SMITYPE_G_AGPIO7		11
#define SMITYPE_G_USBOC0_L		12
#define SMITYPE_G_USBOC1_L		13
#define SMITYPE_G_USBOC2_L		14
#define SMITYPE_G_USBOC3_L		15
#define SMITYPE_G_AGPIO23		16
#define SMITYPE_G_ESPI_RESET_L		17
#define SMITYPE_G_FANIN0		18
#define SMITYPE_G_SYSRESET_L		19
#define SMITYPE_G_AGPIO40		20
#define SMITYPE_G_PWR_BTN_L		21
#define SMITYPE_G_AGPIO9		22
#define SMITYPE_G_AGPIO8		23
#define GEVENT_MASK ((1 << SMITYPE_G_GENINT1_L)		\
		   | (1 << SMITYPE_G_GENINT2_L)		\
		   | (1 << SMITYPE_G_AGPIO3)		\
		   | (1 << SMITYPE_G_LPCPME)		\
		   | (1 << SMITYPE_G_AGPIO4)		\
		   | (1 << SMITYPE_G_LPCPD)		\
		   | (1 << SMITYPE_G_SPKR)		\
		   | (1 << SMITYPE_G_AGPIO5)		\
		   | (1 << SMITYPE_G_WAKE_L)		\
		   | (1 << SMITYPE_G_LPC_SMI_L)		\
		   | (1 << SMITYPE_G_AGPIO6)		\
		   | (1 << SMITYPE_G_AGPIO7)		\
		   | (1 << SMITYPE_G_USBOC0_L)		\
		   | (1 << SMITYPE_G_USBOC1_L)		\
		   | (1 << SMITYPE_G_USBOC2_L)		\
		   | (1 << SMITYPE_G_USBOC3_L)		\
		   | (1 << SMITYPE_G_AGPIO23)		\
		   | (1 << SMITYPE_G_ESPI_RESET_L)	\
		   | (1 << SMITYPE_G_FANIN0)		\
		   | (1 << SMITYPE_G_SYSRESET_L)	\
		   | (1 << SMITYPE_G_AGPIO40)		\
		   | (1 << SMITYPE_G_PWR_BTN_L))
#define SMITYPE_MP2_WAKE		24
#define SMITYPE_MP2_GPIO0		25
#define SMITYPE_ESPI_SYS		26
#define SMITYPE_ESPI_WAKE_PME		27
#define SMITYPE_MP2_GPIO1		28
#define SMITYPE_GPP_PME			29
#define SMITYPE_NB_GPP_HOT_PLUG		30
/* 31 Reserved */
#define SMITYPE_WAKE_L2			32
#define SMITYPE_PSP			33
/* 33 - 38 Reserved */
#define SMITYPE_AZPME			39
#define SMITYPE_USB_PD_I2C4		40
#define SMITYPE_GPIO_CTL		41
/* 42 Reserved */
#define SMITYPE_ALT_HPET_ALARM		43
#define SMITYPE_FAN_THERMAL		44
#define SMITYPE_ASF_MASTER_SLAVE	45
#define SMITYPE_I2S_WAKE		46
#define SMITYPE_SMBUS0_MASTER		47
#define SMITYPE_TWARN			48
#define SMITYPE_TRAFFIC_MON		49
#define SMITYPE_ILLB			50
#define SMITYPE_PWRBUTTON_UP		51
#define SMITYPE_PROCHOT			52
#define SMITYPE_APU_HW			53
#define SMITYPE_NB_SCI			54
#define SMITYPE_RAS_SERR		55
#define SMITYPE_XHC0_PME		56
#define SMITYPE_XHC1_PME		57
#define SMITYPE_ACDC_TIMER		58
/* 59-63 Reserved */
#define SMITYPE_KB_RESET		64
#define SMITYPE_SLP_TYP			65
#define SMITYPE_AL2H_ACPI		66
#define SMITYPE_AHCI			67
#define SMITYPE_NB_GPP_PME_PULSE	68
#define SMITYPE_NB_GPP_HP_PULSE		69
#define SMITYPE_USBPD_I2C_INT		70
/* 71 Reserved */
#define SMITYPE_GBL_RLS			72
#define SMITYPE_BIOS_RLS		73
#define SMITYPE_PWRBUTTON_DOWN		74
#define SMITYPE_SMI_CMD_PORT		75
#define SMITYPE_USB_SMI			76
#define SMITYPE_SERIRQ			77
#define SMITYPE_SMBUS0_INTR		78
/* 79-80 Reserved */
#define SMITYPE_INTRUDER		81
#define SMITYPE_VBAT_LOW		82
#define SMITYPE_PROTHOT			83
#define SMITYPE_PCI_SERR		84
#define SMITYPE_GPP_SERR		85
/* 85-89 Reserved */
#define SMITYPE_EMUL60_64		90
/* 91-132 Reserved */
#define SMITYPE_FANIN0			133
/* 134-140 Reserved */
#define SMITYPE_CF9_WRITE		141
#define SMITYPE_SHORT_TIMER		142
#define SMITYPE_LONG_TIMER		143
#define SMITYPE_AB_SMI			144
/* 145 Reserved */
#define SMITYPE_ESPI_SMI		146
/* 147 Reserved */
#define SMITYPE_IOTRAP0			148
#define SMITYPE_IOTRAP1			149
#define SMITYPE_IOTRAP2			150
#define SMITYPE_IOTRAP3			151
#define SMITYPE_MEMTRAP0		152
/* 153-155 Reserved */
#define SMITYPE_CFGTRAP0		156
/* 157-159 Reserved */
#define NUMBER_SMITYPES			160
#define TYPE_TO_MASK(X)				(1 << (X) % 32)

#define SMI_REG_SMISTS0			0x80
#define SMI_REG_SMISTS1			0x84
# define SMISTS1_FAKESTS0			(1 << 1)
#define SMI_REG_SMISTS2			0x88
#define SMI_REG_SMISTS3			0x8c
#define SMI_REG_SMISTS4			0x90

#define SMI_REG_POINTER			0x94
# define SMI_STATUS_SRC_SCI			(1 << 0)
# define SMI_STATUS_SRC_0			(1 << 1) /* SMIx80 */
# define SMI_STATUS_SRC_1			(1 << 2) /* SMIx84... */
# define SMI_STATUS_SRC_2			(1 << 3)
# define SMI_STATUS_SRC_3			(1 << 4)
# define SMI_STATUS_SRC_4			(1 << 5)

#define SMI_TIMER			0x96
#define SMI_TIMER_MASK				0x7fff
#define SMI_TIMER_EN				(1 << 15)

#define SMI_REG_SMITRIG0		0x98
# define SMITRG0_FAKESTS0			(1 << 25)
# define SMITRG0_EOS				(1 << 28)
# define SMI_TIMER_SEL				(1 << 29)
# define SMITRG0_SMIENB				(1 << 31)

#define SMI_REG_CONTROL0		0xa0
#define SMI_REG_CONTROL1		0xa4
#define SMI_REG_CONTROL2		0xa8
#define SMI_REG_CONTROL3		0xac
#define SMI_REG_CONTROL4		0xb0
#define SMI_REG_CONTROL5		0xb4
#define SMI_REG_CONTROL6		0xb8
#define SMI_REG_CONTROL7		0xbc
#define SMI_REG_CONTROL8		0xc0
#define SMI_REG_CONTROL9		0xc4

#define SMI_MODE_MASK			0x03

enum smi_mode {
	SMI_MODE_DISABLE = 0,
	SMI_MODE_SMI = 1,
	SMI_MODE_NMI = 2,
	SMI_MODE_IRQ13 = 3,
};

enum smi_sci_type {
	INTERRUPT_NONE,
	INTERRUPT_SCI,
	INTERRUPT_SMI,
	INTERRUPT_BOTH,
};

enum smi_sci_lvl {
	SMI_SCI_LVL_LOW,
	SMI_SCI_LVL_HIGH,
};

enum smi_sci_dir {
	SMI_SCI_EDG,
	SMI_SCI_LVL,
};

struct smi_sources_t {
	int type;
	void (*handler)(void);
};

struct sci_source {
	uint8_t scimap;		/* SCIMAP 0-57 */
	uint8_t gpe;		/* 32 GPEs */
	uint8_t direction;	/* Active High or Low,  smi_sci_lvl */
	uint8_t level;		/* Edge or Level,  smi_sci_dir */
};

uint16_t pm_acpi_smi_cmd_port(void);
void configure_smi(uint8_t smi_num, uint8_t mode);
void configure_gevent_smi(uint8_t gevent, uint8_t mode, uint8_t level);
void configure_scimap(const struct sci_source *sci);
void disable_gevent_smi(uint8_t gevent);
void gpe_configure_sci(const struct sci_source *scis, size_t num_gpes);
void soc_route_sci(uint8_t event);
void reset_psp_smi(void);
void configure_psp_smi(void);

#endif /* __SOUTHBRIDGE_AMD_PI_PICASSO_SMI_H__ */
