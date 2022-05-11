# SPDX-License-Identifier: GPL-2.0-or-later

# XXX: Disabled until we get build-time APCB generation working
#subdirs-y += ../../spd

bootblock-$(CONFIG_BOARD_TESLA_KINGBIRD) += helpers.c

romstage-$(CONFIG_BOARD_TESLA_KINGBIRD) += gpio_kingbird.c
romstage-$(CONFIG_BOARD_TESLA_KINGBIRD) += helpers.c

ramstage-$(CONFIG_BOARD_TESLA_KINGBIRD) += gpio_kingbird.c
ramstage-$(CONFIG_BOARD_TESLA_KINGBIRD) += fsps_kingbird.c
ramstage-$(CONFIG_BOARD_TESLA_KINGBIRD) += helpers.c

smm-$(CONFIG_BOARD_TESLA_KINGBIRD) += gpio_kingbird.c
smm-$(CONFIG_BOARD_TESLA_KINGBIRD) += helpers.c

# Add OEM ID table
ifeq ($(CONFIG_USE_OEM_BIN),y)
cbfs-files-y += oem.bin
oem.bin-file := $(call strip_quotes,$(CONFIG_OEM_BIN_FILE))
oem.bin-type := raw
endif #($(CONFIG_USE_OEM_BIN),y)

# APCB Board ID GPIO configuration.
# These GPIOs determine which memory SPD will be used during boot.
# APCB_BOARD_ID_GPIO[0-3] = GPIO_NUMBER GPIO_IO_MUX GPIO_BANK_CTL
# GPIO_NUMBER: FCH GPIO number
# GPIO_IO_MUX: Value write to IOMUX to configure this GPIO
# GPIO_BANK_CTL: Value write to GPIOBankCtl[23:16] to configure this GPIO
ifeq ($(CONFIG_BOARD_TESLA_KINGBIRD),y)
APCB_BOARD_ID_GPIO0 = 130 1 44
APCB_BOARD_ID_GPIO1 = 69 0 44
APCB_BOARD_ID_GPIO2 = 70 0 44
APCB_BOARD_ID_GPIO3 = 116 1 44
APCB_POPULATE_2ND_CHANNEL = true
else
$(error Undefined APCB selection GPIOS for Tesla infoz platform)
endif #($(CONFIG_BOARD_TESLA_KINGBIRD),y)
