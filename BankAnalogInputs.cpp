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
		_period(period), _checkPosition(checkPosition) {

	// turn built-in led off
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	analogReference(DEFAULT);  // 5.0V
	FreqCount.begin(_period);
	_counting = false;
	_distance = 0;

	pinMode(PIN_IN1, INPUT_PULLUP);
	pinMode(PIN_IN2, INPUT_PULLUP);

	BankAnalogInputs::encoder = new RotaryEncoder(PIN_IN1, PIN_IN2,
			RotaryEncoder::LatchMode::TWO03);

	// register interrupt routine (encoder)
	attachInterrupt(digitalPinToInterrupt(PIN_IN1), _checkPosition, CHANGE);
	attachInterrupt(digitalPinToInterrupt(PIN_IN2), _checkPosition, CHANGE);

	// prepare input signals filters
	for (int i = 0; i < MAX_INPUTS; ++i) {
		_mmf[i] = new MMFilter(filter);
	}

}

void BankAnalogInputs::enable() {
	_daq_enabled = true;
}

void BankAnalogInputs::start() {
	_daq_enabled = true;
	_daq_ready = false;
}

bool BankAnalogInputs::ready() {
	return (_daq_ready);
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

bool BankAnalogInputs::check(Condition cond) {
	double rpm = getRpm();

	switch (cond) {

	case Condition::MV_EQ_0:
		if (rpm < 0)
			return false;

		return rpm <= ZERO_MASS_VEL;

	case Condition::MV_GT_0:
		if (rpm < 0)
			return false;

		return rpm > ZERO_MASS_VEL;

	case Condition::MV_GT_MAX:
		if (rpm < 0)
			return false;

		return rpm > testParms.max_mass_vel;

	case Condition::MV_GE_BRAKEV_MIN:
		if (rpm < 0)
			return false;

		return rpm >= testParms.brake_mass_vel_min;

	case Condition::MV_LE_BRAKEV_MAX:
		if (rpm < 0)
			return false;

		return rpm <= testParms.brake_mass_vel_max;

	case Condition::WV_EQ_0:
		return getWv() <= ZERO_WHEEL_VEL;

	case Condition::WV_GT_0:
		return getWv() > ZERO_WHEEL_VEL;

	case Condition::WV_GE_LANDINGV:
		return getWv() >= testParms.landing_wheel_vel;

	case Condition::PH_GT_0:
		return getPh() > ZERO_PH;

	case Condition::PH_GE_PH1:
		return getPh() >= testParms.ph_threshold;

	case Condition::PF_GT_0:
		return getPf() > ZERO_PF;

	case Condition::PF_GE_PF1:
		return getPf() >= testParms.pf_threshold;

	case Condition::ANGLE_EQ_0:
		return encoderRead().angle <= ZERO_ANGLE;

	case Condition::ANGLE_GT_0:
		return encoderRead().angle > ZERO_ANGLE;

	case Condition::PF_LT_PF1:
		return getPf() < testParms.pf_threshold * 0.75;

	case Condition::TIMEOUT:
		return (millis() - _t0) > _dt;

	default:
		return false;
	}
}

void BankAnalogInputs::setTimeOut(unsigned long dt) {
	_t0 = millis();
	_dt = dt;
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

void BankAnalogInputs::setAngleOffset(double angleOffset) {
	_angleOffset = angleOffset;
}

double BankAnalogInputs::getRpm() {
	if (!FreqCount.available())
		return _last_rpm;

	double pps = _mmf[IN_MV]->filter(FreqCount.read());

	if (_counting) {
		/*-----------------------------------
		 v = w r
		   = 2 PI f r;  			pulses per second == pps = 60 f
		 v = 6.28 (pps/60) r
		   = 0.10467 pps r;		r = 1m
		 v = 0.105 pps
		 -----------------------------------*/
		_distance += pps * 0.105;
	}

	mass_rpm = pps * 1000.0 / _period;
	return _last_rpm = mass_rpm;
}

double BankAnalogInputs::getDistance() {
	return _distance;
}

double BankAnalogInputs::getTime() {
	return (millis() - _t0) / 1000.0;
}

double BankAnalogInputs::getWv() {
	double val = wheel_daq_value * calFactors.ka_wheel;
	return _mmf[IN_WV]->filter(val);
}

double BankAnalogInputs::getPh() {
	double val = ph_daq_value * calFactors.ka_ph;
	return _mmf[IN_PH]->filter(val);
}

double BankAnalogInputs::getPf() {
	double val = pf_daq_value * calFactors.ka_pf;
	return _mmf[IN_PF]->filter(val);
}

double BankAnalogInputs::getT1() {
	return t1_daq_value * calFactors.ka_t1;
}

double BankAnalogInputs::getT2() {
	return t2_daq_value * calFactors.ka_t2;
}

void BankAnalogInputs::startCounting() {
	_distance = 0;
	_t0 = millis();
	_counting = true;
}

double BankAnalogInputs::stopCounting() {
	_counting = false;
	return _distance;
}

BankAnalogInputs::EncoderData BankAnalogInputs::encoderRead() {
	long newPos = BankAnalogInputs::encoder->getPosition();

	_encoderData.angle = newPos * 360.0 / 2000.0 + _angleOffset;
	_encoderData.position = newPos;
	_encoderData.direction = (int) (BankAnalogInputs::encoder->getDirection());
	_encoderData.rpm = BankAnalogInputs::encoder->getRPM();

	return _encoderData;
}

double BankAnalogInputs::getDisplayVarValue() {
	switch (_display_var) {

	case VarNames::ANGLE:
		// Update Led Selector here  <-----
		return encoderRead().angle;
		break;

	case VarNames::MASS:
		return mass_rpm;  // do NOT use getRpm()
		break;

	case VarNames::WHEEL:
		return getWv();
		break;

	case VarNames::PF:
		return getPf();
		break;

	case VarNames::PH:
		return getPh();
		break;

	case VarNames::T1:
		return getT1();
		break;

	case VarNames::T2:
	default:
		return getT2();
		break;

	}
}

int BankAnalogInputs::setDisplayVarIndex(int index) {
	return (_display_var = index);
}

int BankAnalogInputs::getDisplayVarIndex() {
	return _display_var;
}

void BankAnalogInputs::setup() {
}

void BankAnalogInputs::reset() {
	_counting = false;
	_distance = 0;
}

char* BankAnalogInputs::getDisplayVarName() {
	switch (_display_var) {

//	MASS, ANGLE, WHEEL, PH, PF, T1, T2, ERROR, END

	case MASS:
		sprintf(_buff, "%s", "RPM ");
		break;
	case ANGLE:
		sprintf(_buff, "%s", "ANGL");
		break;
	case WHEEL:
		sprintf(_buff, "%s", "RUED");
		break;
	case PH:
		sprintf(_buff, "%s", "Ph.  ");
		break;
	case PF:
		sprintf(_buff, "%s", "PFr. ");
		break;
	case T1:
		sprintf(_buff, "%s", "T1° ");
		break;
	case T2:
		sprintf(_buff, "%s", "T2° ");
		break;
	default:
		sprintf(_buff, "%s", "ERR ");
		break;
	}

	return _buff;
}

char* BankAnalogInputs::nextDisplayVar() {
	int dv = (int) _display_var;

	if (++dv == (int) VarNames::ERROR)
		dv = 0;

	_display_var = (VarNames) dv;

	return getDisplayVarName();
}

