@ echo off
setlocal
set BINPATH="C:\avr-gcc\avr-gcc-8.0.1_2018-01-19_mingw32\bin"
set INCPATH="C:\avr-gcc\avr-gcc-8.0.1_2018-01-19_mingw32\avr\include"

set SOURCEPATH=.\source
set BUILDPATH=.\build

set OPTFLAGS=-Os -fno-jump-tables -flto -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax
set CSTDFLAGS=-funsigned-char -funsigned-bitfields -std=gnu++14

rem select atmega168 or atmega328p as target
set TARGETMCU=atmega328p

rem configuration macros
rem NDEBUG -> disable debug output
rem F_CPU=value -> declares at which speed the CPU is running (defaults to 16000000, 16MHz)
rem UPDI_BAUD=value -> sets UPDI baud rate. Maxumum is 225000 (225 kbaud, default value). Minimum is F_CPU/100
set DEFINES=-DNDEBUG -DUPDI_BAUD=225000U -DF_CPU=16000000

echo Compiling for %TARGETMCU%...
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o UPDI_lo_lvl.o %SOURCEPATH%/UPDI_lo_lvl.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o UPDI_hi_lvl.o %SOURCEPATH%/UPDI_hi_lvl.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o jtag2updi.o %SOURCEPATH%/jtag2updi.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o jice_io.o %SOURCEPATH%/jice_io.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o JTAG2.o %SOURCEPATH%/JTAG2.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o updi_io.o %SOURCEPATH%/updi_io.cpp
%BINPATH%\avr-g++.exe %DEFINES% -c -I%INCPATH% %OPTFLAGS% %CSTDFLAGS% -Wall -mmcu=%TARGETMCU% -o crc16.o %SOURCEPATH%/crc16.cpp

echo Linking...
mkdir %BUILDPATH%
set OPTFLAGS=-Os -flto -mrelax
%BINPATH%\avr-g++.exe -o %BUILDPATH%\JTAG2UPDI.elf jtag2updi.o JTAG2.o jice_io.o UPDI_lo_lvl.o UPDI_hi_lvl.o updi_io.o crc16.o -Wl,-Map="%BUILDPATH%\STK2UPDI.map" -Wl,--start-group -Wl,-lm -Wl,--end-group -Wl,--gc-sections -mmcu=%TARGETMCU% %OPTFLAGS%

echo Cleaning up...
del *.o

cd %BUILDPATH%

echo Creating HEX file...
%BINPATH%\avr-objcopy.exe -O ihex "JTAG2UPDI.elf" "JTAG2UPDI.hex"

echo Creating LSS file...
%BINPATH%\avr-objdump.exe -h -S "JTAG2UPDI.elf" > "JTAG2UPDI.lss"
