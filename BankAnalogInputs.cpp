/*
 * BankAnalogInputs.cpp
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#include "BankAnalogInputs.h"

BankAnalogInputs::BankAnalogInputs(int period, int filter) : _period(period), _filter(filter) {
	analogReference(DEFAULT);  // 5.0V
	FreqCount.begin(period);
	_counting = false;
}

void BankAnalogInputs::update() {
	if (_daq_enabled) {

		wheel_daq_value = analogRead(INPUT_WHEEL);
		ph_daq_value = analogRead(INPUT_PH);
		pf_daq_value = analogRead(INPUT_PF);
		t1_daq_value = analogRead(INPUT_T1);
		t2_daq_value = analogRead(INPUT_T2);

		_daq_ready = true;
	}
}

uint32_t BankAnalogInputs::getRpm() {

	uint32_t accum = _freqBuff[_filter] = FreqCount.read();
	int i;

	for (i = 0; i < _filter; ++i) {
		accum += _freqBuff[i];
		_freqBuff[i] = _freqBuff[i+1];   // last i + 1 == _filter
	}

	double pulses = accum / (_filter + 1.0);

	if (_counting) {
		_distance += pulses * 0.105;
	}
	return pulses * 1000.0 / _period;
}

double BankAnalogInputs::getDistance() {
	return _distance;
}

void BankAnalogInputs::startCounting() {
	_distance = 0;
	_counting = true;
}

double BankAnalogInputs::stopCounting() {
	_counting = false;
	return _distance;
}

