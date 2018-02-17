/*
 * UPDI_cmd.cpp
 *
 * Created: 23-11-2017 22:48:36
 *  Author: JMR_2
 */ 

// Includes
#include "UPDI_lo_lvl.h"

// Keys
FLASH(uint8_t) UPDI::Chip_Erase[8] {0x65, 0x73, 0x61, 0x72, 0x45, 0x4D, 0x56, 0x4E};
FLASH(uint8_t) UPDI::NVM_Prog[8] {0x20, 0x67, 0x6F, 0x72, 0x50, 0x4D, 0x56, 0x4E};
FLASH(uint8_t) UPDI::UserRow_Write[8] {0x65, 0x74, 0x26, 0x73, 0x55, 0x4D, 0x56, 0x4E};

// Functions
void UPDI::rep(uint8_t repeats) {
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0xA0);
	UPDI_io::put(repeats);
}

void UPDI::stcs(reg::reg_t r, uint8_t data) {
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0xC0 + r);
	UPDI_io::put(data);
}

uint8_t UPDI::lcds(reg::reg_t r) {
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x80 + r);
	return UPDI_io::get();
}

void UPDI::read_sib(uint8_t (& buffer)[16]) {
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0xE5);
	for (uint8_t i = 0; i < 16; i++) {
		buffer[i] = UPDI_io::get();
	}
}

uint8_t UPDI::lds_b(uint16_t address){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x04);
	UPDI_io::put(address & 0xFF);
	UPDI_io::put(address >> 8);	
	return UPDI_io::get();
}

uint16_t UPDI::lds_w(uint16_t address){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x05);
	UPDI_io::put(address & 0xFF);
	UPDI_io::put(address >> 8);
	return UPDI_io::get() | (UPDI_io::get() << 8);
}

void UPDI::sts_b(uint16_t address, uint8_t data){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x44);
	UPDI_io::put(address & 0xFF);
	UPDI_io::put(address >> 8);
	UPDI_io::get();
	UPDI_io::put(data);	
	UPDI_io::get();
}

void UPDI::sts_w(uint16_t address, uint16_t data){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x45);
	UPDI_io::put(address & 0xFF);
	UPDI_io::put(address >> 8);
	UPDI_io::get();
	UPDI_io::put(data & 0xFF);
	UPDI_io::put(data >> 8);
	UPDI_io::get();
}

uint8_t UPDI::ldptr_b(){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x28);
	return UPDI_io::get();
}

uint16_t UPDI::ldptr_w(){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x29);
	return UPDI_io::get() | (UPDI_io::get() << 8);
}

uint8_t UPDI::ld_b(){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x20);
	return UPDI_io::get();
}

uint16_t UPDI::ld_w(){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x21);
	return UPDI_io::get() | (UPDI_io::get() << 8);
}

uint8_t UPDI::ldinc_b(){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x24);
	return UPDI_io::get();
}

uint16_t UPDI::ldinc_w(){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x25);
	return UPDI_io::get() | (UPDI_io::get() << 8);
}

void UPDI::stptr_b(uint8_t address){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x68);
	UPDI_io::put(address);
	UPDI_io::get();
}

void UPDI::stptr_w(uint16_t address){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x69);
	UPDI_io::put(address & 0xFF);
	UPDI_io::put(address >> 8);
	UPDI_io::get();
}

void UPDI::st_b(uint8_t data){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x60);
	UPDI_io::put(data);
	UPDI_io::get();
}

void UPDI::st_w(uint16_t data){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x61);
	UPDI_io::put(data & 0xFF);
	UPDI_io::put(data >> 8);
	UPDI_io::get();
}

void UPDI::stinc_b(uint8_t data){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x64);
	UPDI_io::put(data);
	UPDI_io::get();
}

void UPDI::stinc_w(uint16_t data){
	UPDI_io::put(UPDI::SYNCH);
	UPDI_io::put(0x65);
	UPDI_io::put(data & 0xFF);
	UPDI_io::put(data >> 8);
	UPDI_io::get();
}