/*
 * BankAnalogInputs.cpp
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#include "BankAnalogInputs.h"

/*
					 _                _               ___                   _
					/ \   _ __   __ _| | ___   __ _  |_ _|_ __  _ __  _   _| |_ ___
				   / _ \ | '_ \ / _` | |/ _ \ / _` |  | || '_ \| '_ \| | | | __/ __|
				  / ___ \| | | | (_| | | (_) | (_| |  | || | | | |_) | |_| | |_\__ \
				 /_/   \_\_| |_|\__,_|_|\___/ \__, | |___|_| |_| .__/ \__,_|\__|___/
											  |___/            |_|

 */

BankAnalogInputs::BankAnalogInputs(void (*checkPosition)(), int period,
		int filter) :
		_period(period), _filter(filter), _checkPosition(checkPosition) {

	// turn built-in led off
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	analogReference(DEFAULT);  // 5.0V
	FreqCount.begin(period);
	_counting = false;

	pinMode(PIN_IN1, INPUT_PULLUP);
	pinMode(PIN_IN2, INPUT_PULLUP);

	BankAnalogInputs::encoder = new RotaryEncoder(PIN_IN1, PIN_IN2,
			RotaryEncoder::LatchMode::TWO03);
	_encoderData.position = encoder->getPosition();
	_encoderData.direction = (int) (encoder->getDirection());
	_encoderData.rpm = encoder->getRPM();

	// register interrupt routine
	attachInterrupt(digitalPinToInterrupt(PIN_IN1), _checkPosition, CHANGE);
	attachInterrupt(digitalPinToInterrupt(PIN_IN2), _checkPosition, CHANGE);

}

void BankAnalogInputs::enable() {
	_daq_enabled = true;
}

void BankAnalogInputs::start() {
	_daq_enabled = true;
	_daq_ready = false;
	FreqCount.begin(_period);
}

bool BankAnalogInputs::ready() {
	return (FreqCount.available() && _daq_ready);
}

void BankAnalogInputs::saveSettings() {
	int eeAddress = 0;  // Bank 0/3 : Cell 0x000/0x3FF
	EEPROM.put(eeAddress, calFactors);

	eeAddress += sizeof(calFactors);
	EEPROM.put(eeAddress, testParms);
}

void BankAnalogInputs::loadSettings() {
	int eeAddress = 0;
	EEPROM.get(0, calFactors);

	eeAddress += sizeof(calFactors);
	EEPROM.get(eeAddress, testParms);
}

void BankAnalogInputs::eePreset() {
	/*********************************
	 * CALIBRATION PRESET
	 *********************************/
	testParms.max_mass_vel = 500.28;  // 500 rpm
	testParms.brake_mass_vel_min = 410.28;  // 420 rpm nominal
	testParms.brake_mass_vel_max = 430.28;
	testParms.landing_wheel_vel = 500.2;
	testParms.ph_threshold = 512.28;
	testParms.pf_threshold = 100.28;
	testParms.t1_hot = 60.0;
	testParms.t2_hot = 60.0;

	calFactors.ka_mass = 1.0;		// ka_mass = 1 rpm/Hz
	calFactors.ka_wheel = 1.0;
	calFactors.ka_ph = 1.0;
	calFactors.ka_pf = 1.0;
	calFactors.ka_t1 = 1000.0;
	calFactors.ka_t2 = 1000.0;
	calFactors.kb_alpha = 0.0;

	int eeAddress = 0;  // Bank 0/3 : Cell 0x000/0x3FF
	EEPROM.put(eeAddress, calFactors);

	eeAddress += sizeof(calFactors);
	EEPROM.put(eeAddress, testParms);

	EEpromPlus::EEshow(32, 1, 'm');
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

void BankAnalogInputs::setAngleOffset(int angleOffset) {
	_angleOffset = angleOffset;
}

double BankAnalogInputs::getRpm() {

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

	mass_rpm = pulses * 1000.0 / _period;
	return mass_rpm;
}

double BankAnalogInputs::getDistance() {
	return _distance;
}

double BankAnalogInputs::getTime() {
	return (millis() - _t0) / 1000.0;
}

double BankAnalogInputs::getWv() {
	return wheel_daq_value * calFactors.ka_wheel;
}

double BankAnalogInputs::getPh() {
	return ph_daq_value * calFactors.ka_ph;
}

double BankAnalogInputs::getPf() {
	return pf_daq_value * calFactors.ka_pf;
}

double BankAnalogInputs::getT1() {
	return t1_daq_value * calFactors.ka_t1;
}

double BankAnalogInputs::getT2() {
	return t2_daq_value * calFactors.ka_t2;
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

BankAnalogInputs::EncoderData BankAnalogInputs::encoderRead() {
	long newPos = BankAnalogInputs::encoder->getPosition();

	if (_encoderData.position != newPos) {
		_encoderData.angle = newPos * 360.0 / 2000.0 + _angleOffset;
		angle = _encoderData.angle;
		_encoderData.position = newPos;
		_encoderData.direction =
				(int) (BankAnalogInputs::encoder->getDirection());
		_encoderData.rpm = BankAnalogInputs::encoder->getRPM();
	}
	return _encoderData;
}

double BankAnalogInputs::getDisplayVar() {
	switch (_display_var) {

	case Reference::ANGLE:
		// Update Selector LED here  <-----
		return angle;
		break;

	case Reference::MASS:
		return mass_rpm;  // do NOT use getRpm()
		break;

	case Reference::WHEEL:
		return getWv();
		break;

	case Reference::PF:
		return getPf();
		break;

	case Reference::PH:
		return getPh();
		break;

	case Reference::T1:
		return getT1();
		break;

	case Reference::T2:
	default:
		return getT2();
		break;

	}
}

void BankAnalogInputs::nextDisplayVar() {
	int dv = (int) _display_var;

	if (++dv == (int) Reference::END)
		dv = 1;

	_display_var = (Reference) dv;

}

