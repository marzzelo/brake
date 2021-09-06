/*
 * Bank.h
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#ifndef BANK_H_
#define BANK_H_

class Bank {
private:
	struct TestParms {
		double max_mass_vel;
		double brake_mass_vel_min;
		double brake_mass_vel_max;
		double landing_wheel_vel;
		double ph_threshold;
		double pf_threshold;
	};

	struct CalFactors {
		double ka_mass;
		double ka_wheel;
		double ka_ph;
		double ka_pf;
	};



public:

	TestParms testParms;
	CalFactors calFactors;


	Bank();

	void saveSettings();

	void loadSettings();
};

#endif /* BANK_H_ */


/*********************************
 * CALIBRATION
 *********************************/
//	testParms.max_mass_vel = 500.28f;  // 500 rpm
//	testParms.brake_mass_vel_min = 410.28f;  // 420 rpm nominal
//	testParms.brake_mass_vel_max = 430.28f;
//	testParms.landing_wheel_vel = 500.2f;
//	testParms.ph_threshold = 512.28f;
//	testParms.pf_threshold = 100.28f;
//
//	calFactors.ka_mass = 1.0f;		// ka_mass = 1 rpm/Hz
//	calFactors.ka_wheel = 1.0f;
//	calFactors.ka_ph = 1.0f;
//	calFactors.ka_pf = 1.0f;
//
//	int eeAddress = 0;  // Bank 0/3 : Cell 0x000/0x3FF
//	EEPROM.put(eeAddress, calFactors);
//
//	eeAddress += sizeof(calFactors);
//	EEPROM.put(eeAddress, testParms);


//	EEpromPlus::EEshow(32, 1, 'm');
