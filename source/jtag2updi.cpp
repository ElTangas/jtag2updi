/*
 * j2updi.cpp
 *
 * Created: 11-11-2017 22:29:58
 * Author : JMR_2
 */

// Includes
#include "sys.h"
#include "updi_io.h"
#include "JICE_io.h"
#include "JTAG2.h"
#include "dbg.h"
#ifdef USE_WDT_RESET
  #include <avr/wdt.h>
#endif

/* Internal stuff */
namespace {
  // Prototypes
  void setup();
  void loop();
  void process_command();
}

int main(void)
{
  setup();
  loop();
}

/* Internal stuff */
namespace {
  inline void setup() {
    /* Initialize MCU */
    //wdt_disable();
    SYS::init();

    /* Initialize serial links */
    JICE_io::init();
    UPDI_io::init();
  }

  inline void loop() {
    #ifndef DISABLE_HOST_TIMEOUT
    uint8_t HostErrorCount=0;
    #endif
    //wdt_enable(WDTO_500MS);
    while (1) {
      // Receive command
      #ifndef DISABLE_HOST_TIMEOUT
        while(!(JTAG2::receive()||(SYS::checkTimeouts() & WAIT_FOR_HOST)));
        if (!(SYS::checkTimeouts() & WAIT_FOR_HOST)) {
          HostErrorCount=0;
          SYS::clearTimeouts(); //clear the timeouts, because WAIT_FOR_TARGET may be set here because of how this is implemented...
      #else
        while(!(JTAG2::receive()));
      #endif
      // Process command
        #if defined(DEBUG_ON)
          DBG::debug('c',JTAG2::packet.body[0]);
          DBG::debug('C',JTAG2::ConnectedTo);
        #endif
        process_command();
        // send response
        JTAG2::answer();
        // some commands need to be executed after sending the answer
        JTAG2::delay_exec();
      #ifndef DISABLE_HOST_TIMEOUT
      } else { // timed out waiting for host communication
        // We timed out waiting for the host to send something
        // if we thought we were talking with host, that would be a sign that something went wrong
        if (JTAG2::ConnectedTo&0x02) {
          #if defined(DEBUG_ON)
            DBG::debug('t',SYS::checkTimeouts());
            DBG::debug('h',HostErrorCount);
          #endif
          if (HostErrorCount++>3) {
            // Time to give up on host, restore default baud rate, and wait for future contact
            #if defined(DEBUG_ON)
              DBG::debug("Giving up...");
            #endif
            JTAG2::PARAM_BAUD_RATE_VAL = JTAG2::BAUD_19200;
            JICE_io::set_baud(JTAG2::BAUD_19200);
            JTAG2::ConnectedTo&=0xFD; // no longer talking to host.
            if (JTAG2::ConnectedTo&0x01) {
              //if this is true, we're still talking to the target! We're better tell it we're done...
              JTAG2::leave_progmode();
              JTAG2::go();
            }
          } else { // send a fail and give them a chance to start talking again
            JTAG2::set_status(JTAG2::RSP_FAILED);
            // send response
            JTAG2::answer();
            // some commands need to be executed after sending the answer
            JTAG2::delay_exec();
          }
        }
      }
      SYS::clearTimeouts();
      #endif
    }
  }

  inline void process_command() {
    switch (JTAG2::packet.body[0]) {
      case JTAG2::CMND_GET_SIGN_ON:
        JTAG2::sign_on();
        break;
      case JTAG2::CMND_GET_PARAMETER:
        JTAG2::get_parameter();
        break;
      case JTAG2::CMND_SET_PARAMETER:
        JTAG2::set_parameter();
        break;
      case JTAG2::CMND_ENTER_PROGMODE:
        JTAG2::enter_progmode();
        break;
      case JTAG2::CMND_RESET:
        #ifdef DISABLE_HOST_TIMEOUT
        JTAG2::enter_progmode();
        #else
        set_status(JTAG2::RSP_OK);
        #endif
        break;
      case JTAG2::CMND_SIGN_OFF:
        // Restore default baud rate before exiting
        JTAG2::PARAM_BAUD_RATE_VAL = JTAG2::BAUD_19200;
        if (JTAG2::ConnectedTo&0x01) {
          //if this is true, we're talking to the target too! We're better tell it we're done...
          JTAG2::leave_progmode();
          JTAG2::go();
        }
        JTAG2::ConnectedTo&=0xFD; // no longer talking to host either, anymore.
        set_status(JTAG2::RSP_OK);
        break;
      case JTAG2::CMND_LEAVE_PROGMODE:
        JTAG2::leave_progmode();
        break;
      case JTAG2::CMND_GET_SYNC:
        JTAG2::set_status(JTAG2::RSP_OK);
        break;
      case JTAG2::CMND_GO:
        #ifdef DISABLE_HOST_TIMEOUT
        JTAG2::leave_progmode();
        #endif
        JTAG2::go();
        break;
      case JTAG2::CMND_SET_DEVICE_DESCRIPTOR:
        JTAG2::set_device_descriptor();
        break;
      case JTAG2::CMND_READ_MEMORY:
        JTAG2::read_mem();
        break;
      case JTAG2::CMND_WRITE_MEMORY:
        JTAG2::write_mem();
        break;
      case JTAG2::CMND_XMEGA_ERASE:
        JTAG2::erase();
        break;
      default:
        JTAG2::set_status(JTAG2::RSP_FAILED);
        break;
    }
    #ifndef DISABLE_TARGET_TIMEOUT
      if (SYS::checkTimeouts() & WAIT_FOR_TARGET) {
        #ifdef DEBUG_ON
         DBG::debug('T',SYS::checkTimeouts());
        #endif
        // If we got a timeout while waiting for the target during the preceeding command, then warn the host:
        JTAG2::set_status(JTAG2::RSP_NO_TARGET_POWER); //this error looks like the best fit
      }
    #endif
  }

}
