/*
 * updi_io.cpp
 *
 * Created: 18-11-2017 10:36:54
 *  Author: JMR_2
 */ 

// Includes
#include <avr/io.h>
#include "updi_io.h"

// Defines
#define F_CPU 16000000U
#define BIT_RATE 250000U // (max 570000)
#define BIT_TIME (F_CPU/BIT_RATE)
//#define _DEBUG

// Functions
/* Sends regular characters through the UPDI link */
int UPDI_io::put(char c) {
	/* Wait for end of stop bits */
	wait_for_bit();
	stop_timer();
	/* Send start bit */
	OCR0A = BIT_TIME - 1;
	TCNT0 = BIT_TIME - 2;
	setup_bit_low();
	start_timer();
	/* Enable TX output */
	DDRD |= (1 << DDD6);
	/* Calculate parity */
	uint8_t parity;		//get_parity(c);
	parity = 0;
	/* If we can be sure an overflow has happened by now due to instruction latency, */
	/* no more wait is needed and we only need to clear overflow flag */
	//wait_for_bit();
	TIFR0 = (1 << OCF0A);
	/* Send data bits */
	for (uint8_t i = 1; i; i <<= 1) {
		if (c & i)	{		// Current bit is 1
			setup_bit_high();
			parity = ~parity;
		}
		else				// Current bit is 0
			setup_bit_low();
		wait_for_bit();
	}
	/* Send parity bit */
	if (parity)
		setup_bit_high();
	else
		setup_bit_low();
	wait_for_bit();
	/* Send stop bits */
	setup_bit_high();
	wait_for_bit();
	OCR0A = 2 * BIT_TIME - 1;		// 2 bits
	/* Ready for RX input, but high due to pull-up */
	DDRD &= ~(1 << DDD6);
	return c;
	//return EOF;
}

/* Sends special sequences through the UPDI link */
int UPDI_io::put(ctrl c)
{
	/* This nested function expects the timer output to just have gone low */
	/* It waits for 12 minimum baud bit times (break character) then goes high */
	auto break_pulse = [] {
		TCCR0B = 4;
		OCR0A = 127;
		for (uint8_t i = 0; i < 11; i++) wait_for_bit();
		setup_bit_high();
		wait_for_bit();
		DDRD &= ~(1 << DDD6);
	};
	
	stop_timer();
	/* Send falling edge */
	OCR0A = BIT_TIME - 1;
	TCNT0 = BIT_TIME - 2;
	setup_bit_low();
	start_timer();
	/* Enable TX output */
	DDRD |= (1 << DDD6);
	/* clear overflow flag */
	TIFR0 = (1 << OCF0A);
	switch (c) {
		case double_break:
			break_pulse();
			setup_bit_low();
			wait_for_bit();
			DDRD |= (1 << DDD6);	
		case single_break:
			break_pulse();
			wait_for_bit();	
			break;
		case enable:
		/*
			TCCR0A = 0;
			DDRD |= (1 << DDD6);
			PORTD &= ~(1 << DDD6);
			__builtin_avr_nops(5);
			PORTD |= (1 << DDD6);
			DDRD &= ~(1 << DDD6);
			setup_bit_high();
			break;
		*/
		default:
			break;
	}
	OCR0A = BIT_TIME - 1;
	TCNT0 = BIT_TIME - 2;
	start_timer();
	return 0;
}

int UPDI_io::get() {
	stop_timer();
	/* Wait for middle of start bit */
	OCR0A = BIT_TIME / 2 - 1;
	TCNT0 = 12;		// overhead time; needs to be calibrated
	/* Make sure overflow flag is reset */
	TIFR0 = (1 << OCF0A);
	
	/* Must disable pull-up, because the UPDI UART just sends very short output pulses at the beginning of each bit time. */
	/* If pull up is enabled, there will be a drift to high state that results in erroneous input sampling. */
	/* As a side effect, random electrical fluctuations of the input prevent an infinite wait loop */
	/* in case no target is connected. */
	PORTD &= ~(1 << PIND6);
	/* Wait for start bit */
	loop_until_bit_is_clear(PIND, PIND6);

	start_timer();
	wait_for_bit();
#	ifdef _DEBUG
	/* Timing pulse */
	PIND |= (1 << PIND7);
	PIND |= (1 << PIND7);
#	endif // _DEBUG
	/* Setup sampling time */
	OCR0A = BIT_TIME - 1;
	/* Sample bits */
	uint8_t c;
	for (uint8_t i = 0; i < 8; i++) {
		wait_for_bit();
		/* Take sample */
		c = (c >> 1) | ((uint8_t) ((PIND & (1 << PIND6)) << 1));		// The cast is to prevent promotion to 16 bit
#		ifdef _DEBUG
		/* Timing pulse */
		PIND |= (1 << PIND7);
		PIND |= (1 << PIND7);
#		endif // _DEBUG
	}
	/* To Do Sample Parity */
	wait_for_bit();
#	ifdef _DEBUG
	/* Timing pulse */
	PIND |= (1 << PIND7);
	PIND |= (1 << PIND7);
#	endif // _DEBUG
	OCR0A = 2 * BIT_TIME + BIT_TIME / 2 - 1;		// 2.5 bits
	/* Return as soon as high parity or stop bits start */
	loop_until_bit_is_set(PIND, PIND6);
	/* Re-enable pull up */
	PORTD |= (1 << PIND6);
	return c;
}

void UPDI_io::init(void)
{
#	ifdef _DEBUG
	/* For RX timing measurement and debugging, make PD7 output */
	DDRD |= (1 << DDD7);
#	endif // _DEBUG
	setup_bit_high();
	/* initialize counter to near terminal count */
	TCNT0 = BIT_TIME - 2;
	/* initialize OCR0A to 200k counts per second */
	OCR0A = BIT_TIME - 1;
	start_timer();
}

inline void UPDI_io::setup_bit_low() {
	/* OC0A will go low on match with OCR0A */
	/* Also, set CTC mode - reset timer on match with OCR0A */
	TCCR0A = (1 << COM0A1) | (0 << COM0A0) | (1 << WGM01);
}

inline void UPDI_io::setup_bit_high() {
	/* OC0A will go high on match with OCR0A */
	/* Also, set CTC mode - reset timer on match with OCR0A */
	TCCR0A = (1 << COM0A1) | (1 << COM0A0) | (1 << WGM01);
}

inline void UPDI_io::wait_for_bit() {
	/* Wait for compare match */
	loop_until_bit_is_set(TIFR0, OCF0A);
	TIFR0 = (1 << OCF0A);
}

inline void UPDI_io::stop_timer() {
	TCCR0B = 0;
}

inline void UPDI_io::start_timer() {
	TCCR0B = 1;
}

/*
inline uint8_t UPDI_io::get_parity(uint8_t c) {
	asm(
	"mov	r0, %0	\n"
	"swap	r0		\n"
	"eor	%0, r0	\n"
	"mov	r0, %0	\n"
	"lsr	r0		\n"
	"lsr	r0		\n"
	"eor	%0, r0	\n"
	"mov	r0, %0	\n"
	"lsr	r0		\n"
	"eor	%0, r0	\n"
	:"+r" (c)
	:
	:"r0"
	);
	return c & 0x01;
}
*/
