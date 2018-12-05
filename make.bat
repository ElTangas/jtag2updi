@ echo off
setlocal
set BINPATH="C:\avr-gcc\avr-gcc-8.0.1_2018-01-19_mingw32\bin"
set INCPATH="C:\avr-gcc\avr-gcc-8.0.1_2018-01-19_mingw32\avr\include"

set SOURCEPATH=.\source
set BUILDPATH=.\build

set OPTFLAGS=-Os -fno-jump-tables -fno-gcse -flto -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax
set CSTDFLAGS=-funsigned-char -funsigned-bitfields -std=gnu++14

rem select atmega168 or atmega328p as target
set TARGETMCU=atmega328p

rem configuration macros
rem NDEBUG -> disable debug output
rem F_CPU=value -> declares at which speed the CPU is running (defaults to 16000000, 16MHz)
rem UPDI_BAUD=value -> sets UPDI baud rate. Maxumum is 225000 (225 kbaud, default value). Minimum is F_CPU/100
set DEFINES=-DNDEBUG -DUPDI_BAUD=225000U -DF_CPU=16000000

rem Optional optimization settings
rem Otherwise unused r/w I/O registers residing in I/O addresses 0-63 can be used here
set DEFINES=%DEFINES% -DTEMP0=GPIOR0 -DTEMP1=GPIOR1 -DTEMP2=GPIOR2

rem 
rem select AVRJTAGICE v2.0 as target (override upper)
rem 
rem set TARGETMCU=atmega16
rem set DEFINES=-DNDEBUG -DUPDI_BAUD=120000U -DF_CPU=7372800 -DLED_PORT=B -DLED_PIN=3 -DUPDI_PORT=D -DUPDI_PIN=2


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
