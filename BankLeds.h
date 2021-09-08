/*
 * BankLeds.h
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#ifndef BANKLEDS_H_
#define BANKLEDS_H_

#include "SuperLed.h"
#include "MyLed7.h"

#define LED0	22
#define LED1	24
#define LED2	26
#define LED3	28
#define LED4	30
#define LED5	32
#define LED6	34
#define LED7	36

#define BUZZ	38

class BankLeds {

private:

	SuperLed *led[8];
	SuperLed *buzz;

	MyLed7 *led7s;

public:
	BankLeds();

	void update();

	void beep() {
		buzz->start();
	}

	void beep(unsigned long ton, unsigned long toff, unsigned long nCycles);

	void ledStart(int ledIndex) {
		led[ledIndex]->start();
	}

	void ledStop(int ledIndex) {
		led[ledIndex]->stop();
	}

	void ledOff(int ledIndex) {
		led[ledIndex]->set(LOW);
	}

	void ledOn(int ledIndex) {
		led[ledIndex]->set(HIGH);
	}

	void ledOnAll(int = HIGH);

	void ledOffAll();

	void ledStartAll();

	void ledStopAll();

	void display(int symbol, bool dp = false) {
		led7s->noBlink();
		led7s->display(symbol, dp);
	}

	void blink(int symbol, uint16_t period = 200) {
		led7s->display(symbol, false);
		led7s->blink(period);
	}

};

#endif /* BANKLEDS_H_ */
