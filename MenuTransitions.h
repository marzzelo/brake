/*
 * MenuTransitions.h
 *
 *  Created on: 28 sep. 2021
 *      Author: valdez
 */

#ifndef MENUTRANSITIONS_H_
#define MENUTRANSITIONS_H_

extern void print_header(String s, bool closed = true);

extern void print_item(String s);

extern void make_item(char *fmt, double value);

extern void print_header(String s, bool closed = true);


#define ADD_TRANSITION_FROM_MAIN_KEY(key) 	[]() { if (ev_key[(key)]) { ev_key[(key)] = false; return true;} return false; }


bool updateParam(double &param, uint16_t daqValue) {
	if (keyPadRx->dataReady()) {	// espera comando terminado en #
		bankLeds.beep(100, 1, 1);
		double dblReadVal = String(keyPadRx->buffer()).toDouble();// convierte entrada a Double
		param = dblReadVal / daqValue;// calcula el factor de calibración para param
		Serial << "\n-------\nNew Value: " << _FLOAT(param, 3);
		bankInputs.saveSettings();
		return true;  // --> to main menu
	}
	return false;  // keep reading keypad
}


/**
 * Array of transition functions.
 * NOTE: ORDER MUST FOLLOW THE ENUM  MenuFSM::Transitions
 */
bool (*transitions[])(void) = {

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



/**
 * Array of on-entering functions.
 * NOTE: ORDER MUST FOLLOW THE ENUM  MenuFSM::OnEnterings
 */
void (*onEnterings[])(void) = {

	// -> IDLE  (index = 0)
	[]() {
		keyPadRx->asterisk('*');
		checkCommands = true;  // modo normal al salir de menu
		bankLeds.display('P');
//		Serial << F("\n------\nENTERING ST_MENU_IDLE\n\n");
		FSM.SetState(ST_IDLE, false, true);
	},


	// -> MAIN MENU
	[]() {
		keyPadRx->asterisk('.');
		keyPadRx->start();
		checkCommands = true;  // modo normal al salir de menu
		ev_key[0] = false;  // clear eventual buffered event;

		// TODO: Make print_header a method of a class, and add a reference to the class in the constructor
		print_header("CONFIGURACIÓN DEL SISTEMA", false);
		print_header("PARÁMETROS DE CALIBRACIÓN");

		make_item("1. Calibrar Vel Rueda [k = %s]", bankInputs.calFactors.ka_wheel);
		make_item("2. Calibrar Ph [k = %s]", bankInputs.calFactors.ka_ph);
		make_item("3. Calibrar Pf [k = %s]", bankInputs.calFactors.ka_pf);
		make_item("4. Calibrar T1 [k = %s]", bankInputs.calFactors.ka_t1);
		make_item("5. Calibrar T2 [k = %s]", bankInputs.calFactors.ka_t2);
		make_item("6. Calibrar Ángulo a [k = %s]", bankInputs.calFactors.kb_alpha);

		print_header("PARÁMETROS DE ENSAYO");

		make_item("7. Vel Máx de Masa: %s", bankInputs.testParms.max_mass_vel);
		make_item("8. Vel Lím Sup de Frenado: %s", bankInputs.testParms.brake_mass_vel_max);
		make_item("9. Vel Lím Inf de Frenado: %s", bankInputs.testParms.brake_mass_vel_min);
		make_item("10. Presión Nom de Horquilla: %s", bankInputs.testParms.ph_threshold);
		make_item("11.Presión Nom de Freno: %s", bankInputs.testParms.pf_threshold);
		make_item("12.Temperatura T1: %s", bankInputs.testParms.t1_hot);
		make_item("13.Temperatura T2: %s", bankInputs.testParms.t2_hot);


		print_header("0. Salir del Menú");
		Serial << "\n\n==>";
	},

	// -> WHEELCAL
	[ ] ( ) {
		Serial << "\n\nCALIBRACIÓN VEL DE RUEDA";
		Serial << "\nGirar rueda a velocidad cte. e ingresar valor real:";
		Serial << "\n\nVelocidad [rpm] ==> ";
	},

	// -> PHCAL
	[ ] ( ) {
		Serial << "\n\nCALIBRACIÓN PRESIÓN DE HORQUILLA";
		Serial << "\nAplicar presión de Horquilla cte. e ingresar valor real:";
		Serial << "\n\nPresión H [bar] ==> ";
	},

	// -> PFCAL
	[ ] ( ) {
		Serial << "\n\nCALIBRACIÓN PRESIÓN DE FRENO";
		Serial << "\nAplicar presión de Freno cte. e ingresar valor real:";
		Serial << "\n\nPresión F [bar] ==> ";
	},

	// -> T1CAL
	[ ] ( ) {
		Serial << "\n\nCALIBRACIÓN TEMP 1";
		Serial << "\nAplicar temperatura 1 e ingresar valor real:";
		Serial << "\n\nTemperatura [°C] ==> ";
	},

	// -> T2CAL
	[ ] ( ) {
		Serial << "\n\nCALIBRACIÓN TEMP 2";
		Serial << "\nAplicar temperatura 2 e ingresar valor real:";
		Serial << "\n\nTemperatura [°C] ==> ";
	},

	// -> ALPHACAL
	[ ] ( ) {
		Serial << "\n\nCALIBRACIÓN ÁNGULO";
		Serial << "\nPosicionar en un ángulo conocido e ingresar el valor:";
		Serial << "\n\nÁngulo [grados] ==> ";
	},



	// -> MVMAX     (index = 8)
	[ ] ( ) {
		Serial << "\n\nVELOCIDAD MÁXIMA DE MASA - Actual: " << _FLOAT(bankInputs.testParms.max_mass_vel, 3);
		Serial << "\n\nVelocidad máxima [rpm] ==> ";
	},

	// -> VBMAX
	[ ] ( ) {
		Serial << "\n\nLIM SUPERIOR VEL DE FRENO - Actual: " << _FLOAT(bankInputs.testParms.brake_mass_vel_max, 3);
		Serial << "\n\nLímite superior [rpm] ==> ";
	},

	// -> VBMIN
	[ ] ( ) {
		Serial << "\n\nLIM INFERIOR VEL DE FRENO - Actual: " << _FLOAT(bankInputs.testParms.brake_mass_vel_min, 3);
		Serial << "\n\nLímite inferior [rpm] ==> ";
	},

	// -> PHLIM
	[ ] ( ) {
		Serial << "\n\nPRESIÓN DE HORQUILLA NOMINAL - Actual: " << _FLOAT(bankInputs.testParms.ph_threshold, 3);
		Serial << "\n\nPresión nominal [bar] ==> ";
	},

	// -> PFLIM
	[ ] ( ) {
		Serial << "\n\nPRESIÓN DE FRENO NOMINAL - Actual: " << _FLOAT(bankInputs.testParms.pf_threshold, 3);
		Serial << "\n\nPresión nominal [bar] ==> ";
	},

	// -> T1HOT
	[ ] ( ) {
		Serial << "\n\nTEMPERATURA LÍMITE 1 - Actual: " << _FLOAT(bankInputs.testParms.t1_hot, 3);
		Serial << "\n\nTemp límite 1 [°C] ==> ";
	},

	// -> T2HOT
	[ ] ( ) {
		Serial << "\n\nTEMPERATURA LÍMITE 2 - Actual: " << _FLOAT(bankInputs.testParms.t2_hot, 3);
		Serial << "\n\nTemp límite 2 [°C] ==> ";
	},

};


/**
 * Array of on-leaving functions.
 * NOTE: ORDER MUST FOLLOW THE ENUM  MenuFSM::OnLeavings
 */
void (*onLeavings[])(void) = {

	// IDLE ->
	[] () {
		Serial << "\n------\nLEAVING ST_MENU_IDLE\n\n";
	},

	// MENU MAIN ->
	[ ] ( ) {
		checkCommands = false;
		keyPadRx->start( );
	},

};

#endif /* MENUTRANSITIONS_H_ */
