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

#define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))

#define EMBEDDED_FW_SIGNATURE 0x55aa55aa
#define PSP_COOKIE 0x50535024		/* 'PSP$' */
#define BDT1_COOKIE 0x44484224		/* 'DHB$ */

typedef enum _amd_psp_type {
	AMD_PSP_PUBKEY = 0,
	AMD_PSP_BOOTLOADER = 1,
} amd_psp_type;

typedef enum _amd_bios_type {
	AMD_BIOS_PUBKEY = 0x05,
	AMD_BIOS_SIG = 0x07,
	AMD_BIOS_BIN = 0x62,
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

struct rom {
	int fd;
	uint32_t base;
	embedded_firmware fw;
	psp_directory_hdr psp_hdr;
	bios_directory_hdr bios_hdr;
};

static const char *optstring  = "r:ud:b:p:s:k:l:h";

static struct option long_options[] = {
	{"rom",         required_argument, 0, 'r' },
	{"update",      no_argument,       0, 'u' },
	{"bios-dir",    required_argument, 0, 'd' },
	{"bios-bin",    required_argument, 0, 'b' },
	{"bios-pubkey", required_argument, 0, 'p' },
	{"bios-sig",    required_argument, 0, 's' },
	{"root-pubkey", required_argument, 0, 'k' },
	{"bootloader",  required_argument, 0, 'l' },
	{"help",        no_argument,       0, 'h' },
	{NULL,          0,                 0,  0  }
};

static char tmp_path[PATH_MAX+1];

static void usage(void)
{
	printf("amdsigtool: Sign AMD BIOS image\n");
	printf("Usage: amdsigtool [options]\n");
	printf("-r | --rom <FILE>                 Coreboot ROM\n");
	printf("-u | --update                     Update BIOS image(s)\n");
	printf("-d | --bios-dir <FILE>            BIOS directory table\n");
	printf("-b | --bios-bin <FILE>            BIOS binary image\n");
	printf("-p | --bios-pubkey <FILE>         BIOS public key token\n");
	printf("-s | --bios-sig <FILE>            BIOS signature\n");
	printf("-k | --root-pubkey <FILE>         Root public key token\n");
	printf("-l | --bootloader <FILE>          PSP bootloader\n");
	printf("-h | --help                       Show this help\n");
}

static const char *bios_type_to_string(amd_bios_type type)
{
	switch (type) {
	case AMD_BIOS_PUBKEY:
		return "pubkey";
	case AMD_BIOS_SIG:
		return "sig";
	case AMD_BIOS_BIN:
		return "bin";
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

static int update_psp_entry(struct rom *rom, amd_psp_type type, const char *path)
{
	int ret = -1;
	psp_directory_entry *entries = NULL;
	psp_directory_entry *entry;

	entries = read_psp_dir_entries(rom);
	if (!entries)
		return -1;

	entry = find_psp_dir_entry(rom, entries, type);
	if (!entry) {
		fprintf(stderr, "ERROR: PSP entry not found\n");
		goto out;
	}

	printf("PSP %s: 0x%lx 0x%x\n", psp_type_to_string(type), entry->addr - rom->base, entry->size);

	ret = write_psp_dir_entry(rom, entry, path);
	if (ret) {
		fprintf(stderr, "ERROR: failed to write PSP entry\n");
		goto out;
	}

	ret = 0;
out:
	if (entries)
		free(entries);
	return ret;
}

static int update_bios_entry(struct rom *rom, amd_bios_type type, const char *path)
{
	int ret = -1;
	bios_directory_entry *entries = NULL;
	bios_directory_entry *entry;

	entries = read_bios_dir_entries(rom);
	if (!entries)
		return -1;

	entry = find_bios_dir_entry(rom, entries, type);
	if (!entry) {
		fprintf(stderr, "ERROR: BIOS entry not found\n");
		goto out;
	}

	if (entry->compressed) {
		path = compress_file(path);
		if (!path)
			goto out;
	}

	printf("BIOS %s: 0x%lx 0x%x\n", bios_type_to_string(type), entry->source - rom->base, entry->size);

	ret = write_bios_dir_entry(rom, entry, path);
	if (ret) {
		fprintf(stderr, "ERROR: failed to write BIOS entry\n");
		goto out;
	}

	ret = 0;
out:
	if (entries)
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

	ret = 0;
done:
	close_rom(&rom);
	return ret;
}
