/*
 * BankAnalogInputs.cpp
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#include "BankAnalogInputs.h"

BankAnalogInputs::BankAnalogInputs() {
	analogReference(DEFAULT);  // 5.0V

}

void BankAnalogInputs::update() {
	if (_daq_enabled) {
		wheel_daq_value = analogRead(INPUT_WHEEL);
		ph_daq_value = analogRead(INPUT_PH);
		pf_daq_value = analogRead(INPUT_PF);

		_daq_ready = true;
	}
}

