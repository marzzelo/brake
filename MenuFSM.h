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

		MENU_COUNT			//!< reserved
	};

	int _st_menu;

	StateMachine *_menu;

};


#endif /* MENUFSM_H_ */
