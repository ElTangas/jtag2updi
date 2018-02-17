/*
 * j2updi.cpp
 *
 * Created: 11-11-2017 22:29:58
 * Author : JMR_2
 */ 

// Includes
#include "JTAG2.h"
#include "updi_io.h"
#include "JICE_io.h"
#include "UPDI_lo_lvl.h"
#include "NVM.h"

// Prototypes
void setup();
void loop();
void sys_init();

int main(void)
{
	setup();
	loop();
}

inline void setup() {
	/* Initialize MCU */
	sys_init();
	
	/* Initialize serial links */
	JICE_io::init();
	UPDI_io::init();
}


inline void loop() {
	while (1) {
		
		// Receive command
		while(!JTAG2::receive());
		// Process command
		switch (JTAG2::body[0]) {
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
				JTAG2::PARAM_BAUD_RATE_VAL = 0x04;
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
		JTAG2::answer();
		// some commands need to be executed after sending the answer
		JTAG2::delay_exec();
	}
}

void sys_init() {
	/* Disable digital input buffers on port C */
	DIDR0 = 0x3F;
	/* Enable all port D pull-ups */
	PORTD = 0xFF;
	/* Enable all port B pull-ups, except for LED */
	PORTB = 0xFF - (1 << PORTB5);
	/* Disable unused peripherals */
	ACSR = 1 << ACD;		// turn off comparator
	PRR =
		(1 << PRTWI) |		// turn off 2 wire interface
		(1 << PRTIM2) |		// turn off timer 2
		(1 << PRTIM1) |		// turn off timer 1
		(1 << PRSPI) |		// turn off SPI interface
		(1 << PRADC);		// turn off the ADC		
}