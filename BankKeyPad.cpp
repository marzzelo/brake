/*
 * BankKeyPad.cpp
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#include "BankKeyPad.h"

BankKeyPad::BankKeyPad(void (*keyPadPressedHandler)(char),
		void (*keyPadDataReadyHandler)(void), BankLeds *bankLeds) :
		KeyPadRX(keyBuff, 40, this->keyPad),
		_bankLeds(bankLeds) {

	this->keyPad = new Keypad(keys, rowPins, colPins, ROWS, COLS); // keyPad

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

/**
 * KEYPAD KEYS & COMMANDS
 * Verifica si hay un comando terminado con # en el buffer del keyPad.
 * En caso afirmativo, setea las flags ev_key[n] o ev_cmd[n].
 * Las flags deberán ser reseteadas cuando sean servidas.
 */
void BankKeyPad::check() {
	if (!_checkCommands)
		return;

	if (_dataReady) {
		_bankLeds->beep(100, 1, 1);

		int cc = getCmd(getBuff(), cmdTable);

		if (cc < 16) {
			ev_key[cc] = true;
		} else if (cc < _END) {
			ev_cmd[cc - 16] = true;
		} else {
			Serial << "\ncomando inválido: " << cc;
		}

		start();
	}
}

