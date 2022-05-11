/* SPDX-License-Identifier: GPL-2.0-only */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <zlib.h>
#include <dirent.h>

#define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))

#define EMBEDDED_FW_SIGNATURE 0x55aa55aa
#define PSP_COOKIE 0x50535024		/* 'PSP$' */
#define BDT1_COOKIE 0x44484224		/* 'DHB$ */

typedef enum _amd_psp_type {
	AMD_PSP_PUBKEY = 0,
	AMD_PSP_BOOTLOADER = 1,
	AMD_PSP_SECURED_OS = 2,
	AMD_PSP_RECOVERY = 3,
	AMD_PSP_NVRAM = 4,
	AMD_PSP_SMU_FIRMWARE = 8,
	AMD_PSP_SECURE_DEBUG_PUBKEY = 9,
	AMD_PSP_FUSE_CHAIN = 0xb,
	AMD_PSP_TRUSTLETS = 0xc,
	AMD_PSP_TRUSTLETKEY = 0xd,
	AMD_PSP_SMU_FIRMWARE2 = 0x12,
	AMD_PSP_SECURE_DEBUG = 0x13,
	AMD_PSP_WRAPPED_IKEK = 0x21,
	AMD_PSP_TOKEN_UNLOCK = 0x22,
	AMD_PSP_SEC_GASKET = 0x24,
	AMD_PSP_MP2_FW = 0x25,
	AMD_PSP_DRIVERS = 0x28,
	AMD_PSP_ABL0 = 0x30,
	AMD_PSP_ABL1 = 0x31,
	AMD_PSP_ABL2 = 0x32,
	AMD_PSP_ABL3 = 0x33,
	AMD_PSP_ABL4 = 0x34,
	AMD_PSP_ABL5 = 0x35,
	AMD_PSP_ABL6 = 0x36,
	AMD_PSP_ABL7 = 0x37,
	AMD_PSP_RPMC_NVRAM = 0x54,
	AMD_PSP_SPL_TABLE = 0x55,
	AMD_PSP_OEM_TRUSTLETS = 0x80,
	AMD_PSP_OEM_TRUSTLETKEY = 0x81,
} amd_psp_type;

typedef enum _amd_bios_type {
	AMD_BIOS_PUBKEY = 0x05,
	AMD_BIOS_SIG = 0x07,
	AMD_BIOS_APCB = 0x60,
	AMD_BIOS_APOB = 0x61,
	AMD_BIOS_BIN = 0x62,
	AMD_BIOS_APOB_NV = 0x63,
	AMD_BIOS_PMUI = 0x64,
	AMD_BIOS_PMUD = 0x65,
	AMD_BIOS_UCODE = 0x66,
	AMD_BIOS_APCB_BK = 0x68,
	AMD_BIOS_MP2_CFG = 0x6a,
} amd_bios_type;

typedef struct _embedded_firmware {
	uint32_t signature; /* 0x55aa55aa */
	uint32_t imc_entry;
	uint32_t gec_entry;
	uint32_t xhci_entry;
	uint32_t psp_entry;
	uint32_t comboable;
	uint32_t bios0_entry; /* todo: add way to select correct entry */
	uint32_t bios1_entry;
	uint32_t bios2_entry;
	uint32_t reserved[0x2c]; /* 0x24 - 0x4f */
} __attribute__((packed, aligned(16))) embedded_firmware;

typedef struct _psp_directory_hdr {
	uint32_t cookie;
	uint32_t checksum;
	uint32_t num_entries;
	uint32_t reserved;
} __attribute__((packed, aligned(16))) psp_directory_hdr;

typedef struct _psp_directory_entry {
	uint8_t type;
	uint8_t subprog;
	uint16_t rsvd;
	uint32_t size;
	uint64_t addr; /* or a value in some cases */
} __attribute__((packed)) psp_directory_entry;

typedef struct _bios_directory_hdr {
	uint32_t cookie;
	uint32_t checksum;
	uint32_t num_entries;
	uint32_t reserved;
} __attribute__((packed, aligned(16))) bios_directory_hdr;

typedef struct _bios_directory_entry {
	uint8_t type;
	uint8_t region_type;
	int reset:1;
	int copy:1;
	int ro:1;
	int compressed:1;
	int inst:4;
	uint8_t subprog; /* b[7:3] reserved */
	uint32_t size;
	uint64_t source;
	uint64_t dest;
} __attribute__((packed)) bios_directory_entry;

typedef struct _compression_hdr {
	uint32_t rsvd1[5];
	uint32_t size;
	uint32_t rsvd2[58];
}  __attribute__((packed)) compression_hdr;

typedef struct _psp_image_hdr {
	uint8_t nonce[16];
	uint32_t version;
	uint32_t fw_size;
	uint32_t enc_opt;
	uint32_t enc_algo;
	uint8_t enc_iv[16];
	uint32_t sig_opt;
	uint32_t sig_algo;
	uint8_t sig_key_id[16];
	uint32_t comp_opt;
	uint32_t spl;
	uint32_t uncomp_size;
	uint32_t comp_size;
	uint64_t comp_param;
	uint32_t fw_version;
	uint32_t unk_0;
	uint32_t unk_1;
	uint32_t image_size;
	uint32_t fw_unsigned_size;
	uint32_t unk_2;
	uint32_t unk_3;
	uint8_t type;
	uint8_t subprog;
	uint8_t unk_4;
	uint8_t unk_5;
	uint8_t enc_key[16];
	uint8_t sign_info[16];
	uint8_t spec_info[32];
	uint8_t dbg_enc_key[16];
	uint8_t pad[48];
}  __attribute__((packed)) psp_image_hdr;

struct rom {
	int fd;
	uint32_t base;
	embedded_firmware fw;
	psp_directory_hdr psp_hdr;
	bios_directory_hdr bios_hdr;
};

static const char *optstring  = "r:ud:b:p:s:k:l:f:h";

static struct option long_options[] = {
	{"rom",             required_argument, 0, 'r' },
	{"update",          no_argument,       0, 'u' },
	{"bios-dir",        required_argument, 0, 'd' },
	{"bios-bin",        required_argument, 0, 'b' },
	{"bios-pubkey",     required_argument, 0, 'p' },
	{"bios-sig",        required_argument, 0, 's' },
	{"root-pubkey",     required_argument, 0, 'k' },
	{"bootloader",      required_argument, 0, 'l' },
	{"trustlet-pubkey", required_argument, 0, 't' },
	{"firmware",        required_argument, 0, 'f' },
	{"help",            no_argument,       0, 'h' },
	{NULL,              0,                 0,  0  }
};

static char tmp_path[PATH_MAX+1];
static unsigned char empty_sig_id[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static unsigned char invalid_sig_id[16] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static void usage(void)
{
	printf("amdsigtool: Sign AMD BIOS image\n");
	printf("Usage: amdsigtool [options]\n");
	printf("-r | --rom <FILE>                 Coreboot ROM\n");
	printf("-u | --update                     Update image(s)\n");
	printf("-d | --bios-dir <FILE>            BIOS directory table\n");
	printf("-b | --bios-bin <FILE>            BIOS binary image\n");
	printf("-p | --bios-pubkey <FILE>         BIOS public key token\n");
	printf("-s | --bios-sig <FILE>            BIOS signature\n");
	printf("-k | --root-pubkey <FILE>         Root public key token\n");
	printf("-l | --bootloader <FILE>          PSP bootloader\n");
	printf("-f | --firmware <DIR>             PSP firmware images directory\n");
	printf("-h | --help                       Show this help\n");
}

static const char *bios_type_to_string(amd_bios_type type)
{
	switch (type) {
	case AMD_BIOS_PUBKEY:
		return "pubkey";
	case AMD_BIOS_SIG:
		return "sig";
	case AMD_BIOS_APCB:
		return "apcb";
	case AMD_BIOS_APOB:
		return "apob";
	case AMD_BIOS_BIN:
		return "bin";
	case AMD_BIOS_APOB_NV:
		return "apob-nvram";
	case AMD_BIOS_PMUI:
		return "pmu-instructions";
	case AMD_BIOS_PMUD:
		return "pmu-data";
	case AMD_BIOS_UCODE:
		return "microcode";
	case AMD_BIOS_APCB_BK:
		return "apcb-backup";
	case AMD_BIOS_MP2_CFG:
		return "mp2-config";
	default:
		return "unknown";
	}
}

static const char *psp_type_to_string(amd_psp_type type)
{
	switch (type) {
	case AMD_PSP_PUBKEY:
		return "pubkey";
	case AMD_PSP_BOOTLOADER:
		return "bootloader";
	case AMD_PSP_SECURED_OS:
		return "secure-os";
	case AMD_PSP_RECOVERY:
		return "recovery";
	case AMD_PSP_NVRAM:
		return "nvram";
	case AMD_PSP_SMU_FIRMWARE:
		return "smu";
	case AMD_PSP_SECURE_DEBUG_PUBKEY:
		return "debug-pubkey";
	case AMD_PSP_FUSE_CHAIN:
		return "fuse-chain";
	case AMD_PSP_TRUSTLETS:
		return "trustlets";
	case AMD_PSP_TRUSTLETKEY:
		return "trustlet-pubkey";
	case AMD_PSP_SMU_FIRMWARE2:
		return "smu2";
	case AMD_PSP_SECURE_DEBUG:
		return "secure-debug";
	case AMD_PSP_WRAPPED_IKEK:
		return "ikek";
	case AMD_PSP_TOKEN_UNLOCK:
		return "unlock-token";
	case AMD_PSP_SEC_GASKET:
		return "security-gasket";
	case AMD_PSP_MP2_FW:
		return "mp2";
	case AMD_PSP_DRIVERS:
		return "drivers";
	case AMD_PSP_ABL0:
		return "abl0";
	case AMD_PSP_ABL1:
		return "abl1";
	case AMD_PSP_ABL2:
		return "abl2";
	case AMD_PSP_ABL3:
		return "abl3";
	case AMD_PSP_ABL4:
		return "abl4";
	case AMD_PSP_ABL5:
		return "abl5";
	case AMD_PSP_ABL6:
		return "abl6";
	case AMD_PSP_ABL7:
		return "abl7";
	case AMD_PSP_RPMC_NVRAM:
		return "rpmc-nvram";
	case AMD_PSP_SPL_TABLE:
		return "spl-table";
	case AMD_PSP_OEM_TRUSTLETS:
		return "oem-trustlets";
	case AMD_PSP_OEM_TRUSTLETKEY:
		return "oem-trustlet-pubkey";
	default:
		return "unknown";
	}
}

static int get_embedded_fw(struct rom *rom, embedded_firmware *fw)
{
	static off_t dir_locs[] = {
		0x00FA0000,
		0x00F20000,
		0x00E20000,
		0x00C20000,
		0x00820000,
		0x00020000,
	};
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(dir_locs); i++) {
		if (lseek(rom->fd, dir_locs[i], SEEK_SET) == -1)
			continue;
		if (read(rom->fd, fw, sizeof(*fw)) != sizeof(*fw))
			continue;
		if (fw->signature != EMBEDDED_FW_SIGNATURE)
			continue;
		printf("Embedded firmware: 0x%lx\n", dir_locs[i]);
		return 0;
	}

	return -1;
}

static int get_psp_dir_hdr(struct rom *rom, psp_directory_hdr *hdr)
{
	off_t dir_loc = rom->fw.comboable - rom->base;

	if (lseek(rom->fd, dir_loc, SEEK_SET) == -1) {
		fprintf(stderr, "ERROR: failed to seek to PSP directory table at %ld: %s\n",
			dir_loc, strerror(errno));
		return -1;
	}

	if (read(rom->fd, hdr, sizeof(*hdr)) != sizeof(*hdr)) {
		fprintf(stderr, "ERROR: failed to read PSP directory table at %ld: %s\n",
			dir_loc, strerror(errno));
		return -1;
	}

	if (hdr->cookie != PSP_COOKIE) {
		fprintf(stderr, "ERROR: invalid PSP directory table at %ld\n",
			dir_loc);
		return -1;
	}

	printf("PSP directory table: 0x%lx %d\n", dir_loc, hdr->num_entries);

	return 0;
}

static int get_bios_dir_hdr(struct rom *rom, bios_directory_hdr *hdr)
{
	off_t dir_loc = rom->fw.bios1_entry - rom->base;

	if (lseek(rom->fd, dir_loc, SEEK_SET) == -1) {
		fprintf(stderr, "ERROR: failed to seek to BIOS directory table at %ld: %s\n",
			dir_loc, strerror(errno));
		return -1;
	}

	if (read(rom->fd, hdr, sizeof(*hdr)) != sizeof(*hdr)) {
		fprintf(stderr, "ERROR: failed to read BIOS directory table at %ld: %s\n",
			dir_loc, strerror(errno));
		return -1;
	}

	if (hdr->cookie != BDT1_COOKIE) {
		fprintf(stderr, "ERROR: invalid BIOS directory table at %ld\n",
			dir_loc);
		return -1;
	}

	printf("BIOS directory table: 0x%lx %d\n", dir_loc, hdr->num_entries);

	return 0;
}

static int open_rom(const char *path, struct rom *rom)
{
	int ret;
	struct stat st;

	rom->fd = open(path, O_RDWR);
	if (rom->fd == -1) {
		fprintf(stderr, "ERROR: failed to open %s: %s\n",
			path, strerror(errno));
		return -1;
	}

	if (fstat(rom->fd, &st)) {
		fprintf(stderr, "ERROR: failed to stat %s: %s\n",
			path, strerror(errno));
		goto err;
	}

	rom->base = 0xFFFFFFFF - st.st_size + 1;

	ret = get_embedded_fw(rom, &rom->fw);
	if (ret) {
		fprintf(stderr, "ERROR: failed to find embedded firmware in %s\n",
			path);
		goto err;
	}

	ret = get_psp_dir_hdr(rom, &rom->psp_hdr);
	if (ret) {
		fprintf(stderr, "ERROR: failed to find PSP directory header in %s\n",
			path);
		goto err;
	}

	ret = get_bios_dir_hdr(rom, &rom->bios_hdr);
	if (ret) {
		fprintf(stderr, "ERROR: failed to find BIOS directory header in %s\n",
			path);
		goto err;
	}

	return 0;

err:
	close(rom->fd);
	return -1;
}

static int close_rom(struct rom *rom)
{
	return close(rom->fd);
}

static psp_directory_entry *read_psp_dir_entries(struct rom *rom)
{
	void *dir = NULL;
	size_t dir_size;
	off_t loc;

	dir_size = rom->psp_hdr.num_entries * sizeof(psp_directory_entry);

	dir = malloc(dir_size);
	if (!dir) {
		fprintf(stderr, "ERROR: failed to allocate memory for PSP directory table entries\n");
		return NULL;
	}

	loc = (rom->fw.comboable - rom->base) + sizeof(rom->psp_hdr);

	if (lseek(rom->fd, loc, SEEK_SET) == -1) {
		fprintf(stderr, "ERROR: failed to seek to PSP directory table: %s\n",
			strerror(errno));
		goto err;
	}

	if (read(rom->fd, dir, dir_size) != (ssize_t)dir_size) {
		fprintf(stderr, "ERROR: failed to read PSP directory table entries: %s\n",
			strerror(errno));
		goto err;
	}

	return dir;
err:
	free(dir);
	return NULL;
}

static bios_directory_entry *read_bios_dir_entries(struct rom *rom)
{
	void *dir = NULL;
	size_t dir_size;
	off_t loc;

	dir_size = rom->bios_hdr.num_entries * sizeof(bios_directory_entry);

	dir = malloc(dir_size);
	if (!dir) {
		fprintf(stderr, "ERROR: failed to allocate memory for BIOS directory table entries\n");
		return NULL;
	}

	loc = (rom->fw.bios1_entry - rom->base) + sizeof(rom->bios_hdr);

	if (lseek(rom->fd, loc, SEEK_SET) == -1) {
		fprintf(stderr, "ERROR: failed to seek to BIOS directory table: %s\n",
			strerror(errno));
		goto err;
	}

	if (read(rom->fd, dir, dir_size) != (ssize_t)dir_size) {
		fprintf(stderr, "ERROR: failed to read BIOS directory table entries: %s\n",
			strerror(errno));
		goto err;
	}

	return dir;
err:
	free(dir);
	return NULL;
}

static int save_bios_dir_table(struct rom *rom, const char *path)
{
	int ret = -1;
	bios_directory_entry *entries;
	int fd = -1;

	entries = read_bios_dir_entries(rom);
	if (!entries)
		return -1;

	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1) {
		fprintf(stderr, "ERROR: failed to open %s: %s\n", path, strerror(errno));
		goto out;
	}

	if (write(fd, &rom->bios_hdr, sizeof(rom->bios_hdr)) != sizeof(rom->bios_hdr)) {
		fprintf(stderr, "ERROR: failed to write BIOS directory table header: %s\n",
			strerror(errno));
		goto out;
	}

	if (write(fd, entries, rom->bios_hdr.num_entries * sizeof(*entries))
	    != (ssize_t)(rom->bios_hdr.num_entries * sizeof(*entries))) {
		fprintf(stderr, "ERROR: failed to write BIOS directory table entries: %s\n",
			strerror(errno));
		goto out;
	}

	printf("BIOS directory table written to %s\n", path);

	ret = 0;
out:
	free(entries);
	if (fd != -1)
		close(fd);
	return ret;
}

static psp_directory_entry *find_psp_dir_entry(struct rom *rom,
		psp_directory_entry *entries, amd_psp_type type)
{
	unsigned int i;

	for (i = 0; i < rom->psp_hdr.num_entries; i++) {
		if (entries[i].type != type)
			continue;
		return entries + i;
	}

	return NULL;
}

static bios_directory_entry *find_bios_dir_entry(struct rom *rom,
		bios_directory_entry *entries, amd_bios_type type)
{
	unsigned int i;

	for (i = 0; i < rom->bios_hdr.num_entries; i++) {
		if (entries[i].type != type)
			continue;
		return entries + i;
	}

	return NULL;
}

static void *read_psp_dir_entry(struct rom *rom, psp_directory_entry *entry)
{
	void *data;
	off_t loc;

	data = malloc(entry->size);
	if (!data) {
		fprintf(stderr, "ERROR: failed to allocate memory for PSP entry\n");
		return NULL;
	}

	loc = entry->addr - rom->base;

	if (lseek(rom->fd, loc, SEEK_SET) == -1) {
		fprintf(stderr, "ERROR: failed to seek to PSP entry: %s\n",
			strerror(errno));
		goto err;
	}

	if (read(rom->fd, data, entry->size) != entry->size) {
		fprintf(stderr, "ERROR: failed to read PSP entry: %s\n",
			strerror(errno));
		goto err;
	}

	return data;
err:
	free(data);
	return NULL;
}

static void *read_bios_dir_entry(struct rom *rom, bios_directory_entry *entry)
{
	void *data;
	size_t data_size;
	off_t loc;
	compression_hdr hdr;
	void *buf = NULL;

	data = malloc(entry->size);
	if (!data) {
		fprintf(stderr, "ERROR: failed to allocate memory for BIOS entry\n");
		return NULL;
	}

	loc = entry->source - rom->base;

	if (lseek(rom->fd, loc, SEEK_SET) == -1) {
		fprintf(stderr, "ERROR: failed to seek to BIOS entry: %s\n",
			strerror(errno));
		goto err;
	}

	if (entry->compressed) {
		if (read(rom->fd, &hdr, sizeof(hdr)) != sizeof(hdr)) {
			fprintf(stderr, "ERROR: failed to read compression header: %s\n",
				strerror(errno));
			goto err;
		}

		buf = malloc(hdr.size);
		if (!buf) {
			fprintf(stderr, "ERROR: failed to allocate memory for compressed data: %s\n",
				strerror(errno));
			goto err;
		}

		if (read(rom->fd, buf, hdr.size) != hdr.size) {
			fprintf(stderr, "ERROR: failed to read BIOS entry: %s\n",
				strerror(errno));
			free(buf);
			goto err;
		}

		data_size = entry->size;

		if (uncompress(data, &data_size, buf, hdr.size) != Z_OK) {
			fprintf(stderr, "ERROR: failed to decompress BIOS entry\n");
			free(buf);
			goto err;
		}

		free(buf);

		if (data_size != entry->size) {
			fprintf(stderr, "ERROR: uncompressed size mismatch\n");
			goto err;
		}
	} else {
		if (read(rom->fd, data, entry->size) != entry->size) {
			fprintf(stderr, "ERROR: failed to read BIOS entry: %s\n",
				strerror(errno));
			goto err;
		}
	}

	return data;
err:
	free(data);
	return NULL;
}

static int save_psp_entry(struct rom *rom, amd_psp_type type, const char *path)
{
	int ret = -1;
	psp_directory_entry *entries = NULL;
	psp_directory_entry *entry;
	void *buf = NULL;
	int fd = -1;

	entries = read_psp_dir_entries(rom);
	if (!entries)
		return -1;

	entry = find_psp_dir_entry(rom, entries, type);
	if (!entry) {
		fprintf(stderr, "ERROR: PSP entry not found\n");
		goto out;
	}

	printf("PSP %s: 0x%lx 0x%x\n", psp_type_to_string(type), entry->addr - rom->base, entry->size);

	buf = read_psp_dir_entry(rom, entry);
	if (!buf) {
		fprintf(stderr, "ERROR: failed to read PSP entry\n");
		goto out;
	}

	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1) {
		fprintf(stderr, "ERROR: failed to open %s: %s\n", path, strerror(errno));
		goto out;
	}

	if (write(fd, buf, entry->size) != entry->size) {
		fprintf(stderr, "ERROR: failed to write BIOS entry: %s\n",
			strerror(errno));
		goto out;
	}

	ret = 0;
out:
	if (entries)
		free(entries);
	if (buf)
		free(buf);
	if (fd != -1)
		close(fd);
	return ret;
}

static int save_bios_entry(struct rom *rom, amd_bios_type type, const char *path)
{
	int ret = -1;
	bios_directory_entry *entries = NULL;
	bios_directory_entry *entry;
	void *buf = NULL;
	int fd = -1;

	entries = read_bios_dir_entries(rom);
	if (!entries)
		return -1;

	entry = find_bios_dir_entry(rom, entries, type);
	if (!entry) {
		fprintf(stderr, "ERROR: BIOS entry not found\n");
		goto out;
	}

	printf("BIOS %s: 0x%lx 0x%x\n", bios_type_to_string(type), entry->source - rom->base, entry->size);

	buf = read_bios_dir_entry(rom, entry);
	if (!buf) {
		fprintf(stderr, "ERROR: failed to read BIOS entry\n");
		goto out;
	}

	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1) {
		fprintf(stderr, "ERROR: failed to open %s: %s\n", path, strerror(errno));
		goto out;
	}

	if (write(fd, buf, entry->size) != entry->size) {
		fprintf(stderr, "ERROR: failed to write BIOS entry: %s\n",
			strerror(errno));
		goto out;
	}

	ret = 0;
out:
	if (entries)
		free(entries);
	if (buf)
		free(buf);
	if (fd != -1)
		close(fd);
	return ret;
}

static void *read_file(const char *path, size_t *size)
{
	int fd;
	struct stat st;
	void *buf = NULL;

	fd = open(path, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "ERROR: failed to open %s: %s\n",
			path, strerror(errno));
		return NULL;
	}

	if (fstat(fd, &st)) {
		fprintf(stderr, "ERROR: failed to stat %s: %s\n",
			path, strerror(errno));
		goto out;
	}

	buf = malloc(st.st_size);
	if (!buf) {
		fprintf(stderr, "ERROR: failed to allocate memory for file data\n");
		goto out;
	}

	if (read(fd, buf, st.st_size) != st.st_size) {
		fprintf(stderr, "ERROR: failed to read file data: %s\n",
			strerror(errno));
		free(buf);
		buf = NULL;
		goto out;
	}

	*size = st.st_size;

out:
	close(fd);
	return buf;
}

static int write_psp_dir_entry(struct rom *rom, psp_directory_entry *entry,
		const char *path)
{
	int ret = -1;
	void *buf = NULL;
	size_t size;
	off_t loc;

	buf = read_file(path, &size);
	if (!buf)
		goto out;

	loc = entry->addr - rom->base;

	if (lseek(rom->fd, loc, SEEK_SET) == -1) {
		fprintf(stderr, "ERROR: failed to seek to PSP entry: %s\n",
			strerror(errno));
		goto out;
	}

	if (size != entry->size) {
		fprintf(stderr, "ERROR: file size 0x%lx does not match PSP entry size 0x%x\n",
			size, entry->size);
		goto out;
	}

	if (write(rom->fd, buf, size) != (ssize_t)size) {
		fprintf(stderr, "ERROR: failed to write BIOS PSP: %s\n",
			strerror(errno));
		goto out;
	}

	ret = 0;
out:
	if (buf)
		free(buf);
	return ret;
}

static int write_bios_dir_entry(struct rom *rom, bios_directory_entry *entry,
		const char *path)
{
	int ret = -1;
	void *buf = NULL;
	size_t size;
	off_t loc;
	compression_hdr hdr;

	buf = read_file(path, &size);
	if (!buf)
		goto out;

	loc = entry->source - rom->base;

	if (lseek(rom->fd, loc, SEEK_SET) == -1) {
		fprintf(stderr, "ERROR: failed to seek to BIOS entry: %s\n",
			strerror(errno));
		goto out;
	}

	if (entry->compressed) {
		if (read(rom->fd, &hdr, sizeof(hdr)) != sizeof(hdr)) {
			fprintf(stderr, "ERROR: failed to read BIOS entry compression header: %s\n",
				strerror(errno));
			goto out;
		}

		if (size != hdr.size) {
			fprintf(stderr, "ERROR: file size 0x%lx does not match BIOS compression header size 0x%x\n",
				size, hdr.size);
			goto out;
		}
	} else {
		if (size != entry->size) {
			fprintf(stderr, "ERROR: file size 0x%lx does not match BIOS entry size 0x%x\n",
				size, entry->size);
			goto out;
		}
	}

	if (write(rom->fd, buf, size) != (ssize_t)size) {
		fprintf(stderr, "ERROR: failed to write BIOS entry: %s\n",
			strerror(errno));
		goto out;
	}

	ret = 0;
out:
	if (buf)
		free(buf);
	return ret;
}

static const char *compress_file(const char *path)
{
	void *buf = NULL;
	size_t size;
	void *comp = NULL;
	size_t comp_size;
	int fd = -1;

	buf = read_file(path, &size);
	if (!buf)
		goto out;

	comp_size = size;

	comp = malloc(comp_size);
	if (!comp) {
		fprintf(stderr, "ERROR: failed to allocate memory for compressed file\n");
		goto out;
	}

	if (compress(comp, &comp_size, buf, size) != Z_OK) {
		fprintf(stderr, "ERROR: failed to compress file data\n");
		goto out;
	}

	snprintf(tmp_path, sizeof(tmp_path), "/tmp/amdsigtool.XXXXXX");

	fd = mkstemp(tmp_path);
	if (fd == -1) {
		fprintf(stderr, "ERROR: failed to create temporary file: %s\n", strerror(errno));
		goto out;
	}

	if (write(fd, comp, comp_size) != (ssize_t)comp_size) {
		fprintf(stderr, "ERROR: failed to write compressed data: %s\n",
			strerror(errno));
		unlink(tmp_path);
		goto out;
	}

out:
	if (buf)
		free(buf);
	if (comp)
		free(comp);
	if (fd == -1)
		close(fd);
	return tmp_path;
}

static int set_psp_entry(struct rom *rom, psp_directory_entry *entries, amd_psp_type type, const char *path)
{
	int ret;
	psp_directory_entry *entry;

	entry = find_psp_dir_entry(rom, entries, type);
	if (!entry) {
		fprintf(stderr, "ERROR: PSP entry not found\n");
		return -1;
	}

	printf("PSP %s: 0x%lx 0x%x\n", psp_type_to_string(type), entry->addr - rom->base, entry->size);

	ret = write_psp_dir_entry(rom, entry, path);
	if (ret) {
		fprintf(stderr, "ERROR: failed to write PSP entry\n");
		return -1;
	}

	return 0;
}

static int update_psp_entry(struct rom *rom, amd_psp_type type, const char *path)
{
	int ret = -1;
	psp_directory_entry *entries = NULL;

	entries = read_psp_dir_entries(rom);
	if (!entries)
		return -1;

	ret = set_psp_entry(rom, entries, type, path);

	free(entries);
	return ret;
}

static int set_bios_entry(struct rom *rom, bios_directory_entry *entries, amd_bios_type type, const char *path)
{
	int ret = -1;
	bios_directory_entry *entry;

	entry = find_bios_dir_entry(rom, entries, type);
	if (!entry) {
		fprintf(stderr, "ERROR: BIOS entry not found\n");
		return -1;
	}

	if (entry->compressed) {
		path = compress_file(path);
		if (!path)
			return -1;
	}

	printf("BIOS %s: 0x%lx 0x%x\n", bios_type_to_string(type), entry->source - rom->base, entry->size);

	ret = write_bios_dir_entry(rom, entry, path);
	if (ret) {
		fprintf(stderr, "ERROR: failed to write BIOS entry\n");
		return -1;
	}

	return 0;
}

static int update_bios_entry(struct rom *rom, amd_bios_type type, const char *path)
{
	int ret = -1;
	bios_directory_entry *entries = NULL;

	entries = read_bios_dir_entries(rom);
	if (!entries)
		return -1;

	ret = set_bios_entry(rom, entries, type, path);

	free(entries);
	return ret;
}

static int do_psp_entry(struct rom *rom, amd_psp_type type, const char *path, int update)
{
	int ret;

	if (update) {
		ret = update_psp_entry(rom, type, path);
		if (ret) {
			fprintf(stderr, "ERROR: failed to write PSP %s\n", psp_type_to_string(type));
			return -1;
		}

		printf("PSP %s updated from %s\n", psp_type_to_string(type), path);
		return 0;
	}

	ret = save_psp_entry(rom, type, path);
	if (ret) {
		fprintf(stderr, "ERROR: failed to write PSP %d image\n", type);
		return -1;
	}

	printf("PSP %s written to %s\n", psp_type_to_string(type), path);

	return 0;
}

static int do_bios_entry(struct rom *rom, amd_bios_type type, const char *path, int update)
{
	int ret;

	if (update) {
		ret = update_bios_entry(rom, type, path);
		if (ret) {
			fprintf(stderr, "ERROR: failed to write BIOS %s\n", bios_type_to_string(type));
			return -1;
		}

		printf("BIOS %s updated from %s\n", bios_type_to_string(type), path);
		return 0;
	}

	ret = save_bios_entry(rom, type, path);
	if (ret) {
		fprintf(stderr, "ERROR: failed to write BIOS %d image\n", type);
		return -1;
	}

	printf("BIOS %s written to %s\n", bios_type_to_string(type), path);

	return 0;
}

static int psp_image_signed(psp_image_hdr *hdr)
{
	if (hdr->sig_opt != 1)
		return 0;
	if (hdr->sig_algo != 0)
		return 0;
	if (memcmp(hdr->sig_key_id, empty_sig_id, sizeof(empty_sig_id)) == 0)
		return 0;
	if (memcmp(hdr->sig_key_id, invalid_sig_id, sizeof(invalid_sig_id)) == 0)
		return 0;
	return 1;
}


static char *find_firmware_image(const char *path, psp_image_hdr *match)
{
	DIR *dir;
	struct dirent *entry;
	size_t len;
	uint8_t sig_key_id[16];
	char img_path[PATH_MAX];
	int fd = -1;
	psp_image_hdr img;
	ssize_t n;
	char *ret = NULL;

	dir = opendir(path);
	if (!dir) {
		fprintf(stderr, "ERROR: failed to open directory %s\n", path);
		return NULL;
	}

	memcpy(sig_key_id, match->sig_key_id, sizeof(sig_key_id));
	memset(match->sig_key_id, 0, sizeof(match->sig_key_id));

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type != DT_REG)
			continue;

		len = strlen(entry->d_name);
		if (len < 3 || strcmp(entry->d_name + (len - 3), "bin"))
			continue;

		snprintf(img_path, sizeof(img_path), "%s/%s", path, entry->d_name);

		fd = open(img_path, O_RDONLY);
		if (fd == -1) {
			fprintf(stderr, "ERROR: failed to open firmware image %s\n", entry->d_name);
			goto out;
		}

		n = read(fd, &img, sizeof(img));
		if (n == -1) {
			fprintf(stderr, "ERROR: failed to read firmware image %s\n", entry->d_name);
			goto out;
		}

		close(fd);
		fd = -1;

		if (n != sizeof(img))
			continue;

		memset(&img.sig_key_id, 0, sizeof(img.sig_key_id));
		if (memcmp(&img, match, sizeof(img)) == 0) {
			ret = strdup(img_path);
			break;
		}
	}

out:
	if (fd != -1)
		close(fd);
	closedir(dir);
	memcpy(match->sig_key_id, sig_key_id, sizeof(sig_key_id));
	return ret;
}

static int update_psp_images(struct rom *rom, const char *dir)
{
	int ret = -1;
	unsigned int i;
	psp_directory_entry *entries = NULL;
	psp_directory_entry *entry;
	psp_image_hdr *img;
	char *path;

	entries = read_psp_dir_entries(rom);
	if (!entries)
		goto out;

	for (i = 0; i < rom->psp_hdr.num_entries; i++) {
		entry = &entries[i];

		if (entry->size == 0xffffffff) {
			fprintf(stderr, "WARN: skipping empty PSP entry %s %d\n", psp_type_to_string(entry->type), entry->subprog);
			continue;
		}

		if (entry->size < sizeof(*img)) {
			fprintf(stderr, "WARN: skipping non-firmware PSP entry %s %d\n", psp_type_to_string(entry->type), entry->subprog);
			continue;
		}

		img = read_psp_dir_entry(rom, entry);
		if (!img) {
			fprintf(stderr, "ERROR: failed to read PSP entry %s %d\n", psp_type_to_string(entry->type), entry->subprog);
			goto out;
		}

		if (!psp_image_signed(img)) {
			fprintf(stderr, "WARN: skipping unsigned PSP entry %s %d\n", psp_type_to_string(entry->type), entry->subprog);
			continue;
		}

		path = find_firmware_image(dir, img);
		if (!path) {
			fprintf(stderr, "WARN: no PSP %s %d image found\n", psp_type_to_string(entry->type), entry->subprog);
			continue;
		}

		printf("Replacing PSP %s %d with %s\n", psp_type_to_string(entry->type), entry->subprog, path);

		ret = write_psp_dir_entry(rom, entry, path);
		free(path);
		if (ret)
			goto out;
	}

out:
	if (entries)
		free(entries);
	return ret;
}

static int update_bios_images(struct rom *rom, const char *dir)
{
	int ret = -1;
	unsigned int i;
	bios_directory_entry *entries = NULL;
	bios_directory_entry *entry;
	psp_image_hdr *img;
	char *path;

	entries = read_bios_dir_entries(rom);
	if (!entries)
		goto out;

	for (i = 0; i < rom->bios_hdr.num_entries; i++) {
		entry = &entries[i];

		if (entry->size == 0) {
			fprintf(stderr, "WARN: skipping empty BIOS entry %s %d%d\n", bios_type_to_string(entry->type), entry->subprog, entry->inst);
			continue;
		}

		if (entry->size < sizeof(*img)) {
			fprintf(stderr, "WARN: skipping non-firmware BIOS entry %s %d%d\n", bios_type_to_string(entry->type), entry->subprog, entry->inst);
			continue;
		}

		img = read_bios_dir_entry(rom, entry);
		if (!img) {
			fprintf(stderr, "ERROR: failed to read BIOS entry %s %d%d\n", bios_type_to_string(entry->type), entry->subprog, entry->inst);
			goto out;
		}

		if (!psp_image_signed(img)) {
			fprintf(stderr, "WARN: skipping unsigned BIOS entry %s %d%d\n", bios_type_to_string(entry->type), entry->subprog, entry->inst);
			free(img);
			continue;
		}

		path = find_firmware_image(dir, img);
		free(img);
		if (!path) {
			fprintf(stderr, "WARN: no BIOS %s %d%d image found\n", bios_type_to_string(entry->type), entry->subprog, entry->inst);
			continue;
		}

		printf("Replacing BIOS %s %d%d with %s\n", bios_type_to_string(entry->type), entry->subprog, entry->inst, path);

		ret = write_bios_dir_entry(rom, entry, path);
		free(path);
		if (ret)
			goto out;
	}

out:
	if (entries)
		free(entries);
	return ret;
}

static int do_firmware(struct rom *rom, const char *path, int update)
{
	int ret;

	if (!update) {
		fprintf(stderr, "ERROR: only update supported for firwmare\n");
		return -1;
	}

	ret = update_psp_images(rom, path);
	if (ret)
		return -1;

	ret = update_bios_images(rom, path);
	if (ret)
		return -1;

	return 0;
}

int main(int argc, char **argv)
{
	int c;
	int optindex = 0;
	int ret = -1;
	int update = 0;
	const char *rom_path = NULL;
	const char *bios_dir_path = NULL;
	const char *bios_bin_path = NULL;
	const char *bios_pubkey_path = NULL;
	const char *bios_sig_path = NULL;
	const char *root_pubkey_path = NULL;
	const char *bootloader_path = NULL;
	const char *trustlet_pubkey_path = NULL;
	const char *firmware_path = NULL;
	struct rom rom;

	while (1) {
		c = getopt_long(argc, argv, optstring, long_options, &optindex);

		if (c == -1)
			break;

		switch (c) {
		case 'r':
			rom_path = optarg;
			break;
		case 'u':
			update = 1;
			break;
		case 'd':
			bios_dir_path = optarg;
			break;
		case 'b':
			bios_bin_path = optarg;
			break;
		case 'p':
			bios_pubkey_path = optarg;
			break;
		case 's':
			bios_sig_path = optarg;
			break;
		case 'k':
			root_pubkey_path = optarg;
			break;
		case 'l':
			bootloader_path = optarg;
			break;
		case 't':
			trustlet_pubkey_path = optarg;
			break;
		case 'f':
			firmware_path = optarg;
			break;
		case 'h':
			usage();
			return 1;
		default:
			fprintf(stderr, "ERROR: unknown option: '%c'\n\n", c);
			usage();
			return 1;
		}
	}

	if (!rom_path) {
		fprintf(stderr, "ERROR: failed to specify ROM\n");
		usage();
		return 1;
	}

	ret = open_rom(rom_path, &rom);
	if (ret) {
		fprintf(stderr, "ERROR: failed to open ROM %s\n", rom_path);
		return 1;
	}

	if (bios_dir_path && !update) {
		ret = save_bios_dir_table(&rom, bios_dir_path);
		if (ret)
			goto done;
	}

	if (bios_bin_path) {
		ret = do_bios_entry(&rom, AMD_BIOS_BIN, bios_bin_path, update);
		if (ret)
			goto done;
	}

	if (bios_pubkey_path) {
		ret = do_bios_entry(&rom, AMD_BIOS_PUBKEY, bios_pubkey_path, update);
		if (ret)
			goto done;
	}

	if (bios_sig_path) {
		ret = do_bios_entry(&rom, AMD_BIOS_SIG, bios_sig_path, update);
		if (ret)
			goto done;
	}

	if (root_pubkey_path) {
		ret = do_psp_entry(&rom, AMD_PSP_PUBKEY, root_pubkey_path, update);
		if (ret)
			goto done;
	}

	if (bootloader_path) {
		ret = do_psp_entry(&rom, AMD_PSP_BOOTLOADER, bootloader_path, update);
		if (ret)
			goto done;
	}

	if (trustlet_pubkey_path) {
		ret = do_psp_entry(&rom, AMD_PSP_OEM_TRUSTLETKEY, trustlet_pubkey_path, update);
		if (ret)
			goto done;
	}

	if (firmware_path) {
		ret = do_firmware(&rom, firmware_path, update);
		if (ret)
			goto done;
	}

	ret = 0;
done:
	close_rom(&rom);
	return ret;
}
