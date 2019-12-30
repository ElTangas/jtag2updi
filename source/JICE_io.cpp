/*
 * stk_io.cpp
 *
 * Created: 18-11-2017 15:20:29
 *  Author: JMR_2
 */ 

// Includes
#include <Arduino.h>
//#include <avr/io.h>
#include "JICE_io.h"
#include "sys.h"

namespace {
	// *** Baud rate lookup table for UBRR0 register ***
	// Indexed by valid values for PARAM_BAUD_RATE_VAL (defined in JTAG2.h)
	FLASH<uint16_t> baud_tbl[8] = {baud(2400), baud(4800), baud(9600), baud(19200), baud(38400), baud(57600), baud(115200), baud(14400)};
}

// Functions
uint8_t JICE_io::put(char c) {
#ifdef __AVR_ATmega16__
	loop_until_bit_is_set(UCSRA, UDRE);
	return UDR = c;
#elif __AVR_ATmega32U4__
  SERIALCOM.write(c); //test 32U4
  return c;           //test 32U4
#else
	loop_until_bit_is_set(UCSR0A, UDRE0);
	return UDR0 = c;
#endif
}

uint8_t JICE_io::get(void) {
#ifdef __AVR_ATmega16__
	loop_until_bit_is_set(UCSRA, RXC); /* Wait until data exists. */
	return UDR;
#elif __AVR_ATmega32U4__
  //while (!Serial.available()); //test 32U4
  uint8_t c = SERIALCOM.read();     //test 32U4
  return c;                      //test 32U4
#else
	loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
	return UDR0;
#endif
}

void JICE_io::init(void) {
#ifdef __AVR_ATmega16__
	/* Set double speed */
	UCSRA = (1<<U2X);
	/* Set initial baud rate */
	UBRRH = 0;
	UBRRL = baud(19200);
	/* Enable receiver and transmitter */
	UCSRB = (1<<RXEN)|(1<<TXEN);
	/* Set frame format: 8data, 1stop bit */
	UCSRC = (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
#elif __AVR_ATmega32U4__
  //SERIALCOM.begin(115200);   //test 32U4 - baudrate irrelevant
  SERIALCOM.begin(19200);   //test 32U4 - baudrate irrelevant
#else
	/* Set double speed */
	UCSR0A = (1<<U2X0);
	/* Set initial baud rate */
	UBRR0 = baud(19200);
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);
#endif
}

void JICE_io::flush(void) {
#ifdef __AVR_ATmega16__
	UCSRA |= 1 << TXC;
	loop_until_bit_is_set(UCSRA, TXC);
#elif __AVR_ATmega32U4__
  SERIALCOM.flush();    //test 32U4
#else
	UCSR0A |= 1 << TXC0;
	loop_until_bit_is_set(UCSR0A, TXC0);
#endif
}

void JICE_io::set_baud(JTAG2::baud_rate rate) {
#ifdef __AVR_ATmega16__
	UBRRH = 0;
	UBRRL = baud_tbl[rate - 1];
#elif __AVR_ATmega32U4__
  true; //test 32U4
#else
	UBRR0 = baud_tbl[rate - 1];
#endif
}
