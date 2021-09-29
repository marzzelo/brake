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


class MenuFSM : public StateMachine {

public:
	MenuFSM(bool (*transitions[])(void), void (*onEnterings[])(void), void (*onLeavings[])(void));

	enum Transitions {
		TR_IDLE_MAIN = 0,

		TR_MAIN_IDLE = 1,
		TR_MAIN_WHEELCAL,
		TR_MAIN_PHCAL,
		TR_MAIN_PFCAL,
		TR_MAIN_T1CAL,
		TR_MAIN_T2CAL,
		TR_MAIN_ALPHACAL,

		TR_MAIN_MVMAX = 8,
		TR_MAIN_BVMAX,
		TR_MAIN_BVMIN,
		TR_MAIN_PHLIM,
		TR_MAIN_PFLIM,
		TR_MAIN_T1HOT,
		TR_MAIN_T2HOT,

		TR_WHEELCAL_MAIN = 15,
		TR_PHCAL_MAIN,
		TR_PFCAL_MAIN,
		TR_T1CAL_MAIN,
		TR_T2CAL_MAIN,
		TR_ALPHACAL_MAIN,

		TR_MVMAX_MAIN = 21,
		TR_BVMAX_MAIN,
		TR_BVMIN_MAIN,
		TR_PHLIM_MAIN,
		TR_PFLIM_MAIN,
		TR_T1HOT_MAIN,
		TR_T2HOT_MAIN = 27

	};


	enum OnEnterings {
		ENT_IDLE = 0,
		ENT_MAIN,
		ENT_WHEELCAL,
		ENT_PHCAL,
		ENT_PFCAL,
		ENT_T1CAL,
		ENT_T2CAL,
		ENT_ALPHACAL,

		ENT_MVMAX = 8,
		ENT_BVMAX,
		ENT_BVMIN,
		ENT_PHLIM,
		ENT_PFLIM,
		ENT_T1HOT,
		ENT_T2HOT = 14,
	};


	enum OnLeavings {
		EXIT_IDLE = 0,
		EXIT_MAIN,

	};


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
		ST_MENU_PHLIM_PAR,		//!< Submen� para establecer presi�n de horquilla nominal
		ST_MENU_PFLIM_PAR,		//!< Submen� para establecer presi�n nominal de freno
		ST_MENU_T1HOT_PAR,		//!< Submen� para establecer temp1 hot
		ST_MENU_T2HOT_PAR,		//!< Submen� para establecer temp2 hot

		MENU_COUNT			//!< reserved
	};

private:
	int _st_menu;

};


#endif /* MENUFSM_H_ */
