/*
 * flash_vars.h
 *
 * Created: 26-11-2017 16:36:35
 *  Author: JMR_2
 */ 


#ifndef FLASH_VARS_H_
#define FLASH_VARS_H_

#include <avr/pgmspace.h>

#define FLASH(T) const flash<T> PROGMEM

template <typename T>
class flash {
	private:
	const T data;

	public:
	// normal constructor
	constexpr flash (T _data) : data(_data) {}
	// default constructor
	constexpr flash () : data(0) {}

	operator T() const {
		switch (sizeof(T)) {
			case 1: return pgm_read_byte(&data);
			case 2: return pgm_read_word(&data);
			case 4: return pgm_read_dword(&data);
		}
	}
};

#endif /* FLASH_VARS_H_ */