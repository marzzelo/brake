/*
 * BankKeyPad.h
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#ifndef BANKKEYPAD_H_
#define BANKKEYPAD_H_

#include "KeyPadRX.h"
#include "BankLeds.h"
#include "Streaming.h"


#define KP_ROW0		A9
#define KP_ROW1		A12
#define KP_ROW2		A11
#define KP_ROW3		A14

#define KP_COL0		A8
#define KP_COL1		A10
#define KP_COL2		A13

#define ROWS  	4 //four rows
#define COLS	3 //three columns


/**
 * Se encarga de la configuraci�n e instanciaci�n del pad num�rico a partir de la clase MyLib::KeyPadRX y
 * la clase de la biblioteca extendida de Arduino Keypad.
 */
class BankKeyPad : public KeyPadRX {

private:
	char keyBuff[40];

	char keys[ROWS * COLS] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#' };

	byte rowPins[ROWS] = { KP_ROW0, KP_ROW1, KP_ROW2, KP_ROW3 }; //connect to the row pinouts of the keypad
	byte colPins[COLS] = { KP_COL0, KP_COL1, KP_COL2 }; //connect to the column pinouts of the keypad


	enum _cmdEnum {
		KEY0, KEY1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9, KEY10, KEY11, KEY12, KEY13, KEY14, KEY15,
		CMD0, CMD1, CMD2, CMD3, CMD4, CMD5, CMD6, CMD7, CMD8, CMD9,
		_END
	};


	Keypad *keyPad;

	BankLeds *_bankLeds;

	bool _keyPadEnabled = true;
	volatile bool _keypad_data_ready;

	bool _checkCommands = true;

	int getCmd(char *strCmd, const char *table[]);

	char const *cmdTable[26] = {
			"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15",
			"*0", "*1", "*2", "*3", "*4", "*5", "*6", "*7", "*8", "*9" };

	bool ev_key[16] = { false };
	bool ev_cmd[10] = { false };


public:
	BankKeyPad(void (*_keyPressedHandler)(char), void (*_dataReadyHandler)(void), BankLeds *bankLeds);  // constructor



	char* getBuff() {
		return this->keyBuff;
	}

	void check();

	bool readKey(int key) {
		bool state = ev_key[key];
		ev_key[key] = false;
		return state;
	}

	bool readCmd(int cmd) {
		bool state = ev_cmd[cmd];
		ev_cmd[cmd] = false;
		return state;
	}

	void setDataReady(bool state) {
		_keypad_data_ready = state;
	}

	void setCheckCommands(bool state) {
		_checkCommands = state;
	}

	bool ready() {
		return _dataReady;
	}

};

#endif /* BANKKEYPAD_H_ */
