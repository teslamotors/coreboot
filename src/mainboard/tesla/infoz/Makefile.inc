# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c
bootblock-y += gpio.c

romstage-y += sku_id.c
romstage-y += gpio.c
romstage-y += romstage.c

ramstage-y += sku_id.c
ramstage-y += gpio.c
ramstage-y += security.c
ramstage-y += board_utils.c
ramstage-y += dxio.c

smm-y += smihandler.c

subdirs-y += spd

CPPFLAGS_common += -I$(src)/mainboard/tesla
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
