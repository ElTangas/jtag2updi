/*
 * dbg.h
 *
 * Created:5/11/2020
 * Author: Spence Konde  (tindie.com/stores/drazzy)
 * github.com/SpenceKonde
 */

#ifndef DBG_H_
#define DBG_H_

#include <avr/io.h>
#include "sys.h"


/* User Configuration goes up here - the rest down below is defaults that you can override
   or stuff that is hardware specific   */

// Uncomment these to enable debug output recording every time one of these UPDI commands is sent and (for load commands) the response.
// note that this will have NO EFFECT if you have not defined either USE_SPIDEBUG or USE_USARTDEBUG and any required parameters!

//#define DEBUG_STCS
//#define DEBUG_LDCS
//#define DEBUG_REP
//#define DEBUG_STS
//#define DEBUG_STPTR
//#define DEBUG_LDS
//#define DEBUG_KEY
//#define DEBUG_RESET



#if defined(__AVR_ATtiny_Zero_One__)
  // tinyAVR 0-series and 1-series parts
  // 3216, 1604, that kind of thing

  #define LED_PORT A
  #define LED_PIN 7

  // Second LED is used to indicate NVM version, or as an additional debugging aid.
  #define LED2_PORT A
  #define LED2_PIN 6


  //USARTDEBUG not practical here because only one UART.



#elif defined( __AVR_ATmega_Mighty__ )
  // For ATmega16, ATmega32, and the later x4 parts (up to the 1284P).
  // On the ones with two USARTS, you can use USART debugging.
  //
  // Here USART debug output is an option too
  // at least on parts with the second USART
  // You should be able to hit 2MBaud with F_CPU=16000000 and DEBUG_BAUDVAL 0 or 1mbaud with 1 if your serial adapter can't handle that.
  // At 20, though, on classic AVRs, you're out of luck unless you have an adapter that can do 2.5 mbaud...have to go all the way down to 500kbaud (DEBUG_BAUDVAL 4) to divide it to something common.. Good thing almost everyone runs them at 16.





#elif defined (__AVR_ATmega_Mini__) || defined(ARDUINO_AVR_LARDU_328E)
  // For ATmega328 and 168 (P, PB) parts
  // Same deal as before/ Remember that the buildin LED is on the same pin as SCK on most boards, so you'll want to cadd LEDs on other pinn, defaults to D2
  // On PB parts can also use second USART. Using the second SPI is not supported.



#elif defined (__AVR_ATmega_Mega__)
  // 2560 and that family, like the ones used on the Arduino Mega
  // Same as the others with extra USARTS, only here you can specify which one if you must


#elif defined (__AVR_ATmega_Zero__)
// 4808, 4809. and the rest of the megaAVR 0-series
// Same as above, pretty much
// big difference here is your specify the name of the peripheral instead of the number, and the target baud rate, because we grab the OSCCAL value per datasheet./

#endif


/* Defaults and hardware-specific stuff                 */
/* Shouldn't need to change anything here               */
/* IF you want to override, copy it to the blocks above */


#if defined(__AVR_ATtiny_Zero_One__)
// tinyAVR 0-series and 1-series parts HOST_RX_PIN 3

  #ifdef USE_SPIDEBUG

      #define SPIDEBUG SPI0

      //SPI0 is on PORTA
      #define SPIPORT A
      //PA1, PA3, PA4 are MOSI,SCK,SS on XTINY parts with more than 8 pins, and the ones with 8 pins don't have enough ram to fit the device descriptor so we don't care about them.
      //On the megasAVR 0-series and AVR DA-series, this would be 0x50 but otherise everything same I think.. not that people are lining up to use those parts as boring programmers!
      #define MOSIPIN 1
      #define SCKPIN 3

    #ifndef SSPIN
    //They get option to change this because they may want to use a different pin,m and they an do that on the parts with the new peripherals that have the SSD bit.
      #define SSPIN 4
    #endif

  #endif //emd of SPIDEBUG section for XTINY

  #ifdef USE_USARTDEBUG
    #error "Only has one USART!"
  #endif

#elif defined( __AVR_ATmega_Mighty__ )
// For ATmega16, ATmega32, and the later x4 parts (up to the 1284P).


  #ifdef USE_SPIDEBUG

    #define SPIPORT B

    #define MOSIPIN 1
    #define SCKPIN 3
    #define SSPIN 4
  #endif

  #ifdef USE_USARTDEBUG
    #ifdef UCSR1A
      #if (!defined(DEBUG_USART)
        #define DEBUG_USART 1
      #elif (!(DEBUG_USART==1)
        #undef DEBUG_USART
        #define DEBUG_USART 1
      #endif
    #else
      #error "That part doesn't have another USART"
    #endif
  #endif


#elif defined (__AVR_ATmega_Mini__) || defined(ARDUINO_AVR_LARDU_328E)
  // For ATmega328 and 168 (P, PB) parts

  #ifdef USE_SPIDEBUG

    #define SPIPORT B

    #define MOSIPIN 3
    #define SCKPIN 5
    #define SSPIN 2

  #endif

  #ifdef USE_USARTDEBUG
    #ifdef UCSR1A
      #if (!defined(DEBUG_USART)
        #define DEBUG_USART 1
      #elif (!(DEBUG_USART==1)
        #undef DEBUG_USART
        #define DEBUG_USART 1
      #endif
    #else
      #error "That part doesn't have another USART"
    #endif
  #endif




#elif defined (__AVR_ATmega_Mega__)
// 2560 and that family, like the ones used on the Arduino Mega

  #ifdef USE_SPIDEBUG

    #define SPIPORT D

    #define MOSIPIN 2
    #define SCKPIN 1
    #define SSPIN 0

  #endif

  #ifdef USE_USARTDEBUG

    #if (!defined(DEBUG_USART)
      #define DEBUG_USART 1
    #endif

    #if (!(DEBUG_USART==1||DEBUG_USART==2||DEBUG_USART==3)
      #undef DEBUG_USART
      #define DEBUG_USART 1
    #endif
  #endif

#elif defined (__AVR_ATmega_Zero__)
// 4808, 4809. and the rest of the megaAVR 0-series

  #ifdef USE_SPIDEBUG

    #define SPIDEBUG SPI0

    //SPI0 is on PORTA
    #define SPIPORT A
    //PA4, PA6, PA7 are MOSI,SCK,SS on megaAVR 0-series
    #define MOSIPIN 4
    #define SCKPIN 6

    #ifndef SSPIN
      #define SSPIN 7
    #endif

  #endif //emd of SPIDEBUG
  #ifdef USE_USARTDEBUG
    #define DEBUG_TX_PIN 0
    #define DEBUG_RX_PIN 1
    #if (!defined(DEBUG_USART)
      #define DEBUG_USART USART1
    #endif
    #if DEBUG_USART==USART1
      #define DEBUG_TX_PORT C
    #elif DEBUG_USART==USART2
      #define DEBUG_TX_PORT F
    #elif DEBUG_USART==USART3
      #define DEBUG_TX_PORT B
    #elif DEBUG_USART==USART0
      #define DEBUG_TX_PORT A
    #else
      #undef DEBUG_USART
      #define DEBUG_USART USART1
      #define DEBUG_TX_PORT C
    #endif
  #endif //end if USARTDEBUG
#else
  #warning "Part not supported - if you didn't provide all the needed pin definitions, that's why it's not compiling"
#endif //End of the defaults!


// Noe we do some general stuff relating the the debugging so they share most of the code...
#if (defined(USE_SPIDEBUG) && defined(USE_USARTDEBUG))
  #error "Cannot use both SPI and UART for debugging, pick one"
#endif

#ifdef USE_SPIDEBUG
  #define DEBUG_ON
  #ifdef XAVR
    #define DEBUGDATA SPIDEBUG.DATA
    #define DEBUGFLAGS SPIDEBUG.INTFLAGS
    #define DEBUGSENDNOW SPI_IF_bp
  #else
    #ifndef SPDR
    //means it's one of the ones woth two of them, 0 and 1...
      #define SPIF SPIF0
      #define MSTR MSTR0
      #define WDE WDE0
      #if (SPIDEBUG==1)
        #define SPDR SPDR1
        #define SPSR SPSR1
        #define SPCR SPCR1
      #else
        #define SPDR SPDR0
        #define SPSR SPSR0
        #define SPCR SPCR0
      #endif
    #endif  //now it has standard names definedm,= so we define them to match the harmonized names...
    #define DEBUGDATA SPDR
    #define DEBUGFLAGS SPSR
    #define DEBUGSENDNOW SPIF
  #endif
#endif

#ifdef USE_USARTDEBUG
  #define DEBUG_ON
  #ifdef XAVR
    #if (HOST_USART == DEBUG_USART)
      #error "Host and Debug cannot be on same USART"
    #endif
    #define DEBUGDATA DEBUG_USART.TXDATAL
    #define DEBUGBAUDREG DEBUG_USART.BAUD
    #define DEBUGFLAGS DEBUG_USART.STATUS
    #define DEBUGSENDNOW USART_DREIF_bp
    #ifndef DEBUG_BAUDRATE
      #define DEBUG_BAUDRATE 2000000UL
    #endif
  #else
    #define DEBUGSENDNOW UDRE0
    #if DEBUG_USART==1
      #define DEBUGDATA UDR1
      #define DEBUGFLAGS UCSR1A
      #define DEBUG_UCSRB UCSR1B
      #define DEBUGBAUDREG UBRR1
    #elif DEBUG_USART==2
      #define DEBUGDATA UDR2
      #define DEBUGFLAGS UCSR2A
      #define DEBUG_UCSRB UCSR2B
      #define DEBUGBAUDREG UBRR2
    #elif DEBUG_USART==3
      #define DEBUGDATA UDR3
      #define DEBUGFLAGS UCSR3A
      #define DEBUG_UCSRB UCSR3B
      #define DEBUGBAUDREG UBRR3
    #elif DEBUG_USART==0
      #error "Using USART other than USART0 to communicate with host not supported on classic AVR"
    #else
      #define DEBUG_USART 1
      #define DEBUGDATA UDR1
      #define DEBUGFLAGS UCSR1A
      #define DEBUG_UCSRB UCSR1B
      #define DEBUGBAUDREG UBRR1
    #endif
  #endif
#endif

#if defined(DEBUG_ON)

namespace DBG {
  void initDebug(void);
  void updi_st_ptr_l(uint32_t address);
  void updi_st_ptr_w(uint16_t address);
  void updi_lds(uint32_t address);
  void updi_lds(uint16_t address);
  void updi_sts(uint32_t address, uint16_t data);
  void updi_sts(uint32_t address, uint8_t data);
  void updi_sts(uint16_t address, uint16_t data);
  void updi_sts(uint16_t address, uint8_t data);
  void updi_key();
  void updi_rep(uint8_t reps);
  void updi_stcs(uint8_t command, uint8_t data);
  void updi_ldcs(uint8_t command);
  void updi_res(uint32_t data, uint8_t isaddr=1);
  void updi_res(uint16_t data, uint8_t isaddr=0);
  void updi_res(uint8_t data);
  void updi_reset();
  void updi_reset_off();
  void updi_reset_on();
  void updi_post_reset(uint8_t mode);
  void debug(const char *str, uint8_t newline=1);
  void debug(const uint8_t *data, size_t length, uint8_t newline=0, uint8_t ashex=0);
  void debugWriteStr(const char *str);
  void debugWriteBytes(const uint8_t *data, size_t length, uint8_t ashex=0);
  void debugWriteByte(uint8_t databyte);
  void debugWriteHex(uint8_t databyte);
  void debug(char prefix, uint8_t data0);
  void debug(char prefix, uint8_t data0, uint8_t data1);
  void debug(char prefix, uint8_t data0, uint8_t data1, uint8_t data2);
}
#else
// if debug is not on, we need to deal with the specific DEBUG_(subsystem) defines. Rather than make a mess of everywhere that checks for them by adding another check for DEBUG_ON
// we'll just undefine them....
#ifdef DEBUG_STCS
  #warning "No debug output method is configured, so DEBUG_STCS being undefined"
  #undef DEBUG_STCS
#endif

#ifdef DEBUG_LDCS
  #warning "No debug output method is configured, so DEBUG_LDCS being undefined"
  #undef DEBUG_LDCS
#endif

#ifdef DEBUG_STS
  #warning "No debug output method is configured, so DEBUG_STS being undefined"
  #undef DEBUG_STS
#endif

#ifdef DEBUG_LDS
  #warning "No debug output method is configured, so DEBUG_LDS being undefined"
  #undef DEBUG_LDS
#endif

#ifdef DEBUG_STPTR
  #warning "No debug output method is configured, so DEBUG_STPTR being undefined"
  #undef DEBUG_STPTR
#endif

#ifdef DEBUG_KEY
  #warning "No debug output method is configured, so DEBUG_KEY being undefined"
  #undef DEBUG_KEY
#endif

#ifdef DEBUG_REP
  #warning "No debug output method is configured, so DEBUG_REP being undefined"
  #undef DEBUG_REP
#endif

#ifdef DEBUG_RESET
  #warning "No debug output method is configured, so DEBUG_RESET being undefined"
  #undef DEBUG_RESET
#endif

namespace DBG {
	inline void initDebug(){}
	inline void updi_st_ptr_l(uint32_t address){}
	inline void updi_st_ptr_w(uint16_t address){}
	inline void updi_lds(uint32_t address){}
	inline void updi_lds(uint16_t address){}
	inline void updi_sts(uint32_t address, uint16_t data){}
	inline void updi_sts(uint32_t address, uint8_t data){}
	inline void updi_sts(uint16_t address, uint16_t data){}
	inline void updi_sts(uint16_t address, uint8_t data){}
	inline void updi_key(){}
	inline void updi_rep(uint8_t reps){}
	inline void updi_stcs(uint8_t command, uint8_t data){}
	inline void updi_ldcs(uint8_t command){}
	inline void updi_res(uint32_t data, uint8_t isaddr=1){}
	inline void updi_res(uint16_t data, uint8_t isaddr=0){}
	inline void updi_res(uint8_t data){}
	inline void updi_reset(){}
	inline void updi_reset_off(){}
	inline void updi_reset_on(){}
	inline void updi_post_reset(uint8_t mode){}
	inline void debug(const char *str, uint8_t newline=1){}
	inline void debug(const uint8_t *data, size_t length, uint8_t newline=0, uint8_t ashex=0){}
	inline void debugWriteStr(const char *str){}
	inline void debugWriteBytes(const uint8_t *data, size_t length, uint8_t ashex=0){}
	inline void debugWriteByte(uint8_t databyte){}
	inline void debugWriteHex(uint8_t databyte){}
	inline void debug(char prefix, uint8_t data0){}
	inline void debug(char prefix, uint8_t data0, uint8_t data1){}
	inline void debug(char prefix, uint8_t data0, uint8_t data1, uint8_t data2){}
}

#endif

#endif /* DBG_H_ */
