/*
 * j2updi.cpp
 *
 * Created: 11-11-2017 22:29:58
 * Author : JMR_2
 */ 

// Includes
#if defined __AVR_ATmega32U4__
	#include <Arduino.h>
	#include "USBAPI.h"
  
// Declared weak in Arduino.h to allow user redefinitions.
	int atexit(void (* /*func*/ )()) { return 0; }

// Weak empty variant initialization function.
// May be redefined by variant files.
	void initVariant() __attribute__((weak));
	void initVariant() { }

	void setupUSB() __attribute__((weak));
	void setupUSB() { }

#endif

#include "sys.h"
#if UPDI_IO_TYPE==3
  #include "updi_io_uart.h"
#else
  #include "updi_io.h"
#endif
#include "JICE_io.h"
#include "JTAG2.h"

/* Internal stuff */
namespace {
	// Prototypes
	void setup2();
	void loop2();
}

int main(void)
{
	#if defined __AVR_ATmega32U4__
		init();
		initVariant();
	#endif

	#if defined(USBCON)
		USBDevice.attach();
	#endif
	setup2();
	loop2();
}

/* Internal stuff */
namespace {
	inline void setup2() {
		/* Initialize MCU */
		SYS::init();
	
		/* Initialize serial links */
		JICE_io::init();
		UPDI_io::init();
	}


	inline void loop2() {
		while (1) {
		
			// Receive command
			while(!JTAG2::receive());
			// Process command
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
				case JTAG2::CMND_RESET:
				case JTAG2::CMND_ENTER_PROGMODE:
					JTAG2::enter_progmode();
					break;
				case JTAG2::CMND_SIGN_OFF:
					// Restore default baud rate before exiting
					JTAG2::PARAM_BAUD_RATE_VAL = JTAG2::baud_19200;
				case JTAG2::CMND_LEAVE_PROGMODE:
					JTAG2::leave_progmode();
					break;
				case JTAG2::CMND_GET_SYNC:
				case JTAG2::CMND_GO:
					JTAG2::set_status(JTAG2::RSP_OK);
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
			// send response
			//SYS::LED_blink(2, 5, 100);
			JTAG2::answer();
			// some commands need to be executed after sending the answer
			JTAG2::delay_exec();
		}
	}
}
