/*
 * MENU.h
 *
 *  Created on: 1 sep. 2021
 *      Author: valdez
 */


#ifndef MENU_H_
#define MENU_H_

extern KeyPadRX *keyPadRx;

/**
 * # Estados para la FSM de Menu del sistema
 */
enum MenuState {
	ST_MENU_IDLE = 0,		//!< Menú oculto
	ST_MENU_MAIN,			//!< Menú principal

	ST_MENU_WHEEL_CAL,		//!< Submenú para calibración de velocidad de rueda
	ST_MENU_PH_CAL,			//!< Submenú para calibración de presión de horquilla
	ST_MENU_PF_CAL,			//!< Submenú para calibración de presión de freno
	ST_MENU_T1_CAL,			//!< Submenú para calibración de temperatura 1
	ST_MENU_T2_CAL,			//!< Submenú para calibración de temperatura 2

	ST_MENU_MVMAX_PAR,		//!< Submenú para establecer velocidad máxima de masa
	ST_MENU_BVMAX_PAR,		//!< Submenú para establecer velocidad máxima de frenado
	ST_MENU_BVMIN_PAR,		//!< Submenú para establecer velocidad mínima de frenado
	ST_MENU_PH_PAR,			//!< Submenú para establecer presión de horquilla nominal
	ST_MENU_PF_PAR,			//!< Submenú para establecer presión nominal de freno
	ST_MENU_T1HOT_PAR,		//!< Submenú para establecer temp1 hot
	ST_MENU_T2HOT_PAR,		//!< Submenú para establecer temp2 hot

	ST_MENU_COUNT			//!< reserved
};

int st_menu;

#define ADD_MENU_TRANSITION_TO(state, key) 	MENU.AddTransition(ST_MENU_MAIN, (state), []() {  	\
													if (ev_key[(key)]) { ev_key[(key)] = false;	\
														return true; } return false; });

#define MENU_TRANSITIONS	35

StateMachine MENU(ST_MENU_COUNT, MENU_TRANSITIONS);

bool ev_key[16] = { false };
bool ev_cmd[10] = { false };

bool updateParam(double &param, uint16_t daqValue) {
	if (keyPadRx->dataReady()) {	// espera comando terminado en #
		bankLeds.beep(100, 1, 1);
		double dblReadVal = String(keyPadRx->buffer()).toDouble();// convierte entrada a Double
		param = dblReadVal / daqValue;// calcula el factor de calibración para param
		Serial << "\n-------\nNew Value: " << _FLOAT(param, 3);
		bank.saveSettings();
		return true;  // --> to main menu
	}
	return false;  // keep reading keypad
}

void setupMENU() {

//////////////////////////////////////////////////////////////////
// 					T R A N S I T I O N S
//////////////////////////////////////////////////////////////////
	MENU.AddTransition(ST_MENU_IDLE, ST_MENU_MAIN, []() {
		if (ev_cmd[1]) {		// cmd[1]:  *1#
			ev_cmd[1] = false;
			return true;
		}
		return false;
	});

	ADD_MENU_TRANSITION_TO(ST_MENU_IDLE, 0);

	ADD_MENU_TRANSITION_TO(ST_MENU_WHEEL_CAL, 1);

	ADD_MENU_TRANSITION_TO(ST_MENU_PH_CAL, 2);

	ADD_MENU_TRANSITION_TO(ST_MENU_PF_CAL, 3);

	ADD_MENU_TRANSITION_TO(ST_MENU_T1_CAL, 4);

	ADD_MENU_TRANSITION_TO(ST_MENU_T2_CAL, 5);

	//-------------------------------------------------------------------------

	ADD_MENU_TRANSITION_TO(ST_MENU_MVMAX_PAR, 6);

	ADD_MENU_TRANSITION_TO(ST_MENU_BVMAX_PAR, 7);

	ADD_MENU_TRANSITION_TO(ST_MENU_BVMIN_PAR, 8);

	ADD_MENU_TRANSITION_TO(ST_MENU_PH_PAR, 9);

	ADD_MENU_TRANSITION_TO(ST_MENU_PF_PAR, 10);

	ADD_MENU_TRANSITION_TO(ST_MENU_T1HOT_PAR, 11);

	ADD_MENU_TRANSITION_TO(ST_MENU_T2HOT_PAR, 12);

	//-------------------------------------------------------------------------

	MENU.AddTransition(ST_MENU_WHEEL_CAL, ST_MENU_MAIN, []() {
		return updateParam(bank.calFactors.ka_wheel, bankInputs.wheel_daq_value);
	});

	MENU.AddTransition(ST_MENU_PH_CAL, ST_MENU_MAIN, []() {
		return updateParam(bank.calFactors.ka_ph, bankInputs.ph_daq_value);
	});

	MENU.AddTransition(ST_MENU_PF_CAL, ST_MENU_MAIN, []() {
		return updateParam(bank.calFactors.ka_pf, bankInputs.pf_daq_value);
	});

	MENU.AddTransition(ST_MENU_T1_CAL, ST_MENU_MAIN, []() {
		return updateParam(bank.calFactors.ka_t1, bankInputs.t1_daq_value);
	});

	MENU.AddTransition(ST_MENU_T2_CAL, ST_MENU_MAIN, []() {
		return updateParam(bank.calFactors.ka_t2, bankInputs.t2_daq_value);
	});

	//-------------------------------------------------------------------------

	MENU.AddTransition(ST_MENU_MVMAX_PAR, ST_MENU_MAIN, []() {
		return updateParam(bank.testParms.max_mass_vel, 1);
	});

	MENU.AddTransition(ST_MENU_BVMAX_PAR, ST_MENU_MAIN, []() {
		return updateParam(bank.testParms.brake_mass_vel_max, 1);
	});

	MENU.AddTransition(ST_MENU_BVMIN_PAR, ST_MENU_MAIN, []() {
		return updateParam(bank.testParms.brake_mass_vel_min, 1);
	});

	MENU.AddTransition(ST_MENU_PH_PAR, ST_MENU_MAIN, []() {
		return updateParam(bank.testParms.ph_threshold, 1);
	});

	MENU.AddTransition(ST_MENU_PF_PAR, ST_MENU_MAIN, []() {
		return updateParam(bank.testParms.pf_threshold, 1);
	});

	MENU.AddTransition(ST_MENU_T1HOT_PAR, ST_MENU_MAIN, []() {
		return updateParam(bank.testParms.t1_hot, 1);
	});

	MENU.AddTransition(ST_MENU_T2HOT_PAR, ST_MENU_MAIN, []() {
		return updateParam(bank.testParms.t2_hot, 1);
	});

	//////////////////////////////////////////////////////////////////
	// ON_ENTERING
	//////////////////////////////////////////////////////////////////

	MENU.SetOnEntering(ST_MENU_IDLE, []() {
		keyPadRx->asterisk('*');
		checkCommands = true;  // modo normal al salir de menu
		bankLeds.display('P');
		Serial << F("\n------\nENTERING ST_MENU_IDLE\n\n");
	});

	MENU.SetOnEntering(ST_MENU_MAIN, []() {
		keyPadRx->asterisk('.');
		keyPadRx->start();
		checkCommands = true;  // modo normal al salir de menu
		ev_key[0] = false;  // clear eventual buffered event;

		Serial << F("\n\nMENU PRINCIPAL");
		Serial << F("\n\nCALIBRACIÓN");
		Serial << F("\n1. Calibrar Vel Rueda [k = ") << _FLOAT(bank.calFactors.ka_wheel, 3) << "]";
		Serial << F("\n2. Calibrar Ph [k = ") << _FLOAT(bank.calFactors.ka_ph, 3) << "]";
		Serial << F("\n3. Calibrar Pf [k = ") << _FLOAT(bank.calFactors.ka_pf, 3) << "]";
		Serial << F("\n4. Calibrar T1 [k = ") << _FLOAT(bank.calFactors.ka_t1, 3) << "]";
		Serial << F("\n5. Calibrar T2 [k = ") << _FLOAT(bank.calFactors.ka_t2, 3) << "]";

		Serial << F("\n\nPARÁMETROS DE ENSAYO");
		Serial << F("\n6. Vel Max de Masa: ") << _FLOAT(bank.testParms.max_mass_vel, 3);
		Serial << F("\n7. Vel Lím Sup de Frenado: ") << _FLOAT(bank.testParms.brake_mass_vel_max, 3);
		Serial << F("\n8. Vel Lím Inf de Frenado: ") << _FLOAT(bank.testParms.brake_mass_vel_min, 3);
		Serial << F("\n9. Presión Nom de Horquilla: ") << _FLOAT(bank.testParms.ph_threshold, 3);
		Serial << F("\n10. Presión Nom de Freno: ") << _FLOAT(bank.testParms.pf_threshold, 3);
		Serial << F("\n11. Temperatura 1: ") << _FLOAT(bank.testParms.t1_hot, 3);
		Serial << F("\n12. Temperatura 2: ") << _FLOAT(bank.testParms.t2_hot, 3);

		Serial << F("\n0. Salir del Menú");
		Serial << F("\n\n==>");
	});

	MENU.SetOnEntering (ST_MENU_WHEEL_CAL , [ ] ( ) {
		Serial << F("\n\nCALIBRACIÓN VEL DE RUEDA");
		Serial << F("\nGirar rueda a velocidad cte. e ingresar valor real:");
		Serial << F("\n\nVelocidad [rpm] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_PH_CAL , [ ] ( ) {
		Serial << F("\n\nCALIBRACIÓN PRESIÓN DE HORQUILLA");
		Serial << F("\nAplicar presión de Horquilla cte. e ingresar valor real:");
		Serial << F("\n\nPresión H [bar] ==> ");
	});

	MENU.SetOnEntering ( ST_MENU_PF_CAL , [ ] ( ) {
		Serial << F("\n\nCALIBRACIÓN PRESIÓN DE FRENO");
		Serial << F("\nAplicar presión de Freno cte. e ingresar valor real:");
		Serial << F("\n\nPresión F [bar] ==> ");
	});

	MENU.SetOnEntering ( ST_MENU_T1_CAL , [ ] ( ) {
		Serial << F("\n\nCALIBRACIÓN TEMP 1");
		Serial << F("\nAplicar temperatura 1 e ingresar valor real:");
		Serial << F("\n\nTemperatura [°C] ==> ");
	});

	MENU.SetOnEntering ( ST_MENU_T2_CAL , [ ] ( ) {
		Serial << F("\n\nCALIBRACIÓN TEMP 2");
		Serial << F("\nAplicar temperatura 2 e ingresar valor real:");
		Serial << F("\n\nTemperatura [°C] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_MVMAX_PAR , [ ] ( ) {
		Serial << F("\n\nVELOCIDAD MÁXIMA DE MASA - Actual: ") << _FLOAT(bank.testParms.max_mass_vel, 3);
		Serial << F("\n\nVelocidad máxima [rpm] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_BVMAX_PAR , [ ] ( ) {
		Serial << F("\n\nLIM SUPERIOR VEL DE FRENO - Actual: ") << _FLOAT(bank.testParms.brake_mass_vel_max, 3);
		Serial << F("\n\nLímite superior [rpm] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_BVMIN_PAR , [ ] ( ) {
		Serial << F("\n\nLIM INFERIOR VEL DE FRENO - Actual: ") << _FLOAT(bank.testParms.brake_mass_vel_min, 3);
		Serial << F("\n\nLímite inferior [rpm] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_PH_PAR , [ ] ( ) {
		Serial << F("\n\nPRESIÓN DE HORQUILLA NOMINAL - Actual: ") << _FLOAT(bank.testParms.ph_threshold, 3);
		Serial << F("\n\nPresión nominal [bar] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_PF_PAR , [ ] ( ) {
		Serial << F("\n\nPRESIÓN DE FRENO NOMINAL - Actual: ") << _FLOAT(bank.testParms.pf_threshold, 3);
		Serial << F("\n\nPresión nominal [bar] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_T1HOT_PAR , [ ] ( ) {
		Serial << F("\n\nTEMPERATURA LÍMITE 1 - Actual: ") << _FLOAT(bank.testParms.t1_hot, 3);
		Serial << F("\n\nTemp límite 1 [°C] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_T2HOT_PAR , [ ] ( ) {
		Serial << F("\n\nTEMPERATURA LÍMITE 2 - Actual: ") << _FLOAT(bank.testParms.t2_hot, 3);
		Serial << F("\n\nTemp límite 2 [°C] ==> ");
	});

//////////////////////////////////////////////////////////////////
// ON_LEAVING
//////////////////////////////////////////////////////////////////

	MENU.SetOnLeaving(ST_IDLE, [] () {
		Serial << F("\n------\nLEAVING ST_MENU_IDLE\n\n");
	});

	MENU.SetOnLeaving ( ST_MENU_MAIN , [ ] ( ) {
		checkCommands = false;
		keyPadRx->start( );
	});

	for (st_menu = ST_MENU_WHEEL_CAL; st_menu <= ST_MENU_T2HOT_PAR;	++st_menu	)	{
		MENU.SetOnLeaving(st_menu, [](){
			Serial <<"\n...leaving state #"	<< st_menu;
		}																															)																						;
	}
}

#endif /* MENU_H_ */
