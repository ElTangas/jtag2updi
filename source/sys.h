/*
 * sys.h
 *
 * Created: 02-10-2018 13:07:18
 *  Author: JMR_2
 */ 

#ifndef SYS_H_
#define SYS_H_

// Auxiliary Macros
#define CONCAT(A,B) A##B
#define PIN(x) CONCAT(PIN,x)
#define PORT(x) CONCAT(PORT,x)
#define DDR(x) CONCAT(DDR,x)

// Configuration for Arduino Mega
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
# ifndef UPDI_PORT
#  define UPDI_PORT D
# endif

# ifndef UPDI_PIN
#  define UPDI_PIN 3
# endif

# ifndef LED_PORT
#  define LED_PORT B
# endif

# ifndef LED_PIN
#  define LED_PIN 7
# endif

# ifndef UPDI_IO_TYPE
#  define UPDI_IO_TYPE 2
# endif
#endif


// Default configuration (suitable for ATmega 328P and similar devices @16MHz)
#ifndef UPDI_PORT
#define UPDI_PORT D
#endif

#ifndef UPDI_PIN
#define UPDI_PIN 6
#endif

#ifndef LED_PORT
#define LED_PORT B
#endif

#ifndef LED_PIN
#define LED_PIN 5
#endif

#ifndef F_CPU
#define F_CPU 16000000U
#endif

#ifndef UPDI_BAUD
#define UPDI_BAUD 225000U	// (max 225000 min approx. F_CPU/100)
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
#define UPDI_IO_TYPE 1		
#endif

// Flash constants class
#include <avr/pgmspace.h>
#define FLASH const PROGMEM flash

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

#ifndef F_CPU
#define F_CPU 16000000U
#endif

constexpr unsigned int baud(unsigned long b) {
	return F_CPU/(b*8.0) - 0.5;
}

namespace SYS {
	void init(void);
	void setLED(void);
	void clearLED(void);
}

#endif /* SYS_H_ */