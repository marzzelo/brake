/*
 * HC20040IC.cpp
 *
 *  Created on: 29 oct. 2021
 *      Author: valdez
 */

#include "HC20040IC.h"

HC20040IC::HC20040IC(uint8_t rs,  uint8_t enable,
	     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) : LiquidCrystal(rs, enable, d0, d1, d2, d3) {
	begin(20, 4);
}

void HC20040IC::write4l(char const *lines[], uint8_t nl = 4) {
	clear();
	for (int i = 0; i < nl; ++i) {
		setCursor(0, i); print(lines[i]);
	}
}
