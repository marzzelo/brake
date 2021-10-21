/*
 * BankLeds.h
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#ifndef BANKLEDS_H_
#define BANKLEDS_H_

#include "SuperLed.h"

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

public:
	BankLeds();

	void update();

	void beep() {
		buzz->start();
	}

	void beep(unsigned long ton, unsigned long toff, unsigned long nCycles);

	void relayStart(int relayIndex);

	void relayStop(int relayIndex);

	void relayOff(int relayIndex);

	void relayOn(int relayIndex);

	void relayOnly(int relayIndex);

	void relayOnAll(int = HIGH);

	void relayOffAll();

	void relayStartAll();

	void relayStopAll();

};

#endif /* BANKLEDS_H_ */
