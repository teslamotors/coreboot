/* SPDX-License-Identifier: GPL-2.0-or-later */

Scope (\_SB.I2C3)
{
	Device (D024)
	{
		Name (_HID, "CYT0001")  // _HID: Hardware ID
		Name (_CID, "CYTTSP6")  // _CID: Compatible ID
		Name (_UID, Zero)  // _UID: Unique ID
		Name (_DDN, "Touch Device: Rear Display")  // _DDN: DOS Device Name
		Method (_STA, 0, NotSerialized)  // _STA: Status
		{
			Return (0x0F)
		}
		Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
		{
			I2cSerialBusV2 (0x0024, ControllerInitiated, 0x00061A80,
			AddressingMode7Bit, "\\_SB.I2C3",
			0x00, ResourceConsumer, , Exclusive,
			)
		})

		/* cy,core */
		Device (CY0) {
			Name (_HID, "PRP0001")
			Name (_DSD, Package () {
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package () {
					Package (2) { "name", "cy,core" },
					Package (2) { "irq_gpio", 395 },
					Package (2) { "rst_gpio", 278 },
					Package (2) { "err_gpio", 278 },
					Package (2) { "max_xfer_len", 0x100 },
					Package (2) { "flags", 0x2 }, /* CY_CORE_FLAG_POWEROFF_ON_SLEEP */
				}
			})
		}


		/* cy,mt */
		Device (CY1) {
			Name (_HID, "PRP0001")
			Name (_DSD, Package () {
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package () {
					Package (2) { "name", "cy,mt" },
					Package (2) { "framework.abs", Package(50) {
						0x35,	/* ABS_MT_POSITION_X */
						0,	/* CY_ABS_MIN_X */
						1526,	/* CY_ABS_MAX_X */
						0, 0,
						0x36,	/* ABS_MT_POSITION_Y */
						0,	/* CY_ABS_MIN_Y */
						768,	/* CY_ABS_MAX_Y */
						0, 0,
						0x3a,	/* ABS_MT_PRESSURE */
						0,	/* CY_ABS_MIN_P */
						255,	/* CY_ABS_MAX_P */
						0, 0,
						0xffff,	/* CY_IGNORE_VALUE */
						0,	/* CY_ABS_MIN_W */
						255,	/* CY_ABS_MAX_W */
						0, 0,
						0x39,	/* ABS_MT_TRACKING_ID */
						0,	/* CY_ABS_MIN_T */
						15,	/* CY_ABX_MAX_T */
						0, 0,
						0x30,	/* ABS_MT_TOUCH_MAJOR */
						0, 255, 0, 0,
						0x31,	/* ABS_MT_TOUCH_MINOR */
						0, 255, 0, 0,
						0x34,	/* ABS_MT_ORIENTATION */
						0xff81,	/* -127 */
						127,
						0, 0,
						0x37,	/* ABS_MT_TOOL_TYPE */
						0,
						2,	/* MT_TOOL_MAX */
						0, 0,
						0x19,	/* ABS_DISTANCE */
						0, 255,	/* Used with hover */
						0, 0,
					}},
					Package (2) { "framework.enable_vkeys", 0 },
					Package (2) { "inp_dev_name", "cyttsp6_mt" },
					Package (2) { "vkeys_x", 1366 },
					Package (2) { "vkeys_y", 768 },
				}
			})
		}

		/* cy,btn */
		Device (CY2) {
			Name (_HID, "PRP0001")
			Name (_DSD, Package () {
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package () {
					Package (2) { "name", "cy,btn" },
					Package (2) { "inp_dev_name", "cyttsp6_btn" },
				}
			})
		}

		/* cy,prox */
		Device (CY3) {
			Name (_HID, "PRP0001")
			Name (_DSD, Package () {
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package () {
					Package (2) { "name", "cy,prox" },
					Package (2) { "framework.abs", Package(5) {
						0x19,	/* ABS_DISTANCE */
						0,	/* CY_PROXIMITY_MIN_VAL */
						1,	/* CY_PROXIMITY_MAX_VAL */
						0, 0
					}},
					Package (2) { "inp_dev_name", "cyttsp6_proximity" },
				}
			})
		}

	}
}
