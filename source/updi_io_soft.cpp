/*
 * updi_io_soft.cpp
 *
 * Created: 11-08-2018 22:08:14
 *  Author: Cristian Balint <cristian dot balint at gmail dot com>
 */


// Includes (note: sys.h defines F_CPU, so it should be included before util/delay.h)
#include "sys.h"
#include "updi_io.h"

#include <avr/io.h>
#include <util/delay.h>

#if UPDI_IO_TYPE == 2

// Cycle timing (convert to float and add 0.5 to achieve round to nearest instead of truncate)
#define TXDELAY (uint8_t) ( ((1.0 * F_CPU)/UPDI_BAUD) / 3 + 0.5 )
#define RXDELAY (uint8_t) ( ((1.0 * F_CPU)/UPDI_BAUD) / 3 + 0.5 )

// Check
#if ( (2 * (F_CPU/UPDI_BAUD) / 3) > 254 )
# error Low baud rates are not supported - use higher UPDI_BAUD
#endif

// Functions
/* Sends regular characters through the UPDI link */
uint8_t UPDI_io::get() {

        // rx input
        DDR(UPDI_PORT)  &= ~(1 << UPDI_PIN);
        // no pullup
        PORT(UPDI_PORT) &= ~(1 << UPDI_PIN);

        uint8_t c;

        __asm volatile
        (
            " ldi  %0, 128 \n\t"        // init

            // wait for start edge
            "WaitStart: \n\t"
            " sbic %[uart_port], %[uart_pin] \n\t"
            " rjmp WaitStart \n\t"

            // skew into middle of bit
            " ldi r18, %[rxdelay] /2 + 6/3  \n\t"  // 0.5 bit cycle delay
            "HBitDelay: \n\t"
            " dec r18 \n\t"
            " brne HBitDelay \n\t"

            // 8 bits
            "RxBLoop: \n\t"
            " ldi r18, %[rxdelay] - 6/3 \n\t"  // 1 bit cycle delay
            "RxBDelay: \n\t"
            " subi r18, 1 \n\t"
            " brne RxBDelay \n\t"
			" sbic %[uart_port], %[uart_pin] \n\t"
			" sec \n\t"
			" ror %0 \n\t"
            " brcc RxBLoop \n\t"

            // Wait approx. 2 bit times: skip to centre of 1st stop bit (ignore parity).
  			// The function returns approx 1.5 bit times before the 2nd stop bit completes
			// to allow burst reads at high UPDI speeds on 8MHz chips.
			// This time needs to be compensated in the Tx function.
            " ldi r18, 2 * %[rxdelay] \n\t"  // delay counter
            "StopDelay: \n\t"
            " dec r18 \n\t"
            " brne StopDelay \n\t"


            : "=r" (c)
            : [uart_port] "i" (_SFR_IO_ADDR(PIN(UPDI_PORT))),
              [uart_pin]  "i" (UPDI_PIN),
              [rxdelay]   "i" (RXDELAY)
            : "r18"
        );

        // re-enable pull up
        PORT(UPDI_PORT) |= (1 << UPDI_PIN);

        return c;
}

uint8_t UPDI_io::put(char c) {

        // tx enable
        DDR(UPDI_PORT) |= (1 << UPDI_PIN);

        __asm volatile
        (
            " in r0, %[uart_port] \n\t"  // port state
            " ldi r27, 8 \n\t"           // 8 bit parity
            " ldi r30, 8 \n\t"           // 8 bit loop

            // pre delay (stop bits from previous sent byte)
            // ~2x bit time
            " ldi r18, %[txdelay] + %[txdelay] \n\t"
            "TxDelay: \n\t"
            " dec r18 \n\t"
            " brne TxDelay \n\t"

            // start bit
            " cbi %[uart_port], %[uart_pin] \n\t"
            " breq TxLoop \n\t"          // 2 cycle delay to equalize timing

            // 8 bits
            "TxLoop: \n\t"
            " ldi r18, %[txdelay] - 9/3 \n\t"  // load delay counter
            "TxDelayB: \n\t"             // delay (3 cycle * delayCount) - 1
            " dec r18 \n\t"
            " brne TxDelayB \n\t"
            " bst %[ch], 0 \n\t"         // load bit in T
            " bld r0, %[uart_pin] \n\t"  // store T bit in r0
            " ror %[ch] \n\t"            // shift right into carry
            " sbci r27, 0 \n\t"          // subtract carry (accumulate parity)
            " dec r30 \n\t"              // decrement bits counter
            " out %[uart_port], r0 \n\t" // send bit out
            " brne TxLoop \n\t"          // loop for each bit

            // parity bit
            " ldi r18, %[txdelay] - 3/3 \n\t"
            "TxDelayP: \n\t"
            " dec r18 \n\t"
            " brne TxDelayP \n\t"
            " bst r27, 0 \n\t"           // extract accumulated parity
            " bld r0, %[uart_pin] \n\t"
            " out %[uart_port], r0 \n\t" // send bit out to serial link

            // stop bits
            "StopLoop: \n\t"
            " ldi r18, %[txdelay] \n\t"
            "TxDelayStop: \n\t"
            " dec r18 \n\t"
            " brne TxDelayStop \n\t"
            " sbi %[uart_port], %[uart_pin] \n\t" // send bit out to serial link


            :
            : [uart_port] "i" (_SFR_IO_ADDR(PORT(UPDI_PORT))),
              [uart_pin]  "i" (UPDI_PIN),
              [txdelay]   "i" (TXDELAY),
              [ch]        "r" (c)
            : "r0","r27","r18","r30"
        );

        // Ready for RX input
        DDR(UPDI_PORT) &= ~(1 << UPDI_PIN);

        return c;
}

static inline void send_break() {

        // tx enable
        DDR(UPDI_PORT) |= (1 << UPDI_PIN);

        //
        // 13 cycles = 24.60ms
        //

        // low 12 cycle
        PORT(UPDI_PORT) &= ~(1 << UPDI_PIN);
        _delay_us(2048*11);

        // high 1 cycle
        PORT(UPDI_PORT) |=  (1 << UPDI_PIN);
        _delay_us(2048);

        // RX enable
        DDR(UPDI_PORT) &= ~(1 << UPDI_PIN);

        return;
}

/* Sends special sequences through the UPDI link */
uint8_t UPDI_io::put(ctrl c) {

        switch (c) {

          case double_break:
              send_break();
              send_break();
              break;

          case single_break:
              send_break();
              break;

          case enable:

          default:
              break;
        }

        return 0;
}

void UPDI_io::init(void) {

}

#endif //__AVR_ATmega16__
