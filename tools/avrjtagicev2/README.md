# AVR JTAG ICE v2.0

The firmware can be uploaded to **AVR JTAG ICE** usb stick with no dismantling or soldering just by a simple upload tool.


Currently AVR JTAG ICE can be bought from various sources, one of them: [Aliexpress](https://www.aliexpress.com/item/AVR-JTAG-ICE-USB-Download-Programmer-Emulator-Aluminum-Shell-Over-Current-Protection-Wide-Voltage-Buffer-Chip/32656234883.html)

<pre>



 +----------+          +---------------------+                           +--------------------+
 | PC       |          | AVR JTAG ICE  VTref +(4)--------->--------------+ Vcc                |
 | avrdude  |          |                     |      +----------+         |                    |
 |          |          |               nSRST +(6)---+   4k7    +---------+ UPDI               |
 |      USB +----------+ USB                 |      +----------+         |                    |
 |          |          |                     |                           |       Target       |
 |          |          |                     |                           |                    |
 |          |          |                     |                           |                    |
 |          |          |                 GND +(2)-+                   +--+ GND                |
 +----------+          +---------------------+    |                   |  +--------------------+
             JTAGICE MkII                        +-+     UPDI        +-+
             Protocol                            GND     Protocol    GND

</pre>


Just remember **VTref** will supply the target MCU chip with **5V** from the AVR JTAG ICE ! For using with different own MCU board potential (e.g. 3.3V or 1.8V) leave **VTref** unconnected. Also you can step down the voltage using a regulator (e.g. 3.3V) from **VTref** if target mcu supports lower voltage only.

## Building the firmware

Uncomment lines below to enable AVR JTAG ICE support and then run makefile to compile the firmware.

* For windows build edit **make.bat** remove remarks from two lines:

```
...
rem
rem select AVRJTAGICE v2.0 as target (override upper)
rem
set TARGETMCU=atmega16
set DEFINES=-DNDEBUG -DUPDI_BAUD=120000U -DF_CPU=7372800 ...
...
```

* For linux build edit **make.sh** uncomment two of the lines:
```
...
##
## select AVRJTAGICE v2.0 as target (override upper)
##
TARGETMCU=atmega16
DEFINES="-DNDEBUG -DUPDI_BAUD=120000U -DF_CPU=7372800 ...
...
```

## Flashing the firmware

To burn the USB module, use [avr-aosp.py](https://github.com/cbalint13/avr-aosp/) uploader tool (as superuser) plug in USB module and while the LED blinks fast red-blue (5 seconds window at statup) run these steps:

1) Erase content of module:

```
python3 avr-aosp.py -op erase

INFO

  S = AVRBOOT 	#programmer id
  V = 10 	#software version
  v = ? 	#hardware version
  p = S 	#programmer type
  a = Y 	#autoincrement support
  b = ? 	#block mode support
  t = [7400] 	#supported device code
  s = 02941e 	#signature
  N = ? 	#high fuse bits
  F = ? 	#low fuse bits
  r = ? 	#lock bits
  Q = ? 	#extended fuse bits

ERASE program memory

```

2) Upload the firmware:

```
python3 avr-aosp.py -op write -file ../../build/JTAG2UPDI.hex

INFO

  S = AVRBOOT 	#programmer id
  V = 10 	#software version
  v = ? 	#hardware version
  p = S 	#programmer type
  a = Y 	#autoincrement support
  b = ? 	#block mode support
  t = [7400] 	#supported device code
  s = 02941e 	#signature
  N = ? 	#high fuse bits
  F = ? 	#low fuse bits
  r = ? 	#lock bits
  Q = ? 	#extended fuse bits

WRITE program memory [../../build/JTAG2UPDI.hex] #2378 bytes
0...10...20...30...40...50...60...70...80...90...100 - done.

```

3) Unplug and then replug the USB stick, red-blue blinking stops with permanent red LED turned on.

4) With red LED turned on firmware is active and can be used with avrdude, see provided wiring to target MCU.
