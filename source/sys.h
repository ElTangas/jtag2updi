/*
 * sys.h
 *
 * Created: 02-10-2018 13:07:18
 *  Author: JMR_2
 */ 

#ifndef SYS_H_
#define SYS_H_

#include<avr/io.h>
#include<Arduino.h> //for recognizing HW_SERIAL

//#warning "modify this to match your USB serial port name"
#define SERIALCOM Serial

// default UART is Serial (HAVE_SERIAL), look for additional ones
// see if additional HW-SERIAL is available, take biggest for UDPI as default
#if defined(HAVE_HWSERIAL3)
#  define HW_SERIAL Serial3
#elif defined(HAVE_HWSERIAL2)
#  define HW_SERIAL Serial2
#elif defined(HAVE_HWSERIAL1)
#  define HW_SERIAL Serial1
#endif

//#define HW_SERIAL Serial1

// if HW_SERIAL exists change UDPI mode
#if defined(HW_SERIAL)
#    define UPDI_IO_TYPE 3
#endif

// See if we are compiling for an UPDI chip (xmega3 core)
#if __AVR_ARCH__ == 103
# define XTINY
#endif

// Auxiliary Macros
#define CONCAT(A,B) A##B
#if defined XTINY
#	define PIN(x) CONCAT(VPORT,x).IN
#	define PORT(x) CONCAT(VPORT,x).OUT
#	define DDR(x) CONCAT(VPORT,x).DIR
#else
#	define PIN(x) CONCAT(PIN,x)
#	define PORT(x) CONCAT(PORT,x)
#	define DDR(x) CONCAT(DDR,x)
#endif

// Configuration for AVR with additional UART - only LED pin needed, UDPI via UART
#if defined(HW_SERIAL)
// Leonardo / ProMicro PortB, Pin5 = D9
#  ifndef LED_PORT
#    define LED_PORT B
#  endif

# ifndef LED_PIN
#   define LED_PIN 5
# endif
// Configuration for Arduino Mega
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#	ifndef UPDI_PORT
#		define UPDI_PORT D
#	endif

#	ifndef UPDI_PIN
#		define UPDI_PIN 3
#	endif

#	ifndef LED_PORT
#		define LED_PORT B
#	endif

#	ifndef LED_PIN
#		define LED_PIN 7
#	endif

#	ifndef UPDI_IO_TYPE
#		define UPDI_IO_TYPE 2
#	endif

// Configuration for AVR-0/1
#elif defined XTINY
#	ifndef UPDI_PORT
#		define UPDI_PORT B
#	endif

#	ifndef UPDI_PIN
#		define UPDI_PIN 0
#	endif

#	ifndef LED_PORT
#		define LED_PORT B
#	endif

#	ifndef LED_PIN
#		define LED_PIN 1
#	endif

#	ifndef UPDI_IO_TYPE
#		define UPDI_IO_TYPE 2
#	endif
//	These are currently used only for AVR-0/1 chips
//	Select which USART peripheral is being used for host PC communication
//	Also, indicate the port/pin of the HOST_USART Tx pin
#	ifndef HOST_USART
#		define HOST_USART USART0
#	endif

#	ifndef HOST_TX_PORT
#		define HOST_TX_PORT B
#	endif

#	ifndef HOST_TX_PIN
#		define HOST_TX_PIN 2
#	endif

// Default configuration (suitable for ATmega 328P and similar devices @16MHz)
#else
#	ifndef UPDI_PORT
#		define UPDI_PORT D
#	endif

#	ifndef UPDI_PIN
#		define UPDI_PIN 6
#	endif

#	ifndef LED_PORT
#		define LED_PORT B
#	endif

#	ifndef LED_PIN
#		define LED_PIN 5
#	endif
#endif


#ifndef F_CPU
#	define F_CPU 16000000U
#endif

#ifndef UPDI_BAUD
#	define UPDI_BAUD 225000U	// (max 225000 min approx. F_CPU/100)
#endif

/*
 * Available UPDI I/O types are:
 *
 * 1 - timer sofware UART:		Compatible only with Mega328P and other AVRs with identical 8 bit timer 0.
 *								Only the OC0A pin can be used for UPDI I/O. Slightly faster upload speed for a given UPDI_BAUD value.
 *
 * 2 - bitbang software UART:	Compatible with many chips and broad choice of UPDI pins are selectable.
 *								Slightly slower upload speed for a given UPDI_BAUD value. Download speed is the same.
 */
#ifndef UPDI_IO_TYPE
#	define UPDI_IO_TYPE 2
#endif

// Flash constants class
#if defined XTINY
	template <typename T>
	using FLASH = const T;
#else
#	include <avr/pgmspace.h>
#	define FLASH const PROGMEM flash

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

#if defined XTINY
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
	void init(void);
	void setLED(void);
	void clearLED(void);
  void LED_blink (int led_no, int led_blinks, int length_ms);
}

#endif /* SYS_H_ */
