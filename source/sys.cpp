/*
 * sys.cpp
 *
 * Created: 02-10-2018 13:07:52
 *  Author: JMR_2
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "sys.h"
#include "dbg.h"
#include <stdio.h>
#include <string.h>


void SYS::init(void) {
  #ifdef DEBUG_ON
    DBG::initDebug();
  #endif

  #ifdef XAVR
    #ifdef __AVR_DX__
      #if (F_CPU == 24000000 | F_CPU == 20000000 | F_CPU == 16000000 | F_CPU == 12000000 | F_CPU == 8000000)
        #define FREQSEL_VAL (F_CPU/4000000L + 3)
        /* No division on clock */
        _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (CLKCTRL_OSCHFCTRLA & ~CLKCTRL_FREQSEL_gm ) | (FREQSEL_VAL << CLKCTRL_FREQSEL_gp ));
      #else
        #error "F_CPU defined as an unsupported value"
      #endif
    #else //0-series or 1-series
      _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0);
    #endif
  #else
    #if defined(ARDUINO_AVR_LARDU_328E)
    #include <avr/power.h>
    clock_prescale_set ( (clock_div_t) __builtin_log2(32000000UL / F_CPU));
    #endif
    PORT(UPDI_PORT) = 1<<UPDI_PIN;
  #endif


  DDR(LED_PORT) |= (1 << LED_PIN);
  #ifdef LED2_PORT
  DDR(LED2_PORT) |= (1 << LED2_PIN);
  #endif
  #ifndef DISABLE_HOST_TIMEOUT
  TIMER_HOST_MAX=HOST_TIMEOUT;
  #endif
  #ifndef DISABLE_TARGET_TIMEOUT
  TIMER_TARGET_MAX=TARGET_TIMEOUT;
  #endif
  #if defined(DEBUG_ON)
  DBG::debug(0x18,0xC0,0xFF, 0xEE);
  #endif
}

void SYS::setLED(void){
  PORT(LED_PORT) |= 1 << LED_PIN;
}

void SYS::clearLED(void){
  PORT(LED_PORT) &= ~(1 << LED_PIN);
}

void SYS::setVerLED(void){
  #ifdef LED2_PORT
  PORT(LED2_PORT) |= 1 << LED2_PIN;
  #endif
}

void SYS::clearVerLED(void){
  #ifdef LED2_PORT
  PORT(LED2_PORT) &= ~(1 << LED2_PIN);
  #endif
}


