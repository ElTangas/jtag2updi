/*
 * stk_io.cpp
 *
 * Created: 18-11-2017 15:20:29
 *  Author: JMR_2
 */ 

// Includes
#include <Arduino.h>
#include <avr/io.h>
#include "JICE_io.h"
#include "sys.h"

namespace {
	// *** Baud rate lookup table for UBRR0 register ***
	// Indexed by valid values for PARAM_BAUD_RATE_VAL (defined in JTAG2.h)
	FLASH<uint16_t> baud_tbl[8] = {baud_reg_val(2400), baud_reg_val(4800), baud_reg_val(9600), baud_reg_val(19200), baud_reg_val(38400), baud_reg_val(57600), baud_reg_val(115200), baud_reg_val(14400)};
}

// Functions
uint8_t JICE_io::put(char c) {
#ifdef __AVR_ATmega16__
	loop_until_bit_is_set(UCSRA, UDRE);
	return UDR = c;
#elif defined XTINY
	loop_until_bit_is_set(HOST_USART.STATUS, USART_DREIF_bp);
	return HOST_USART.TXDATAL = c;
#elif defined __AVR_ATmega32U4__
  // wait for Serial to be active
  // while (!SERIALCOM);
	// commented out: timeout/error communicating with programmer (status -1) 
	SERIALCOM.write(c);
	return c;
#else
	loop_until_bit_is_set(UCSR0A, UDRE0);
	return UDR0 = c;
#endif
}

uint8_t JICE_io::get(void) {
#ifdef __AVR_ATmega16__
	loop_until_bit_is_set(UCSRA, RXC); /* Wait until data exists. */
	return UDR;
#elif defined XTINY
	loop_until_bit_is_set(HOST_USART.STATUS, USART_RXCIF_bp); /* Wait until data exists. */
	return HOST_USART.RXDATAL;
#elif defined __AVR_ATmega32U4__
	uint8_t c = SERIALCOM.read();
	return c;
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
	UBRRL = baud_reg_val(19200);
	/* Enable receiver and transmitter */
	UCSRB = (1<<RXEN)|(1<<TXEN);
	/* Set frame format: 8data, 1stop bit */
	UCSRC = (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
#elif defined XTINY
	// Init TxD pin (PA6 on tiny412)
	PORT(HOST_TX_PORT) |= 1 << HOST_TX_PIN;
	DDR(HOST_TX_PORT) |= 1 << HOST_TX_PIN;
	/* Set initial baud rate */
	HOST_USART.BAUD = baud_reg_val(19200);
	/* Enable receiver and transmitter */
	HOST_USART.CTRLB = USART_TXEN_bm | USART_RXEN_bm | USART_RXMODE_NORMAL_gc;
#elif defined __AVR_ATmega32U4__
	SERIALCOM.begin(19200);   //32U4 uses USB - baudrate irrelevant
	// wait for Serial to be active
	while (!SERIALCOM){ SYS::LED_blink(5, 1, 100);};
  //while (!SERIALCOM);

#else
	/* Set double speed */
	UCSR0A = (1<<U2X0);
	/* Set initial baud rate */
	UBRR0 = baud_reg_val(19200);
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
#elif defined XTINY
	HOST_USART.STATUS = 1 << USART_TXCIF_bp;
	loop_until_bit_is_set(HOST_USART.STATUS, USART_TXCIF_bp);
#elif defined __AVR_ATmega32U4__
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
#elif defined XTINY
	HOST_USART.BAUD = baud_tbl[rate - 1];
#elif defined __AVR_ATmega32U4__
	true; //test 32U4
#else
	UBRR0 = baud_tbl[rate - 1];
#endif
}
