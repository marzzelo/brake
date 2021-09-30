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

#define ADD_TRANSITION_FROM_MAIN_KEY(key) 	[]() { if (ev_key[(key)]) { ev_key[(key)] = false; return true;} return false; }


bool updateParam(double &param, uint16_t daqValue) {
	if (keyPadRx->dataReady()) {	// espera comando terminado en #
		bankLeds.beep(100, 1, 1);
		double dblReadVal = String(keyPadRx->buffer()).toDouble();// convierte entrada a Double
		param = dblReadVal / daqValue;// calcula el factor de calibración para param
//		Serial << "\n-------\nNew Value: " << _FLOAT(param, 3);
		bankInputs.saveSettings();
		return true;  // --> to main menu
	}
	return false;  // keep reading keypad
}


/**
 * Array of transition functions.
 * NOTE: ORDER MUST FOLLOW THE ENUM  MenuFSM::Transitions
 */
bool (*menuTransitions[])(void) = {

	//////////////////////////////////////////
	// IDLE TO MAIN (array index: 0)
	//////////////////////////////////////////
	[]() {
		if (ev_key[1]) {
			ev_key[1] = false;
			return true;
		}
		return false;
	},

	//////////////////////////////////////////
	// FACTORES DE CALIBRACIÓN
	//////////////////////////////////////////
	ADD_TRANSITION_FROM_MAIN_KEY(0),		//  (array index = 1, key = 0)

	ADD_TRANSITION_FROM_MAIN_KEY(1),		//  (array index = 2, key = 1)

	ADD_TRANSITION_FROM_MAIN_KEY(2),

	ADD_TRANSITION_FROM_MAIN_KEY(3),

	ADD_TRANSITION_FROM_MAIN_KEY(4),

	ADD_TRANSITION_FROM_MAIN_KEY(5),

	ADD_TRANSITION_FROM_MAIN_KEY(6),		//  (array index = 7, key = 6)

	//////////////////////////////////////////
	// PARÁMETROS DE ENSAYO
	//////////////////////////////////////////
	ADD_TRANSITION_FROM_MAIN_KEY(7),		//  (array index = 8, key = 7)

	ADD_TRANSITION_FROM_MAIN_KEY(8),

	ADD_TRANSITION_FROM_MAIN_KEY(9),

	ADD_TRANSITION_FROM_MAIN_KEY(10),

	ADD_TRANSITION_FROM_MAIN_KEY(11),

	ADD_TRANSITION_FROM_MAIN_KEY(12),

	ADD_TRANSITION_FROM_MAIN_KEY(13),		//  (array index = 14, key = 13)

	//////////////////////////////////////////
	// RETORNOS A MAIN MENU
	//////////////////////////////////////////

	// WHEELCAL -> MAIN
	[]() {
		return updateParam(bankInputs.calFactors.ka_wheel, bankInputs.wheel_daq_value);
	},

	// PHCAL -> MAIN
	[]() {
		return updateParam(bankInputs.calFactors.ka_ph, bankInputs.ph_daq_value);
	},

	// PFCAL -> MAIN
	[]() {
		return updateParam(bankInputs.calFactors.ka_pf, bankInputs.pf_daq_value);
	},

	// T1CAL -> MAIN
	[]() {
		return updateParam(bankInputs.calFactors.ka_t1, bankInputs.t1_daq_value);
	},

	// T2CAL -> MAIN
	[]() {
		return updateParam(bankInputs.calFactors.ka_t2, bankInputs.t2_daq_value);
	},

	// ALPHACAL -> MAIN
	[]() {

		if (keyPadRx->dataReady()) {	// espera comando terminado en #
			bankLeds.beep(100, 1, 1);
			double dblReadVal = String(keyPadRx->buffer()).toDouble();// convierte entrada a Double

			// calcula el factor de calibración para param
			bankInputs.calFactors.kb_alpha = int(dblReadVal - bankInputs.encoder->getPosition() * 360 / 2000);
			bankInputs.setAngleOffset(bankInputs.calFactors.kb_alpha);

			Serial << "\n-------\nNew Value: " << _FLOAT(bankInputs.calFactors.kb_alpha, 3);
			bankInputs.saveSettings();
			return true;  // --> to main menu
		}

		return false;  // keep reading keypad
	},


	// MVMAX -> MAIN
	[]() {
		return updateParam(bankInputs.testParms.max_mass_vel, 1);
	},

	// BVMAX -> MAIN
	[]() {
		return updateParam(bankInputs.testParms.brake_mass_vel_max, 1);
	},

	// BVMIN -> MAIN
	[]() {
		return updateParam(bankInputs.testParms.brake_mass_vel_min, 1);
	},

	// PHLIM -> MAIN
	[]() {
		return updateParam(bankInputs.testParms.ph_threshold, 1);
	},

	// PFLIM -> MAIN
	[]() {
		return updateParam(bankInputs.testParms.pf_threshold, 1);
	},

	// T1HOT -> MAIN
	[]() {
		return updateParam(bankInputs.testParms.t1_hot, 1);
	},

	// T2HOT -> MAIN
	[]() {
		return updateParam(bankInputs.testParms.t2_hot, 1);
	},


};


#endif /* MENUTRANSITIONS_H_ */
