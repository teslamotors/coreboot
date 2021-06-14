/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMDBLOCKS_SPI_H__
#define __AMDBLOCKS_SPI_H__

#include <types.h>

#define SPI_CNTRL0			0x00
#define   SPI_BUSY			BIT(31)

enum spi_read_mode {
	SPI_READ_MODE_NORMAL33M = 0,
	/* 1 is reserved. */
	SPI_READ_MODE_DUAL112 = 2,
	SPI_READ_MODE_QUAD114 = 3,
	SPI_READ_MODE_DUAL122 = 4,
	SPI_READ_MODE_QUAD144 = 5,
	SPI_READ_MODE_NORMAL66M = 6,
	SPI_READ_MODE_FAST_READ = 7,
};
/*
 * SPI read mode is split into bits 18, 29, 30 such that [30:29:18] correspond to bits [2:0] for
 * SpiReadMode.
 */
#define   SPI_READ_MODE_MASK		(BIT(30) | BIT(29) | BIT(18))
#define   SPI_READ_MODE_UPPER_BITS(x)	((((x) >> 1) & 0x3) << 29)
#define   SPI_READ_MODE_LOWER_BITS(x)	(((x) & 0x1) << 18)
#define   SPI_READ_MODE(x)		(SPI_READ_MODE_UPPER_BITS(x) | \
					 SPI_READ_MODE_LOWER_BITS(x))
#define   SPI_ACCESS_MAC_ROM_EN		BIT(22)

/*
 * Re-purpose unused SPI controller register for semaphore to synchronize SPI access
 * between SMM and non-SMM software.
 */
#define SPI_SEMAPHORE			0xfc
#define   SPI_SEM_SMI_DENY		BIT(5)
#define   SPI_SEM_LOCKED		BIT(4)

#define SPI_ALT_CS			0x1d
#define   SPI_ALT_CS_EN_MASK		(BIT(0) | BIT(1))

#define SPI100_ENABLE			0x20
#define   SPI_USE_SPI100		BIT(0)

/* Use SPI_SPEED_16M-SPI_SPEED_66M below for the southbridge */
#define SPI100_SPEED_CONFIG		0x22
enum spi100_speed {
	SPI_SPEED_66M = 0,
	SPI_SPEED_33M = 1,
	SPI_SPEED_22M = 2,
	SPI_SPEED_16M = 3,
	SPI_SPEED_100M = 4,
	SPI_SPEED_800K = 5,
};

#define   SPI_SPEED_MASK		0xf
#define   SPI_SPEED_MODE(x, shift)	(((x) & SPI_SPEED_MASK) << shift)
#define   SPI_NORM_SPEED(x)		SPI_SPEED_MODE(x, 12)
#define   SPI_FAST_SPEED(x)		SPI_SPEED_MODE(x, 8)
#define   SPI_ALT_SPEED(x)		SPI_SPEED_MODE(x, 4)
#define   SPI_TPM_SPEED(x)		SPI_SPEED_MODE(x, 0)

#define   SPI_SPEED_CFG(n, f, a, t)	(SPI_NORM_SPEED(n) | SPI_FAST_SPEED(f) | \
					 SPI_ALT_SPEED(a) | SPI_TPM_SPEED(t))

#define SPI100_HOST_PREF_CONFIG		0x2c
#define   SPI_RD4DW_EN_HOST		BIT(15)

#define SPI_FIFO			0x80
#define SPI_FIFO_LAST_BYTE		0xc7
#define SPI_FIFO_DEPTH			(SPI_FIFO_LAST_BYTE - SPI_FIFO)

struct spi_config {
	/*
	 * Default values if not overridden by mainboard:
	 * Read mode - Normal 33MHz
	 * Normal speed - 66MHz
	 * Fast speed - 66MHz
	 * Alt speed - 66MHz
	 * TPM speed - 66MHz
	 */
	enum spi_read_mode read_mode;
	enum spi100_speed normal_speed;
	enum spi100_speed fast_speed;
	enum spi100_speed altio_speed;
	enum spi100_speed tpm_speed;
};

/*
 * Perform early SPI initialization:
 * 1. Sets SPI ROM base and enables SPI ROM
 * 2. Enables SPI ROM prefetching
 * 3. Disables 4dw burst
 * 4. Configures SPI speed and read mode.
 *
 * This function expects SoC to include soc_amd_common_config in chip SoC config and uses
 * settings from mainboard devicetree to configure speed and read mode.
 */
void fch_spi_early_init(void);

/*
 * Configure SPI speed and read mode.
 *
 * This function expects SoC to include soc_amd_common_config in chip SoC config and uses
 * settings from mainboard devicetree to configure speed and read mode.
 */
void fch_spi_config_modes(void);

/* Set the SPI base address variable */
void spi_set_base(void *base);

/* Get the SPI base address variable's value */
uintptr_t spi_get_bar(void);
uint8_t spi_read8(uint8_t reg);
uint16_t spi_read16(uint8_t reg);
uint32_t spi_read32(uint8_t reg);
void spi_write8(uint8_t reg, uint8_t val);
void spi_write16(uint8_t reg, uint16_t val);
void spi_write32(uint8_t reg, uint32_t val);

#endif /* __AMDBLOCKS_SPI_H__ */
