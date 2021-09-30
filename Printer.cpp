/*
 * Printer.cpp
 *
 *  Created on: 29 sep. 2021
 *      Author: valdez
 */


//					  ____  ____  ___ _   _ _____ _____ ____         ____ ____  ____
//					 |  _ \|  _ \|_ _| \ | |_   _| ____|  _ \       / ___|  _ \|  _ \
//					 | |_) | |_) || ||  \| | | | |  _| | |_) |     | |   | |_) | |_) |
//					 |  __/|  _ < | || |\  | | | | |___|  _ <   _  | |___|  __/|  __/
//					 |_|   |_| \_\___|_| \_| |_| |_____|_| \_\ (_)  \____|_|   |_|
//

#include "Printer.h"

Printer::Printer(uint8_t defaultColumns) :
		_columns(defaultColumns) {
	_defaultCols = defaultColumns;
}

void Printer::print_item(String s) {
	int i;
	for (i = 0; i < _columns - 3 - s.length(); ++i)
		_spaces[i] = ' ';
	_spaces[i] = '\0';
	Serial << "\n| " << s << _spaces << "|";
}

void Printer::make_item(char *fmt, double value) {
	dtostrf(value, 0, 3, _num);
	sprintf(_text, fmt, _num);
	print_item(_text);
}

void Printer::print_header(String s, bool closed = true, uint8_t cols = 0) {
	if (cols > 0)
		_columns = cols;
	else
		_columns = _defaultCols;

	print_separator(_columns);
	print_item(s);
	if (closed) {
		print_separator(_columns);
	}
}

void Printer::print_separator(uint8_t cols = 0) {
	if (cols > 0)
		_columns = cols;
	else
		_columns = _defaultCols;

	int i = 0;
	_spaces[i++] = '\n';
	_spaces[i++] = '+';
	for (; i < _columns; ++i)
		_spaces[i] = '-';
	_spaces[i++] = '+';
	_spaces[i] = '\0';
	Serial << _spaces;
}


// https://patorjk.com/software/taag/#p=display&f=Standard&t=Men%C3%BA%20FSM
