/*
 * MenuFSM.h
 *
 *  Created on: 28 sep. 2021
 *      Author: valdez
 */

#ifndef MENUFSM_H_
#define MENUFSM_H_

#include "StateMachineLib.h"
#include "BankButtons.h"
#include "BankLeds.h"
#include "BankAnalogInputs.h"
#include "Streaming.h"
#include "Arduino.h"


#define N_TRANSITIONS	35

//#define ADD_MENU_TRANSITION_TO(state, key) 	MENU.AddTransition(ST_MENU_MAIN, (state), []() {  	\
//													if (ev_key[(key)]) { ev_key[(key)] = false;	\
//														return true; } return false; });
class MenuFSM {

public:
	MenuFSM(bool (*f)(void));   // TODO: CONVERTIR EN ARRAY Y COMPLETAR TRANSITIONS


private:
	/**
	 * # Estados para la FSM de Menu del sistema
	 */
	enum MenuState {
		ST_MENU_IDLE = 0,		//!< Menú idle
		ST_MENU_MAIN,			//!< Menú principal

		ST_MENU_WHEEL_CAL,		//!< Submenú para calibración de velocidad de rueda
		ST_MENU_PH_CAL,			//!< Submenú para calibración de presión de horquilla
		ST_MENU_PF_CAL,			//!< Submenú para calibración de presión de freno
		ST_MENU_T1_CAL,			//!< Submenú para calibración de temperatura 1
		ST_MENU_T2_CAL,			//!< Submenú para calibración de temperatura 2
		ST_MENU_ALPHA_CAL,		//!< Submenú para calibración de ángulo

		ST_MENU_MVMAX_PAR,		//!< Submenú para establecer velocidad máxima de masa
		ST_MENU_BVMAX_PAR,		//!< Submenú para establecer velocidad máxima de frenado
		ST_MENU_BVMIN_PAR,		//!< Submenú para establecer velocidad mínima de frenado
		ST_MENU_PH_PAR,			//!< Submenú para establecer presión de horquilla nominal
		ST_MENU_PF_PAR,			//!< Submenú para establecer presión nominal de freno
		ST_MENU_T1HOT_PAR,		//!< Submenú para establecer temp1 hot
		ST_MENU_T2HOT_PAR,		//!< Submenú para establecer temp2 hot

		MENU_COUNT			//!< reserved
	};

	int _st_menu;

	StateMachine *_menu;

};


#endif /* MENUFSM_H_ */
