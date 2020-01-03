##
## This file is part of the coreboot project.
##
## Copyright (C) 2017 Samsung Electronics
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

INCLUDES += -Isrc/soc/turbo/trav/include/

# BOOTBLOCK : UART related and minimum required hardware init
bootblock-y += bootblock.c
bootblock-y += netspeed.S
bootblock-y += bootblock_asm.S
bootblock-y += cbmem.c
bootblock-y += timer.c
bootblock-y += gpio.c
bootblock-y += pinmux.c
bootblock-y += clock.c
bootblock-y += clock_init.c
bootblock-y += uart.c
bootblock-y += power.c
bootblock-y += scs_helpers_v2.c
bootblock-y += boot_control.c
bootblock-y += alternate_cbfs.c
bootblock-y += sms_helpers.c
bootblock-y += wdt.c
bootblock-$(CONFIG_TRAV_UFS) += ufs.c
bootblock-$(CONFIG_TRAV_UFS) += ufs_api.c
bootblock-$(CONFIG_TRAV_UFS) += ufs_mphy.c
bootblock-$(CONFIG_TRAV_UFS) += ufs_cmd.c
bootblock-$(CONFIG_TRAV_UFS) += cache_helpers.S

ifndef CONFIG_BOOTBLOCK_X
bootblock-y += dmc_lpddr4phy_init.c
bootblock-y += dmc_pie.c
bootblock-y += trustzone.c
bootblock-y += mmu.c
bootblock-y += netspeed_c.c
bootblock-y += gpt.c
endif

# RAMSTAGE : Prepare and load payload
ramstage-y += cpu.c
ramstage-y += boot_control.c
ramstage-y += alternate_cbfs.c
ramstage-y += cbmem.c
ramstage-y += timer.c
ramstage-y += gpio.c
ramstage-y += pinmux.c
ramstage-y += clock.c
ramstage-y += uart.c
ramstage-y += power.c
ramstage-y += arm_tf.c
ramstage-y += scs_helpers_v2.c
ramstage-y += sms_helpers.c

CPPFLAGS_common += -Isrc/soc/turbo/trav/include/

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	@printf "    BOOTBLOCK $(subst $(obj)/,,$(@))\n"
	cp $< $@

atf_bl31_base := $(CONFIG_ATF_BL31_BASE)
atf_bl31_limit := $(CONFIG_ATF_BL31_BASE)+$(CONFIG_ATF_BL31_SIZE)
psci_shared_mem_base := $(CONFIG_PSCI_SHARED_MEM_BASE)

BL31_MAKEARGS += PLAT=turbo TARGET_SOC=trav RTL_VER=ML4_DEV07 CORE_FLAG=0xfff ATF_BL31_BASE=${atf_bl31_base} ATF_BL31_LIMIT=${atf_bl31_limit} PSCI_SHARED_MEM_BASE=${psci_shared_mem_base}
