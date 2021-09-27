/*
 * MENU.h
 *
 *  Created on: 1 sep. 2021
 *      Author: valdez
 */


#ifndef MENU_H_
#define MENU_H_

const String separator = "\n+----------------------------------------------------------+";
char spaces[60];
char text[60];
char num[10];

#define print_separator() 	Serial << separator;

void print_item(String s) {
	int i;
	for (i = 0; i < 60 - 3 - s.length(); ++i)
		spaces[i] = ' ';

	spaces[i] = '\0';

	Serial << "\n| " << s << spaces << "|";
}

void make_item(char *fmt, double value) {
	dtostrf(value, 0, 3, num);
	sprintf(text, fmt, num);
	print_item(text);
}

void print_header(String s, bool closed = true) {
	print_separator();
	print_item(s);
	if (closed) print_separator();
}

extern KeyPadRX *keyPadRx;
extern bool ev_key[], ev_cmd[];

/**
 * # Estados para la FSM de Menu del sistema
 */
enum MenuState {
	ST_MENU_IDLE = 0,		//!< Men� idle
	ST_MENU_MAIN,			//!< Men� principal

	ST_MENU_WHEEL_CAL,		//!< Submen� para calibraci�n de velocidad de rueda
	ST_MENU_PH_CAL,			//!< Submen� para calibraci�n de presi�n de horquilla
	ST_MENU_PF_CAL,			//!< Submen� para calibraci�n de presi�n de freno
	ST_MENU_T1_CAL,			//!< Submen� para calibraci�n de temperatura 1
	ST_MENU_T2_CAL,			//!< Submen� para calibraci�n de temperatura 2
	ST_MENU_ALPHA_CAL,		//!< Submen� para calibraci�n de �ngulo

	ST_MENU_MVMAX_PAR,		//!< Submen� para establecer velocidad m�xima de masa
	ST_MENU_BVMAX_PAR,		//!< Submen� para establecer velocidad m�xima de frenado
	ST_MENU_BVMIN_PAR,		//!< Submen� para establecer velocidad m�nima de frenado
	ST_MENU_PH_PAR,			//!< Submen� para establecer presi�n de horquilla nominal
	ST_MENU_PF_PAR,			//!< Submen� para establecer presi�n nominal de freno
	ST_MENU_T1HOT_PAR,		//!< Submen� para establecer temp1 hot
	ST_MENU_T2HOT_PAR,		//!< Submen� para establecer temp2 hot

	ST_MENU_COUNT			//!< reserved
};

int st_menu;

#define ADD_MENU_TRANSITION_TO(state, key) 	MENU.AddTransition(ST_MENU_MAIN, (state), []() {  	\
													if (ev_key[(key)]) { ev_key[(key)] = false;	\
														return true; } return false; });

#define MENU_TRANSITIONS	35

StateMachine MENU(ST_MENU_COUNT, MENU_TRANSITIONS);

bool updateParam(double &param, uint16_t daqValue) {
	if (keyPadRx->dataReady()) {	// espera comando terminado en #
		bankLeds.beep(100, 1, 1);
		double dblReadVal = String(keyPadRx->buffer()).toDouble();// convierte entrada a Double
		param = dblReadVal / daqValue;// calcula el factor de calibraci�n para param
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
		if (ev_key[1]) {
			ev_key[1] = false;
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

	ADD_MENU_TRANSITION_TO(ST_MENU_ALPHA_CAL, 6);

	//-------------------------------------------------------------------------

	ADD_MENU_TRANSITION_TO(ST_MENU_MVMAX_PAR, 7);

	ADD_MENU_TRANSITION_TO(ST_MENU_BVMAX_PAR, 8);

	ADD_MENU_TRANSITION_TO(ST_MENU_BVMIN_PAR, 9);

	ADD_MENU_TRANSITION_TO(ST_MENU_PH_PAR, 10);

	ADD_MENU_TRANSITION_TO(ST_MENU_PF_PAR, 11);

	ADD_MENU_TRANSITION_TO(ST_MENU_T1HOT_PAR, 12);

	ADD_MENU_TRANSITION_TO(ST_MENU_T2HOT_PAR, 13);

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

	MENU.AddTransition(ST_MENU_ALPHA_CAL, ST_MENU_MAIN, []() {
		if (keyPadRx->dataReady()) {	// espera comando terminado en #
			bankLeds.beep(100, 1, 1);
			double dblReadVal = String(keyPadRx->buffer()).toDouble();// convierte entrada a Double
			bank.calFactors.kb_alpha = int(dblReadVal - bankInputs.encoder->getPosition() * 360 / 2000);// calcula el factor de calibraci�n para param
			bankInputs.setAngleOffset(bank.calFactors.kb_alpha);
			Serial << "\n-------\nNew Value: " << _FLOAT(bank.calFactors.kb_alpha, 3);
			bank.saveSettings();
			return true;  // --> to main menu
		}
		return false;  // keep reading keypad
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
//		Serial << F("\n------\nENTERING ST_MENU_IDLE\n\n");
		FSM.SetState(ST_IDLE, false, true);
	});

	MENU.SetOnEntering(ST_MENU_MAIN, []() {
		keyPadRx->asterisk('.');
		keyPadRx->start();
		checkCommands = true;  // modo normal al salir de menu
		ev_key[0] = false;  // clear eventual buffered event;

		print_header("CONFIGURACI�N DEL SISTEMA", false);
		print_header("PAR�METROS DE CALIBRACI�N");

		make_item("1. Calibrar Vel Rueda [k = %s]", bank.calFactors.ka_wheel);
		make_item("2. Calibrar Ph [k = %s]", bank.calFactors.ka_ph);
		make_item("3. Calibrar Pf [k = %s]", bank.calFactors.ka_pf);
		make_item("4. Calibrar T1 [k = %s]", bank.calFactors.ka_t1);
		make_item("5. Calibrar T2 [k = %s]", bank.calFactors.ka_t2);
		make_item("6. Calibrar �ngulo a [k = %s]", bank.calFactors.kb_alpha);

		print_header("PAR�METROS DE ENSAYO");

		make_item("7. Vel M�x de Masa: %s", bank.testParms.max_mass_vel);
		make_item("8. Vel L�m Sup de Frenado: %s", bank.testParms.brake_mass_vel_max);
		make_item("9. Vel L�m Inf de Frenado: %s", bank.testParms.brake_mass_vel_min);
		make_item("10. Presi�n Nom de Horquilla: %s", bank.testParms.ph_threshold);
		make_item("11.Presi�n Nom de Freno: %s", bank.testParms.pf_threshold);
		make_item("12.Temperatura T1: %s", bank.testParms.t1_hot);
		make_item("13.Temperatura T2: %s", bank.testParms.t2_hot);


		print_header("0. Salir del Men�");
		Serial << F("\n\n==>");
	});

	//----- AJUSTES DE CALIBRACI�N ---------------------------------------------------------

	MENU.SetOnEntering (ST_MENU_WHEEL_CAL , [ ] ( ) {
		Serial << F("\n\nCALIBRACI�N VEL DE RUEDA");
		Serial << F("\nGirar rueda a velocidad cte. e ingresar valor real:");
		Serial << F("\n\nVelocidad [rpm] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_PH_CAL , [ ] ( ) {
		Serial << F("\n\nCALIBRACI�N PRESI�N DE HORQUILLA");
		Serial << F("\nAplicar presi�n de Horquilla cte. e ingresar valor real:");
		Serial << F("\n\nPresi�n H [bar] ==> ");
	});

	MENU.SetOnEntering ( ST_MENU_PF_CAL , [ ] ( ) {
		Serial << F("\n\nCALIBRACI�N PRESI�N DE FRENO");
		Serial << F("\nAplicar presi�n de Freno cte. e ingresar valor real:");
		Serial << F("\n\nPresi�n F [bar] ==> ");
	});

	MENU.SetOnEntering ( ST_MENU_T1_CAL , [ ] ( ) {
		Serial << F("\n\nCALIBRACI�N TEMP 1");
		Serial << F("\nAplicar temperatura 1 e ingresar valor real:");
		Serial << F("\n\nTemperatura [�C] ==> ");
	});

	MENU.SetOnEntering ( ST_MENU_T2_CAL , [ ] ( ) {
		Serial << F("\n\nCALIBRACI�N TEMP 2");
		Serial << F("\nAplicar temperatura 2 e ingresar valor real:");
		Serial << F("\n\nTemperatura [�C] ==> ");
	});

	MENU.SetOnEntering ( ST_MENU_ALPHA_CAL , [ ] ( ) {
		Serial << F("\n\nCALIBRACI�N �NGULO");
		Serial << F("\nPosicionar en un �ngulo conocido e ingresar el valor:");
		Serial << F("\n\n�ngulo [grados] ==> ");
	});

	//----- PAR�METROS --------------------------------------------------------------------

	MENU.SetOnEntering (ST_MENU_MVMAX_PAR , [ ] ( ) {
		Serial << F("\n\nVELOCIDAD M�XIMA DE MASA - Actual: ") << _FLOAT(bank.testParms.max_mass_vel, 3);
		Serial << F("\n\nVelocidad m�xima [rpm] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_BVMAX_PAR , [ ] ( ) {
		Serial << F("\n\nLIM SUPERIOR VEL DE FRENO - Actual: ") << _FLOAT(bank.testParms.brake_mass_vel_max, 3);
		Serial << F("\n\nL�mite superior [rpm] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_BVMIN_PAR , [ ] ( ) {
		Serial << F("\n\nLIM INFERIOR VEL DE FRENO - Actual: ") << _FLOAT(bank.testParms.brake_mass_vel_min, 3);
		Serial << F("\n\nL�mite inferior [rpm] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_PH_PAR , [ ] ( ) {
		Serial << F("\n\nPRESI�N DE HORQUILLA NOMINAL - Actual: ") << _FLOAT(bank.testParms.ph_threshold, 3);
		Serial << F("\n\nPresi�n nominal [bar] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_PF_PAR , [ ] ( ) {
		Serial << F("\n\nPRESI�N DE FRENO NOMINAL - Actual: ") << _FLOAT(bank.testParms.pf_threshold, 3);
		Serial << F("\n\nPresi�n nominal [bar] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_T1HOT_PAR , [ ] ( ) {
		Serial << F("\n\nTEMPERATURA L�MITE 1 - Actual: ") << _FLOAT(bank.testParms.t1_hot, 3);
		Serial << F("\n\nTemp l�mite 1 [�C] ==> ");
	});

	MENU.SetOnEntering (ST_MENU_T2HOT_PAR , [ ] ( ) {
		Serial << F("\n\nTEMPERATURA L�MITE 2 - Actual: ") << _FLOAT(bank.testParms.t2_hot, 3);
		Serial << F("\n\nTemp l�mite 2 [�C] ==> ");
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
