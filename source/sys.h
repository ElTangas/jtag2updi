/*
 * sys.h
 *
 * Created: 02-10-2018 13:07:18
 *  Author: JMR_2
 */

#ifndef SYS_H_
#define SYS_H_

#include <avr/io.h>
#include <string.h>
#include <stdio.h> // for size_t
#include "parts.h"

//Disable the response signature during burst writes to permit faster writes
#define NO_ACK_WRITE

// Disable the host timeout - this is required for use with avrdude in terminal mode (-t)
// but with this disabled, can get "stuck" if communication with host is interrupted at 115200 baud
// as avrdude on next start will try to communicate at 19200 baud. Reset is required to fix this.
// Potential enhancement - if this is set, while waiting for message from host, ALSO count transitions on the RX pin
// If we see a bunch of transitions, but no received characters, we know the host is using the wrong baud rate, and
// so, switch back to 19200 baud.
//#define DISABLE_HOST_TIMEOUT

// Disable the timeout waiting for response from target. Probably not necessary to set. With this disabled, if the target is expected to respond
// but does not, jtag2updi will get stuck waiting for it.
//#define DISABLE_TARGET_TIMEOUT

// Deduces the NVM version from reading the SIB string. Otherwise, the NVM version is deduced from the size of flash pages.
//#define DEDUCE_NVM_VERSION_FROM_SIB

// this will include the SIB, deduced NVM version, and silicon revision in early responses to SET_DEVICE_DESCRIPTPOR (for SIB and NVM version) and ENTER_PROGMODE (for silicon rev)
//#define INCLUDE_EXTRA_INFO_JTAG


// Auxiliary Macros
#define CONCAT(A,B) A##B                // concatenate
#define XCONCAT(A,B) CONCAT(A,B)        // expand and concatenate
//#define CONCAT3(A,B,C) A##B##C
#if (__AVR_ARCH__ == 103) || (__AVR_ARCH__ == 104)
//We'll call these ones XAVR for purposes of defines, instead of XTINY. This encompasses megaAVR 0-series and DA-series parts

  #define XAVR
  # define PIN(x) CONCAT(VPORT,x).IN
  # define PORT(x) CONCAT(VPORT,x).OUT
  # define DDR(x) CONCAT(VPORT,x).DIR
//  # define PULLUP_ON(x,y) CONCAT(PORT,x).CONCAT3(PIN,y,CTRL)=0x04
//  # define PULLUP_OFF(x,y) CONCAT(PORT,x).CONCAT3(PIN,y,CTRL)=0x04

#else

  # define PIN(x) CONCAT(PIN,x)
  # define PORT(x) CONCAT(PORT,x)
  # define DDR(x) CONCAT(DDR,x)

#endif

/* User Configuration goes up here - the rest down below is defaults that you can override
   or stuff that is hardware specific   */

#if defined(__AVR_ATtiny_Zero_One__)
  // tinyAVR 0-series and 1-series parts
  // 3216, 1604, that kind of thing

  #define LED_PORT A
  #define LED_PIN 7

  // Second LED is used to indicate NVM version, or as an additional debugging aid.
  #define LED2_PORT A
  #define LED2_PIN 6

  //USARTDEBUG not practical here because only one UART.
  //    #define USE_SPIDEBUG

  //SPIPRESC sets prescaler of SPI clock - it can go *INSANELY* fast, such that very little could keep up with it.

  #define SPIPRESC (SPI_CLK2X_bm|SPI_PRESC1_bm)


#elif defined( __AVR_ATmega_Mighty__ )
  // For ATmega16, 32, ... 128 and the later x4 parts (up to the 1284P).
  // On the ones with two USARTS, you can use USART debugging.
  //
  // Here USART debug output is an option too
  // at least on parts with the second USART
  // You should be able to hit 2MBaud with F_CPU=16000000 and DEBUG_BAUDVAL 0 or 1mbaud with 1 if your serial adapter can't handle that.
  // At 20, though, on classic AVRs, you're out of luck unless you have an adapter that can do 2.5 mbaud...have to go all the way down to 500kbaud (DEBUG_BAUDVAL 4) to divide it to something common.. Good thing almost everyone runs them at 16.
  // #define USE_USARTDEBUG
  // #define DEBUG_BAUDVAL 0




#elif defined (__AVR_ATmega_Mini__) || defined(ARDUINO_AVR_LARDU_328E)
  // For ATmega8/88/168/328 (P, PB) parts
  // Same deal as before/ Remember that the buildin LED is on the same pin as SCK on most boards, so you'll want to cadd LEDs on other pinn, defaults to D2
  // On PB parts can also use second USART. Using the second SPI is not supported.


  //#define USE_SPIDEBUG
  //#define SPIPRESC (0x05)



#elif defined (__AVR_ATmega_Mega__)
  // 2560 and that family, like the ones used on the Arduino Mega
  // Same as the others with extra USARTS, only here you can specify which one if you must
  // #define USE_USARTDEBUG
  // #define DEBUG_USART 2
  // Can even change the host USART if you want


#elif defined (__AVR_ATmega_Zero__ ) || defined( __AVR_DX__)
// 4808, 4809. and the rest of the megaAVR 0-series
// Same as above, pretty much
// big difference here is your specify the name of the peripheral instead of the number, and the target baud rate, because we grab the OSCCAL value per datasheet./
//  #define USE_USARTDEBUG
//  #define DEBUG_USART USART1
//  #define DEBUG_BAUDRATE 2000000UL

#endif


/* Defaults and hardware-specific stuff                 */
/* Shouldn't need to change anything here               */
/* IF you want to override, copy it to the blocks above */


#if defined(__AVR_ATtiny_Zero_One__)
// tinyAVR 0-series and 1-series parts

  # ifndef UPDI_PORT
  #   define UPDI_PORT B
  # endif

  # ifndef UPDI_PIN
  #   define UPDI_PIN 0
  # endif

  # ifndef LED_PORT
  #   define LED_PORT B
  # endif

  # ifndef LED_PIN
  #   define LED_PIN 1
  # endif

  # ifndef HOST_USART
  #   define HOST_USART USART0
  # endif

  # ifndef HOST_TX_PORT
  #   define HOST_TX_PORT B
  # endif

  # ifndef HOST_TX_PIN
  #   define HOST_TX_PIN 2
  # endif

  # ifndef HOST_RX_PIN
  #   define HOST_RX_PIN 3
  # endif


#elif defined( __AVR_ATmega_Mighty__ )
// For ATmega16, 32, ... 128 and the later x4 parts (up to the 1284P).

  # ifndef HOST_USART
  #   define HOST_USART 0
  # endif

  # ifndef UPDI_PORT
  #   define UPDI_PORT C
  # endif

  # ifndef UPDI_PIN
  #   define UPDI_PIN 7
  # endif

  # ifndef LED_PORT
  #   define LED_PORT B
  # endif

  # ifndef LED_PIN
  #   define LED_PIN 7
  # endif


#elif defined (__AVR_ATmega_Mini__) || defined(ARDUINO_AVR_LARDU_328E)
// For ATmega8/88/168/328 (P, PB) parts

  # ifndef HOST_USART
  #   define HOST_USART 0
  # endif

  # ifndef UPDI_PORT
  #   define UPDI_PORT D
  # endif

  # ifndef UPDI_PIN
  #   define UPDI_PIN 6
  # endif

  #ifdef USE_SPIDEBUG

    # ifndef LED_PORT
    #   define LED_PORT C
    # endif

    # ifndef LED_PIN
    #   define LED_PIN 5
    # endif

  #else

    # ifndef LED_PORT
    #   define LED_PORT B
    # endif

    # ifndef LED_PIN
    #   define LED_PIN 5
    # endif

  #endif


#elif defined (__AVR_ATmega_Mega__)
// 2560 and that family, like the ones used on the Arduino Mega

  # ifndef HOST_USART
  #   define HOST_USART 0
  # endif

  # ifndef UPDI_PORT
  #   define UPDI_PORT D
  # endif

  # ifndef UPDI_PIN
  #   define UPDI_PIN 3
  # endif

  # ifndef LED_PORT
  #   define LED_PORT B
  # endif

  # ifndef LED_PIN
  #   define LED_PIN 7
  # endif


#elif defined (__AVR_ATmega_Zero__)
// 4808, 4809. and the rest of the megaAVR 0-series

  # ifndef UPDI_PORT
  #   define UPDI_PORT B
  # endif

  # ifndef UPDI_PIN
  #   define UPDI_PIN 0
  # endif

  # ifndef LED_PORT
  #   define LED_PORT B
  # endif

  # ifndef LED_PIN
  #   define LED_PIN 1
  # endif

  # ifndef HOST_USART
  #   define HOST_USART USART0
  # endif

  # ifndef HOST_TX_PORT
  #   define HOST_TX_PORT A
  # endif

  # ifndef HOST_TX_PIN
  #   define HOST_TX_PIN 0
  # endif

  # ifndef HOST_RX_PIN
  #   define HOST_RX_PIN 1
  # endif


#elif defined (__AVR_DX__)
// AVR-DA series
// Note: the UPDI and LED pins were chosen to correspond to the pins in the same position on the DIP-28 atmega328p

  # ifndef UPDI_PORT
  #   define UPDI_PORT D
  # endif

  # ifndef UPDI_PIN
  #   define UPDI_PIN 6
  # endif

  # ifndef LED_PORT
  #   define LED_PORT A
  # endif

  # ifndef LED_PIN
  #   define LED_PIN 6
  # endif

  # ifndef HOST_USART
  #   define HOST_USART USART0
  # endif

  # ifndef HOST_TX_PORT
  #   define HOST_TX_PORT A
  # endif

  # ifndef HOST_TX_PIN
  #   define HOST_TX_PIN 0
  # endif

  # ifndef HOST_RX_PIN
  #   define HOST_RX_PIN 1
  # endif

#else
  #warning "Part not supported - if you didn't provide all the needed pin definitions, that's why it's not compiling"
#endif //End of the defaults!

// Define USART registers and bits based on the selected HOST_USART for non-XAVR parts
#ifndef XAVR
  // Define generic USART flags if not defined already
  #ifndef UDRE
    #define UDRE XCONCAT(UDRE, HOST_USART)
    #define U2X XCONCAT(U2X, HOST_USART)
    #define TXEN XCONCAT(TXEN, HOST_USART)
    #define RXEN XCONCAT(RXEN, HOST_USART)
    #define RXC XCONCAT(RXC, HOST_USART)
    #define TXC XCONCAT(TXC, HOST_USART)
  #endif
  // USART registers
  #ifdef UCSRA
    #define HOST_UCSRA UCSRA
    #define HOST_UCSRB UCSRB
    #define HOST_UDR UDR
  #else
    #define HOST_UCSRA XCONCAT(UCSR, XCONCAT(HOST_USART, A))
    #define HOST_UCSRB XCONCAT(UCSR, XCONCAT(HOST_USART, B))
    #define HOST_UDR XCONCAT(UDR, HOST_USART)
  #endif
  // UBRR is more complex and needs a special case
  #ifndef UBRR0
    // For older AVRs use only low 8 bits of UBRR because low and high parts are not adjacent
    #ifdef UBRRL
      #define HOST_UBRR UBRRL
    #else
      #define HOST_UBRR XCONCAT(UBRR, XCONCAT(HOST_USART, L))
    #endif
  #else
    // This is a more modern part; we can use 16 bit UBRR
    #define HOST_UBRR XCONCAT(UBRR, HOST_USART)
  #endif
#endif

// Some classic AVRs have a single flags register for timers 0 and 1 (TIFR0 and TIFR1 are merged in a single register TIFR)
#ifndef XAVR
  #ifndef TIFR1
    #define TIFR1 TIFR
  #endif
#endif


#ifdef XAVR
  #define TIMER_ON TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm
  #define TIMER_OFF TCA_SINGLE_CLKSEL_DIV256_gc
  #define TIMEOUT_REG TCA0.SINGLE.INTFLAGS
  #define TIMER_CONTROL_REG TCA0.SINGLE.CTRLA
  #define TIMER_COUNT_REG TCA0.SINGLE.CNT
  #define TIMER_HOST_MAX TCA0.SINGLE.CMP0
  #define TIMER_TARGET_MAX TCA0.SINGLE.CMP1
  #define WAIT_FOR_HOST TCA_SINGLE_CMP0_bm
  #define WAIT_FOR_TARGET TCA_SINGLE_CMP1_bm
  #define TIMER_RESET_REG TCA0.SINGLE.CTRLESET
  #define TIMER_RESET_CMD TCA_SINGLE_CMD_RESTART_gc
#else
  #define TIMER_ON 0x04
  #define TIMER_OFF 0x00
  #define TIMEOUT_REG TIFR1
  #define TIMER_CONTROL_REG TCCR1B
  #define TIMER_COUNT_REG TCNT1
  #define TIMER_HOST_MAX OCR1A
  #define TIMER_TARGET_MAX OCR1B
  #define WAIT_FOR_HOST (1<<OCF1A)
  #define WAIT_FOR_TARGET (1<<OCF1B)
#endif


#ifndef F_CPU
  #warning "F_CPU not defined, assuming 16MHz"
  #define F_CPU 16000000U
#endif

// TIMEOUTS
// HOST_TIMEOUT =~250ms
// TARGET_TIMEOUT=~100ms
// Timeout timer is initialized to use /256 divisor
#define HOST_TIMEOUT (F_CPU / 256.0 * 0.25)
#define TARGET_TIMEOUT (F_CPU / 256.0 * 0.1)

#ifndef UPDI_BAUD
  #define UPDI_BAUD 225000U    // (max 225000 min approx. F_CPU/100)
#endif

/*
 * Available UPDI I/O types are:
 *
 * 1 - timer sofware UART:      Compatible only with Mega328P and other AVRs with identical 8 bit timer 0.
 *                              Only the OC0A pin can be used for UPDI I/O. Slightly faster upload speed for a given UPDI_BAUD value.
 *
 * 2 - bitbang software UART:   Compatible with many chips and broad choice of UPDI pins are selectable.
 *                              Slightly slower upload speed for a given UPDI_BAUD value. Download speed is the same.
 */
#ifndef UPDI_IO_TYPE
  #define UPDI_IO_TYPE 2
#endif

// Flash constants class
#if defined XAVR
  template <typename T>
  using FLASH = const T;
#else
# include <avr/pgmspace.h>
# define FLASH const PROGMEM flash

  template <typename T>
  class flash {
    private:
    const T data;

    public:
    // normal constructor
    constexpr flash (T _data) : data(_data) {}
    // default constructor
    constexpr flash () : data(0) {}

    operator T() const {
      switch (sizeof(T)) {
        case 1: return pgm_read_byte(&data);
        case 2: return pgm_read_word(&data);
        case 4: return pgm_read_dword(&data);
      }
    }
  };
#endif

#if defined XAVR
  // Note: adapted from MicroChip appnote TB3216 "Getting Started with USART"
  constexpr unsigned int baud_reg_val(unsigned long baud) {
    return  (F_CPU * 64.0) / (16.0 * baud) + 0.5;
  }
#else
  constexpr unsigned int baud_reg_val(unsigned long baud) {
    return F_CPU/(baud * 8.0) - 0.5;
  }
#endif

namespace SYS {
  /*
  Timeout mechanisms, 5/2020, Spence Konde
  */
  inline uint8_t checkTimeouts() {
    return TIMEOUT_REG;
  }
  inline void clearTimeouts() {
    TIMEOUT_REG=WAIT_FOR_HOST|WAIT_FOR_TARGET;
  }
  inline void startTimer(void) __attribute__((always_inline));
  inline void startTimer(void) {
    #ifdef TIMER_RESET_REG //some timers have a reset register.
    TIMER_RESET_REG=TIMER_RESET_CMD;
    #else
    TIMER_COUNT_REG=0;
    #endif
    TIMER_CONTROL_REG=TIMER_ON;
  }
  inline void stopTimer(void) __attribute__((always_inline));
  inline void stopTimer(void){ TIMER_CONTROL_REG=TIMER_OFF; }
  void init(void);
  void setLED(void);
  void clearLED(void);
  void setVerLED(void);
  void clearVerLED(void);
}

#endif /* SYS_H_ */
