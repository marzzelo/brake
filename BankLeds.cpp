/*
 * BankLeds.cpp
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#include "BankLeds.h"

BankLeds::BankLeds() {
	led[0] = new SuperLed(LED0, 10, 40, 0);
	led[1] = new SuperLed(LED1, 10, 40, 0);
	led[2] = new SuperLed(LED2, 10, 40, 0);
	led[3] = new SuperLed(LED3, 10, 40, 0);
	led[4] = new SuperLed(LED4, 10, 40, 0);
	led[5] = new SuperLed(LED5, 10, 40, 0);
	led[6] = new SuperLed(LED6, 10, 40, 0);
	led[7] = new SuperLed(LED7, 10, 40, 0);

	buzz = new SuperLed(BUZZ, 6, 1, 0);
	buzz->setCycles(1);
}

void BankLeds::beep(unsigned long ton, unsigned long toff, unsigned long nCycles) {
	buzz->start(ton, toff, nCycles);
}

void BankLeds::update() {
	for (int ledIndex = 0; ledIndex < 8; ++ledIndex) {
		led[ledIndex]->update();
	}

	buzz->update();
}

void BankLeds::ledOnAll(int on = HIGH) {
	for (int ledIndex = 0; ledIndex < 8; ++ledIndex) {
		led[ledIndex]->set(on);
	}
}

void BankLeds::ledOffAll() {
	ledOnAll(LOW);
}

void BankLeds::ledStartAll() {
	for (int ledIndex = 0; ledIndex < 8; ++ledIndex) {
		led[ledIndex]->start();
	}
}

void BankLeds::ledStopAll() {
	for (int ledIndex = 0; ledIndex < 8; ++ledIndex) {
		led[ledIndex]->stop();
	}
}
