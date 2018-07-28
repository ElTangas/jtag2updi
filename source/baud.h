/*
 * baud.h
 *
 * Created: 25-07-2018 21:55:43
 *  Author: JMR_2
 */ 


#ifndef BAUD_H_
#define BAUD_H_

#ifndef F_CPU
#define F_CPU 16000000U
#endif
constexpr unsigned int baud(unsigned long b) {
	return F_CPU/(b*8.0) - 0.5;
}

#endif /* BAUD_H_ */