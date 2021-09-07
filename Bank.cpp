/*
 * Bank.cpp
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#include "Arduino.h"
#include "Bank.h"
#include "EEPROM.h"

Bank::Bank() {

}

void Bank::setup() {
	// STARTUP ACTIONS

	// turn built-in led off
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	Serial.println(F("\n\nBrake Test"));
}

void Bank::saveSettings() {
	int eeAddress = 0;  // Bank 0/3 : Cell 0x000/0x3FF
	EEPROM.put(eeAddress, calFactors);

	eeAddress += sizeof(calFactors);
	EEPROM.put(eeAddress, testParms);
}

void Bank::loadSettings() {
	int eeAddress = 0;
	EEPROM.get(0, calFactors);

	eeAddress += sizeof(calFactors);
	EEPROM.get(eeAddress, testParms);
}
