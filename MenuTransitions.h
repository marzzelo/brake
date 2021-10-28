/*
 * MenuTransitions.h
 *
 *  Created on: 28 sep. 2021
 *      Author: valdez
 */

#ifndef MENUTRANSITIONS_H_
#define MENUTRANSITIONS_H_

/*
 _____                    _ _   _
 |_   _| __ __ _ _ __  ___(_) |_(_) ___  _ __  ___
 | || '__/ _` | '_ \/ __| | __| |/ _ \| '_ \/ __|
 | || | | (_| | | | \__ \ | |_| | (_) | | | \__ \
   |_||_|  \__,_|_| |_|___/_|\__|_|\___/|_| |_|___/

 */

#define ADD_TRANSITION_FROM_MAIN_KEY(key) 	[]() { return bankKp->readKey((key)); }

bool updateParam(double &param, uint16_t daqValue) {
	if (bankKp->dataReady()) {
		bankLeds->beep(100, 1, 1);
		double dblReadVal = String(bankKp->buffer()).toDouble();
		param = dblReadVal / daqValue;
		bankInputs->saveSettings();
		return true;  // --> to main menu
	}
	return false;  // keep reading keypad
}

/**
 * Array of transition functions.
 * NOTE: ORDER MUST FOLLOW THE ENUM  MenuFSM::Transitions
 */
bool (*menuTransitions[])(void) = {

	[]() {
		return bankKp->readKey(1);			// Idle -> Main
	},

	//////////////////////////////////////////
	// FACTORES DE CALIBRACIÃ“N
	//////////////////////////////////////////
	[]() {
		return bankKp->readKey(0);			// Main -> idle
	},

	[]() {
		return bankKp->readKey(1);			// Main -> wheel
	},

	[]() {
		return bankKp->readKey(2);			// Main -> Ph
	},

	[]() {
		return bankKp->readKey(3);			// Pf
	},

	[]() {
		return bankKp->readKey(4);			// Main -> T1
	},

	[]() {
		return bankKp->readKey(5);			// Main -> T2
	},

	[]() {
		return bankKp->readKey(6);			// Main -> Alpha
	},

	//////////////////////////////////////////
	// PARÃ�METROS DE ENSAYO
	//////////////////////////////////////////
	[]() {
		return bankKp->readKey(7);			// Vel max
	},

	[]() {
		return bankKp->readKey(8);			// Vel sup
	},

	[]() {
		return bankKp->readKey(9);			// Vel inf
	},

	[]() {
		return bankKp->readKey(10);			// Ph nom
	},

	[]() {
		return bankKp->readKey(11);			// Pf nom
	},

	[]() {
		return bankKp->readKey(12);			// T1hot
	},

	[]() {
		return bankKp->readKey(13);			// T2hot
	},

	//////////////////////////////////////////
	// FACTORY RESET
	//////////////////////////////////////////
	[]() {
		return bankKp->readKey(14);			// T2hot
	},

	//////////////////////////////////////////
	// RETORNOS A MAIN MENU
	//////////////////////////////////////////

	// WHEELCAL -> MAIN
	[]() {
		return updateParam(bankInputs->calFactors.ka_wheel, bankInputs->wheel_daq_value);
	},

	// PHCAL -> MAIN
	[]() {
		return updateParam(bankInputs->calFactors.ka_ph, bankInputs->ph_daq_value);
	},

	// PFCAL -> MAIN
	[]() {
		return updateParam(bankInputs->calFactors.ka_pf, bankInputs->pf_daq_value);
	},

	// T1CAL -> MAIN
	[]() {
		return updateParam(bankInputs->calFactors.ka_t1, bankInputs->t1_daq_value);
	},

	// T2CAL -> MAIN
	[]() {
		return updateParam(bankInputs->calFactors.ka_t2, bankInputs->t2_daq_value);
	},

	// ALPHACAL -> MAIN
	[]() {

		if (bankKp->dataReady()) {
			bankLeds->beep(100, 1, 1);

			double dblReadVal = String(bankKp->buffer()).toDouble();

			bankInputs->calFactors.kb_alpha = int(dblReadVal - bankInputs->encoder->getPosition() * 360 / 2000);
			bankInputs->setAngleOffset(bankInputs->calFactors.kb_alpha);
			bankInputs->saveSettings();
			return true;  // --> to main menu
		}

		return false;  // keep reading keypad
	},

	// MVMAX -> MAIN
	[]() {
		return updateParam(bankInputs->testParms.max_mass_vel, 1);
	},

	// BVMAX -> MAIN
	[]() {
		return updateParam(bankInputs->testParms.brake_mass_vel_max, 1);
	},

	// BVMIN -> MAIN
	[]() {
		return updateParam(bankInputs->testParms.brake_mass_vel_min, 1);
	},

	// PHLIM -> MAIN
	[]() {
		return updateParam(bankInputs->testParms.ph_threshold, 1);
	},

	// PFLIM -> MAIN
	[]() {
		return updateParam(bankInputs->testParms.pf_threshold, 1);
	},

	// T1HOT -> MAIN
	[]() {
		return updateParam(bankInputs->testParms.t1_hot, 1);
	},

	// T2HOT -> MAIN
	[]() {
		return updateParam(bankInputs->testParms.t2_hot, 1);
	},

	// FACTORY RESET -> MAIN
	[]() {
		matrix->text("** RESET **");
		tm1638->dispstr("FACTORY RESET");
		bankInputs->eePreset();			// default calibration/parameter values
		bankInputs->loadSettings();// Loads calibration/test parameters from EEprom
		bankInputs->start();// Enables DAQ
		return true;// go inmediately to main menu
	},

};

#endif /* MENUTRANSITIONS_H_ */
