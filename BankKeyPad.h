/*
 * BankKeyPad.h
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#ifndef BANKKEYPAD_H_
#define BANKKEYPAD_H_

#include "KeyPadRX.h"


#define KP_ROW0		A9
#define KP_ROW1		A12
#define KP_ROW2		A11
#define KP_ROW3		A14

#define KP_COL0		A8
#define KP_COL1		A10
#define KP_COL2		A13

#define ROWS  	4 //four rows
#define COLS	3 //three columns

class BankKeyPad {

private:
	char keyBuff[40];

	char keys[ROWS * COLS] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '*',	'0', '#' };

	byte rowPins[ROWS] = { KP_ROW0, KP_ROW1, KP_ROW2, KP_ROW3 }; //connect to the row pinouts of the keypad
	byte colPins[COLS] = { KP_COL0, KP_COL1, KP_COL2 }; //connect to the column pinouts of the keypad

	Keypad *keyPad;

	KeyPadRX *keyPadRX;

	bool keyPadEnabled = true;
	volatile bool keypad_data_ready;

public:
	BankKeyPad();  // constructor

	KeyPadRX *getKeyPadRX() { return this->keyPadRX; };

	char *getBuff() { return this->keyBuff; };
};

#endif /* BANKKEYPAD_H_ */
