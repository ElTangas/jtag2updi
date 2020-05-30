# jtag2updi

This is a firmware, that when uploaded on an atmega328p, or a similar AVR MCU (including experimental support for atmega1280/2560, and for Logic Green LGT8F328P/D AVR clones with 32 pins or more), enables it to interface with avrdude using the jtagice Mk2 protocol via a serial link. In particular, you can use an Arduino Uno/Nano to host this firmware (experimental support for Arduino Mega and LGT based Arduino clones).
It provides a bridge to program the new attiny817 family of MCUs, that use the UPDI interface:

avrdude -> HW Serial interface -> Programmer MCU (e.g. Mega328P) -> SW Serial on PD6 -> Target MCU (e.g. tiny817)

Currently, I have not tested this software with a level shifter, however, since the UPDI pin is high voltage tolerant, it's ok to have V_prog > V_target, but not the reverse. <strong>Warning: only the UPDI pins of devices that support high voltage programming should be assumed to be high voltage tolerant. This is not the case for the MegaAVR 0-series or AVR-DA series! </strong> However, the current injection spec for I/O pins on these parts is far more generous that it is for older AVR devices it is not clear if the UPDI pin is included in this.

Notice, however, that the logic levels need to be compatible for successful programming: V_target cannot be lower than about 60% of V_prog (60% will likelly work, 70% is guaranteed to work). Therefore, it will not be possible to program a 2.5V target with a 5.0V programmer, because communication errors will surely occur (but no electrical damage), but if V_target is 3.3V (66% of 5.0V) chances are good.


<pre>
                                            V_prog                 V_target
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

If you use an Arduino as host for this program, be sure that, after burning the software, you disable its auto-reset feature, using one of the techniques described here:
https://playground.arduino.cc/Main/DisablingAutoResetOnSerialConnection

Alternatively, you can use an Arduino without integrated USB/serial adapter, like the pro-mini; in that case, just disconecting the DTR wire will disable the auto-reset. Just remember the UPDI chip must be connected to the same supply voltage as the Arduino's MCU!

## Building with avr-gcc

To build, run the make.bat file, after editing it with the following options:
1) path of AVR-GCC on your system
2) correct target MCU
3) Frequency at which your MCU is running (F_CPU, defaults to 16MHz)
4) Baud rate of UPDI link (UPDI_BAUD, defaults to 225 kbaud)

I provide a makefile suitable for a MS-Windows environment, but I'm sure Linux users can manage.

There are also pre-built files on the "build" directory. They were built using avr-gcc 8.0.1 compiled for MinGW by sprintersb:
https://sourceforge.net/projects/mobilechessboar/files/avr-gcc%20snapshots%20%28Win32%29/

The pre-built hex file is for ATMega 328P@16MHz; to use a different MCU of the same family, like the mega168, or running at a frequency different from 16MHz, you will need to rebuild.

When building for Logic Green AVR clones, select "atmega328p" as target. The macro ARDUINO_AVR_LARDU_328E must also be defined, either in the "sys.h" file (using #define) or in the gcc command line (by adding -DARDUINO_AVR_LARDU_328E). This allows the code to know the target is a LGT chip, and set the chip core speed at run time from F_CPU. Recommended values for F_CPU are 32MHz or 16MHz; 8MHz also works but is less reliable.


## Building with Arduino IDE

If you prefer, the program can be built as if it was an Arduino sketch. Inside the "source" directory, there is an empty file called "jtag2updi.ino" so that the Arduino IDE can recognize the source code.

Just copy all the files inside "source" to a new directory called "jtag2updi" inside your sketch main directory.

The Arduino IDE will automatically set the correct MCU model and F_CPU, but if you want to change the speed of the UPDI link, you will have to edit UPDI_BAUD directly in the source code.

When building for Logic Green AVR clones, I recommend that you install the board definitions from dbuezas (https://github.com/dbuezas/lgt8fx).

This version allows selection of the MCU running speed, unlike the official release ([Larduino_HSP](https://github.com/LGTMCU/Larduino_HSP))

<b>Note:</b>
By default, the following pins are used for UPDI communication on common Arduino boards:
- Arduino Mega (1280/2560): PD3 -> Digital Pin 18
- Arduino (328P): PD6 -> Digital Pin 6

## Using with avrdude

You will find a modified avrdude.conf file in the base folder. This is based on the current avrdude.conf file from:
http://svn.savannah.gnu.org/viewvc/*checkout*/avrdude/trunk/avrdude/avrdude.conf.in?revision=1422

It has been modified to work with avrdude 6.3, by removing (actually, commenting out) some incompatible stuff, and adding the "jtag2updi" programmer type.

The definitions for UPDI chips were slightly modified so that avrdude thinks they use the PDI programming interface instead of UPDI (i.e., avrdude thinks they are some kind of XMegas).

This allows the jtagice mk2 protocol to be used for programming UPDI chips, since this protocol predates UPDI and is not formally compatible with it. Originally, I had planed to use the STK500v2 protocol, and emulate the ISP interface, and I actually wrote an ISP version of the programmer software.

However, this would require entirely new definitions for the UPDI chips inside the avrdude.conf file, while using jtagice2 requires only very slight changes to the definions provided by Atmel (now Microchip).

<b>Note:</b>
If you install the Arduino board "Nano Every" in your Arduino IDE, it will come with versions of avrdude and avrdude.conf files that support jtag2updi. You can use those files instead of the compatibility avrdude.conf supplied here which is meant for older avrdude versions.

Jtagice mk2 is the most advanced of Atmel's programming protocols that still supports a UART serial connection instead of USB, making it easily compatible with any Arduino you choose to host this software, and any OS you run avrdude on.

It's major limitation is speed; it can't go over 115200 Baud, because the protocol lacks definitions for higher speeds. It's actually inferior to the STK500v2 protocol in this respect, this older standard can run at any speed avrdude instructs it to.

Fortunately, the current UPDI chips do not have very large flash memories, so I think this isn't a major issue.

<b>Example command line (windows):</b>
<pre>
avrdude -c jtag2updi -P com7 -p t1614
</pre>

If all the connections are correct and the target is indeed an *unlocked* tiny1614, the output will be:
<pre>
avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.03s

avrdude: Device signature = 0x1e9422 (probably t1614)

avrdude done.  Thank you.
</pre>

If the chip is locked, the output will be:
<pre>
avrdude: jtagmkII_reset(): bad response to reset command: RSP_ILLEGAL_MCU_STATE
avrdude: initialization failed, rc=-1
         Double check connections and try again, or use -F to override
         this check.

avrdude: jtagmkII_close(): bad response to sign-off command: RSP_ILLEGAL_MCU_STATE

avrdude done.  Thank you.
</pre>

To unlock the chip, you need to erase it, using the -e option on avrdude:
<pre>
avrdude -c jtag2updi -P com7 -p t1614 -e
</pre>

Alternatively, you can erase the chip from interactive mode, enter it using "-t", and "-F" to override the error:
Note: You must build with DISABLE_HOST_TIMEOUT defined for terminal mode to work. See below for more information on the timeouts.
<pre>
avrdude -c jtag2updi -P com7 -p t1614 -t -F
</pre>

You will enter the avrdude prompt:
<pre>
avrdude: jtagmkII_reset(): bad response to reset command: RSP_ILLEGAL_MCU_STATE
avrdude: initialization failed, rc=-1
avrdude: AVR device initialized and ready to accept instructions
avrdude: Device signature = 0xffff00
avrdude: Expected signature for ATtiny1614 is 1E 94 22
avrdude: NOTE: Programmer supports page erase for Xmega devices.
         Each page will be erased before programming it, but no chip erase is performed.
         To disable page erases, specify the -D option; for a chip-erase, use the -e option.
avrdude>
</pre>

Enter "erase" then "quit" and the chip will be unlocked (and erased).
<pre>
avrdude> erase
>>> erase
avrdude: erasing chip
avrdude> quit
>>> quit

avrdude done.  Thank you.
</pre>


## Extra information
If you call avrdude with -v -v -v -v (maximum verbosity) so you see all the data sent back, it will add


## Debug channel
Regardless of what AVR you are running it on, you can enable SPI debugging in sys.h (see commented out examples) - this outputs data on SPI that is meant to be converted to serial on another board - this debug channel allows you to get something approxiomating a serial port to print debug info to on parts without a second USART
On parts with multiple USARTS, you can just use that for debugging (similarly, there are commented out examples). Both print the same data.
In dbg.h, you can enable extended information for most UPDI calls.

## Timeouts
Previous versions of jtag2updi could get hung up waiting for a target that wasn't working (for example, because it was connected incorrectly) or the host (for example, if you entered a slow command, like reading a 128k flash memory when you didn't intend to, and ctrl-c hoping to "save time"), requiring a reset to reconnect to it. This version has timeouts on communication with both the host (250ms - it gives up and assumes the host isn't going to sat anything after three timeouts, and awaits a fresh session from avrdude), and when talking to the target (100ms - which is far more time than the target should ever take to respond). In the event of a timeout communicating with the target, it will return RSP_NO_TARGET_POWER response. The host timeout of course prevents terminal mode from working (target timeout should just prevent hangs without downside)
Both can be disabled by uncommenting #define DISABLE_TARGET_TIMEOUT and/or #define DISABLE_HOST_TIMEOUT in sys.h - the latter should uncommented if terminal mode is to be used, and should remain commented out if terminal mode will not be used.

## Supported parts
jtag2updi was tested on tinyAVR 0-series and 1-series and ATmega 328p/168p.
It is expected to work on classic megaAVR x8, x4, x1, and x0 parts, as well as megaAVR 0-series and AVR-DA parts.

## Using with AVR JTAG ICE usb stick

The code can be used with the compact **AVR JTAG ICE** usb stick available to buy online from various sources.

See [Tools](tools/avrjtagicev2) section of the project on how to prepare and use the stick.


## Troubleshooting

If you have triple-checked all the connections but still getting errors, the problem might be the speed of the serial links. I have set the jtag2updi entry on the avrdude configuration file to run at 115200 baud by default. This baud rate can cause errors if, for example, your MCU is running at 8MHz.

This can be changed with the avrdude "-b" option. Valid baud rates are 2400, 4800, 9600, 14400, 19200, 38400, 57600 and 115200. You can make the setting permanent by editing the jtag2updi entry on "avrdude.conf".

If the trouble is on the UPDI link, a slower speed can be selected by changing UPDI_BAUD and recompiling.
You can also try to use the alternate bit banging USART by setting UPDI_IO_TYPE to 2.
You will find these and other configuration settings in the "sys.h" header file.

Sometimes, the programmer can become irresponsive, in that case it should be reset.

## Contact the author

I usually lurk around in the AVRFreaks forum. If you have questions/suggestions/etc. about this program, please post to this thread:

https://www.avrfreaks.net/forum/updi-programmer-software-arduino-compatible-avrdude

For serious problems with the code, like bugs, please open a github issue.
