/*
 * HC20040IC.h
 *
 *  Created on: 29 oct. 2021
 *      Author: valdez
 */

#ifndef HC20040IC_H_
#define HC20040IC_H_

#include "LiquidCrystal.h"

class HC20040IC: public LiquidCrystal { // @suppress("Class has a virtual method and non-virtual destructor")
private:
	bool _writeEnabled = false;

public:
	HC20040IC(uint8_t rs,  uint8_t enable,
		     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

	void write4l(char const *lines[], uint8_t nl = 4);

	// conditional write
	void cwrite(char key);

	void enableCWrite(bool enable = true);

};

#endif /* HC20040IC_H_ */
