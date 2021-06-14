/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/biosram.h>
#include <device/mmio.h>
#include <stdint.h>

/* BiosRam Ranges at 0xfed80500 or I/O 0xcd4/0xcd5 */
#define BIOSRAM_AP_ENTRY		0xe8 /* 8 bytes */
#define BIOSRAM_CBMEM_TOP		0xf0 /* 4 bytes */
#define BIOSRAM_UMA_SIZE		0xf4 /* 4 bytes */
#define BIOSRAM_UMA_BASE		0xf8 /* 8 bytes */
#define BIOSRAM_KP_MARKER		0x00 /* 4 bytes */
#define BIOSRAM_KP_MSG			0x04 /* NULL-terminated string */
#define BIOSRAM_KP_MSG_SIZE		156  /* Max bytes of KP_MSG */

static uint8_t biosram_read8(uint8_t reg)
{
	return read8(acpimmio_biosram + reg);
}

static void biosram_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_biosram + reg, value);
}

static uint16_t biosram_read16(uint8_t reg) /* Must be 1 byte at a time */
{
	return (biosram_read8(reg + sizeof(uint8_t)) << 8 | biosram_read8(reg));
}

static void biosram_write16(uint8_t reg, uint16_t value)
{
	biosram_write8(reg, value & 0xff);
	value >>= 8;
	biosram_write8(reg + sizeof(uint8_t), value & 0xff);
}

static uint32_t biosram_read32(uint8_t reg)
{
	uint32_t value = biosram_read16(reg + sizeof(uint16_t)) << 16;
	return value | biosram_read16(reg);
}

static void biosram_write32(uint8_t reg, uint32_t value)
{
	biosram_write16(reg, value & 0xffff);
	value >>= 16;
	biosram_write16(reg + sizeof(uint16_t), value & 0xffff);
}

/* Access to BIOSRAM is only allowed through the abstractions below. */

void *get_ap_entry_ptr(void)
{
	return (void *)biosram_read32(BIOSRAM_AP_ENTRY);
}

void set_ap_entry_ptr(void *entry)
{
	biosram_write32(BIOSRAM_AP_ENTRY, (uintptr_t)entry);
}

void backup_top_of_low_cacheable(uintptr_t ramtop)
{
	biosram_write32(BIOSRAM_CBMEM_TOP, ramtop);
}

uintptr_t restore_top_of_low_cacheable(void)
{
	return biosram_read32(BIOSRAM_CBMEM_TOP);
}

void save_uma_size(uint32_t size)
{
	biosram_write32(BIOSRAM_UMA_SIZE, size);
}

void save_uma_base(uint64_t base)
{
	biosram_write32(BIOSRAM_UMA_BASE, (uint32_t) base);
	biosram_write32(BIOSRAM_UMA_BASE + 4, (uint32_t) (base >> 32));
}

uint32_t get_uma_size(void)
{
	return biosram_read32(BIOSRAM_UMA_SIZE);
}

uint64_t get_uma_base(void)
{
	uint64_t base;
	base = biosram_read32(BIOSRAM_UMA_BASE);
	base |= ((uint64_t)(biosram_read32(BIOSRAM_UMA_BASE + 4)) << 32);
	return base;
}

#if CONFIG(SOC_AMD_BIOSRAM_PANIC_MSG)
#include <console/console.h>
#include <ctype.h>
/* Magic marker at beginning of BIOSRAM region */
#define	BIOSRAM_KP_MSG_MAGIC	0x504b0ff0	/* 0xf0 0x0f 'K' 'P' */

int panic_detected(void)
{
	return biosram_read32(BIOSRAM_KP_MARKER) == BIOSRAM_KP_MSG_MAGIC;
}

void panic_msg_copy(char *dest, int max_len)
{
	int i = BIOSRAM_KP_MSG;
	int n = 0;
	char c;

	max_len = MIN(max_len, BIOSRAM_KP_MSG_SIZE);

	while (n < max_len) {
		c = biosram_read8(i++);
		dest[n++] = c;
		if (c == '\0')
			break;
	}
	dest[max_len-1] = '\0';
}

void panic_msg_print(int loglevel)
{
	int i = BIOSRAM_KP_MSG;
	int n = 0;
	char c;

	while (n < BIOSRAM_KP_MSG_SIZE) {
		c = biosram_read8(i++);
		if (c == '\0')
			break;

		printk(loglevel, "%c", isprint(c) ? c : '.');
		n++;
	}
}

void panic_msg_clear(void)
{
	biosram_write8(BIOSRAM_KP_MARKER, 0xaa);
}
#endif
