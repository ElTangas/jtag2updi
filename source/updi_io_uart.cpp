/*
 * updi_io_uart.cpp
 *
 * Created: 01-02-2020 10:36:54
 *  Author: cherry pick from AvrMega_MuxTO
 */ 



// Includes
#include <avr/io.h>
#include "updi_io.h"
#include "sys.h"


#if UPDI_IO_TYPE == 3
#include <Arduino.h>

// Functions
/* Sends regular characters through the UPDI link */
uint8_t UPDI_io::put(char c) {
  HW_SERIAL.write(c);
  HW_SERIAL.flush();
  //delayMicroseconds(10);
  long start = millis();
  while (!HW_SERIAL.available() && millis() - start < 20) {}
  char d = HW_SERIAL.read();
  if (c != d) {
    // Serial.println("echo failed! " + String(d, HEX));
  }
  return c;
}

/* Sends special sequences through the UPDI link */
uint8_t UPDI_io::put(ctrl c)
{
  HW_SERIAL.begin(300, SERIAL_8N1);
  switch (c) {
    case double_break:
      HW_SERIAL.write((uint8_t)0x00);
      HW_SERIAL.flush();
      HW_SERIAL.write((uint8_t)0x00);
      HW_SERIAL.flush();
      break;
    case single_break:
      HW_SERIAL.write((uint8_t)0x00);
      HW_SERIAL.flush();
      break;
    default:
      break;
  }
  delay(15);
  while (HW_SERIAL.available()) {
    HW_SERIAL.read();
  }
  HW_SERIAL.begin(230400, SERIAL_8E2);
  return 0;
}

uint8_t UPDI_io::get() {
  uint8_t c;
  while (!HW_SERIAL.available()) {}
  c = HW_SERIAL.read();
  //delayMicroseconds(5);
  //Serial.println("get! " + String(c, HEX));
  return c;
}

void UPDI_io::init(void)
{
  HW_SERIAL.begin(230400, SERIAL_8E2);
}
#endif //UPDI_IO_TYPE == 3
