/*
 * stk_io.cpp
 *
 * Created: 18-11-2017 15:20:29
 *  Author: JMR_2
 */ 

// Includes
#include <avr/io.h>
#include "JICE_io.h"
#include "sys.h"

namespace {
	// *** Baud rate lookup table for UBRR0 register ***
	// Indexed by valid values for PARAM_BAUD_RATE_VAL (defined in JTAG2.h)
	FLASH<uint16_t> baud_tbl[8] = {baud(2400), baud(4800), baud(9600), baud(19200), baud(38400), baud(57600), baud(115200), baud(14400)};
}

// Functions
uint8_t JICE_io::put(char c) {
	loop_until_bit_is_set(UCSR0A, UDRE0);
	return UDR0 = c;
}

uint8_t JICE_io::get(void) {
	loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
	return UDR0;
}

void JICE_io::init(void) {
	/* Set double speed */
	UCSR0A = (1<<U2X0);
	/* Set initial baud rate */
	UBRR0L = baud(19200);
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);
}

void JICE_io::flush(void) {
	UCSR0A |= 1 << TXC0;
	loop_until_bit_is_set(UCSR0A, TXC0);
}

void JICE_io::set_baud(JTAG2::baud_rate rate) {
	UBRR0 = baud_tbl[rate - 1];
}