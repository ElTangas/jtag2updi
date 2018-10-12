/*
 * sys.cpp
 *
 * Created: 02-10-2018 13:07:52
 *  Author: JMR_2
 */ 

#include <avr/io.h>
#include "sys.h"

#define LED_PORT B
#define LED_PIN 5

void SYS::init(void) {
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
}

void SYS::setLED(void){
	PORT(LED_PORT) |= 1 << LED_PIN;	
}

void SYS::clearLED(void){
	PORT(LED_PORT) &= ~(1 << LED_PIN);	
}