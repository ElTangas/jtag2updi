# jtag2updi

This is a firmware, that when uploaded on an atmega328p, or a similar AVR MCU running @16Mhz, enables it to interface with avrdude using the jtagice Mk2 protocol via a serial link. In particular, you can use an Arduino to host this firmware.
It provides a bridge to program the new attiny817 family of MCUs, that use the UPDI interface:

avrdude -> HW Serial interface -> Programmer MCU (e.g. Mega328P) -> SW Serial on PD6 -> Target MCU (e.g. tiny817)

Currently, I have not tested this software with a level shifter, so the target MCU must run at the same voltage as the programmer.

<pre>
                                              Vcc                     Vcc
                                              +-+                     +-+
                                               |                       |
 +----------+          +---------------------+ |                       | +--------------------+
 | PC       |          | Programmer          +-+                       +-+  Target            |
 | avrdude  |          |                     |      +----------+         |                    |
 |       TX +----------+ RX              PD6 +------+   4k7    +---------+ UPDI               |
 |          |          |                     |      +----------+         |                    |
 |       RX +----------+ TX                  |                           |                    |
 |          |          |                     |                           |                    |
 |          |          |                     |                           |                    |
 |          |          |                     +--+                     +--+                    |
 +----------+          +---------------------+  |                     |  +--------------------+
             JTAGICE MkII                      +-+     UPDI          +-+
             Protocol                          GND     Protocol      GND

</pre>
Drawing adapted from: https://github.com/mraardvark/pyupdi


To build, run the make.bat file, after editing it with the path of AVR-GCC on your system. I provide a makefile suitable for a MS-Windows environment, but I'm sure Linux users can manage.

There are also pre-built files on the "build" directory. They were built using avr-gcc 8.0.1 compiled for MinGW by sprintersb:
https://sourceforge.net/projects/mobilechessboar/files/avr-gcc%20snapshots%20%28Win32%29/
