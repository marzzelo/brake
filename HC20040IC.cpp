/*
 * HC20040IC.cpp
 *
 *  Created on: 29 oct. 2021
 *      Author: valdez
 */

#include "HC20040IC.h"

HC20040IC::HC20040IC(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1,
		uint8_t d2, uint8_t d3) :
		Lcd20x4(rs, enable, d0, d1, d2, d3) {
}


//void HC20040IC::correct(uint8_t page) {
//	switch (page) {
//	case 1:
//		writeSymbol(12, 1, ATILDE);
//		writeSymbol(14, 2, ATILDE);
//		writeSymbol(11, 3, ITILDE);
//		break;
//
//	case 2:
//		writeSymbol(11, 0, ITILDE);
//		writeSymbol(16, 3, ATILDE);
//		break;
//
//	case 3:
//		writeSymbol(16, 0, ATILDE);
//		break;
//
//	default:
//		break;
//	}
//}

