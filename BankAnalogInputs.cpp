/*
 * BankAnalogInputs.cpp
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#include "BankAnalogInputs.h"

BankAnalogInputs::BankAnalogInputs(int period = 200) : _PERIOD(period) {
	analogReference(DEFAULT);  // 5.0V
	FreqCount.begin(period);
}

void BankAnalogInputs::update() {
	if (_daq_enabled) {

		wheel_daq_value = analogRead(INPUT_WHEEL);
		ph_daq_value = analogRead(INPUT_PH);
		pf_daq_value = analogRead(INPUT_PF);

		_daq_ready = true;
	}
}

