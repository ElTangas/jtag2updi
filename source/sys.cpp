/*
 * sys.cpp
 *
 * Created: 02-10-2018 13:07:52
 *  Author: JMR_2
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "sys.h"

#ifndef LED_PORT
#define LED_PORT B
#endif
#ifndef LED_PIN
#define LED_PIN 5
#endif

void SYS::init(void) {

#ifndef __AVR_ATmega16__

	/* Disable digital input buffers on port C */
	DIDR0 = 0x3F;
	/* Enable all port D pull-ups */
	PORTD = 0xFF;
	/* Enable all port B pull-ups, except for LED */
	PORT(LED_PORT) = 0xFF - (1 << LED_PIN);
	/* Disable unused peripherals */
	ACSR = 1 << ACD;		// turn off comparator
	PRR =
		(1 << PRTWI) |		// turn off 2 wire interface
		(1 << PRTIM2) |		// turn off timer 2
		(1 << PRTIM1) |		// turn off timer 1
		(1 << PRSPI) |		// turn off SPI interface
		(1 << PRADC);		// turn off the ADC

#else

        /* No interrupts */
        sei();
        /* Enable all port D pull-ups */
        PORT(UPDI_PORT) = 0xFF;
        /* Enable LED */
        PORT(LED_PORT) |= (1 << LED_PIN);
        /* Enable all port B pull-ups, except for LED */
        PORT(LED_PORT) = 0xFF - (1 << LED_PIN);

        /* Disable unused peripherals */
        SPCR &= ~(1<<SPE);
        ADC  &= ~(1<<ADEN);
        TWCR &= ~(1<<TWEN);

        /* Disable resources after bootloader */
        TIFR   = 0x00;
        TIMSK  = 0x00;
        TCNT1  = 0x0000;
        OCR1A  = 0x0000;
        OCR1B  = 0x0000;
        TCCR1A = 0x0000;
        TCCR1B = 0x0000;

#endif

}

void SYS::setLED(void){
	PORT(LED_PORT) |= 1 << LED_PIN;	
}

void SYS::clearLED(void){
	PORT(LED_PORT) &= ~(1 << LED_PIN);	
}
