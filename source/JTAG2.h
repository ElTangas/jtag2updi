/*
 * packet.h
 *
 * Created: 12-11-2017 11:10:31
 *  Author: JMR_2
 */ 


#ifndef JTAG2_H_
#define JTAG2_H_

#include "JICE_io.h"
#include <stdint.h>
#include "flash_vars.h"
#include "NVM.h"

namespace JTAG2 {
	
	// *** Parameter IDs ***
	constexpr uint8_t PARAM_HW_VER					= 0x01;
	constexpr uint8_t PARAM_FW_VER					= 0x02;
	constexpr uint8_t PARAM_EMU_MODE				= 0x03;
	constexpr uint8_t PARAM_BAUD_RATE				= 0x05;
	constexpr uint8_t PARAM_VTARGET					= 0x06;
	
	// *** Parameter Values ***
	constexpr uint8_t PARAM_HW_VER_M_VAL			= 0x01;
	constexpr uint8_t PARAM_HW_VER_S_VAL			= 0x01;
	constexpr uint8_t PARAM_FW_VER_M_MIN_VAL		= 0x00;
	constexpr uint8_t PARAM_FW_VER_M_MAJ_VAL		= 0x06;
	constexpr uint8_t PARAM_FW_VER_S_MIN_VAL		= 0x00;
	constexpr uint8_t PARAM_FW_VER_S_MAJ_VAL		= 0x06;
	extern uint8_t PARAM_EMU_MODE_VAL;
	extern uint8_t PARAM_BAUD_RATE_VAL;
	constexpr uint16_t PARAM_VTARGET_VAL			= 5000;	

	// *** General command constants ***
	constexpr uint8_t CMND_SIGN_OFF					= 0x00;
	constexpr uint8_t CMND_GET_SIGN_ON				= 0x01;
	constexpr uint8_t CMND_SET_PARAMETER			= 0x02;
	constexpr uint8_t CMND_GET_PARAMETER			= 0x03;
	constexpr uint8_t CMND_WRITE_MEMORY				= 0x04;
	constexpr uint8_t CMND_READ_MEMORY				= 0x05;
	constexpr uint8_t CMND_GO						= 0x08;
	constexpr uint8_t CMND_RESET					= 0x0b;
	constexpr uint8_t CMND_SET_DEVICE_DESCRIPTOR	= 0x0c;
	constexpr uint8_t CMND_GET_SYNC					= 0x0f;
	constexpr uint8_t CMND_ENTER_PROGMODE			= 0x14;
	constexpr uint8_t CMND_LEAVE_PROGMODE			= 0x15;
	constexpr uint8_t CMND_XMEGA_ERASE				= 0x34;

	// *** JTAG Mk2 Single byte status responses ***
	// Success
	constexpr uint8_t RSP_OK						= 0x80;
	constexpr uint8_t RSP_PARAMETER					= 0x81;
	constexpr uint8_t RSP_MEMORY					= 0x82;
	// Errors
	constexpr uint8_t RSP_FAILED					= 0xA0;
	constexpr uint8_t RSP_ILLEGAL_PARAMETER			= 0xA1;
	constexpr uint8_t RSP_ILLEGAL_MEMORY_TYPE		= 0xA2;
	constexpr uint8_t RSP_ILLEGAL_MEMORY_RANGE		= 0xA3;
	constexpr uint8_t RSP_ILLEGAL_MCU_STATE			= 0xA5;
	constexpr uint8_t RSP_ILLEGAL_VALUE				= 0xA6;
	constexpr uint8_t RSP_ILLEGAL_BREAKPOINT		= 0xA8;
	constexpr uint8_t RSP_ILLEGAL_JTAG_ID			= 0xA9;
	constexpr uint8_t RSP_ILLEGAL_COMMAND			= 0xAA;
	constexpr uint8_t RSP_NO_TARGET_POWER			= 0xAB;
	constexpr uint8_t RSP_DEBUGWIRE_SYNC_FAILED		= 0xAC;
	constexpr uint8_t RSP_ILLEGAL_POWER_STATE		= 0xAD;
	
	// *** memory types for CMND_{READ,WRITE}_MEMORY ***
	constexpr uint8_t MTYPE_IO_SHADOW				= 0x30;		// cached IO registers?
	constexpr uint8_t MTYPE_SRAM					= 0x20;		// target's SRAM or [ext.] IO registers
	constexpr uint8_t MTYPE_EEPROM					= 0x22;		// EEPROM, what way?
	constexpr uint8_t MTYPE_EVENT					= 0x60;		// ICE event memory
	constexpr uint8_t MTYPE_SPM						= 0xA0;		// flash through LPM/SPM
	constexpr uint8_t MTYPE_FLASH_PAGE				= 0xB0;		// flash in programming mode
	constexpr uint8_t MTYPE_EEPROM_PAGE				= 0xB1;		// EEPROM in programming mode
	constexpr uint8_t MTYPE_FUSE_BITS				= 0xB2;		// fuse bits in programming mode
	constexpr uint8_t MTYPE_LOCK_BITS				= 0xB3;		// lock bits in programming mode
	constexpr uint8_t MTYPE_SIGN_JTAG				= 0xB4;		// signature in programming mode
	constexpr uint8_t MTYPE_OSCCAL_BYTE				= 0xB5;		// osccal cells in programming mode
	constexpr uint8_t MTYPE_CAN						= 0xB6;		// CAN mailbox
	constexpr uint8_t MTYPE_FLASH					= 0xc0;		// xmega (app.) flash
	constexpr uint8_t MTYPE_BOOT_FLASH				= 0xc1;		// xmega boot flash
	constexpr uint8_t MTYPE_EEPROM_XMEGA			= 0xc4;		// xmega EEPROM in debug mode
	constexpr uint8_t MTYPE_USERSIG					= 0xc5;		// xmega user signature
	constexpr uint8_t MTYPE_PRODSIG					= 0xc6;		// xmega production signature
	
	
	// *** STK500 packet *** 
	constexpr uint8_t MESSAGE_START = 0x1B;
	extern uint8_t prologue[6];
	extern uint16_t & number;
	extern uint8_t (& number_byte)[2];
	extern uint32_t & size;
	extern uint8_t (& size_byte)[4];
	extern uint16_t (& size_word)[2];
	constexpr uint8_t TOKEN = 0x0E;
	extern uint8_t body [512];
	
	//  *** Signature response ***
	extern FLASH<uint8_t> sgn_resp[29];
	// *** Parameter initialization ***
	void init();

	// *** Packet functions *** 
	bool receive();
	void answer();
	void delay_exec();

	// *** Set status function ***
	void set_status(uint8_t);

	// *** General command functions ***
	void sign_on();
	void get_parameter();
	void set_parameter();
	void set_device_descriptor();

	// *** ISP command functions ***
	void enter_progmode();
	void leave_progmode();
	void read_signature();
	void read_mem();
	void write_mem();
	void erase();
}



#endif /* STK_H_ */