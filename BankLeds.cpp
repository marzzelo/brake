/*
 * BankLeds.cpp
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#include "BankLeds.h"

BankLeds::BankLeds() {
	relay[0] = new SuperLed(RELAY0, 400, 400, 0);
	relay[1] = new SuperLed(RELAY1, 400, 400, 0);
	relay[2] = new SuperLed(RELAY2, 400, 400, 0);
	relay[3] = new SuperLed(RELAY3, 400, 400, 0);
	relay[4] = new SuperLed(RELAY4, 400, 400, 0);
	relay[5] = new SuperLed(RELAY5, 400, 400, 0);
	relay[6] = new SuperLed(RELAY6, 400, 400, 0);
	relay[7] = new SuperLed(RELAY7, 400, 400, 0);

	buzz = new SuperLed(BUZZ, 60, 1, 0);
	buzz->setCycles(1);

}

void BankLeds::beep(unsigned long ton, unsigned long toff,
		unsigned long nCycles) {
	buzz->start(ton, toff, nCycles);
}

void BankLeds::update() {
	for (int relayIndex = 0; relayIndex < 8; ++relayIndex) {
		relay[relayIndex]->update();
	}

	buzz->update();
}

void BankLeds::relayOnAll(int on = HIGH) {
	for (int relayIndex = 0; relayIndex < 8; ++relayIndex) {
		relay[relayIndex]->set(on);
	}
}

void BankLeds::relayOffAll() {
	relayOnAll(LOW);
}

void BankLeds::relayStartAll() {
	for (int relayIndex = 0; relayIndex < 8; ++relayIndex) {
		relay[relayIndex]->start();
	}
}

void BankLeds::relayStopAll() {
	for (int relayIndex = 0; relayIndex < 8; ++relayIndex) {
		relay[relayIndex]->stop();
	}
}

void BankLeds::relayStart(int relayIndex) {
	relay[relayIndex]->start();
}

void BankLeds::relayStop(int relayIndex) {
	relay[relayIndex]->stop();
}

void BankLeds::relayOff(int relayIndex) {
	relay[relayIndex]->set(LOW);
}

void BankLeds::relayOn(int relayIndex) {
	relay[relayIndex]->set(HIGH);
}

void BankLeds::relayOnly(int relayIndex) {
	relayOffAll();
	relayOn(relayIndex);
}
