/*
 * Printer.h
 *
 *  Created on: 29 sep. 2021
 *      Author: valdez
 */

//					  ____  ____  ___ _   _ _____ _____ ____        _   _
//					 |  _ \|  _ \|_ _| \ | |_   _| ____|  _ \      | | | |
//					 | |_) | |_) || ||  \| | | | |  _| | |_) |     | |_| |
//					 |  __/|  _ < | || |\  | | | | |___|  _ <   _  |  _  |
//					 |_|   |_| \_\___|_| \_| |_| |_____|_| \_\ (_) |_| |_|
//

#ifndef PRINTER_H_
#define PRINTER_H_

#include "Arduino.h"
#include "Streaming.h"

#define TXT_BUFF_SIZE	80
#define NUM_BUFF_SIZE	10

class Printer {

private:
	uint8_t _columns, _defaultCols;

	char _spaces[TXT_BUFF_SIZE] = { ' ' };
	char _text[TXT_BUFF_SIZE] = { 0 };
	char _num[NUM_BUFF_SIZE] = { 0 };

public:
	Printer(uint8_t defaultColumns);

	void print_item(String s);

	void make_item(char *fmt, double value);

	void print_header(String s, bool closed = true, uint8_t cols = 0);

	void print_separator(uint8_t cols = 0);

};

#endif /* PRINTER_H_ */
