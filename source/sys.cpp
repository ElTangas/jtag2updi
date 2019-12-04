/*
 * sys.cpp
 *
 * Created: 02-10-2018 13:07:52
 *  Author: JMR_2
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#include "sys.h"

void SYS::init(void) {

#ifndef __AVR_ATmega16__
#	if defined XTINY
		// Set clock speed to maximum (default 20MHz, or 16MHz set by fuse)
		_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0);
		/* Disable unused peripherals */
		//ToDo
#	else
#		if defined(ARDUINO_AVR_LARDU_328E)
		clock_prescale_set ( (clock_div_t) __builtin_log2(32000000UL / F_CPU));
#		endif
		/* Disable digital input buffers on port C */
		DIDR0 = 0x3F;
		/* Disable unused peripherals */
		ACSR = 1 << ACD;		// turn off comparator
#	endif		
		/* Enable all UPDI port pull-ups */
		PORT(UPDI_PORT) = 0xFF;
		/* Enable all LED port pull-ups, except for the LED pin */
		PORT(LED_PORT) = 0xFF - (1 << LED_PIN);
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
