/*
 * STK500.cpp
 *
 * Created: 08-12-2017 19:47:27
 *  Author: JMR_2
 */

#include "JTAG2.h"
#include "crc16.h"
#include "UPDI_hi_lvl.h"

// *** Writeable Parameter Values ***
uint8_t JTAG2::PARAM_EMU_MODE_VAL;
uint8_t JTAG2::PARAM_BAUD_RATE_VAL;


// *** STK500 packet ***
uint8_t JTAG2::prologue[6];

uint16_t & JTAG2::number = reinterpret_cast<uint16_t &> (JTAG2::prologue[0]);
uint8_t (& JTAG2::number_byte)[2] = (uint8_t (&)[2]) JTAG2::number;
uint32_t & JTAG2::size = reinterpret_cast<uint32_t &> (JTAG2::prologue[2]);
uint8_t (& JTAG2::size_byte)[4] = (uint8_t (&)[4]) JTAG2::size;
uint16_t (& JTAG2::size_word)[2] = (uint16_t (&)[2]) JTAG2::size;


uint8_t JTAG2::body [512];

// *** Baud rate lookup table for UBRR0 register ***
FLASH(uint16_t) baud_tbl[8] = {0x0340, 0x01A0, 0x00CF, 0x0067, 0x0033, 0x0022, 0x0010, 0x008A};

// *** Local variables ***
uint8_t flash_pagesize;
uint8_t eeprom_pagesize;

// *** Local functions declaration ***
void NVM_fuse_write (uint16_t address, uint8_t data);
void NVM_buffered_write(uint16_t address, uint16_t lenght, uint8_t buff_size, uint8_t write_type);

// *** Packet functions *** 
	bool JTAG2::receive() {
		while (JICE_io::get() != MESSAGE_START);
		uint16_t crc = CRC::next(MESSAGE_START, CRC::first);
		for (uint8_t i = 0; i < sizeof(prologue); i++) {
			crc = CRC::next(prologue[i] = JICE_io::get(), crc);
		}
		if (size > sizeof(body)) return false;
		if (JICE_io::get() != TOKEN) return false;
		crc = CRC::next(TOKEN, crc);
		for (uint16_t i = 0; i < size; i++) {
			crc = CRC::next(body[i] = JICE_io::get(), crc);
		}
		if ((uint16_t)(JICE_io::get() | (JICE_io::get() << 8)) != crc) return false;
		return true;
	}

	void JTAG2::answer() {
		uint16_t crc = CRC::next(JICE_io::put(MESSAGE_START), CRC::first);
		for (uint8_t i = 0; i < sizeof(prologue); i++) {
			crc = CRC::next(JICE_io::put(prologue[i]), crc);
		}
		crc = CRC::next(JICE_io::put(TOKEN), crc);
		for (uint16_t i = 0; i < size; i++) {
			crc = CRC::next(JICE_io::put(body[i]), crc);
		}
		JICE_io::put(crc);
		JICE_io::put(crc >> 8);
	}
	
	void JTAG2::delay_exec() {
		// wait for transmission complete
		UCSR0A |= 1 << TXC0;
		loop_until_bit_is_set(UCSR0A, TXC0);
		// set baud rate
		UBRR0 = baud_tbl[PARAM_BAUD_RATE_VAL - 1];
	}

// *** Set status function ***
	void JTAG2::set_status(uint8_t status_code){
		size = 1;
		body[0] = status_code;
	}

// *** General command functions ***
	// *** Signature response message ***
	FLASH(uint8_t) JTAG2::sgn_resp[29] {	0x86, 1,
											1, PARAM_FW_VER_M_MIN_VAL, PARAM_FW_VER_M_MAJ_VAL, PARAM_HW_VER_M_VAL,
											1, PARAM_FW_VER_S_MIN_VAL, PARAM_FW_VER_S_MAJ_VAL, PARAM_HW_VER_S_VAL,
											0, 0, 0, 0, 0, 0,
											'J', 'T', 'A', 'G', 'I', 'C', 'E', ' ', 'm', 'k', 'I', 'I', 0};
	void JTAG2::sign_on(){
		// Initialize JTAGICE2 variables
		JTAG2::PARAM_EMU_MODE_VAL = 0x02;
		JTAG2::PARAM_BAUD_RATE_VAL = 0x04;
		/* Initialize or enable UPDI */
		UPDI_io::put(UPDI_io::double_break);
		UPDI::stcs(UPDI::reg::Control_A, 6);
		// Send sign on message
		size_word[0] = sizeof(sgn_resp);
		for (uint8_t i = 0; i < sizeof(sgn_resp); i++) {
			body[i] = sgn_resp[i];
		}
	}

	void JTAG2::get_parameter(){
		uint8_t & status = body[0];
		uint8_t & parameter = body[1];
		switch (parameter) {
			case PARAM_HW_VER:
				size_word[0] = 3;
				body[1] = PARAM_HW_VER_M_VAL;
				body[2] = PARAM_HW_VER_S_VAL;
				break;
			case PARAM_FW_VER:
				size_word[0] = 5;
				body[1] = PARAM_FW_VER_M_MIN_VAL;
				body[2] = PARAM_FW_VER_M_MAJ_VAL;
				body[3] = PARAM_FW_VER_S_MIN_VAL;
				body[4] = PARAM_FW_VER_S_MAJ_VAL;
				break;
			case PARAM_EMU_MODE:
				size_word[0] = 2;
				body[1] = PARAM_EMU_MODE_VAL;
				break;
			case PARAM_BAUD_RATE:
				size_word[0] = 2;
				body[1] = PARAM_BAUD_RATE_VAL;
				break;
			case PARAM_VTARGET:
				size_word[0] = 3;
				body[1] = PARAM_VTARGET_VAL & 0xFF;
				body[2] = PARAM_VTARGET_VAL >> 8;
				break;
			default:
				set_status(RSP_ILLEGAL_PARAMETER);
				return;
		}
		status = RSP_PARAMETER;
		return;
	}

	void JTAG2::set_parameter(){
		uint8_t & parameter = body[1];
		switch (parameter) {
			case PARAM_EMU_MODE:
				PARAM_EMU_MODE_VAL = body[2];
				break;
			case PARAM_BAUD_RATE:
				PARAM_BAUD_RATE_VAL = body[2];
				break;
			default:
				set_status(RSP_ILLEGAL_PARAMETER);
				return;
		}
		set_status(RSP_OK);
	}

	void JTAG2::set_device_descriptor(){
		flash_pagesize = body[244];
		eeprom_pagesize = body[246];
		set_status(RSP_OK);
	}
		
// *** Target mode set functions ***
	// *** Sets MCU in program mode, if possibe ***
	void JTAG2::enter_progmode(){
		const uint8_t system_status = UPDI::CPU_mode<0xEF>();
		switch (system_status){
			// in normal operation mode
			case 0x82:
				// Write NVN unlock key (allows read access to all addressing space)
				UPDI::write_key(UPDI::NVM_Prog);
				// Request reset
				UPDI::CPU_reset();
				// Wait for NVM unlock state
				while (UPDI::CPU_mode() != 0x08);
			// already in program mode
			case 0x08:
				// better clear the page buffer, just in case.
				UPDI::sts_b(NVM::NVM_base | NVM::CTRLA, NVM::PBC);
				// Turn on LED to indicate program mode
				PORTB |= 1 << 5;
				set_status(RSP_OK);
				return;
			// in other modes fail and inform host of wrong mode
			default:
				size_word[0] = 2;
				body[0] = RSP_ILLEGAL_MCU_STATE;
				body[1] = system_status; // 0x01;
				return;
		}
	}

	// *** Sets MCU in normal runnning mode, if possibe ***
	void JTAG2::leave_progmode(){
		const uint8_t system_status = UPDI::CPU_mode<0xEF>();
		switch (system_status){
			// in program mode
			case 0x08:
				// Request reset
				UPDI::CPU_reset();
				// Wait for normal mode
				while (UPDI::CPU_mode<0xEF>() != 0x82);
			// already in normal mode
			case 0x82:
				// Turn off LED to indicate normal mode
				PORTB &= ~(1 << 5);
				set_status(RSP_OK);
				return;
			// in other modes fail and inform host of wrong mode
			default:
				size_word[0] = 2;
				body[0] = RSP_ILLEGAL_MCU_STATE;
				body[1] = system_status; // 0x01;
				return;
		}
	}
	
// *** Read/Write/Erase functions ***

	void JTAG2::read_mem() {
		if (UPDI::CPU_mode() != 0x08){
			// fail if not in program mode
			size_word[0] = 2;
			body[0] = RSP_ILLEGAL_MCU_STATE;
			body[1] = 0x01;
		}
		else {
			// in program mode
			const uint16_t NumBytes = (body[3] << 8) | body[2];
			// Get physical address for reading
			const uint16_t address = (body[7] << 8) | body[6];
			// Set UPDI pointer to address
			UPDI::stptr_w(address);
			// Read block
			UPDI::rep(NumBytes - 1);
			body[1] = UPDI::ldinc_b();
			for (uint16_t i = 2; i <= NumBytes; i++) {
				body[i] = UPDI_io::get();
			}
			size_word[0] = NumBytes + 1;
			body[0] = RSP_MEMORY;
		}
	}
	
	void JTAG2::write_mem() {
		if (UPDI::CPU_mode() != 0x08){
			// fail if not in program mode
			size_word[0] = 2;
			body[0] = RSP_ILLEGAL_MCU_STATE;
			body[1] = 0x01;
		}
		else {
			// in program mode
			const uint8_t mem_type = body[1];
			const uint16_t address = body[6] | (body[7] << 8);
			const uint16_t lenght = body[2] | (body[3] << 8);							/* number of bytes to write */
			const bool is_flash = ((mem_type == MTYPE_FLASH) || (mem_type == MTYPE_BOOT_FLASH));
			const uint8_t buff_size = is_flash ? flash_pagesize : eeprom_pagesize;
			const uint8_t write_cmnd = is_flash ? NVM::WP : NVM::ERWP;
			switch (mem_type) {
				case MTYPE_FUSE_BITS:
				case MTYPE_LOCK_BITS:
					NVM_fuse_write (address, body[10]);
					break;
				case MTYPE_FLASH:
				case MTYPE_BOOT_FLASH:
				case MTYPE_EEPROM_XMEGA:
				case MTYPE_USERSIG:
					NVM_buffered_write(address, lenght, buff_size, write_cmnd);
					break;
				default:
					set_status(RSP_ILLEGAL_MEMORY_TYPE);
					return;
			}
			set_status(RSP_OK);
		}
	}

	void JTAG2::erase() {
		const uint8_t erase_type = body[1];
		const uint16_t address = body[2] | (body[3] << 8);
		switch (erase_type) {
			case 0:
				// Write Chip Erase key
				UPDI::write_key(UPDI::Chip_Erase);
				// Request reset
				UPDI::CPU_reset();
				// Wait for NVM unlock state
				while (UPDI::CPU_mode<0x01>());
				// Erase chip process exits program mode, reenter...
				enter_progmode();
				return;
			case 4:
			case 5:
				NVM::wait<false>();
				UPDI::sts_b(address, 0xFF);
				NVM::command<false>(NVM::ER);
				break;
			case 6:
			case 7:
				break;
			default:
				set_status(RSP_FAILED);
				return;
		}
		set_status(RSP_OK);
	}


// *** Local functions definition ***
	void NVM_fuse_write (uint16_t address, uint8_t data) {
		// Setup UPDI pointer
		UPDI::stptr_w(NVM::NVM_base + NVM::DATA_lo);
		// Send data to the NVM controller
		UPDI::stinc_b(data);
		UPDI::stinc_b(0x00);
		// Send address to the NVM controller
		UPDI::stinc_b(address & 0xFF);
		UPDI::stinc_b(address >> 8);
		// Execute fuse write
		NVM::command<false>(NVM::WFU);
	}
	
	void NVM_buffered_write(uint16_t address, uint16_t length, uint8_t buff_size, uint8_t write_cmnd) {
		uint8_t current_byte_index = 10;					/* Index of the first byte to send inside the JTAG2 command body */
		uint16_t bytes_remaining = length;					/* number of bytes to write */
		
		// Sends a block of bytes from the command body to memory, using the UPDI interface
		// On entry, the UPDI pointer must already point to the desired address
		// On exit, the UPDI pointer points to the next byte after the last one written
		// The index into the command body is also updated to point to the first unsent byte.
		auto updi_send_block = [] (uint8_t count, uint8_t & index) {
			NVM::wait<true>();
			UPDI::rep(count - 1);
			UPDI::stinc_b(JTAG2::body[index++]);
			for (uint8_t i = 0; i < (count - 1); i++) {
				UPDI_io::put(JTAG2::body[index++]);
				UPDI_io::get();
			}
		};

		// Setup UPDI pointer for block transfer
		UPDI::stptr_w(address);
		/* Check address alignment, calculate number of unaligned bytes to send */
		uint8_t unaligned_bytes = (-address & (buff_size - 1));
		if (unaligned_bytes > bytes_remaining) unaligned_bytes = bytes_remaining;
		/* If there are unaligned bytes, they must be sent first */
		if (unaligned_bytes) {
			// Send unaligned block
			updi_send_block(unaligned_bytes, current_byte_index);
			bytes_remaining -= unaligned_bytes;
			NVM::command<true>(write_cmnd);
		}
		while (bytes_remaining) {
			/* Send a buff_size amount of bytes */
			if (bytes_remaining >= buff_size) {
				updi_send_block(buff_size, current_byte_index);
				bytes_remaining -= buff_size;
			}
			/* Send a NumBytes amount of bytes */
			else {
				updi_send_block(bytes_remaining, current_byte_index);
				bytes_remaining = 0;
			}
			NVM::command<true>(write_cmnd);
		}
	}