/*
 * Confirmator.cpp
 *
 *  Created on: 21 oct. 2021
 *      Author: valdez
 */

#include "Confirmator.h"
#include "Streaming.h"
#include "PrintMacros.h"

Confirmator::Confirmator(uint8_t confirmations = 2) {
	_events = 0;
	_confirmations = confirmations;
}

bool Confirmator::confirm(bool event) {

	if (event) {
		PRINT("\nevent # ", _events);
		if (_events++ > _confirmations) {
			PRINTS("\nevent TRIGGERED! ");
			_events = 0;
			return true;
		}
	} else {
		_events = 0;
	}

	return false;
}

void Confirmator::reset(uint8_t confirmations = NULL) {
	_events = 0;
	if (confirmations != NULL)
		_confirmations = confirmations;
}
