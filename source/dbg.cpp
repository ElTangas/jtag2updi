/*
 * dbg.h
 *
 * Created:5/11/2020
 *  Author: Spence Konde  (tindie.com/stores/drazzy)
 * github.com/SpenceKonde
 */

#include <avr/io.h>
#include <stdio.h>
#include "dbg.h"
#include "sys.h"



#if defined(DEBUG_ON)

void DBG::updi_st_ptr_l(uint32_t address) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("st_ptr ");
    DBG::debugWriteByte(' ');
    DBG::debugWriteHex(address>>16);
    DBG::debugWriteHex(address>>8);
    DBG::debugWriteHex(address);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::updi_st_ptr_w(uint16_t address) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("st_ptr ");
    DBG::debugWriteHex(address>>8);
    DBG::debugWriteByte(' ');
    DBG::debugWriteHex(address);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}


void DBG::updi_lds(uint32_t address) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("lds ");
    DBG::debugWriteHex(address>>16);
    DBG::debugWriteHex(address>>8);
    DBG::debugWriteHex(address);
    DBG::debugWriteByte(' ');
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::updi_lds(uint16_t address) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("lds ");
    DBG::debugWriteHex(address>>8);
    DBG::debugWriteHex(address);
    DBG::debugWriteByte(' ');
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::updi_sts(uint32_t address, uint16_t data) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("sts ");
    DBG::debugWriteHex(address>>16);
    DBG::debugWriteHex(address>>8);
    DBG::debugWriteHex(address);
    DBG::debugWriteByte(' ');
    DBG::debugWriteHex(data);
    DBG::debugWriteByte(' ');
    DBG::debugWriteHex(data>>8);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::updi_sts(uint32_t address, uint8_t data) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("sts ");
    DBG::debugWriteHex(address>>16);
    DBG::debugWriteHex(address>>8);
    DBG::debugWriteHex(address);
    DBG::debugWriteByte(' ');
    DBG::debugWriteHex(data);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::updi_sts(uint16_t address, uint16_t data) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("sts ");
    DBG::debugWriteHex(address>>8);
    DBG::debugWriteHex(address);
    DBG::debugWriteByte(' ');
    DBG::debugWriteHex(data);
    DBG::debugWriteByte(' ');
    DBG::debugWriteHex(data>>8);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::updi_sts(uint16_t address, uint8_t data) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("sts ");
    DBG::debugWriteHex(address>>8);
    DBG::debugWriteHex(address);
    DBG::debugWriteByte(' ');
    DBG::debugWriteHex(data);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}


void DBG::updi_ldcs(uint8_t command) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("ldcs ");
    DBG::debugWriteHex(command);
    DBG::debugWriteByte(' ');
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::updi_stcs(uint8_t command, uint8_t data) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("stcs ");
    DBG::debugWriteHex(command);
    DBG::debugWriteByte(' ');
    DBG::debugWriteHex(data);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}


void DBG::updi_rep(uint8_t reps) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("rep ");
    DBG::debugWriteHex(reps);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::updi_reset() {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("RESET ");
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}
void DBG::updi_reset_on() {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("RESET ON");
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}
void DBG::updi_reset_off() {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("RESET OFF");
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::updi_post_reset(uint8_t mode) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("NewMode ");
    DBG::debugWriteHex(mode);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::updi_res(uint32_t data, uint8_t isaddr) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    if (isaddr) {
      DBG::debugWriteHex(data>>16);
      DBG::debugWriteHex(data>>8);
      DBG::debugWriteHex(data);
    } else {
      DBG::debugWriteHex(data);
      DBG::debugWriteByte(' ');
      DBG::debugWriteHex(data>>8);
      DBG::debugWriteByte(' ');
      DBG::debugWriteHex(data>>16);
    }
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::updi_res(uint16_t data, uint8_t isaddr) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    if (isaddr) {
      DBG::debugWriteHex(data>>8);
      DBG::debugWriteHex(data);
    } else {
      DBG::debugWriteHex(data);
      DBG::debugWriteByte(' ');
      DBG::debugWriteHex(data>>8);
    }
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::updi_res(uint8_t data) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteHex(data);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::updi_key(){
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteStr("key ");
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}

void DBG::initDebug(void) {
  #ifdef USE_SPIDEBUG
    DDR(SPIPORT)|=1<<SCKPIN;
    DDR(SPIPORT)|=1<<MOSIPIN;
    DDR(SPIPORT)|=1<<SSPIN;
    PORT(SPIPORT)|=1<<SSPIN;
  #endif
  #if defined(XAVR)
    // Set clock speed to maximum (default 20MHz, or 16MHz set by fuse)
    #if defined(USE_USARTDEBUG)
      #if !defined(USE_EXTERNAL_OSCILLATOR) && !defined(__AVR_DX__)
        #if (F_CPU==20000000UL) //this means we are on the 20MHz oscillator
          #ifdef UARTBAUD3V
            int8_t sigrow_val = SIGROW.OSC20ERR3V;
          #else
            int8_t sigrow_val = SIGROW.OSC20ERR5V;
          #endif
        #else //we are on 16MHz one
          #ifdef UARTBAUD3V
            int8_t sigrow_val = SIGROW.OSC16ERR3V;
          #else
            int8_t sigrow_val = SIGROW.OSC16ERR5V;
          #endif
        #endif
        uint32_t baud_setting = ((8 * F_CPU) / BAUDRATE);
        baud_setting *= (1024 + sigrow_val);
        baud_setting += 512;
        baud_setting /= 1024;
      #else
        uint32_t baud_setting = (((16 * F_CPU) / BAUDRATE) + 1) / 2;
      #endif
      DEBUGBAUDREG=(uint16_t)baud_setting;
      DEBUG_USART.CTRLB = USART_RXMODE_CLK2X_gc | USART_TXEN_bm;
      DDR(DEBUG_TX_PORT)|=1<<DEBUG_TX_PIN;
      PORT(DEBUG_TX_PORT)|=1<<DEBUG_TX_PIN;
    #elif defined(USE_SPIDEBUG)
      SPIDEBUG.CTRLA=SPI_MASTER_bm|SPI_ENABLE_bm|((SPI_PRESC_gm|SPI_CLK2X_bm) & SPIPRESC);
      SPIDEBUG.CTRLB=SPI_SSD_bm;
    #endif
  #else //not XAVR
    #if defined(USE_SPIDEBUG)
      DDR(SPIPORT)|=1<<SCKPIN;
      DDR(SPIPORT)|=1<<MOSIPIN;
      DDR(SPIPORT)|=1<<SSPIN;
      PORT(SPIPORT)|=1<<SSPIN;
      SPSR=(SPIPRESC&1); //set SPI2X if wanted
      SPCR=(1<<MSTR) |(1<<SPE)| ((SPIPRESC>>1)&0x03);
    #elif defined(USE_USARTDEBUG)
      DEBUG_UCSRB=(1<<TXEN);
      DEBUGFLAGS=(1<<U2X);
    #endif
  #endif
}

void DBG::debug(char prefix, uint8_t data0, uint8_t data1, uint8_t data2){
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteByte(prefix);
    DBG::debugWriteHex(data0);
    DBG::debugWriteByte(' ');
    DBG::debugWriteHex(data1);
    DBG::debugWriteByte(' ');
    DBG::debugWriteHex(data2);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}
void DBG::debug(char prefix, uint8_t data0, uint8_t data1){
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteByte(prefix);
    DBG::debugWriteHex(data0);
    DBG::debugWriteByte(' ');
    DBG::debugWriteHex(data1);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}
void DBG::debug(char prefix, uint8_t data0){
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    DBG::debugWriteByte(0x0D);
    DBG::debugWriteByte(0x0A);
    DBG::debugWriteByte(prefix);
    DBG::debugWriteHex(data0);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}


void DBG::debug(const char *str, uint8_t newline) {
  DBG::debug((const uint8_t *)str, strlen(str), newline, 0);
}
void DBG::debug(const uint8_t *data, size_t datalen , uint8_t newline, uint8_t ashex) {
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)&=~(1<<SSPIN); //lower SSPIN
    #endif
    if (newline) {
      DBG::debugWriteByte(0x0D);
      DBG::debugWriteByte(0x0A);
    }
    DBG::debugWriteBytes(data,datalen,ashex);
    #ifdef USE_SPIDEBUG
      PORT(SPIPORT)|=1<<SSPIN; //raise SSPIN
    #endif
}
void DBG::debugWriteStr(const char *str) {
  DBG::debugWriteBytes((const uint8_t *)str, strlen(str),0);
}
void DBG::debugWriteBytes(const uint8_t *data, size_t datalen, uint8_t ashex) {
    while (datalen--) {
      if(ashex){
        DBG::debugWriteHex(*data++);
      } else {
        DBG::debugWriteByte(*data++);
      }
    }

}
void DBG::debugWriteHex(uint8_t databyte) {
  uint8_t b1=(databyte>>4)|'0';
  uint8_t b2=(databyte&0x0F)|'0';
  if (b1 > '9') b1+=7;
  if (b2 > '9') b2+=7;
  DBG::debugWriteByte(b1);
  DBG::debugWriteByte(b2);
}

void DBG::debugWriteByte(uint8_t databyte) {
  DEBUGDATA=databyte;
  loop_until_bit_is_set(DEBUGFLAGS,DEBUGSENDNOW);
}

#endif
