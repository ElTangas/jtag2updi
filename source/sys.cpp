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
#	if defined HW_SERIAL
// skip all init, UART used
# 	elif defined XTINY
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
#	endif	//HW_SERIAL, XTINY	
#   ifndef HW_SERIAL
		/* Enable all UPDI port pull-ups */
		PORT(UPDI_PORT) = 0xFF;
#	endif
		/* Enable LED */
		//PORT(LED_PORT) |= (1 << LED_PIN);
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
#endif  //__AVR_ATmega16__

/*
 * Added for testing purposes
*/
SYS::LED_blink(0, 2, 300);
SYS::LED_blink(1, 2, 300);
SYS::LED_blink(2, 2, 300);
SYS::LED_blink(3, 2, 300);
SYS::LED_blink(4, 2, 300);
SYS::LED_blink(5, 2, 300);

SYS::LED_blink(0, 1, 100);
SYS::LED_blink(1, 1, 100);
SYS::LED_blink(2, 1, 100);
SYS::LED_blink(3, 1, 100);
SYS::LED_blink(4, 1, 100);
SYS::LED_blink(5, 1, 100);
/**/

}

void SYS::setLED(void){
	PORT(LED_PORT) |= 1 << LED_PIN;	
}

void SYS::clearLED(void){
	PORT(LED_PORT) &= ~(1 << LED_PIN);	
}

const int LED[] = {6, 3, 5, 9, 10, 14};
//const int blinklength_ms = 1000;
//unsigned long milli = 0UL;
void SYS::LED_blink (int led_no, int led_blinks, int length_ms) {
  for (int i=0; i <led_blinks; i++) {
    digitalWrite(LED[led_no], HIGH);   // set the RX LED ON
    delay(length_ms);
    digitalWrite(LED[led_no], LOW);   // set the RX LED ON
    delay(length_ms);
  }
}
