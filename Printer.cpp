/*
 * Printer.cpp
 *
 *  Created on: 29 sep. 2021
 *      Author: valdez
 */

#include "Printer.h"

Printer::Printer(uint8_t defaultColumns) : _columns(defaultColumns) {
	_defaultCols = defaultColumns;
}

