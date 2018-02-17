/*
 * crc16.h
 *
 * Created: 16-01-2018 23:07:05
 *  Author: JMR_2
 */ 


#ifndef CRC16_H_
#define CRC16_H_

#include <stdint.h>

namespace CRC {
	constexpr uint16_t first = 0xFFFF;
	
	uint16_t next (uint8_t newchar, uint16_t previous);
}

#endif /* CRC16_H_ */