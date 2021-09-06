/*
 * MENU.h
 *
 *  Created on: 1 sep. 2021
 *      Author: valdez
 */

#ifndef MENU_H_
#define MENU_H_

//void saveSettings();

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
		param = dblReadVal / daqValue;	// calcula el factor de calibración para param
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

	ADD_MENU_TRANSITION_TO(ST_MENU_MVMAX_PAR, 4);

	ADD_MENU_TRANSITION_TO(ST_MENU_BVMAX_PAR, 5);

	ADD_MENU_TRANSITION_TO(ST_MENU_BVMIN_PAR, 6);

	ADD_MENU_TRANSITION_TO(ST_MENU_PH_PAR, 7);

	ADD_MENU_TRANSITION_TO(ST_MENU_PF_PAR, 8);


	MENU.AddTransition(ST_MENU_WHEEL_CAL, ST_MENU_MAIN, []() {
		return updateParam(bank.calFactors.ka_wheel, bankInputs.wheel_daq_value);
	});

	MENU.AddTransition(ST_MENU_PH_CAL, ST_MENU_MAIN, []() {
		return updateParam(bank.calFactors.ka_ph, bankInputs.ph_daq_value);
	});

	MENU.AddTransition(ST_MENU_PF_CAL, ST_MENU_MAIN, []() {
		return updateParam(bank.calFactors.ka_pf, bankInputs.pf_daq_value);
	});

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
		Serial << F("\n\nCALIBRACIÓN");
		Serial << F("\n1. Calibrar Vel Rueda [k = ") << _FLOAT(bank.calFactors.ka_wheel, 3) << "]";
		Serial << F("\n2. Calibrar Ph [k = ") << _FLOAT(bank.calFactors.ka_ph, 3) << "]";
		Serial << F("\n3. Calibrar Pf [k = ") << _FLOAT(bank.calFactors.ka_pf, 3) << "]";

		Serial << F("\n\nPARÁMETROS DE ENSAYO");
		Serial << F("\n4. Vel Max de Masa: ") << _FLOAT(bank.testParms.max_mass_vel, 3);
		Serial << F("\n5. Vel Lím Sup de Frenado: ") << _FLOAT(bank.testParms.brake_mass_vel_max, 3);
		Serial << F("\n6. Vel Lím Inf de Frenado: ") << _FLOAT(bank.testParms.brake_mass_vel_min, 3);
		Serial << F("\n7. Presión Nom de Horquilla: ") << _FLOAT(bank.testParms.ph_threshold, 3);
		Serial << F("\n8. Presión Nom de Freno: ")  << _FLOAT(bank.testParms.pf_threshold, 3);

		Serial << F("\n0. Salir del Menú");
		Serial << F("\n\n==>");

			});

	MENU.SetOnEntering(ST_MENU_WHEEL_CAL, []() {

		Serial << F("\n\nCALIBRACIÓN VEL DE RUEDA");
		Serial << F("\nGirar rueda a velocidad cte. e ingresar valor real:");
		Serial << F("\n\nVelocidad [rpm] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_PH_CAL, []() {

		Serial << F("\n\nCALIBRACIÓN PRESIÓN DE HORQUILLA");
		Serial << F("\nAplicar presión de Horquilla cte. e ingresar valor real:");
		Serial << F("\n\nPresión H [bar] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_PF_CAL, []() {

		Serial << F("\n\nCALIBRACIÓN PRESIÓN DE FRENO");
		Serial << F("\nAplicar presión de Freno cte. e ingresar valor real:");
		Serial << F("\n\nPresión F [bar] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_MVMAX_PAR, []() {

		Serial << F("\n\nVELOCIDAD MÁXIMA DE MASA - Actual: ") << _FLOAT(bank.testParms.max_mass_vel, 3);
		Serial << F("\n\nVelocidad máxima [rpm] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_BVMAX_PAR, []() {

		Serial << F("\n\nLIM SUPERIOR VEL DE FRENO - Actual: ") << _FLOAT(bank.testParms.brake_mass_vel_max, 3);
		Serial << F("\n\nLímite superior [rpm] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_BVMIN_PAR, []() {

		Serial << F("\n\nLIM INFERIOR VEL DE FRENO - Actual: ") << _FLOAT(bank.testParms.brake_mass_vel_min, 3);
		Serial << F("\n\nLímite inferior [rpm] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_PH_PAR, []() {

		Serial << F("\n\nPRESIÓN DE HORQUILLA NOMINAL - Actual: ") << _FLOAT(bank.testParms.ph_threshold, 3);
		Serial << F("\n\nPresión nominal [bar] ==> ");

		keyPadEnabled = false;  // handle keypad here
		keyPadRx->asterisk('.');
	});

	MENU.SetOnEntering(ST_MENU_PF_PAR, []() {

		Serial << F("\n\nPRESIÓN DE FRENO NOMINAL - Actual: ") << _FLOAT(bank.testParms.pf_threshold, 3);
		Serial << F("\n\nPresión nominal [bar] ==> ");

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
