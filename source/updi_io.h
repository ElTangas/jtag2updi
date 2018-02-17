/*
 * updi_io.h
 *
 * Created: 18-11-2017 10:38:31
 *  Author: JMR_2
 */ 


#ifndef UPDI_IO_H_
#define UPDI_IO_H_

namespace UPDI_io {
	// Enums
	enum ctrl {single_break, double_break, enable};

	// Function prototypes
	void setup_bit_low();
	void setup_bit_high();
	void wait_for_bit() __attribute__((always_inline));
	void stop_timer();
	void start_timer();
	int put(char) __attribute__((optimize("no-tree-loop-optimize")));
	int put(ctrl);	
	int get();
	void init(void);
}

#endif /* UPDI_IO_H_ */