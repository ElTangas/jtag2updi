/*
 * updi_io.cpp
 *
 * Created: 18-11-2017 10:36:54
 *  Author: JMR_2
 */



// Includes
#include <avr/io.h>
#include "updi_io.h"
#include "sys.h"

#if UPDI_IO_TYPE == 1

// Cycle timing
#define BIT_TIME (F_CPU/UPDI_BAUD)

// Local functions
namespace {
  void setup_bit_low();
  void setup_bit_high();
  void wait_for_bit() __attribute__((always_inline));
  void stop_timer();
  void start_timer();
}

// Enable to get pulses on PD7 showing the sample times for the software UART input
//#define _DEBUG

// Functions
/* Sends regular characters through the UPDI link */
uint8_t UPDI_io::put(char c) {
  /* Wait for end of stop bits */
  wait_for_bit();
  stop_timer();
  /* Send start bit */
  OCR0A = BIT_TIME - 1;
  TCNT0 = BIT_TIME - 2;
  setup_bit_low();
  start_timer();
  /* Enable TX output */
  DDR(UPDI_PORT) |= (1 << UPDI_PIN);
  /* Calculate parity */
  uint8_t parity;       //get_parity(c);
  parity = 0;
  /* If we can be sure an overflow has happened by now due to instruction latency, */
  /* no more wait is needed and we only need to clear overflow flag */
  //wait_for_bit();
  TIFR0 = (1 << OCF0A);
  /* Send data bits and calculate parity */
  for (uint8_t mask = 1; mask; mask <<= 1) {
    // Check bit, transmit high or low bit accordingly and update parity bit
    parity = (c & mask) ? (setup_bit_high(), ~parity) : (setup_bit_low(), parity);
    wait_for_bit();
  }
  /* Send parity bit */
  parity ? setup_bit_high() : setup_bit_low();
  wait_for_bit();
  /* Send stop bits */
  setup_bit_high();
  wait_for_bit();
  OCR0A = 2 * BIT_TIME - 1;     // 2 bits
  /* Ready for RX input, but high due to pull-up */
  DDR(UPDI_PORT) &= ~(1 << UPDI_PIN);
  return c;
}

/* Sends special sequences through the UPDI link */
uint8_t UPDI_io::put(ctrl c)
{
  /* This nested function expects the timer output to just have gone low */
  /* It waits for 12 minimum baud bit times (break character) then goes high */
  auto break_pulse = [] {
    TCCR0B = 4;                                         // timer tick = 256/F_CPU seconds
    OCR0A = F_CPU/125000;                               // bit time = F_CPU/125000 ticks ~ 2.048 ms
    for (uint8_t i = 0; i < 11; i++) wait_for_bit();    // 12 bits ~ 24.6 ms, as recommended on the datasheet
    setup_bit_high();
    wait_for_bit();
    DDR(UPDI_PORT) &= ~(1 << UPDI_PIN);
  };

  stop_timer();
  /* Send falling edge */
  OCR0A = BIT_TIME - 1;
  TCNT0 = BIT_TIME - 2;
  setup_bit_low();
  start_timer();
  /* Enable TX output */
  DDR(UPDI_PORT) |= (1 << UPDI_PIN);
  /* clear overflow flag */
  TIFR0 = (1 << OCF0A);
  switch (c) {
    case double_break:
      break_pulse();
      setup_bit_low();
      wait_for_bit();
      DDR(UPDI_PORT) |= (1 << UPDI_PIN);
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

uint8_t UPDI_io::get() {
  stop_timer();
  /* Wait for middle of start bit */
  OCR0A = BIT_TIME / 2 - 1;
  TCNT0 = 12;       // overhead time; needs to be calibrated
  /* Make sure overflow flag is reset */
  TIFR0 = (1 << OCF0A);

  /* Must disable pull-up, because the UPDI UART just sends very short output pulses at the beginning of each bit time. */
  /* If pull up is enabled, there will be a drift to high state that results in erroneous input sampling. */
  /* As a side effect, random electrical fluctuations of the input prevent an infinite wait loop */
  /* in case no target is connected. */
  PORT(UPDI_PORT) &= ~(1 << UPDI_PIN);
  /* Wait for start bit */
  loop_until_bit_is_clear(PIN(UPDI_PORT), UPDI_PIN);

  start_timer();
  wait_for_bit();
  /* Setup sampling time */
  OCR0A = BIT_TIME - 1;
#   ifdef _DEBUG
  /* Timing pulse */
  PIND |= (1 << PIND7);
  PIND |= (1 << PIND7);
#   endif // _DEBUG
  /* Sample bits */
  uint8_t c = 0;
  //for (uint8_t i = 0; i < 8; i++) {
  for (uint8_t mask = 1; mask; mask <<= 1) {
    wait_for_bit();
    /* Take sample */
    //c /= 2;
    if ( PIN(UPDI_PORT) & (1 << UPDI_PIN) ) {
      //c |=  0x80;
      c |= mask;
    }
#       ifdef _DEBUG
    /* Timing pulse */
    PIND |= (1 << PIND7);
    PIND |= (1 << PIND7);
#       endif // _DEBUG
  }
  /* To Do Sample Parity */
  wait_for_bit();
#   ifdef _DEBUG
  /* Timing pulse */
  PIND |= (1 << PIND7);
  PIND |= (1 << PIND7);
#   endif // _DEBUG
  OCR0A = 2 * BIT_TIME + BIT_TIME / 2 - 1;      // 2.5 bits
  /* Return as soon as high parity or stop bits start */
  loop_until_bit_is_set(PIN(UPDI_PORT), UPDI_PIN);
  /* Re-enable pull up */
  PORT(UPDI_PORT) |= (1 << UPDI_PIN);
  return c;
}

void UPDI_io::init(void)
{
#   ifdef _DEBUG
  /* For RX timing measurement and debugging, make PD7 output */
  DDRD |= (1 << DDD7);
#   endif // _DEBUG
  setup_bit_high();
  /* initialize counter to near terminal count */
  TCNT0 = BIT_TIME - 2;
  /* initialize OCR0A to 200k counts per second */
  OCR0A = BIT_TIME - 1;
  start_timer();
}

namespace {
  inline void setup_bit_low() {
    /* OC0A will go low on match with OCR0A */
    /* Also, set CTC mode - reset timer on match with OCR0A */
    TCCR0A = (1 << COM0A1) | (0 << COM0A0) | (1 << WGM01);
  }

  inline void setup_bit_high() {
    /* OC0A will go high on match with OCR0A */
    /* Also, set CTC mode - reset timer on match with OCR0A */
    TCCR0A = (1 << COM0A1) | (1 << COM0A0) | (1 << WGM01);
  }

  inline void wait_for_bit() {
    /* Wait for compare match */
    loop_until_bit_is_set(TIFR0, OCF0A);
    TIFR0 = (1 << OCF0A);
  }

  inline void stop_timer() {
    TCCR0B = 0;
  }

  inline void start_timer() {
    TCCR0B = 1;
  }
}


#endif
