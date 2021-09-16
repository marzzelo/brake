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

#define SEGM0	22
#define SEGM1	24
#define SEGM2	26
#define SEGM3	28
#define SEGM4	30
#define SEGM5	32
#define SEGM6	34
#define SEGM7	36

#define RELAY0	2
#define RELAY1	3
#define RELAY2	4
#define RELAY3	5
#define RELAY4	6
#define RELAY5	7
#define RELAY6	8
#define RELAY7	9

#define BUZZ	38

class BankLeds {

private:

	SuperLed *relay[8];
	SuperLed *buzz;

	MyLed7 *led7s;

public:
	BankLeds();

	void update();

	void beep() {
		buzz->start();
	}

	void beep(unsigned long ton, unsigned long toff, unsigned long nCycles);

	void relayStart(int relayIndex) {
		relay[relayIndex]->start();
	}

	void relayStop(int relayIndex) {
		relay[relayIndex]->stop();
	}

	void relayOff(int relayIndex) {
		relay[relayIndex]->set(LOW);
	}

	void relayOn(int relayIndex) {
		relay[relayIndex]->set(HIGH);
	}

	void relayOnAll(int = HIGH);

	void relayOffAll();

	void relayStartAll();

	void relayStopAll();

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
