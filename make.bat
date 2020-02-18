@ echo off
setlocal
set BINPATH="C:\avr-gcc\avr-gcc-arduino\bin"
set INCPATH="C:\avr-gcc\avr-gcc-arduino\avr\include"

set SOURCEPATH=.\source
set BUILDPATH=.\build

set OPTFLAGS=-Os -fno-jump-tables -fno-gcse -flto -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax
set CSTDFLAGS=-funsigned-char -funsigned-bitfields -std=gnu++11

rem Select host device
rem
rem Supported classic AVR: atmega16, atmega88/168/328p, atmega1280/2560, probably others
rem Supported Logic Green devices: LGT8F328P/D, however, atmega328p must be set as target
rem Supported AVR-0/1 devices (aka avrxmega3 architecture on gcc): all that have at least 512 KB RAM
rem
set TARGETMCU=atmega328p

rem configuration macros
rem
rem NDEBUG -> if defined, disable debug output
rem
rem F_CPU=value -> declares at which speed the host device is running (defaults to 16000000, 16MHz)
rem
rem UPDI_BAUD=value -> sets UPDI baud rate. Maximum is 225000 (225 kbaud, default value). Minimum is F_CPU/200
rem
rem ARDUINO_AVR_LARDU_328E -> if defined, host device is a Logic Green LGT8F328P/D
rem
rem UPDI_IO_TYPE=value -> Selects bitbang UART for interfacing with the UPDI target device. Possible values are 1 and 2.
rem Type 1 is deprecated, so use type 2 (default).
rem
rem UPDI_PORT=port_letter and UPDI_PIN=pin_number -> Select port and pin for UPDI interface to target.
rem If using UPDI_IO_TYPE=1, these must be set to the OC0A timer output pin.
rem If using UPDI_IO_TYPE=2, these can be any free I/O pin.
rem If the target is the Mega328P, defaults to the OC0A pin (PD6). See "sys.h" file for more details.
rem
rem HOST_USART=USARTx -> Select USART to be used for serial communication with the host PC, for host devices that
rem have more than one USART peripheral (tiny AVR-0/1 and mega AVR-0 only). Defaults to USART0.
rem
rem HOST_TX_PORT=port_letter and HOST_TX_PIN=pin_number -> Must correspond to port and pin where the Tx signal of HOST_USART
rem is present (tiny AVR-0/1 and mega AVR-0 only). Defaults to PB2, which is the Tx pin for USART0 on many tiny AVR-0/1 chips.
rem
set DEFINES=-DNDEBUG -DUPDI_BAUD=225000U -DF_CPU=16000000

rem Optional optimization settings
rem Otherwise unused r/w I/O registers residing in I/O addresses 0-63 can be used here
set DEFINES=%DEFINES% -DTEMP0=GPIOR0 -DTEMP1=GPIOR1 -DTEMP2=GPIOR2

rem 
rem select AVRJTAGICE v2.0 as target (override upper)
rem 
rem set TARGETMCU=atmega16
rem set DEFINES=-DNDEBUG -DUPDI_BAUD=120000U -DF_CPU=7372800 -DLED_PORT=B -DLED_PIN=3 -DUPDI_PORT=D -DUPDI_PIN=2 -DUPDI_IO_TYPE=2


echo Compiling for %TARGETMCU%...
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o UPDI_lo_lvl.o %SOURCEPATH%/UPDI_lo_lvl.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o UPDI_hi_lvl.o %SOURCEPATH%/UPDI_hi_lvl.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o jtag2updi.o %SOURCEPATH%/jtag2updi.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o jice_io.o %SOURCEPATH%/jice_io.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o JTAG2.o %SOURCEPATH%/JTAG2.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o updi_io_soft.o %SOURCEPATH%/updi_io_soft.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o updi_io.o %SOURCEPATH%/updi_io.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o crc16.o %SOURCEPATH%/crc16.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o sys.o %SOURCEPATH%/sys.cpp

echo Linking...
mkdir %BUILDPATH%
set OPTFLAGS=-Os -flto -mrelax
%BINPATH%\avr-g++.exe -o %BUILDPATH%\JTAG2UPDI.elf jtag2updi.o JTAG2.o jice_io.o UPDI_lo_lvl.o UPDI_hi_lvl.o updi_io.o updi_io_soft.o crc16.o sys.o -Wl,-Map="%BUILDPATH%\STK2UPDI.map" -Wl,--start-group -Wl,-lm -Wl,--end-group -Wl,--gc-sections -mmcu=%TARGETMCU% %OPTFLAGS%

echo Cleaning up...
del *.o

cd %BUILDPATH%

echo Creating HEX file...
%BINPATH%\avr-objcopy.exe -O ihex "JTAG2UPDI.elf" "JTAG2UPDI.hex"

echo Creating LSS file...
%BINPATH%\avr-objdump.exe -h -S "JTAG2UPDI.elf" > "JTAG2UPDI.lss"
