/*
 * sys.cpp
 *
 * Created: 02-10-2018 13:07:52
 *  Author: JMR_2
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "sys.h"

void SYS::init(void) {

#ifndef __AVR_ATmega16__

	/* Disable digital input buffers on port C */
	DIDR0 = 0x3F;
	/* Enable all UPDI port pull-ups */
    PORT(UPDI_PORT) = 0xFF;
	/* Enable all LED port pull-ups, except for the LED pin */
	PORT(LED_PORT) = 0xFF - (1 << LED_PIN);
	/* Disable unused peripherals */
	ACSR = 1 << ACD;		// turn off comparator

#else

        /* No interrupts */
        sei();
        /* Enable all UPDI port pull-ups */
        PORT(UPDI_PORT) = 0xFF;
        /* Enable LED */
        PORT(LED_PORT) |= (1 << LED_PIN);
        /* Enable all LED port pull-ups, except for the LED pin */
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
