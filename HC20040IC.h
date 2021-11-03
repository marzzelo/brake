/*
 * HC20040IC.h
 *
 *  Created on: 29 oct. 2021
 *      Author: valdez
 */

#ifndef HC20040IC_H_
#define HC20040IC_H_

#include "Lcd20x4.h"
#include "Arduino.h"
#include "Util.h"
#include "PrintMacros.h"


class HC20040IC: public Lcd20x4 { // @suppress("Class has a virtual method and non-virtual destructor")

public:

	HC20040IC(uint8_t rs,  uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

	void correct(uint8_t page);

};

#endif /* HC20040IC_H_ */
