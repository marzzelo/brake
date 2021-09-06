/*
 * MENU.h
 *
 *  Created on: 1 sep. 2021
 *      Author: valdez
 */

#ifndef MENU_H_
#define MENU_H_

void saveSettings();

// STATES
enum MenuState {
	ST_MENU_IDLE = 0,
	ST_MENU_MAIN,

	ST_MENU_WHEEL_CAL,
	ST_MENU_PH_CAL,
	ST_MENU_PF_CAL,
	ST_MENU_MVMAX_PAR,
	ST_MENU_BVMAX_PAR,
	ST_MENU_BVMIN_PAR,
	ST_MENU_PH_PAR,
	ST_MENU_PF_PAR,

	ST_MENU_COUNT
};

int st_menu;


#define ADD_MENU_TRANSITION_TO(state, key) 	MENU.AddTransition(ST_MENU_MAIN, (state), []() {  	\
													if (ev_key[(key)]) { ev_key[(key)] = false;	\
														return true; } return false; });

#define MENU_TRANSITIONS	30

StateMachine MENU(ST_MENU_COUNT, MENU_TRANSITIONS);

bool ev_key[10] = { false };
bool ev_cmd[10] = { false };

bool updateParam(double &param, uint16_t daqValue) {
	if (keyPadRx->dataReady()) {	// espera comando terminado en #
		buzz->start(10, 1, 1);
		double dblReadVal = String(keyPadRx->buffer()).toDouble();	// convierte entrada a Double
		param = dblReadVal / daqValue;	// calcula el factor de calibraci�n para param
		Serial << "\n-------\nNew Value: " << _FLOAT(param, 3);
		saveSettings();
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

	ADD_MENU_TRANSITION_TO(ST_MENU_MVMAX_PAR, 4);

	ADD_MENU_TRANSITION_TO(ST_MENU_BVMAX_PAR, 5);

	ADD_MENU_TRANSITION_TO(ST_MENU_BVMIN_PAR, 6);

	ADD_MENU_TRANSITION_TO(ST_MENU_PH_PAR, 7);

	ADD_MENU_TRANSITION_TO(ST_MENU_PF_PAR, 8);


	MENU.AddTransition(ST_MENU_WHEEL_CAL, ST_MENU_MAIN, []() {
		return updateParam(calFactors.ka_wheel, wheel_daq_value);
	});

	MENU.AddTransition(ST_MENU_PH_CAL, ST_MENU_MAIN, []() {
		return updateParam(calFactors.ka_ph, ph_daq_value);
	});

	MENU.AddTransition(ST_MENU_PF_CAL, ST_MENU_MAIN, []() {
		return updateParam(calFactors.ka_pf, pf_daq_value);
	});

	MENU.AddTransition(ST_MENU_MVMAX_PAR, ST_MENU_MAIN, []() {
		return updateParam(testParms.max_mass_vel, 1);
	});

	MENU.AddTransition(ST_MENU_BVMAX_PAR, ST_MENU_MAIN, []() {
		return updateParam(testParms.brake_mass_vel_max, 1);
	});

	MENU.AddTransition(ST_MENU_BVMIN_PAR, ST_MENU_MAIN, []() {
		return updateParam(testParms.brake_mass_vel_min, 1);
	});

	MENU.AddTransition(ST_MENU_PH_PAR, ST_MENU_MAIN, []() {
		return updateParam(testParms.ph_threshold, 1);
	});

	MENU.AddTransition(ST_MENU_PF_PAR, ST_MENU_MAIN, []() {
		return updateParam(testParms.pf_threshold, 1);
	});

	//////////////////////////////////////////////////////////////////
	// ON_ENTERING
	//////////////////////////////////////////////////////////////////

	MENU.SetOnEntering(ST_MENU_IDLE, []() {
		Serial << F("\n------\n...ST_MENU_IDLE\n\n");
	});

	MENU.SetOnEntering(ST_MENU_MAIN,
			[]() {
				// @formatter:off
		keyPadEnabled = true;
		keyPadRx->asterisk('*');
		ev_key[0] = false;  // clear eventual buffered event;

		keyPadRx->start();


		Serial << F("\n\nMENU PRINCIPAL");
		Serial << F("\n\nCALIBRACI�N");
		Serial << F("\n1. Calibrar Vel Rueda [k = ") << _FLOAT(calFactors.ka_wheel, 3) << "]";
		Serial << F("\n2. Calibrar Ph [k = ") << _FLOAT(calFactors.ka_ph, 3) << "]";
		Serial << F("\n3. Calibrar Pf [k = ") << _FLOAT(calFactors.ka_pf, 3) << "]";

		Serial << F("\n\nPAR�METROS DE ENSAYO");
		Serial << F("\n4. Vel Max de Masa: ") << _FLOAT(testParms.max_mass_vel, 3);
		Serial << F("\n5. Vel L�m Sup de Frenado: ") << _FLOAT(testParms.brake_mass_vel_max, 3);
		Serial << F("\n6. Vel L�m Inf de Frenado: ") << _FLOAT(testParms.brake_mass_vel_min, 3);
		Serial << F("\n7. Presi�n Nom de Horquilla: ") << _FLOAT(testParms.ph_threshold, 3);
		Serial << F("\n8. Presi�n Nom de Freno: ")  << _FLOAT(testParms.pf_threshold, 3);

		Serial << F("\n0. Salir del Men�");
		Serial << F("\n\n==>");

			});

	MENU.SetOnEntering(ST_MENU_WHEEL_CAL, []() {

		Serial << F("\n\nCALIBRACI�N VEL DE RUEDA");
		Serial << F("\nGirar rueda a velocidad cte. e ingresar valor real:");
		Serial << F("\n\nVelocidad [rpm] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_PH_CAL, []() {

		Serial << F("\n\nCALIBRACI�N PRESI�N DE HORQUILLA");
		Serial << F("\nAplicar presi�n de Horquilla cte. e ingresar valor real:");
		Serial << F("\n\nPresi�n H [bar] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_PF_CAL, []() {

		Serial << F("\n\nCALIBRACI�N PRESI�N DE FRENO");
		Serial << F("\nAplicar presi�n de Freno cte. e ingresar valor real:");
		Serial << F("\n\nPresi�n F [bar] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_MVMAX_PAR, []() {

		Serial << F("\n\nVELOCIDAD M�XIMA DE MASA - Actual: ") << _FLOAT(testParms.max_mass_vel, 3);
		Serial << F("\n\nVelocidad m�xima [rpm] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_BVMAX_PAR, []() {

		Serial << F("\n\nLIM SUPERIOR VEL DE FRENO - Actual: ") << _FLOAT(testParms.brake_mass_vel_max, 3);
		Serial << F("\n\nL�mite superior [rpm] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_BVMIN_PAR, []() {

		Serial << F("\n\nLIM INFERIOR VEL DE FRENO - Actual: ") << _FLOAT(testParms.brake_mass_vel_min, 3);
		Serial << F("\n\nL�mite inferior [rpm] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_PH_PAR, []() {

		Serial << F("\n\nPRESI�N DE HORQUILLA NOMINAL - Actual: ") << _FLOAT(testParms.ph_threshold, 3);
		Serial << F("\n\nPresi�n nominal [bar] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_PF_PAR, []() {

		Serial << F("\n\nPRESI�N DE FRENO NOMINAL - Actual: ") << _FLOAT(testParms.pf_threshold, 3);
		Serial << F("\n\nPresi�n nominal [bar] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	//////////////////////////////////////////////////////////////////
	// ON_LEAVING
	//////////////////////////////////////////////////////////////////

//	MENU.SetOnLeaving(ST_MENU_MAIN, []() {
//		FSM.SetState(ST_IDLE, false, true);
//	});

	for (st_menu = ST_MENU_WHEEL_CAL; st_menu <= ST_MENU_PF_PAR; ++st_menu) {
		MENU.SetOnLeaving(st_menu, []() {
			keyPadEnabled = true;
			keyPadRx->asterisk('*');
			Serial << "\n...leaving state #" << st_menu;
		});
	}

}

#endif /* MENU_H_ */
