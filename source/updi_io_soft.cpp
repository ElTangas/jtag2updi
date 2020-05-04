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
#define BITTIME (uint8_t) ( ((1.0 * F_CPU)/UPDI_BAUD) / 3 + 0.5 )

// Check
#if ( (2 * (F_CPU/UPDI_BAUD) / 3) > 254 )
# error Low baud rates are not supported - use higher UPDI_BAUD
#endif

#define ASM_MACROS \
        ".ifndef delay_macros                           \n"\
        ".set delay_macros, 1                           \n"\
                                                        \
        /* Delays 3n cycles, preserves carry flag */    \
        ".macro delay n                                 \n"\
        " ldi r18, \\n                                  \n"\
        "0:                                             \n"\
        " dec r18                                       \n"\
        " brne 0b                                       \n"\
        ".endm                                          \n"\
                                                        \
        /* Delays 3n cycles, clears carry flag */       \
        ".macro delay_cc n                              \n"\
        " ldi r18, \\n                                  \n"\
        "0:                                             \n"\
        " subi r18, 1                                   \n"\
        " brne 0b                                       \n"\
        ".endm                                          \n"\
                                                        \
        ".endif                                         \n"

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
            ASM_MACROS
            " ldi  %0, 128 \n\t"                            // variable will receive return data, doubles as counter

            // wait for start edge
            "WaitStart: \n\t"
            " sbic %[uart_port], %[uart_pin] \n\t"
            " rjmp WaitStart \n\t"

            // skew into middle of first data bit
            " delay_cc %[rxdelay] + %[rxdelay] /2 \n\t"     // 1.5 bit cycle delay and leave carry = 0 for next loop

            // 8 bits
            "RxBLoop: \n\t"
            " sbic %[uart_port], %[uart_pin] \n\t"          // carry = 0 here; check I/O data
            " sec \n\t"                                     // if data = 1, set carry
            " nop \n\t"                                     // correct loop execution time to 6 cycles
            " delay (%[rxdelay] - 6/3) \n\t"                // 1 bit time delay, minus loop execution time
            " ror %0 \n\t"                                  // store I/O bit; will set carry when loop ends
            " brcc RxBLoop \n\t"

            // Wait 1 bit time: skip to centre of 1st stop bit (ignore parity).
            // The function returns approx 1.5 bit times before the 2nd stop bit completes
            // to allow burst reads at high UPDI speeds on 8MHz chips.
            // This time needs to be compensated in the Tx function.
            " delay %[rxdelay] \n\t"


            : "=r" (c)
            : [uart_port] "i" (_SFR_IO_ADDR(PIN(UPDI_PORT))),
              [uart_pin]  "i" (UPDI_PIN),
              [rxdelay]   "i" (BITTIME)
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
            ASM_MACROS
            " in r0, %[uart_port] \n\t"  // port state
            " ldi r19, 0x78 \n\t"        // High nibble: bits counter (8); Low nibble: parity accumulator

            // pre delay (stop bits from previous sent byte)
            // ~2x bit time
            " delay %[txdelay] + %[txdelay] \n\t"

            // start bit
            " cbi %[uart_port], %[uart_pin] \n\t"
            " breq TxLoop \n\t"          // 2 cycle delay to equalize timing

            // 8 bits
            "TxLoop: \n\t"
            " delay (%[txdelay] - 9/3) \n\t"
            " bst %[ch], 0 \n\t"         // load bit in T
            " bld r0, %[uart_pin] \n\t"  // store T bit in r0
            " ror %[ch] \n\t"            // shift right into carry
            " sbci r19, 0x10 \n\t"       // subtract carry (accumulate parity) and decrement bits counter
            " nop \n\t"                  // adjust loop cycle count to 9
            " bld %[ch], 7 \n\t"         // store data back in bit 7 to leave argument unchanged after the 8 cycles
            " out %[uart_port], r0 \n\t" // send bit out
            " brcc TxLoop \n\t"          // loop for each bit

            // parity bit
            " delay (%[txdelay] - 3/3) \n\t"
            " bst r19, 0 \n\t"           // extract accumulated parity
            " bld r0, %[uart_pin] \n\t"
            " out %[uart_port], r0 \n\t" // send bit out to serial link

            // stop bits
            " delay %[txdelay] \n\t"
            " sbi %[uart_port], %[uart_pin] \n\t" // send bit out to serial link

            :
            : [uart_port] "i" (_SFR_IO_ADDR(PORT(UPDI_PORT))),
              [uart_pin]  "i" (UPDI_PIN),
              [txdelay]   "i" (BITTIME),
              [ch]        "r" (c)
            : "r0","r18","r19"
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
