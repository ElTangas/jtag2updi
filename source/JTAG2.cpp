/*
   STK500.cpp

   Created: 08-12-2017 19:47:27
    Author: JMR_2
*/

#include "JTAG2.h"
#include "JICE_io.h"
#include "NVM.h"
#include "NVM_v2.h"
#include "crc16.h"
#include "UPDI_hi_lvl.h"
#include "dbg.h"

// *** Writeable Parameter Values ***
uint8_t JTAG2::PARAM_EMU_MODE_VAL;
JTAG2::baud_rate JTAG2::PARAM_BAUD_RATE_VAL;
uint8_t JTAG2::ConnectedTo;
// *** STK500 packet ***
JTAG2::packet_t JTAG2::packet;

// Local objects
namespace {
  // *** Local variables ***
  uint16_t flash_pagesize;
  uint8_t eeprom_pagesize;
  uint8_t nvm_version = 1;

  // *** Local functions declaration ***
  void NVM_fuse_write (uint16_t address, uint8_t data);
  void NVM_v2_write (uint32_t address, uint16_t length, uint8_t flash_cmd);
  void NVM_buffered_write(uint16_t address, uint16_t length, uint8_t buff_size, uint8_t write_type);
  void include_extra_info (const uint8_t sernumlen);
  void set_nvm_version();

  // *** Signature response message ***
  FLASH<uint8_t> sgn_resp[29] {
    JTAG2::RSP_SIGN_ON, 1,
    1, JTAG2::PARAM_FW_VER_M_MIN_VAL, JTAG2::PARAM_FW_VER_M_MAJ_VAL, JTAG2::PARAM_HW_VER_M_VAL,
    1, JTAG2::PARAM_FW_VER_S_MIN_VAL, JTAG2::PARAM_FW_VER_S_MAJ_VAL, JTAG2::PARAM_HW_VER_S_VAL,
    0, 0, 0, 0, 0, 0,
    'J', 'T', 'A', 'G', 'I', 'C', 'E', ' ', 'm', 'k', 'I', 'I', 0
  };
}

// *** Packet functions ***
bool JTAG2::receive() {
  while (JICE_io::get() != MESSAGE_START) {
    #ifndef DISABLE_HOST_TIMEOUT
    if ((SYS::checkTimeouts() & WAIT_FOR_HOST)) return false;
    #endif
  }
  uint16_t crc = CRC::next(MESSAGE_START);
  for (uint16_t i = 0; i < 6; i++) {
    crc = CRC::next(packet.raw[i] = JICE_io::get(), crc);
  }
  if (packet.size_word[0] > sizeof(packet.body)) return false;
  if (JICE_io::get() != TOKEN) return false;
  crc = CRC::next(TOKEN, crc);
  for (uint16_t i = 0; i < packet.size_word[0]; i++) {
    crc = CRC::next(packet.body[i] = JICE_io::get(), crc);
  }
  if ((uint16_t)(JICE_io::get() | (JICE_io::get() << 8)) != crc) return false;
  return true;
}

void JTAG2::answer() {
  uint16_t crc = CRC::next(JICE_io::put(MESSAGE_START));
  for (uint16_t i = 0; i < 6; i++) {
    crc = CRC::next(JICE_io::put(packet.raw[i]), crc);
  }
  crc = CRC::next(JICE_io::put(TOKEN), crc);
  for (uint16_t i = 0; i < packet.size_word[0]; i++) {
    crc = CRC::next(JICE_io::put(packet.body[i]), crc);
  }
  JICE_io::put(crc);
  JICE_io::put(crc >> 8);
}

void JTAG2::delay_exec() {
  // wait for transmission complete
  JICE_io::flush();
  // set baud rate
  JICE_io::set_baud(PARAM_BAUD_RATE_VAL);
}

// *** Set status function ***
void JTAG2::set_status(response status_code) {
  packet.size_word[0] = 1;
  packet.body[0] = status_code;
}
void JTAG2::set_status(response status_code, uint8_t param) {
    packet.size_word[0] = 2;
    packet.body[0] = status_code;
    packet.body[1] = param;
}

// *** General command functions ***

void JTAG2::sign_on() {
  // Initialize JTAGICE2 variables
  JTAG2::PARAM_EMU_MODE_VAL = 0x02;
  JTAG2::PARAM_BAUD_RATE_VAL = JTAG2::BAUD_19200;
  // Send sign on message
  packet.size_word[0] = sizeof(sgn_resp);
  for (uint8_t i = 0; i < sizeof(sgn_resp); i++) {
    packet.body[i] = sgn_resp[i];
  }
  JTAG2::ConnectedTo |= 0x02; //now connected to host
}

void JTAG2::get_parameter() {
  uint8_t & status = packet.body[0];
  uint8_t & parameter = packet.body[1];
  switch (parameter) {
    case PARAM_HW_VER:
      packet.size_word[0] = 3;
      packet.body[1] = PARAM_HW_VER_M_VAL;
      packet.body[2] = PARAM_HW_VER_S_VAL;
      break;
    case PARAM_FW_VER:
      packet.size_word[0] = 5;
      packet.body[1] = PARAM_FW_VER_M_MIN_VAL;
      packet.body[2] = PARAM_FW_VER_M_MAJ_VAL;
      packet.body[3] = PARAM_FW_VER_S_MIN_VAL;
      packet.body[4] = PARAM_FW_VER_S_MAJ_VAL;
      break;
    case PARAM_EMU_MODE:
      packet.size_word[0] = 2;
      packet.body[1] = PARAM_EMU_MODE_VAL;
      break;
    case PARAM_BAUD_RATE:
      packet.size_word[0] = 2;
      packet.body[1] = PARAM_BAUD_RATE_VAL;
      break;
    case PARAM_VTARGET:
      packet.size_word[0] = 3;
      packet.body[1] = PARAM_VTARGET_VAL & 0xFF;
      packet.body[2] = PARAM_VTARGET_VAL >> 8;
      break;
    default:
      set_status(RSP_ILLEGAL_PARAMETER);
      return;
  }
  status = RSP_PARAMETER;
  return;
}

void JTAG2::set_parameter() {
  uint8_t param_type = packet.body[1];
  uint8_t param_val = packet.body[2];
  switch (param_type) {
    case PARAM_EMU_MODE:
      PARAM_EMU_MODE_VAL = param_val;
      break;
    case PARAM_BAUD_RATE:
      // check if baud rate parameter is valid
      if ((param_val >= BAUD_LOWER) && (param_val <= BAUD_UPPER)) {
        PARAM_BAUD_RATE_VAL = (baud_rate)param_val;
        break;
      }
      // else fall through (invalid baud rate)
    default:
      set_status(RSP_ILLEGAL_PARAMETER);
      return;
  }
  set_status(RSP_OK);
}

void JTAG2::set_device_descriptor() {
  flash_pagesize = packet.body[244] + 256*packet.body[245];
  eeprom_pagesize = packet.body[246];
  // Now they've told us what we're talking to, and we will try to connect to it
  /* Initialize or enable UPDI */
  UPDI_io::put(UPDI_io::double_break);
  UPDI::stcs(UPDI::reg::Control_A, 0x06);
  set_nvm_version();
  JTAG2::ConnectedTo |= 0x01; // connected to target
  set_status(RSP_OK);
}

// *** Target mode set functions ***
// *** Sets MCU in program mode, if possibe ***
void JTAG2::enter_progmode() {
  const uint8_t system_status = UPDI::CPU_mode<0xEF>();
  switch (system_status) {
    // reset in progress, may be caused by WDT
    case 0x21:
      /* fall-thru */
    // in normal operating mode, reset held (why can reset be held here, anyway? Needs investigation)
    case 0xA2:
      /* fall-thru */
    // in normal operation mode
    case 0x82:
      // Reset the MCU now, to prevent the WDT (if active) to reset it at an unpredictable moment
      if (!UPDI::CPU_reset()){ //if we timeout while trying to reset, we are not communicating with chip, probably wiring error.
        set_status(RSP_NO_TARGET_POWER);
        break;
      }
      // At this point we need to check if the chip is locked, if so don't attempt to enter program mode
      // This is because the previous chip reset may enable a lock bit that was just written (it only becomes active after reset).
      if (UPDI::CPU_mode<0x01>()) {
        set_status(RSP_ILLEGAL_MCU_STATE, system_status | 0x01); // return system status (bit 0 will be set to indicate the MCU is locked)
        break;
      }
      // Now we have time to enter program mode (this mode also disables the WDT)
      // Write NVM unlock key (allows read access to all addressing space)
      UPDI::write_key(UPDI::NVM_Prog);
      // Request reset
      if (!UPDI::CPU_reset()){ //if we timeout while trying to reset, we are not communicating with chip, probably wiring error.
        set_status(RSP_NO_TARGET_POWER);
        break;
      }
      /* fall-thru */
    // already in program mode
    case 0x08: //make sure we're really in programming mode
      if (nvm_version == 1) {
        // For NVM version 1 parts, there's a page buffer.
        // It might have data in it if something else was writing to the flash when
        // we so rudely interrupted, so better clear the page buffer, just in case.
        UPDI::sts_b(NVM::NVM_base | NVM::CTRLA, NVM::PBC);
      } else {
        // NVM v2 devices can have error codes in NVMCTRL.STATUS
        // They also require that NVMCTRLA be set to NOOP/NOCMD before use
        // So we should do this here!
        #if defined(DEBUG_ON)
          uint8_t NVM_Status = UPDI::lds_b_l(NVM_v2::NVM_base + NVM_v2::STATUS);
          uint8_t NVM_Cmnd = UPDI::lds_b_l(NVM_v2::NVM_base + NVM_v2::CTRLA);
          if (NVM_Status || NVM_Cmnd ) {
            DBG::debug('N', NVM_Status,NVM_Cmnd);
          }
        #endif
        // Wait for completion of any previous NVM command then clear it with NOOP
        NVM_v2::wait<false>();
        UPDI::sts_b_l(NVM_v2::NVM_base + NVM_v2::STATUS, 0);
        NVM_v2::command<false>(NVM_v2::NOOP);
      }
      // Turn on LED to indicate program mode
      SYS::setLED();
      #if defined(DEBUG_ON)
        // report the chip revision
        DBG::debug('R',UPDI::lds_b_l(0x0F01));
      #endif
      set_status(RSP_OK);
      // Optional debug info; moved to separate function to declutter code
      include_extra_info( (nvm_version == 2) ? 15 : 9 );
      break;
    default:
      // If we're somehow NOT in programming mode now, that's no good - inform host of this unfortunate state of affairs
      set_status(RSP_ILLEGAL_MCU_STATE, system_status); // return whatever system status caused this error
  }
}

// *** Sets MCU in normal runnning mode, if possibe ***
void JTAG2::leave_progmode() {
  const uint8_t system_status = UPDI::CPU_mode<0xEF>();
  bool reset_ok=0;
  switch (system_status) {
    // in program mode
    case 0x08:
      // Wait for completion of previous commands
      if (nvm_version==2) {
        NVM_v2::wait<false>();
      } else {
        NVM::wait<false>();
      }
      // Request reset; this should also reset the NVM
      reset_ok=UPDI::CPU_reset();
      // already in normal mode
    /* fall-thru */
    case 0x82:
      // Turn off LED to indicate normal mode
      SYS::clearLED();
      if (reset_ok) {
        set_status(RSP_OK);
      } else {
        set_status(RSP_NO_TARGET_POWER); //this is a strange situation indeed, but tell the host anyway!
      }
      break;
    // in other modes fail and inform host of wrong mode
    default:
      set_status(RSP_ILLEGAL_MCU_STATE, 0x01);
  }
}

// The final command to make the chip go back into normal mode, shutdown UPDI, and start running normally
void JTAG2::go() {
  UPDI::stcs(UPDI::reg::Control_B, 0x04); //set UPDISIS to tell it that we're done and it can stop running the UPDI peripheral.
  JTAG2::ConnectedTo &= ~(0x01); //record that we're no longer talking to the target
  set_status(RSP_OK);
}


  // *** Read/Write/Erase functions ***
  void JTAG2::read_mem() {
    if (UPDI::CPU_mode() != 0x08){
      // fail if not in program mode
      set_status(RSP_ILLEGAL_MCU_STATE, 0x01);
    }
    else {
      // in program mode
      const uint16_t NumBytes = (packet.body[3] << 8) | packet.body[2];
      // Calculate size of address
      uint8_t addr_size = (nvm_version == 1) ? 2 : 3;
      // Set UPDI pointer to address
      UPDI::stptr_p(&(packet.body[6]), addr_size);
      // Read block
      UPDI::rep(NumBytes - 1);
      packet.body[1] = UPDI::ldinc_b();
      for (uint16_t i = 2; i <= NumBytes; i++) {
        packet.body[i] = UPDI_io::get();
      }
      packet.size_word[0] = NumBytes + 1;
      packet.body[0] = RSP_MEMORY;
    }
  }


  void JTAG2::write_mem() {
    if (UPDI::CPU_mode() != 0x08) {
      // fail if not in program mode
      set_status(RSP_ILLEGAL_MCU_STATE, 0x01);
    }
    else {
      // in program mode
      const uint8_t mem_type = packet.body[1];
      const uint16_t length = packet.body[2] | (packet.body[3] << 8);             /* number of bytes to write */
      const bool is_flash = ((mem_type == MTYPE_FLASH) || (mem_type == MTYPE_BOOT_FLASH));
      if (nvm_version == 1) {
        const uint16_t address = packet.body[6] | (packet.body[7] << 8);
        const uint8_t buff_size = is_flash ? flash_pagesize : eeprom_pagesize;
        const uint8_t write_cmnd = is_flash ? NVM::WP : NVM::ERWP;
        switch (mem_type) {
          case MTYPE_FUSE_BITS:
          case MTYPE_LOCK_BITS:
            NVM_fuse_write (address, packet.body[10]);
            break;
          case MTYPE_FLASH:
          case MTYPE_BOOT_FLASH:
          case MTYPE_EEPROM_XMEGA:
          case MTYPE_EEPROM:
          case MTYPE_USERSIG:
            NVM_buffered_write(address, length, buff_size, write_cmnd);
            break;
          default:
            set_status(RSP_ILLEGAL_MEMORY_TYPE);
            return;
        }
      } else {
        const uint32_t address = (((uint32_t)packet.body[8]) << 16) | (((uint16_t)packet.body[7]) << 8) | packet.body[6];
        uint8_t write_cmd = NVM_v2::EEERWR;
        switch (mem_type) {
          case MTYPE_FLASH:
          case MTYPE_BOOT_FLASH:
          case MTYPE_USERSIG:
            write_cmd = NVM_v2::FLWR;
          /* fall-thru */
          case MTYPE_FUSE_BITS:
          case MTYPE_LOCK_BITS:
          case MTYPE_EEPROM_XMEGA:
          case MTYPE_EEPROM:
            NVM_v2_write(address, length, write_cmd);
            break;
          default:
            set_status(RSP_ILLEGAL_MEMORY_TYPE);
            return;
        }
      }
      set_status(RSP_OK);
    }
  }

void JTAG2::erase() {
  const uint8_t erase_type = packet.body[1];
  bool reset_ok=0;
  switch (erase_type) {
    case XMEGA_ERASE_CHIP:
      // Write Chip Erase key
      UPDI::write_key(UPDI::Chip_Erase);
      // Request reset
      reset_ok=UPDI::CPU_reset();
      if (!reset_ok){
        set_status(RSP_NO_TARGET_POWER); //if the reset failed, inform host, break out because the rest ain't gonna work
        break;
      }
      // Erase chip process exits program mode, reenter...
      enter_progmode();
      break;
    case XMEGA_ERASE_APP_PAGE:
    case XMEGA_ERASE_BOOT_PAGE:
    case XMEGA_ERASE_USERSIG: {
    const uint32_t address = (((uint32_t)packet.body[4]) << 16) | (((uint16_t) packet.body[3]) << 8) | packet.body[2];
      if (nvm_version == 1) {
        // If NVM v1, skip page erase for usersig memory, we use erase/write during the write step
        if (erase_type != XMEGA_ERASE_USERSIG) {
          NVM::wait<false>();
          UPDI::sts_b(address, 0xFF);
          NVM::command<false>(NVM::ER);
        }
      } else {
        // Wait for completion of any previous NVM command then clear it with NOOP
        NVM_v2::wait<false>();
        NVM_v2::command<false>(NVM_v2::NOOP);
        // erase flash page
        NVM_v2::command<false>(NVM_v2::FLPER);
        UPDI::sts_b_l(address, 0xFF);
      }
      set_status(RSP_OK);
      break;}
    case XMEGA_ERASE_EEPROM_PAGE:
      // Ignore page erase for eeprom, we use erase/write during the write step
      set_status(RSP_OK);
      break;
    default:
      set_status(RSP_FAILED);
  }
}

// *** Local functions definition ***
// These functions will be called only once; They just exist as separate functions to declutter the main flow of code.
// The C++ optimizer will most likely inline them during compilation.
// They are not for general use and can only be called from the places where they are called.
namespace {
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

  void NVM_v2_write (uint32_t address, uint16_t length, uint8_t write_cmd) {
    uint16_t current_byte_index = 10;         /* Index of the first byte to send inside the JTAG2 command body */
    // Wait for completion of any previous NVM command then clear it with NOOP
    NVM_v2::wait<false>();
    NVM_v2::command<false>(NVM_v2::NOOP);
    // Send the write command
    NVM_v2::command<false>(write_cmd);

    if (length == 1) { //if just one byte write it with no looping
      // write to memory
      UPDI::sts_b_l(address, JTAG2::packet.body[current_byte_index]);
    } else {
      if (length < 4 || write_cmd != NVM_v2::FLWR ) // byte write for short flash writes and all eeprom writes
      {
        uint8_t bytes_remaining = length - 1;         /* number of bytes to write */
        NVM_v2::command<false>(write_cmd);
        // Set UPDI pointer to address
        UPDI::stptr_l(address);
        UPDI::rep(bytes_remaining);
        UPDI::stinc_b(JTAG2::packet.body[current_byte_index++]);
        for (uint8_t i = bytes_remaining; i; i--) {
          UPDI_io::put(JTAG2::packet.body[current_byte_index++]);
          UPDI_io::get();
        }
      } else { //word write
        int8_t words_remaining= (length >> 1)-1;
        UPDI::stptr_l(address);
        #ifndef NO_ACK_WRITE
          uint16_t firstword=JTAG2::packet.body[current_byte_index]|(JTAG2::packet.body[current_byte_index+1] << 8);
          current_byte_index+=2;
          UPDI::rep(words_remaining);
          UPDI::stinc_w(firstword);
          for (uint8_t i = words_remaining;i;i--) {
            UPDI_io::put(JTAG2::packet.body[current_byte_index++]);
            UPDI_io::put(JTAG2::packet.body[current_byte_index++]);
            UPDI_io::get();
          }
          if (length & 0x01) { //in case they send us odd-length write command...
            UPDI::stinc_b(JTAG2::packet.body[current_byte_index++]);
          }
        #else //writing without ACK
          UPDI::stcs(UPDI::reg::Control_A, 0x0E);
          UPDI::rep(words_remaining);
          UPDI::stinc_b_b_noget(JTAG2::packet.body[current_byte_index],JTAG2::packet.body[current_byte_index+1]);
          current_byte_index+=2;
          for (uint8_t i = words_remaining;i;i--) {
            UPDI_io::put(JTAG2::packet.body[current_byte_index++]);
            UPDI_io::put(JTAG2::packet.body[current_byte_index++]);
          }
          if (length & 0x01) { //in case they send us odd-length write command...
            UPDI::stinc_b_noget(JTAG2::packet.body[current_byte_index++]);
          }
          UPDI::stcs(UPDI::reg::Control_A, 0x06);
        #endif
      }
    }
    // Check for NVN errors and clear them (only useful for debug mode)
    #if defined(DEBUG_ON)
    uint8_t stat = UPDI::lds_b_l(NVM_v2::NVM_base + NVM_v2::STATUS);
    if (stat > 3) {
      uint8_t cmd=UPDI::lds_b_l(NVM_v2::NVM_base + NVM_v2::CTRLA);
      DBG::debug('f',stat,cmd,0);
      UPDI::sts_b_l(NVM_v2::NVM_base + NVM_v2::STATUS, 0);
    }
    #endif
  }


  void NVM_buffered_write(const uint16_t address, const uint16_t length, const uint8_t buff_size, const uint8_t write_cmnd) {
    uint8_t current_byte_index = 10;          /* Index of the first byte to send inside the JTAG2 command body */
    uint16_t bytes_remaining = length;        /* number of bytes to write */

    // Sends a block of bytes from the command body to memory, using the UPDI interface
    // On entry, the UPDI pointer must already point to the desired address
    // On exit, the UPDI pointer points to the next byte after the last one written
    // Returns updated index into the command body, pointing to the first unsent byte.
    auto updi_send_block = [] (uint8_t count, uint8_t index) {
      count--;
      NVM::wait<true>();
      #ifndef NO_ACK_WRITE
        UPDI::rep(count);
        UPDI::stinc_b(JTAG2::packet.body[index]);
        for (uint8_t i = count; i; i--) {
          UPDI_io::put(JTAG2::packet.body[++index]);
          UPDI_io::get();
        }
      #else
        UPDI::stcs(UPDI::reg::Control_A, 0x0E);
        UPDI::rep(count);
        UPDI::stinc_b_noget(JTAG2::packet.body[index]);
        for (uint8_t i = count; i; i--) {
          UPDI_io::put(JTAG2::packet.body[++index]);
        }
        UPDI::stcs(UPDI::reg::Control_A, 0x06);
      #endif
      return ++index;
    };

    // Setup UPDI pointer for block transfer
    UPDI::stptr_w(address);
    /* Check address alignment, calculate number of unaligned bytes to send */
    uint8_t unaligned_bytes = (-address & (buff_size - 1));
    if (unaligned_bytes > bytes_remaining) unaligned_bytes = bytes_remaining;
    /* If there are unaligned bytes, they must be sent first */
    if (unaligned_bytes) {
      // Send unaligned block
      current_byte_index = updi_send_block(unaligned_bytes, current_byte_index);
      bytes_remaining -= unaligned_bytes;
      NVM::command<true>(write_cmnd);
    }
    while (bytes_remaining) {
      /* Send a buff_size amount of bytes */
      if (bytes_remaining >= buff_size) {
        current_byte_index = updi_send_block(buff_size, current_byte_index);
        bytes_remaining -= buff_size;
      }
      /* Send a NumBytes amount of bytes */
      else {
        current_byte_index = updi_send_block(bytes_remaining, current_byte_index);
        bytes_remaining = 0;
      }
      NVM::command<true>(write_cmnd);
    }
  }

  void include_extra_info (const uint8_t sernumlen) {
    #if defined(INCLUDE_EXTRA_INFO_JTAG)
    // get the REV ID - I believe (will be confirming with Microchip support) that this is the silicon revision ID
    // this is particularly important with some of these chips - the tinyAVR and megaAVR parts do have differences
    // between silicon revisions, and AVR-DA-series initial rev is a basket case and will almost certainly be respun
    // before volume availability (take a look at the errata if you haven't already. There are fewer entries than on
    // tinyAVR 1-series, but they're BIG... like "digital input disabled after using analog input" "you know that
    // flashmap we said could be used with ld and st? We lied, you can only use it with ld, and even then, there are
    // cases where it won't work" "we didn't do even basic testing of 64-pin version, so stuff on those those pins is
    // hosed" (well, they didn't say they didn't test it, but it's bloody obvious that's how it happened))
    using namespace JTAG2;
    packet.size_word[0]=9+sernumlen;
    packet.body[1]='R';
    packet.body[2]='E';
    packet.body[3]='V';
    packet.body[4]=UPDI::lds_b_l(0x0F01);
    //hell, might as well get the chip serial number too!
    packet.body[5]= 'S';
    packet.body[6]= 'E';
    packet.body[7]= 'R';
    if(nvm_version==2){
      UPDI::stptr_l(0x1110);
    } else {
      UPDI::stptr_w(0x1103);
    }
    UPDI::rep(sernumlen);
    packet.body[8]=UPDI::ldinc_b();
    for(uint8_t i=9;i<(9+sernumlen);i++){
      packet.body[i]=UPDI_io::get();
    }
    #ifdef DEBUG_ON
    DBG::debug("Serial Number: ");
    uint8_t *ptr=(uint8_t*)(&packet.body[8]);
    DBG::debug(ptr,sernumlen+1,0,1);
    #endif
    #elif defined(DEBUG_ON) //if we're not adding extended info in JTAG, but debug is on, I guess we should still report this...
    uint8_t sernumber[10];
    if(nvm_version==2){
      UPDI::stptr_l(0x1100);
    } else {
      UPDI::stptr_w(0x1100);
    }
    UPDI::rep(9);
    sernumber[0]=UPDI::ldinc_b();
    for(uint8_t i=1;i<10;i++){
      sernumber[i]=UPDI_io::get();
    }
    #endif
  }

  void set_nvm_version(){
    #ifdef DEDUCE_NVM_VERSION_FROM_SIB
      using namespace JTAG2;
      // Cast body[4]...[19] as sub-array to receive SIB data
      auto & sib = *(uint8_t (*)[16]) &packet.body[4];
      UPDI::read_sib(sib);
      #if defined(DEBUG_ON)
      DBG::debug(sib, 16, 1);
      #endif
      if (sib[10] == '2') {
        nvm_version = 2;
        } else {
        nvm_version = 1;
      }
      #ifdef INCLUDE_EXTRA_INFO_JTAG
      packet.size_word[0] = 24;
      packet.body[1] = 'S';
      packet.body[2] = 'I';
      packet.body[3] = 'B';
      /* SIB has already been stored in [4]...[19] */
      packet.body[20]= 'N';
      packet.body[21]= 'V';
      packet.body[22]= 'M';
      packet.body[23]= (nvm_version==2?'2':'1');
      #endif
    #else
      nvm_version = (flash_pagesize >= 256) ? 2 : 1;
    #endif
  }
}


