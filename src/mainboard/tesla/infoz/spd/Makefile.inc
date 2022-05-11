# SPDX-License-Identifier: GPL-2.0-or-later

# This directory
SPD_SOURCES_DIR := src/mainboard/$(MAINBOARDDIR)/spd

SPD_SOURCES =
ifeq ($(CONFIG_BOARD_TESLA_CORIANDER)$(CONFIG_BOARD_TESLA_KINGBIRD),y)
SPD_SOURCES += micron-MT40A512M16LY-062E.spd.hex             # APCB instance 0
SPD_SOURCES += micron-MT40A1G16KH-062E.spd.hex               # APCB instance 1

else ifeq ($(CONFIG_BOARD_TESLA_SPINACH),y)
SPD_SOURCES += micron-MT40A1G16KH-062E-nonECC.spd.hex        # APCB instance 0
SPD_SOURCES += micron-MT40A512M16LY-062E-nonECC.spd.hex      # APCB instance 1
SPD_SOURCES += samsung-K4AAG165WA-BHWE02-nonECC.spd.hex      # APCB instance 2
SPD_SOURCES += samsung-K4AAG165WA-BHWE02.spd.hex             # APCB instance 3

else ifeq ($(CONFIG_BOARD_TESLA_SEMI),y))
SPD_SOURCES += micron-MT40A1G16KH-062E.spd.hex               # APCB instance 0
SPD_SOURCES += micron-MT40A512M16LY-062E.spd.hex             # APCB instance 1

else
$(error Undefined SPD selection for InfoZ variant)
endif

APCB_SOURCES=$(foreach f, $(basename $(SPD_SOURCES)), $(obj)/APCB_$(f).gen)

# APCB binary with magic numbers to be replaced by apcb_edit tool
APCB_MAGIC_BLOB:=$(MAINBOARD_BLOBS_DIR)/$(VARIANT_DIR)/APCB_magic.bin

$(obj)/APCB_%.spd.gen::	$(SPD_SOURCES_DIR)/%.spd.hex \
			$(APCB_EDIT_TOOL) \
			$(APCB_MAGIC_BLOB)
	$(APCB_EDIT_TOOL) \
	$(APCB_MAGIC_BLOB) \
	$@ \
	--hex \
	--spd_0_0 $< \
	$(if $(APCB_POPULATE_2ND_CHANNEL), --spd_1_0 $<, ) \
	--board_id_gpio0 $(APCB_BOARD_ID_GPIO0) \
	--board_id_gpio1 $(APCB_BOARD_ID_GPIO1) \
	--board_id_gpio2 $(APCB_BOARD_ID_GPIO2) \
	--board_id_gpio3 $(APCB_BOARD_ID_GPIO3)
