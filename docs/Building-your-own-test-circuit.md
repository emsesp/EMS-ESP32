Here is a prototype board you can build yourself on a breadboard for testing.

The breadboard layout was done using [DIY Layout Creator](https://github.com/bancika/diy-layout-creator) and sources files are included in this repo.

![Breadboard Circuit](_media/schematics/breadboard.png ':size=500')

The schematic used:

![Schematic](_media/schematics/circuit.png ':size=500')

*Optionally I've also added 2 0.5A/72V polyfuses between the EMS and the two inductors L1 and L2 for extra protection.*

**Powering The EMS Circuit**

The EMS circuit will work with both 3.3V and 5V. It's easiest though to power directly from the ESP8266's 3V3 line and run a steady 5V into the microcontroller. Powering the ESP8266 microcontroller can be either:

- via the USB if your dev board has one
- using an external 5V power supply into the 5V vin on the board
- powering from the 3.5mm service jack (stereo jack) on the boiler. This will give you 8V so you need a buck converter (like a [Pololu D24C22F5](https://www.pololu.com/product/2858)) to step this down to 5V to provide enough power to the ESP8266 (250mA at least)
- powering direct from the EMS line, which is 15V DC and using a buck converter as described above.

**With Power Circuit**

![Power circuit](_media/schematics/Schematic_EMS-ESP.png ':size=600') |
