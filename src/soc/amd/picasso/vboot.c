#include <cbmem.h>
#include <commonlib/region.h>
#include <security/vboot/firmware_stash.h>
#include <security/vboot/misc.h>
#include <soc/memmap.h>
#include <vb2_api.h>

/* memory region device to replace mmapped-IO SPI rdev */
static struct mem_region_device mdev;

uint8_t *platform_get_firmware_stash(void)
{
	if (ENV_RAMSTAGE)
		return cbmem_find(CBMEM_ID_VBOOT_FW_STASH);
	else
		return (uint8_t *)_vboot_fw_stash;
}

size_t platform_firmware_stash_size(void)
{
	return REGION_SIZE(vboot_fw_stash);
}

int cbfs_get_boot_device_stash(struct region_device **rdev)
{
	uint8_t *firmware;
	size_t firmware_size;
	struct vb2_context *ctx;

	if (!vboot_logic_executed())
		return -1;

	ctx = vboot_get_context();

	/* get verified firmware from memory */
	firmware = platform_get_firmware_stash();
	firmware_size = vb2api_get_firmware_size(ctx);

	/* memory device to reserved dram/cbmem */
	mem_region_device_ro_init(&mdev, firmware, firmware_size);

	/* set region device to dram device */
	*rdev = &mdev.rdev;

	return 0;
}
