/*
 * Bank.cpp
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#include "Arduino.h"
#include "Bank.h"
#include "EEPROM.h"
#include "EEpromPlus.h"

Bank::Bank() {

}

void Bank::setup() {
	// STARTUP ACTIONS

	// turn built-in led off
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	Serial.println(F("\n\nBrake Test"));
}

void Bank::saveSettings() {
	int eeAddress = 0;  // Bank 0/3 : Cell 0x000/0x3FF
	EEPROM.put(eeAddress, calFactors);

	eeAddress += sizeof(calFactors);
	EEPROM.put(eeAddress, testParms);
}

void Bank::loadSettings() {
	int eeAddress = 0;
	EEPROM.get(0, calFactors);

	eeAddress += sizeof(calFactors);
	EEPROM.get(eeAddress, testParms);
}

void Bank::eePreset() {
	/*********************************
	 * CALIBRATION PRESET
	 *********************************/
	testParms.max_mass_vel = 500.28f;  // 500 rpm
	testParms.brake_mass_vel_min = 410.28f;  // 420 rpm nominal
	testParms.brake_mass_vel_max = 430.28f;
	testParms.landing_wheel_vel = 500.2f;
	testParms.ph_threshold = 512.28f;
	testParms.pf_threshold = 100.28f;
	testParms.t1_hot = 60.0f;
	testParms.t2_hot = 60.0f;

	calFactors.ka_mass = 1.0f;		// ka_mass = 1 rpm/Hz
	calFactors.ka_wheel = 1.0f;
	calFactors.ka_ph = 1.0f;
	calFactors.ka_pf = 1.0f;
	calFactors.ka_t1 = 1000.0f;
	calFactors.ka_t2 = 1000.0f;

	int eeAddress = 0;  // Bank 0/3 : Cell 0x000/0x3FF
	EEPROM.put(eeAddress, calFactors);

	eeAddress += sizeof(calFactors);
	EEPROM.put(eeAddress, testParms);

	EEpromPlus::EEshow(32, 1, 'm');
}

