/* SPDX-License-Identifier: GPL-2.0-or-later */
Device (LEDS)
{
	Name (_HID, "PRP0001")

	Name (_CRS, ResourceTemplate () {
		/* SOC-HEARTBEAT-LED GPIO */
		GpioIo (Exclusive, PullUp, 0, 0,
			IoRestrictionOutputOnly, "\\_SB.GPIO", 0,
			ResourceConsumer) { 12 }
	})

	Name (_DSD, Package () {
		ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package () {
			Package ()
			{
				"compatible", Package () {"gpio-leds"}
			},
		}
	})

	Device (LED1)
	{
		Name (_HID, "PRP0001")
		Name (_DSD, Package () {
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () {"label", "heartbeat:blue:led1"},
				Package () {"gpios", Package () {^^LEDS, 0, 0, 1 /* high-active */}},
				Package () {"default-state", "keep"},
				Package () {"default-trigger", "heartbeat"},
			}
		})
	}
}
