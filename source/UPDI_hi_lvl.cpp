/*
 * UPDI_hi_lvl.cpp
 *
 * Created: 15-02-2018 23:08:39
 *  Author: JMR_2
 */

#include "UPDI_hi_lvl.h"
#include "dbg.h"

bool UPDI::CPU_reset(){
  #if defined(DEBUG_RESET)
    DBG::updi_reset();
  #endif

  // Assert target reset
  UPDI::CPU_reset_on();
  // Deassert target reset and wait for target initialization
  return UPDI::CPU_reset_off();
}

void UPDI::CPU_reset_on(){
  #if defined(DEBUG_RESET)
    DBG::updi_reset_on();
  #endif

  // Request reset
  UPDI::stcs(UPDI::reg::ASI_Reset_Request, UPDI::RESET_ON);
  // System remains in reset state until released
}

bool UPDI::CPU_reset_off(){
  #if defined(DEBUG_RESET)
    DBG::updi_reset_off();
  #endif

  // Release reset (System remains in reset state until released)
  UPDI::stcs(UPDI::reg::ASI_Reset_Request, UPDI::RESET_OFF);

  // Wait for the reset process to end.
  // Either NVMPROG, UROWPROG or BOOTDONE bit will be set in the ASI_SYS_STATUS UPDI register.
  // This indicates reset is complete.
  #ifndef DISABLE_TARGET_TIMEOUT
    uint8_t timeoutcount=0;
    while ( UPDI::CPU_mode<0x0E>() == 0 && timeoutcount<2) //if it takes 200ms to come back after we release reset... it's never going to!
    {
      if (SYS::checkTimeouts() & WAIT_FOR_TARGET) {
        SYS::clearTimeouts();
        timeoutcount++;
      }
    }
  #else
    while ( UPDI::CPU_mode<0x0E>() == 0 );
  #endif

  #if defined(DEBUG_RESET) && !defined(DEBUG_LDCS)
  //if LDCS is defined, it will pick this up
  #ifndef DISABLE_TARGET_TIMEOUT
  if(timeoutcount) {
    DBG::debug('T',timeoutcount);
  }
  #endif
  DBG::updi_post_reset(UPDI::CPU_mode<0xFF>());
  #endif
  #ifndef DISABLE_TARGET_TIMEOUT
    return (timeoutcount<2); //if we didn't timeout and give up trying to bring it out of reset, return true
  #else
    return 1;
  #endif
}
