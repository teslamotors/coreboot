/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Common/baseboard GPIO definitions shared for InfoZ platform
 */
#include <infoz/gpio.h>
#include <infoz/variants.h>
#include <boardid.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <delay.h>
#include <acpi/acpi_device.h>
#include <console/console.h>
#include <amdblocks/acpimmio.h>

/*
 * InfoZ early GPIO settings - set in bootblock
 *
 * Keep these to a minimum for things that are truly needed in early boot
 * (SPI, UARTs, Board ID pins)
 */
static const struct soc_amd_gpio infoz_early_gpio_table[] = {

	/* Pin AW10: SOC-DISP-2-PWR-EN / SOC-HDMI-MUX */
	/* Required early for board rev detection */
	PAD_GPI(GPIO_4, PULL_NONE),

	/* Pin AR6: SOC-SER-2-PWR-EN */
	/* Keep low to avoid SPI REQ/GNT contention */
	PAD_GPO(GPIO_7, LOW),

	/* Pin AV8: SOC-BLINK */
	PAD_NF(GPIO_11, BLINK, PULL_NONE),

	/* Pin AK9: GTW-SOC-nINT */
	PAD_GPI(GPIO_17, PULL_UP),

	/* Pin BB14: LPC-nPD / SOC-DP-MUX-SW-SEL - required early for board rev detection */
	PAD_GPI(GPIO_21, PULL_NONE),

	/* Pin BD5: SOC-ETH-WLAN-PCIE-nRST0-3V3 */
	PAD_GPO(GPIO_26, LOW),

	/* Pin BB6: SOC-PCIE-NVME-nRST1-3V3 */
	PAD_GPO(GPIO_27, LOW),

	/* Pin BA8: SOC-SPI-nCS2_R */
	PAD_NF(GPIO_30, SPI_CS2_L, PULL_NONE),

	/* Pin AW8: SOC-WIR-PWR-EN */
	PAD_GPO(GPIO_42, HIGH),

	/* Pin BC8: SPI_ROM_REQ */
	PAD_NF(GPIO_67, SPI_ROM_REQ, PULL_NONE),

	/* Pin AW13: BRD-ID-CONFIG-1 */
	PAD_GPI(GPIO_69, PULL_NONE),

	/* Pin BD13: BRD-ID-CONFIG-2 */
	PAD_GPI(GPIO_70, PULL_NONE),

	/* Pin BB13: LPCCLK1 / SER2-ICU-TOUCH-NRESET - Don't output clock to a reset line */
	PAD_GPO(GPIO_75, LOW),

	/* Pin BB8: SPI_ROM_GNT */
	PAD_NF(GPIO_76, SPI_ROM_GNT, PULL_NONE),

	/* Pin BC6: GPIO_108, SOC-GPU-PWR-EN */
	PAD_GPO(GPIO_108, HIGH),

	/* Pin AP19: BRD-ID-CONFIG-3 */
	PAD_GPI(GPIO_116, PULL_NONE),

	/* Pin AW18: Variant-specific, but keep as GPIO input in early boot */
	PAD_GPI(GPIO_120, PULL_UP),

	/* Pin AW19: WLAN-SOC-GPPPCIE-nCLKREQ6 */
	PAD_NF(GPIO_121, CLK_REQ6_L, PULL_NONE),

	/* Pin AV15: BRD-ID-CONFIG-0 */
	PAD_GPI(GPIO_130, PULL_NONE),

	/* Pin AU19: NVME-SOC-PCIE-nCLKREQ4 */
	PAD_NF(GPIO_132, CLK_REQ4_L, PULL_NONE),

	/* Pin BA18: SOC-BT-UART0-nCTS */
	PAD_NF(GPIO_135, UART0_CTS_L, PULL_DOWN),

	/* Pin BA16: SOCRX-BTTX-UART0 */
	PAD_NF(GPIO_136, UART0_RXD, PULL_UP),

	/* Pin BC17: SOC-BT-UART0-nRTS */
	PAD_NF(GPIO_137, UART0_RTS_L, PULL_DOWN),

	/* Pin BB18: SOCTX-BTRX-UART0 */
	PAD_NF(GPIO_138, UART0_TXD, PULL_NONE),

	/* Pin BB19: SOC-DEBUG-UART-TX_R */
	PAD_NF(GPIO_140, UART3_TXD, PULL_UP),

	/* Pin BC18: GTWTX-SOCRX-UART */
	PAD_NF(GPIO_141, UART1_RXD, PULL_UP),

	/* Pin BC16: SOC-DEBUG-UART-RX_R */
	PAD_NF(GPIO_142, UART3_RXD, PULL_UP),

	/* Pin BA17: SOCTX-GTWRX-UART */
	PAD_NF(GPIO_143, UART1_TXD, PULL_NONE),
};

const __weak
struct soc_amd_gpio *variant_early_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(infoz_early_gpio_table);
	return infoz_early_gpio_table;
}

/* Return 1 if gateway requesting debug console to be enabled */
int debug_console_requested(void)
{
	/* GTW->SoC interrupt line signals debug request during early boot */
	return gpio_get(GTW_SOC_INT) == 0;
}

/* Return 1 if we sampled a debug-enable request in early boot */
int debug_console_is_enabled(void)
{
	return gpio_read32(GTW_SOC_INT) & GPIO_OUTPUT_VALUE;
}

/* Store gateway request in GPIO pin Output Value bit (reset state: 0) */
void debug_console_set_enabled(void)
{
	/* Does not actually drive pin -- output enable off */
	gpio_set(GTW_SOC_INT, 1);
}

#if CONFIG(CONSOLE_OVERRIDE_LOGLEVEL)
int get_console_loglevel(void)
{
	if (debug_console_is_enabled())
		return BIOS_SPEW;

	return BIOS_NOTICE;
}
#endif

/* Portions below only required by romstage/ramstage - save bootblock space */
#if !ENV_BOOTBLOCK

/* InfoZ common RAM stage GPIO table */
static const struct soc_amd_gpio infoz_gpio_set_stage_ram[] = {
	/* Pin AR15: GTW-SOC-nWAKE */
	PAD_NF(GPIO_0, PWR_BTN_L, PULL_NONE),

	/* Pin AP10: GTW-SOC-SYS-nRESET */
	PAD_NF(GPIO_1, SYS_RESET_L, PULL_NONE),

	/* Pin AV11: SOC-PCIE-nWAKE */
	PAD_NF(GPIO_2, WAKE_L, PULL_NONE),

	/* GPIO_3: Variant-specific definition */
	PAD_GPI(GPIO_3, PULL_NONE),

	/* Pin AP9: SOC-DISP-3-PWR-EN - Rear */
	PAD_GPO(GPIO_5, LOW),

	/* Pin AU10: SOC-DISP-1-PWR-EN - Center */
	PAD_GPO(GPIO_6, HIGH),

	/* Pin AP6: GPIO_8, SOC-NVMe-EN, handled by variant_pcie_devs_power() */

	/* Pin AU7: SOC-UHUB-VBUS-EN */
	PAD_GPO(GPIO_9, HIGH),

	/* Pin AR8: GPIOEXPR3-SOC-nINT */
	PAD_GPI(GPIO_10, PULL_UP),

	/* Pin AV8: SOC-BLINK */
	PAD_NF(GPIO_11, BLINK, PULL_NONE),

	/* Pin AN6: SOC-HEARTBEAT-LED */
	PAD_GPI(GPIO_12, PULL_NONE),

	/* Pin AT12: SOC-nRST-P3V3-GPIO-EXP-1 */
	PAD_GPO(GPIO_13, HIGH),

	/* Pin AW7: CENTER-TOUCH-INTb */
	PAD_GPI(GPIO_14, PULL_UP),

	/* GPIO_16: Variant-specific definition */
	PAD_GPI(GPIO_16, PULL_NONE),

	/* Pin AK9: GTW-SOC-nINT */
	PAD_GPI(GPIO_17, PULL_UP),

	/* Pin AL9: TMP-SOC-nALERT0-3V3 */
	PAD_GPI(GPIO_18, PULL_UP),

	/* Pin AM9: I2C3-SCL-P3V3-S5-SER1 */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),

	/* Pin AM10: I2C3-SDA-P3V3-S5-SER1 */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),

	/* Pin BB14: LPC-nPD / SOC-DP-MUX-SW-SEL */
	PAD_NF(GPIO_21, LPC_PD_L, PULL_NONE),

	/* Pin BA13: LPC-nPME / SER1-REAR-TOUCH-nRESET-3V3 */
	PAD_NF(GPIO_22, LPC_PME_L, PULL_NONE),

	/* Pin AT10: SER2-ICU-INTb-3V3 */
	PAD_GPI(GPIO_23, PULL_UP),

	/* Pin AL8: SER2-ICU-REM-INTb-3V3 */
	PAD_GPI(GPIO_24, PULL_UP),

	/* Pin BD5: SOC-ETH-WLAN-PCIE-nRST0-3V3 */
	/* Pin BD5: GPIO_26, SOC-ETH-WLAN-PCIE-nRST0 */
	/* Handled by variant_pcie_devs_power() */

	/* Pin BB6: GPIO_27, SOC-PCIE-NVME-nRST1-3V3 */
	/* Handled by variant_pcie_devs_power() */

	/* Pin BD8: GPIOEXPR2-SOC-nINT_R */
	PAD_GPI(GPIO_29, PULL_NONE),

	/* Pin BA6: GPIOEXPR1-SOC-nINT_R */
	PAD_GPI(GPIO_31, PULL_UP),

	/* Pin BD11: LPC-nRST */
	PAD_NF(GPIO_32, LPC_RST_L, PULL_NONE),

	/* Pin AU6: SOC-DISP-4-PWR-EN - ICU */
	PAD_GPO(GPIO_40, LOW),

	/* Pin AW8: GPIO_42, SOC-WIR-PWR-EN */
	/* Handled by variant_pcie_devs_power() */

	/* Pin BA11: SOC-SER-1-PWR-EN - Rear */
	PAD_GPO(GPIO_68, LOW),

	/* Pin BA15: LPCCLK0 */
	PAD_NF(GPIO_74, LPCCLK0, PULL_NONE),

	/* Pin BB13: LPCCLK1 / SER2-ICU-TOUCH-NRESET */
	PAD_GPO(GPIO_75, HIGH),

	/* Pin AR18: GTW-SOC-FORCE-nRECOVERY */
	PAD_GPI(GPIO_84, PULL_UP),

	/* Pin AT18: SER1-REAR-INTb-3V3 */
	PAD_GPI(GPIO_85, PULL_UP),

	/* Pin AW15: SOC-ADSP-nWAKE - Output but in Hi-Z during boot */
	PAD_GPI(GPIO_86, PULL_NONE),

	/* Pin BC12: SERIRQ / SER2-ICU-TOUCH-NINT */
	PAD_GPI(GPIO_87, PULL_NONE),

	/* Pin BC13: LPC_nCLKRUN */
	PAD_NF(GPIO_88, LPC_CLKRUN_L, PULL_UP),

	/* Pin AW16: SER1-REAR-REM-INTb-3V3 */
	PAD_GPI(GPIO_89, PULL_UP),

	/* Pin BD15: SOC-GTW-BIOS-BOOT-nOK */
	PAD_GPO(GPIO_90, HIGH),

	/* Pin AU16: SOC-USB3-ENBUCK-3V3 */
	PAD_GPO(GPIO_91, HIGH),

	/* Pin AV18: SOC-GTW-OS-BOOT-nOK */
	PAD_GPO(GPIO_92, HIGH),

	/* Pin BB12: LAD0 / SOC-DISP-2-PWR-EN alt */
	PAD_NF(GPIO_104, LAD0, PULL_NONE),

	/* Pin BC11: LAD1 / PASS-DISP-TOUCH-INTb alt */
	PAD_NF(GPIO_105, LAD1, PULL_NONE),

	/* Pin BB15: LAD2 */
	PAD_NF(GPIO_106, LAD2, PULL_NONE),

	/* Pin BC15: LAD3 */
	PAD_NF(GPIO_107, LAD3, PULL_NONE),

	/* Pin BC6: nLRQ0, SOC-GPU-PWR-EN handled in pcie_devs_power() */

	/* Pin BA12: nLFRAME */
	PAD_NF(GPIO_109, LFRAME_L, PULL_NONE),

	/* Pin BC20: I2C2-SCL-P3V3-S0-DP2 */
	PAD_NF(GPIO_113, I2C2_SCL, PULL_NONE),

	/* Pin BA20: I2C2-SDA-P3V3-S0-DP2 */
	PAD_NF(GPIO_114, I2C2_SDA, PULL_NONE),

	/* Pin AN19: BOOT-BANK-SEL */
	PAD_GPI(GPIO_115, PULL_UP),

	/* Skip 118 - SOC-SPI-nCS1_R */

	/* GPIO_120: HG-only, handled by pcie_devs_power() */

	/* Pin BB11: GTW-SOC-WARM-nRST */
	PAD_NF(GPIO_129, KBRST_L, PULL_NONE),

	/* Pin AT19: ETHSW-SOC-PCIE-nCLKREQ3 */
	PAD_NF(GPIO_131, CLK_REQ3_L, PULL_NONE),

	/* Pin BD18: SER1-REAR-TOUCH-nINT */
	PAD_GPI(GPIO_139, PULL_UP),

	/* GPIO_144: Variant-specific definition */
	PAD_GPI(GPIO_144, PULL_NONE),
};

const __weak
struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	/* Default weak implementation - no overrides */
	*size = 0;
	return NULL;
}

const __weak
struct soc_amd_gpio *variant_base_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(infoz_gpio_set_stage_ram);
	return infoz_gpio_set_stage_ram;
}

/* PCIe peripheral power enables */
static struct acpi_gpio gpio_nvme_pwr_en = ACPI_GPIO_OUTPUT(SOC_NVME_EN);
static struct acpi_gpio gpio_wifi_pwr_en = ACPI_GPIO_OUTPUT(SOC_WIFI_EN);
static struct acpi_gpio gpio_gpu_pwr_en = ACPI_GPIO_OUTPUT(SOC_GPU_EN);

/* PCIe peripheral reset lines */
static struct acpi_gpio gpio_pcie_nrst0 =
	ACPI_GPIO_OUTPUT_ACTIVE_LOW(PCIE_0_RST);	/* WiFi */
static struct acpi_gpio gpio_pcie_nrst1 =
	ACPI_GPIO_OUTPUT_ACTIVE_LOW(PCIE_1_RST);	/* NVME + GPU */
static struct acpi_gpio gpio_pcie_nrst_gpu =
	ACPI_GPIO_OUTPUT_ACTIVE_LOW(GPIO_GPU_RST);	/* GPU alone */

/* GPIO Expander reset lines */
static int gpio_exp_reset_legacy[] = {GPIO_3, GPIO_13, GPIO_16, -1};
static int gpio_exp_reset[] = {GPIO_13, -1};	/* Shared */

/*
 * PCIe peripheral power/reset timing
 *
 * XXX: Should be reworked once proper support for this is supported at the
 * common ACPI device level in coreboot
 */
static struct acpi_power_res_params gpio_nvme_pwr_timing = {
	.reset_gpio = &gpio_pcie_nrst1,
	.reset_delay_ms = 1,
	.reset_off_delay_ms = 10,
	.enable_gpio = &gpio_nvme_pwr_en,
	.enable_delay_ms = 75,		/* 25ms ramp up + 50ms tPVPGL */
};

static struct acpi_power_res_params gpio_wifi_pwr_timing = {
	.reset_gpio = &gpio_pcie_nrst0,
	.reset_delay_ms = 1,
	.reset_off_delay_ms = 10,
	.enable_gpio = &gpio_wifi_pwr_en,
	.enable_delay_ms = 34,
};

static struct acpi_power_res_params gpio_gpu_pwr_timing = {
	.reset_gpio = &gpio_pcie_nrst_gpu,
	.reset_delay_ms = 100,		/* tRST-SEQ-A */
	.reset_off_delay_ms = 0,
	.enable_off_delay_ms = 20,	/* tRAMP-DOWN */
	.enable_gpio = &gpio_gpu_pwr_en,
	.enable_delay_ms = 120,		/* 20ms ramp up + 100ms PERST */
};

static struct acpi_power_res_params *pcie_pwr_timings[] = {
	/* Order of array must match INFOZ_PERIPH_* definitions */
	&gpio_nvme_pwr_timing,
	&gpio_wifi_pwr_timing,
	&gpio_gpu_pwr_timing,
	NULL,
};

/*
 * Power on each device in NULL-terminated array @devs following ACPI Power
 * Resource parameters.  Only devices with high bits in @mask are powered
 * on, with bit position corresponding to @devs array index.
 *
 * Devices are enabled in parallel as permitted to reduce boot time.
 *
 * This aims to be similar to an _ON method sequence.
 */
static void acpi_power_on_devs(struct acpi_power_res_params *devs[],
			       int mask)
{
	int i;
	struct acpi_power_res_params *p = (struct acpi_power_res_params *) (devs[0]);
	unsigned int total_pwr_on_ms = 0;
	unsigned int t = 0;

	printk(BIOS_SPEW, "InfoZ: Powering on PCI peripheral devices\n");

	/* Assert reset and power enable for all masked devs */
	for (i = 0; devs[i] != NULL; i++) {
		p = (struct acpi_power_res_params *) (devs[i]);
		if (!(mask & (1 << i)))
			continue;
		printk(BIOS_SPEW, "InfoZ dev %d: [t=0] Asserting reset GPIO_%d, power enable GPIO_%d\n",
		       i, p->reset_gpio->pins[0], p->enable_gpio->pins[0]);
		gpio_output(p->reset_gpio->pins[0], 0);
		gpio_output(p->enable_gpio->pins[0], 1);
		/* keep track of total power on time required */
		total_pwr_on_ms = MAX(total_pwr_on_ms,
				      p->reset_delay_ms + p->enable_delay_ms);
	}

	/*
	 * Release reset after enable_delay_ms elapses.  This also implicitly
	 * ensures we delay at least reset_delay_ms afterwards, since we
	 * are looping for the full duration of total_pwr_on_ms.
	 */
	while (t <= total_pwr_on_ms) {
		/* Handle overdue reset_gpio deassertions */
		i = 0;
		for (i = 0; devs[i] != NULL; i++) {
			p = (struct acpi_power_res_params *) (devs[i]);
			if (!(mask & (1 << i)))
				continue;
			if (t == p->enable_delay_ms) {
				printk(BIOS_SPEW, "InfoZ dev %d: [t= +%d ms] Releasing reset GPIO_%d\n",
				       i, t, p->reset_gpio->pins[0]);
				/* release reset */
				gpio_output(p->reset_gpio->pins[0], 1);
			}
		}
		mdelay(1);
		t++;
	}

	printk(BIOS_SPEW, "InfoZ dev: [t = +%d ms] Power on complete\n", t);

	/* Note: stop_gpio not handled */
}

/*
 * Power off each device in NULL-terminated array @devs following ACPI Power
 * Resource parameters.  Only devices with high bits in @mask are powered
 * off, with bit position corresponding to @devs array index.
 *
 * Devices are disabled in parallel as permitted to reduce time required.
 *
 * This aims to be similar to an _OFF method sequence.
 */
static void acpi_power_off_devs(struct acpi_power_res_params *devs[],
			       int mask)
{
	int i;
	struct acpi_power_res_params *p = (struct acpi_power_res_params *) (devs[0]);
	unsigned int total_pwr_off_ms = 0;
	unsigned int t = 0;

	/* Note: stop_gpio not handled */
	printk(BIOS_SPEW, "InfoZ: Powering off PCI peripheral devices\n");

	/* assert reset for all masked devs */
	for (i = 0; devs[i] != NULL; i++) {
		p = (struct acpi_power_res_params *) (devs[i]);
		if (!(mask & (1 << i)))
			continue;
		printk(BIOS_SPEW, "InfoZ dev %d: [t=0] Asserting reset GPIO_%d\n",
		       i, p->reset_gpio->pins[0]);
		gpio_output(p->reset_gpio->pins[0], 0);
		/* keep track of total power off time required */
		total_pwr_off_ms = MAX(total_pwr_off_ms,
			      p->reset_off_delay_ms + p->enable_off_delay_ms);
	}

	/*
	 * Deassert enable pin after reset_off_delay_ms elapses.  This also
	 * implicitly ensures we delay at least enable_off_delay_ms afterwards,
	 * since we are looping for the full duration of total_pwr_off_ms.
	 */
	while (t <= total_pwr_off_ms) {
		/* Handle overdue enable_gpio deassertions */
		for (i = 0; devs[i] != NULL; i++) {
			p = (struct acpi_power_res_params *) (devs[i]);
			if (!(mask & (1 << i)))
				continue;
			if (t == p->reset_off_delay_ms) {
				printk(BIOS_SPEW, "InfoZ dev %d: [t= +%d ms] Disabling power GPIO_%d\n",
				       i, t, p->enable_gpio->pins[0]);
				/* disable power */
				gpio_output(p->enable_gpio->pins[0], 0);
			}
		}
		mdelay(1);
		t++;
	}

	printk(BIOS_SPEW, "InfoZ dev: [t = +%d ms] Power off complete\n", t);
}

/* Power on or off board's PCIe peripherals */
void __weak variant_pcie_devs_power(int on)
{
	int mask = INFOZ_PERIPH_ALL;
	int flags;

	flags = variant_get_flags();
	if (!(flags & INFOZ_HAS_DGPU))
		mask &= ~INFOZ_PERIPH_GPU;

	if (on)
		acpi_power_on_devs(pcie_pwr_timings, mask);
	else
		acpi_power_off_devs(pcie_pwr_timings, mask);
}

enum infoz_boot_bank __weak variant_get_boot_bank(void)
{
	return gpio_get(GPIO_BOOT_BANK_SEL) ?
		INFOZ_BOOT_BANK_B : INFOZ_BOOT_BANK_A;
}

bool __weak variant_recovery_forced(void)
{
	return gpio_get(GPIO_BOOT_RECOVERY);
}

/* Reset GPIO expanders */
#define GPIO_EXP_RESET_US		20
#define GPIO_EXP_RESET_RECOVERY_US	2
void __weak variant_gpio_exp_reset(void)
{
	int i;
	int *reset_lines = gpio_exp_reset;

	printk(BIOS_SPEW, "Resetting GPIO expanders\n");
	if (variant_get_flags() & INFOZ_FIXUP_GPIO_EXP_RESET) {
		/* Older revs had individual reset lines */
		reset_lines = gpio_exp_reset_legacy;
	}

	/* Assert reset for all GPIO expanders - all IO lines set as input */
	i = 0;
	while (reset_lines[i] != -1) {
		printk(BIOS_SPEW, "    GPIO%d toggled\n", reset_lines[i]);
		gpio_output(reset_lines[i++], 0);
	}

	udelay(GPIO_EXP_RESET_US);

	/* Release reset */
	i = 0;
	while (reset_lines[i] != -1)
		gpio_output(reset_lines[i++], 1);

	udelay(GPIO_EXP_RESET_RECOVERY_US);
}

#endif	/* !ENV_BOOTBLOCK */
