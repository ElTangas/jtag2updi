/*
   UPDI_cmd.cpp

   Created: 23-11-2017 22:48:36
    Author: JMR_2
*/

// Includes
#include "UPDI_lo_lvl.h"
#include "sys.h"
#include "dbg.h"

// _l  versions have long (24-bit) address pointer
// otherwise, address pointer is 16-bit



// Keys
FLASH<uint8_t> UPDI::Chip_Erase[8] {0x65, 0x73, 0x61, 0x72, 0x45, 0x4D, 0x56, 0x4E};
FLASH<uint8_t> UPDI::NVM_Prog[8] {0x20, 0x67, 0x6F, 0x72, 0x50, 0x4D, 0x56, 0x4E};
FLASH<uint8_t> UPDI::UserRow_Write[8] {0x65, 0x74, 0x26, 0x73, 0x55, 0x4D, 0x56, 0x4E};

/*

STCS - Store to Control Register

*/

void UPDI::stcs(reg r, uint8_t data) {
#ifdef DEBUG_STCS
  DBG::updi_stcs(r,data);
#endif
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0xC0 + r);
  UPDI_io::put(data);
}


/*

LDCS - Load Control Register

*/

uint8_t UPDI::ldcs(reg r) {
#ifdef DEBUG_LDCS
  DBG::updi_ldcs(r);
#endif
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x80 + r);
#ifdef DEBUG_LDCS
  uint8_t resp= UPDI_io::get();
  DBG::updi_res(resp);
  return resp;
#else
  return UPDI_io::get();
#endif
}

/*

REP - Repeat Command

*/

void UPDI::rep(uint8_t repeats) {
#ifdef DEBUG_REP
  DBG::updi_rep(repeats);
#endif
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0xA0);
  UPDI_io::put(repeats);
}

/*

LDS - Load with Direct Addressing

*/

uint8_t UPDI::lds_b(uint16_t address) {
#ifdef DEBUG_LDS
  DBG::updi_lds(address);
#endif
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x04);
  UPDI_io::put(address & 0xFF);
  UPDI_io::put(address >> 8);
#ifdef DEBUG_LDS
  uint8_t resp= UPDI_io::get();
  DBG::updi_res(resp);
  return resp;
#else
  return UPDI_io::get();
#endif
}

uint8_t UPDI::lds_b_l(uint32_t address) {
#ifdef DEBUG_LDS
  DBG::updi_lds(address);
#endif
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x08);
  UPDI_io::put(address);
  UPDI_io::put(address >> 8);
  UPDI_io::put(address >> 16);
#ifdef DEBUG_LDS
  uint8_t resp= UPDI_io::get();
  DBG::updi_res(resp);
  return resp;
#else
  return UPDI_io::get();
#endif
}

uint16_t UPDI::lds_w(uint16_t address) {
#ifdef DEBUG_LDS
  DBG::updi_lds(address);
#endif
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x05);
  UPDI_io::put(address);
  UPDI_io::put(address >> 8);
#ifdef DEBUG_LDS
  uint16_t resp= UPDI_io::get()|(UPDI_io::get() << 8);
  DBG::updi_res(resp);
  return resp;
#else
  return UPDI_io::get() | (UPDI_io::get() << 8);
#endif
}

uint16_t UPDI::lds_w_l(uint32_t address) {
#ifdef DEBUG_LDS
  DBG::updi_lds(address);
#endif
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x09);
  UPDI_io::put(address);
  UPDI_io::put(address >> 8);
  UPDI_io::put(address >> 16);
#ifdef DEBUG_LDS
  uint16_t resp= UPDI_io::get()|(UPDI_io::get() << 8);
  DBG::updi_res(resp);
  return resp;
#else
  return UPDI_io::get() | (UPDI_io::get() << 8);
#endif
}


/*

STS - Byte oriented Store with Direct Addressing

*/

void UPDI::sts_b(uint16_t address, uint8_t data) {
#ifdef DEBUG_STS
  DBG::updi_sts(address,data);
#endif
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x44);
  UPDI_io::put(address & 0xFF);
  UPDI_io::put(address >> 8);
  UPDI_io::get();
  UPDI_io::put(data);
  UPDI_io::get();
}


void UPDI::sts_b_l(uint32_t address, uint8_t data) {
#ifdef DEBUG_STS
  DBG::updi_sts(address,data);
#endif
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x48);
  UPDI_io::put(address & 0xFF);
  UPDI_io::put((address >> 8) & 0xFF);
  UPDI_io::put((address >> 16) & 0xFF);
  UPDI_io::get();
  UPDI_io::put(data);
  UPDI_io::get();
}


/*

STS - Store with Direct Addressing

*/

void UPDI::sts_w(uint16_t address, uint16_t data) {
#ifdef DEBUG_STS
  DBG::updi_sts(address,data);
#endif
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x45);
  UPDI_io::put(address & 0xFF);
  UPDI_io::put(address >> 8);
  UPDI_io::get();
  UPDI_io::put(data & 0xFF);
  UPDI_io::put(data >> 8);
  UPDI_io::get();
}

void UPDI::sts_w_l(uint32_t address, uint16_t data) {
#ifdef DEBUG_STS
  DBG::updi_sts(address,data);
#endif
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x49);
  UPDI_io::put(address & 0xFF);
  UPDI_io::put((address >> 8) & 0xFF);
  UPDI_io::put((address >> 16) & 0xFF);
  UPDI_io::get();
  UPDI_io::put(data & 0xFF);
  UPDI_io::put(data >> 8);
  UPDI_io::get();
}

/*

STPTR - Set pointer for indirect addressing

*/

void UPDI::stptr_b(uint8_t address) {
  // No debug code here - byte-addressed STPTR is never actually used!
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x68);
  UPDI_io::put(address);
  UPDI_io::get();
}

void UPDI::stptr_w(uint16_t address) {
#ifdef DEBUG_STPTR
  DBG::updi_st_ptr_w(address);
#endif
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x69);
  UPDI_io::put(address & 0xFF);
  UPDI_io::put(address >> 8);
  UPDI_io::get();
}

void UPDI::stptr_l(uint32_t address) {
#ifdef DEBUG_STPTR
  DBG::updi_st_ptr_l(address);
#endif
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x6A);
  UPDI_io::put(address & 0xFF);
  UPDI_io::put((address >> 8) & 0xFF);
  UPDI_io::put((address >> 16) & 0xFF);
  UPDI_io::get();
}

void UPDI::stptr_p(const uint8_t* addr_p, uint8_t n) {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x68 + --n);
  UPDI_io::put(*(addr_p++));
  if (n >= 1)
  UPDI_io::put(*(addr_p++));
  if (n >= 2)
  UPDI_io::put(*addr_p);
  UPDI_io::get();
}

/*

LDPTR - Load pointer for indirect addressing

*/

uint8_t UPDI::ldptr_b() {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x28);
  return UPDI_io::get();
}

uint16_t UPDI::ldptr_w() {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x29);
  return UPDI_io::get() | (UPDI_io::get() << 8);
}

uint32_t UPDI::ldptr_l() {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x2A);
  return UPDI_io::get() | (UPDI_io::get() << 8) | (((uint32_t)UPDI_io::get()) << 16);
}
uint8_t UPDI::ld_b() {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x20);
  return UPDI_io::get();
}

uint16_t UPDI::ld_w() {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x21);
  return UPDI_io::get() | (UPDI_io::get() << 8);
}

uint8_t UPDI::ldinc_b() {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x24);
  return UPDI_io::get();
}

uint16_t UPDI::ldinc_w() {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x25);
  return UPDI_io::get() | (UPDI_io::get() << 8);
}


void UPDI::st_b(uint8_t data) {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x60);
  UPDI_io::put(data);
  UPDI_io::get();
}

void UPDI::st_w(uint16_t data) {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x61);
  UPDI_io::put(data & 0xFF);
  UPDI_io::put(data >> 8);
  UPDI_io::get();
}

void UPDI::stinc_b(uint8_t data) {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x64);
  UPDI_io::put(data);
  UPDI_io::get();
}

void UPDI::stinc_w(uint16_t data) {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x65);
  UPDI_io::put(data & 0xFF);
  UPDI_io::put(data >> 8);
  UPDI_io::get();
}

/*

STINC noget variants
For use when the RSD bit is set in control register A
This is done to improve performance when doing burst writes

stinc_b_noget is copy of stinc_b without the get()

stinc_b_b_noget is copy of stinc_w without the get(), and with data expressed as two
bytes instead of a word, since this is more convenient in the single place that it is called from

*/

void UPDI::stinc_b_noget(uint8_t data) {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x64);
  UPDI_io::put(data);
}

void UPDI::stinc_b_b_noget(uint8_t data0, uint8_t data1) {
  UPDI_io::put(UPDI::SYNCH);
  UPDI_io::put(0x65);
  UPDI_io::put(data0);
  UPDI_io::put(data1);
}
