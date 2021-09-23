/*
 * BankAnalogInputs.cpp
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#include "BankAnalogInputs.h"

BankAnalogInputs::BankAnalogInputs(void (*checkPosition)(), int period, int filter) :
		_period(period), _filter(filter), _checkPosition(checkPosition) {
	analogReference(DEFAULT);  // 5.0V
	FreqCount.begin(period);
	_counting = false;

	pinMode(PIN_IN1, INPUT_PULLUP);
	pinMode(PIN_IN2, INPUT_PULLUP);

	BankAnalogInputs::encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
	encoderData.position = BankAnalogInputs::encoder->getPosition();
	encoderData.direction = (int)(BankAnalogInputs::encoder->getDirection());
	encoderData.rpm = BankAnalogInputs::encoder->getRPM();

	// register interrupt routine
	attachInterrupt(digitalPinToInterrupt(PIN_IN1), _checkPosition, CHANGE);
	attachInterrupt(digitalPinToInterrupt(PIN_IN2), _checkPosition, CHANGE);
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
		_freqBuff[i] = _freqBuff[i + 1];   // last i + 1 == _filter
	}

	double pulses = accum / (_filter + 1.0);

	if (_counting) {
		//-----------------------------------
		// v = w r
		//   = 6.28 f r;  		pulses == p = 60 f
		// v = 6.28 (p/60) r
		//   = 0.10467 p r;		r = 1m
		// v = 0.105 p
		//-----------------------------------
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

void BankAnalogInputs::resetTimer() {
	_t0 = millis();
}
;

double BankAnalogInputs::getTime() {
	return (millis() - _t0) / 1000.0;
}


BankAnalogInputs::EncoderData BankAnalogInputs::encoderRead() {
	long newPos = BankAnalogInputs::encoder->getPosition();

	if (encoderData.position != newPos) {
		encoderData.angle = newPos*360.0/2000.0;
		encoderData.position = newPos;
		encoderData.direction = (int)(BankAnalogInputs::encoder->getDirection());
		encoderData.rpm = BankAnalogInputs::encoder->getRPM();
	}
	return encoderData;
}


