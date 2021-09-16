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
		double t1_hot;		//!< too high to begin test
		double t2_hot;		//!< too high to begin test
	};

	struct CalFactors {
		double ka_mass;
		double ka_wheel;
		double ka_ph;
		double ka_pf;
		double ka_t1;
		double ka_t2;
	};

	int relay[4];

public:

	TestParms testParms;
	CalFactors calFactors;

	Bank();

	void setup();

	void saveSettings();

	void loadSettings();

	void eePreset();

};

#endif /* BANK_H_ */

