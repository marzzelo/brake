/*
 * BankKeyPad.cpp
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#include "BankKeyPad.h"

BankKeyPad::BankKeyPad(void (*keyPadPressedHandler)(char),
		void (*keyPadDataReadyHandler)(void)) :
		KeyPadRX(keyBuff, 40, this->keyPad) {

	this->keyPad = new Keypad(keys, rowPins, colPins, ROWS, COLS); // keyPad

	checkCommands = true;

	// KeyPad handlers
	this->setDataReadyHandler(keyPadDataReadyHandler);
	this->setKeyPressedHandler(keyPadPressedHandler);

}

/***************************
 * Obtain command offset (keyboard)
 ***************************/
int BankKeyPad::getCmd(char *strCmd, const char *table[]) {
	int p;

	for (p = 0; p <= _END; p++) {
		if (strcmp(strCmd, table[p]) == 0) {
			break;
		}
	}
	return p;
}


int BankKeyPad::getCommand() {
	return getCmd(keyBuff, cmdTable);
}


bool BankKeyPad::readKey(int key) {
	bool state = ev_key[key];
	ev_key[key] = false;
	return state;
}

bool BankKeyPad::readCmd(int cmd) {
	bool state = ev_cmd[cmd];
	ev_cmd[cmd] = false;
	return state;
}

