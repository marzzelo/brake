/*
 * BankKeyPad.cpp
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#include "BankKeyPad.h"

extern void keyPadDataReadyHandler();
extern void keyPadPressedHandler(char);

BankKeyPad::BankKeyPad() {

	this->keyPad = new Keypad(keys, rowPins, colPins, ROWS, COLS); // keyPad

	this->keyPadRX = new KeyPadRX(keyBuff, 40, this->keyPad);

	// KeyPad handlers
	keyPadRX->setDataReadyHandler(keyPadDataReadyHandler);
	keyPadRX->setKeyPressedHandler(keyPadPressedHandler);

}

