/*
 * BankLeds.h
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#ifndef BANKLEDS_H_
#define BANKLEDS_H_

#include "SuperLed.h"

#define RELAY0	22
#define RELAY1	24
#define RELAY2	26
#define RELAY3	28
#define RELAY4	30
#define RELAY5	32
#define RELAY6	34
#define RELAY7	36

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
