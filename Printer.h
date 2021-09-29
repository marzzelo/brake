/*
 * Printer.h
 *
 *  Created on: 29 sep. 2021
 *      Author: valdez
 */

#ifndef PRINTER_H_
#define PRINTER_H_

#include "Arduino.h"
#include "Streaming.h"

class Printer {

private:
	uint8_t _columns, _defaultCols;

	char _spaces[80] = {' '};
	char _text[80];
	char _num[10];

public:
	Printer(uint8_t defaultColumns);

	void print_item(String s) {
		int i;
		for (i = 0; i < _columns - 3 - s.length(); ++i) _spaces[i] = ' ';
		_spaces[i] = '\0';
		Serial << "\n| " << s << _spaces << "|";
	}

	void make_item(char *fmt, double value) {
		dtostrf(value, 0, 3, _num);
		sprintf(_text, fmt, _num);
		print_item(_text);
	}

	void print_header(String s, bool closed = true, uint8_t cols = 0) {
		if (cols > 0) _columns = cols;
		else _columns = _defaultCols;

		print_separator(_columns);
		print_item(s);
		if (closed) { print_separator(_columns); }
	}

	void print_separator(uint8_t cols = 0) {
		if (cols > 0) _columns = cols;
		else _columns = _defaultCols;

		int i = 0;
		_spaces[i++] = '\n';
		_spaces[i++] = '+';
		for (; i < _columns; ++i) _spaces[i] = '-';
		_spaces[i++] = '+';
		_spaces[i] = '\0';
		Serial << _spaces;
	}


};

#endif /* PRINTER_H_ */
