// Copyright (c) 2010 The Chromium OS Authors. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <arch/io.h>
#include <types.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <console/console.h>
#include <soc/gpt.h>

/* From vboot START */

/* From the specification */
#define GPT_HEADER_SIGNATURE_SIZE 8
#define GPT_HEADER_REVISION 0x00010000
#define GPT_HEADER_SIGNATURE  "EFI PART"

#define UUID_NODE_LEN 6
#define GUID_SIZE 16

/* GUID definition. Defined in appendix A of UEFI standard. */
typedef struct {
	union {
		struct {
			uint32_t time_low;
			uint16_t time_mid;
			uint16_t time_high_and_version;
			uint8_t clock_seq_high_and_reserved;
			uint8_t clock_seq_low;
			uint8_t node[UUID_NODE_LEN];
		} Uuid;
		uint8_t raw[GUID_SIZE];
	} u;
} __attribute__((packed)) Guid;

#define GUID_EXPECTED_SIZE GUID_SIZE

/*
 * GPT header defines how many partitions exist on a drive and sectors managed.
 * For every drive device, there are 2 headers, primary and secondary.  Most of
 * the fields are duplicates except my_lba and entries_lba.
 *
 * You may find more details in chapter 5 of the UEFI standard.
 */
typedef struct {
	char signature[GPT_HEADER_SIGNATURE_SIZE];
	uint32_t revision;
	uint32_t size;
	uint32_t header_crc32;
	uint32_t reserved_zero;
	uint64_t my_lba;
	uint64_t alternate_lba;
	uint64_t first_usable_lba;
	uint64_t last_usable_lba;
	Guid disk_uuid;
	uint64_t entries_lba;
	uint32_t number_of_entries;
	uint32_t size_of_entry;
	uint32_t entries_crc32;
	/* Remainder of sector is reserved and should be 0 */
} __attribute__((packed)) GptHeader;

#define GPTHEADER_EXPECTED_SIZE 92

/*
 * GPT partition entry defines the starting and ending LBAs of a partition.  It
 * also contains the unique GUID, type, and attribute bits.
 *
 * You may find more details in chapter 5 of the UEFI standard.
 */
typedef struct {
	Guid type;
	Guid unique;
	uint64_t starting_lba;
	uint64_t ending_lba;
	union {
		struct {
			uint8_t system:1;
			uint8_t efi_ignore:1;
			uint8_t legacy_boot:1;
			uint8_t reserved1:5;
			uint8_t reserved2;
			uint16_t reserved[2];
			uint16_t gpt_att;
		} __attribute__((packed)) fields;
		uint64_t whole;
	} attrs;
	uint16_t name[36];  /* UTF-16 encoded partition name */
	/* Remainder of entry is reserved and should be 0 */
} __attribute__((packed)) GptEntry;

#define GPTENTRY_EXPECTED_SIZE 128

/* GUID conversion functions. Accepted format:
 *
 *   "C12A7328-F81F-11D2-BA4B-00A0C93EC93B"
 *
 * At least GUID_STRLEN bytes should be reserved in 'str' (included the tailing
 * '\0').
 */
#define GUID_STRLEN 37

static void GuidToStr(const Guid *guid, char *str, unsigned int buflen)
{
	if (buflen < GUID_STRLEN)
		printk(BIOS_INFO, "GuidToStr: buf is too small\n");

	if (snprintf(str, buflen,
                  "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                  le32toh(guid->u.Uuid.time_low),
                  le16toh(guid->u.Uuid.time_mid),
                  le16toh(guid->u.Uuid.time_high_and_version),
                  guid->u.Uuid.clock_seq_high_and_reserved,
                  guid->u.Uuid.clock_seq_low,
                  guid->u.Uuid.node[0], guid->u.Uuid.node[1],
                  guid->u.Uuid.node[2], guid->u.Uuid.node[3],
                  guid->u.Uuid.node[4], guid->u.Uuid.node[5]) != GUID_STRLEN-1)
		printk(BIOS_INFO, "GuidToStr: convert get wrong size\n");
}

enum {
        CGPT_OK = 0,
        CGPT_FAILED,
};

/* Convert possibly unterminated UTF16 string to UTF8.
 * Caller must prepare enough space for UTF8, which could be up to
 * twice the byte length of UTF16 string plus the terminating '\0'.
 * See the following table for encoding lengths.
 *
 *     Code point       UTF16       UTF8
 *   0x0000-0x007F     2 bytes     1 byte
 *   0x0080-0x07FF     2 bytes     2 bytes
 *   0x0800-0xFFFF     2 bytes     3 bytes
 *  0x10000-0x10FFFF   4 bytes     4 bytes
 *
 * This function uses a simple state meachine to convert UTF-16 char(s) to
 * a code point. Once a code point is parsed out, the state machine throws
 * out sequencial UTF-8 chars in one time.
 *
 * Return: CGPT_OK --- all character are converted successfully.
 *         CGPT_FAILED --- convert error, i.e. output buffer is too short.
 */
static int UTF16ToUTF8(const uint16_t *utf16, unsigned int maxinput,
                uint8_t *utf8, unsigned int maxoutput)
{
  size_t s16idx, s8idx;
  uint32_t code_point = 0;
  int code_point_ready = 1;  // code point is ready to output.
  int retval = CGPT_OK;

  if (!utf16 || !maxinput || !utf8 || !maxoutput)
    return CGPT_FAILED;

  maxoutput--;                             /* plan for termination now */

  for (s16idx = s8idx = 0;
       s16idx < maxinput && utf16[s16idx] && maxoutput;
       s16idx++) {
    uint16_t codeunit = le16toh(utf16[s16idx]);

    if (code_point_ready) {
      if (codeunit >= 0xD800 && codeunit <= 0xDBFF) {
        /* high surrogate, need the low surrogate. */
        code_point_ready = 0;
        code_point = (codeunit & 0x03FF) + 0x0040;
      } else {
        /* BMP char, output it. */
        code_point = codeunit;
      }
    } else {
      /* expect the low surrogate */
      if (codeunit >= 0xDC00 && codeunit <= 0xDFFF) {
        code_point = (code_point << 10) | (codeunit & 0x03FF);
        code_point_ready = 1;
      } else {
        /* the second code unit is NOT the low surrogate. Unexpected. */
        code_point_ready = 0;
        retval = CGPT_FAILED;
        break;
      }
    }

    /* If UTF code point is ready, output it. */
    if (code_point_ready) {
      if (code_point > 0x10FFFF) {
        retval = CGPT_FAILED;
        break;
      }
      if (code_point <= 0x7F && maxoutput >= 1) {
        maxoutput -= 1;
        utf8[s8idx++] = code_point & 0x7F;
      } else if (code_point <= 0x7FF && maxoutput >= 2) {
        maxoutput -= 2;
        utf8[s8idx++] = 0xC0 | (code_point >> 6);
        utf8[s8idx++] = 0x80 | (code_point & 0x3F);
      } else if (code_point <= 0xFFFF && maxoutput >= 3) {
        maxoutput -= 3;
        utf8[s8idx++] = 0xE0 | (code_point >> 12);
        utf8[s8idx++] = 0x80 | ((code_point >> 6) & 0x3F);
        utf8[s8idx++] = 0x80 | (code_point & 0x3F);
      } else if (code_point <= 0x10FFFF && maxoutput >= 4) {
        maxoutput -= 4;
        utf8[s8idx++] = 0xF0 | (code_point >> 18);
        utf8[s8idx++] = 0x80 | ((code_point >> 12) & 0x3F);
        utf8[s8idx++] = 0x80 | ((code_point >> 6) & 0x3F);
        utf8[s8idx++] = 0x80 | (code_point & 0x3F);
      } else {
        /* buffer underrun */
        retval = CGPT_FAILED;
        break;
      }
    }
  }
  utf8[s8idx++] = 0;
  return retval;
}

/* From vboot END */

void get_gpt_part_uuid_name(unsigned char *buf, int partno, char *uuid_str, int uuid_len,
			    char *name_str, int name_len)
{
	GptHeader *header;
	uint32_t stride;
	uint8_t *entries;
	GptEntry *entry;

        int nn = strlen(GPT_HEADER_SIGNATURE);

        header = (GptHeader *)strnstr((char *)buf, 1<<20, GPT_HEADER_SIGNATURE, nn); /* search it in 1MiB */

	if (!header) {
		printk(BIOS_INFO, "GPT Header not found\n");
		return;
	}
	stride = header->size_of_entry;
	if (!stride)
		return;

	if (partno > header->number_of_entries)
		return;

	entries = buf + header->entries_lba * 0x1000;
	entry = (GptEntry *)(entries + GPTENTRY_EXPECTED_SIZE * (partno - 1));

	GuidToStr(&entry->unique, uuid_str, uuid_len);

        UTF16ToUTF8(entry->name, sizeof(entry->name) / sizeof(entry->name[0]),
                    (uint8_t *)name_str, name_len);
}
