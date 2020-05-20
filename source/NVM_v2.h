/*
 * NVM_v2.h
 *
 * Created: 15-12-2017 10:59:53
 *  Author: JMR_2
 */


#ifndef NVM_V2_H_
#define NVM_V2_H_

#include <stdint.h>
#include "UPDI_lo_lvl.h"

namespace NVM_v2 {
  // *** Base Addresses ***

  enum base {
    NVM_base    = 0x1000,   /* Base address of the NVM controller */
    Sig_base    = 0x1100,   /* Base address of the signature */
    Fuse_base   = 0x1050,   /* Base address of the fuses */
    User_base   = 0x1080,   /* Base address of the User Row EEPROM */
    EEPROM_base = 0x1400    /* Base address of the main EEPROM */
  };
  // *** NVM Registers (offsets from NVN_base are enum default values) ***
  enum reg {
    CTRLA,
    CTRLB,
    STATUS,
    INTCTRL,
    INTFLAGS,
    Reg_5,
    DATA_lo,
    DATA_hi,
    ADDR_lo,
    ADDR_hi,
    ADDR_ext
  };

  // *** NVM Commands (write to CTRLA to execute) ***
  enum cmnd {
    NOCMD       = 0x00,    /* No command */
    NOOP        = 0x01,    /* No operation */
    FLWR        = 0x02,    /* Flash Write Enable */
    FLPER       = 0x08,    /* Flash Page Erase Enable */
    FLMPER2     = 0x09,    /* Flash 2-page Erase Enable */
    FLMPER4     = 0x0A,    /* Flash 4-page Erase Enable */
    FLMPER8     = 0x0B,    /* Flash 8-page Erase Enable */
    FLMPER16    = 0x0C,    /* Flash 16-page Erase Enable */
    FLMPER32    = 0x0D,    /* Flash 32-page Erase Enable */
    EEWR        = 0x12,    /* EEPROM Write Enable */
    EEERWR      = 0x13,    /* EEPROM Erase and Write Enable */
    EEBER       = 0x18,    /* EEPROM Byte Erase Enable */
    EEMBER2     = 0x19,    /* EEPROM 2-byte Erase Enable */
    EEMBER4     = 0x1A,    /* EEPROM 4-byte Erase Enable */
    EEMBER8     = 0x1B,    /* EEPROM 8-byte Erase Enable */
    EEMBER16    = 0x1C,    /* EEPROM 16-byte Erase Enable */
    EEMBER32    = 0x1D,    /* EEPROM 32-byte Erase Enable */
    CHER        = 0x20,    /* Erase Flash and EEPROM. EEPROM is skipped if EESAVE fuse is set. (UPDI access only.) */
    EECHER      = 0x30    /* Erase EEPROM */
  };
  // *** NVM Functions ***
  template <bool preserve_ptr>
  void command (uint8_t cmd) {
    uint32_t temp;
    /* preserve UPDI pointer if requested */
    if (preserve_ptr) temp = UPDI::ldptr_l();
    /* Execute NVM command */
    UPDI::sts_b_l(NVM_v2::NVM_base + NVM_v2::CTRLA, cmd);
    /* restore UPDI pointer if requested */
    if (preserve_ptr) UPDI::stptr_l(temp);
  }

  template <bool preserve_ptr>
  void wait () {
    uint32_t temp;
    /* preserve UPDI pointer if requested */
    if (preserve_ptr) temp = UPDI::ldptr_l();
    /* Wait while NVM is busy from previous operations */
    while (UPDI::lds_b_l(NVM_v2::NVM_base + NVM_v2::STATUS) & 0x03);
    /* restore UPDI pointer if requested */
    if (preserve_ptr) UPDI::stptr_l(temp);
  }

}

#endif /* NVM_H_ */
