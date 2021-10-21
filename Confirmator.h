/*
 * Confirmator.h
 *
 *  Created on: 21 oct. 2021
 *      Author: valdez
 */

#ifndef CONFIRMATOR_H_
#define CONFIRMATOR_H_

#include "Arduino.h"

class Confirmator {

private:
	uint8_t _confirmations;
	uint8_t _events;

public:
	Confirmator(uint8_t confirmations = 3);

	bool confirm(bool event);

	void reset(uint8_t confirmations = NULL);

};

#endif /* CONFIRMATOR_H_ */
