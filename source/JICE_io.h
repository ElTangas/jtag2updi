/*
 * stk_io.h
 *
 * Created: 18-11-2017 14:55:53
 *  Author: JMR_2
 */


#ifndef JICE_IO_H_
#define JICE_IO_H_

#include <stdint.h>
#include "JTAG2.h"

#ifndef DISABLE_HOST_TIMEOUT
#define loop_until_bit_set_or_host_timeout(register,bitpos) ({ \
  SYS::startTimer(); \
  while(!((register&(1<<bitpos))||(SYS::checkTimeouts() & WAIT_FOR_HOST))); \
  SYS::stopTimer(); \
})
#else
#define loop_until_bit_set_or_host_timeout(register,bitpos) loop_until_bit_is_set(register,bitpos)
#endif

namespace JICE_io {
  // Function prototypes
  uint8_t put(char c);
  uint8_t get(void);
  void init(void);
  void flush(void);
  void set_baud(JTAG2::baud_rate rate);
}

#endif /* JICE_IO_H_ */