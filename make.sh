#!/bin/sh

# avr-gcc++ path
BINPATH="/usr/bin"
INCPATH="./source"

SOURCEPATH="$PWD/source/"
BUILDPATH="$PWD/build"

OPTFLAGS="-Os -fno-jump-tables -fno-gcse -flto -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax"
CSTDFLAGS="-funsigned-char -funsigned-bitfields -std=gnu++14"

# select atmega168, atmega328p, atmega1280 or atmega2560 as target
TARGETMCU=atmega328p

# configuration macros
# NDEBUG -> disable debug output
# F_CPU=value -> declares at which speed the CPU is running (defaults to 16000000, 16MHz)
# UPDI_BAUD=value -> sets UPDI baud rate. Maxumum is 225000 (225 kbaud, default value). Minimum is F_CPU/100
DEFINES="-DNDEBUG -DUPDI_BAUD=225000U -DF_CPU=16000000" #atmega328

# Optional optimization settings
# Otherwise unused r/w I/O registers residing in I/O addresses 0-63 can be used here
DEFINES="$DEFINES -DTEMP0=GPIOR0 -DTEMP1=GPIOR1 -DTEMP2=GPIOR2"

##
## select AVRJTAGICE v2.0 as target (override upper)
##
#TARGETMCU=atmega16
#DEFINES="-DNDEBUG -DUPDI_BAUD=120000U -DF_CPU=7372800 -DLED_PORT=B -DLED_PIN=3 -DUPDI_PORT=D -DUPDI_PIN=2 -DUPDI_IO_TYPE=2"

echo Compiling for $TARGETMCU ...
$BINPATH/avr-g++ $DEFINES -c -I$INCPATH $OPTFLAGS $CSTDFLAGS -Wall -mmcu=$TARGETMCU -o UPDI_lo_lvl.o $SOURCEPATH/UPDI_lo_lvl.cpp
$BINPATH/avr-g++ $DEFINES -c -I$INCPATH $OPTFLAGS $CSTDFLAGS -Wall -mmcu=$TARGETMCU -o UPDI_hi_lvl.o $SOURCEPATH/UPDI_hi_lvl.cpp
$BINPATH/avr-g++ $DEFINES -c -I$INCPATH $OPTFLAGS $CSTDFLAGS -Wall -mmcu=$TARGETMCU -o jtag2updi.o $SOURCEPATH/jtag2updi.cpp
$BINPATH/avr-g++ $DEFINES -c -I$INCPATH $OPTFLAGS $CSTDFLAGS -Wall -mmcu=$TARGETMCU -o JICE_io.o $SOURCEPATH/JICE_io.cpp
$BINPATH/avr-g++ $DEFINES -c -I$INCPATH $OPTFLAGS $CSTDFLAGS -Wall -mmcu=$TARGETMCU -o JTAG2.o $SOURCEPATH/JTAG2.cpp
$BINPATH/avr-g++ $DEFINES -c -I$INCPATH $OPTFLAGS $CSTDFLAGS -Wall -mmcu=$TARGETMCU -o updi_io_soft.o $SOURCEPATH/updi_io_soft.cpp
$BINPATH/avr-g++ $DEFINES -c -I$INCPATH $OPTFLAGS $CSTDFLAGS -Wall -mmcu=$TARGETMCU -o updi_io.o $SOURCEPATH/updi_io.cpp
$BINPATH/avr-g++ $DEFINES -c -I$INCPATH $OPTFLAGS $CSTDFLAGS -Wall -mmcu=$TARGETMCU -o crc16.o $SOURCEPATH/crc16.cpp
$BINPATH/avr-g++ $DEFINES -c -I$INCPATH $OPTFLAGS $CSTDFLAGS -Wall -mmcu=$TARGETMCU -o sys.o $SOURCEPATH/sys.cpp

echo Linking...
if [ ! -d $BUILDPATH ]
then
  mkdir $BUILDPATH
fi
set OPTFLAGS="-Os -flto -mrelax"
$BINPATH/avr-g++ -o $BUILDPATH/JTAG2UPDI.elf \
                    jtag2updi.o JTAG2.o JICE_io.o UPDI_lo_lvl.o \
                    UPDI_hi_lvl.o updi_io.o updi_io_soft.o crc16.o sys.o \
                 -Wl,-Map="$BUILDPATH/STK2UPDI.map" -Wl,-lm \
                 -Wl,--start-group -Wl,--end-group -Wl,--gc-sections \
                 -mmcu=$TARGETMCU $OPTFLAGS

echo Cleaning up...
rm -rf *.o

cd $BUILDPATH

echo Creating HEX file...
$BINPATH/avr-objcopy -O ihex "JTAG2UPDI.elf" "JTAG2UPDI.hex"

echo Creating LSS file...
$BINPATH/avr-objdump -h -S "JTAG2UPDI.elf" > "JTAG2UPDI.lss"
