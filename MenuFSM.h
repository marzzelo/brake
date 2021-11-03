/*
 * MenuFSM.h
 *
 *  Created on: 28 sep. 2021
 *      Author: valdez
 */

//						  __  __              __    _____ ____  __  __
//						 |  \/  | ___ _ __  _/_/_  |  ___/ ___||  \/  |
//						 | |\/| |/ _ \ '_ \| | | | | |_  \___ \| |\/| |
//						 | |  | |  __/ | | | |_| | |  _|  ___) | |  | |
//						 |_|  |_|\___|_| |_|\__,_| |_|   |____/|_|  |_|
//


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
	MenuFSM(bool (*transitions[])(void), void (*onEnterings[])(void), void (*onLeavings[])(void), uint8_t numPages);


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
		ST_MENU_PHLIM_PAR,		//!< Submenú para establecer presión de horquilla nominal
		ST_MENU_PFLIM_PAR,		//!< Submenú para establecer presión nominal de freno
		ST_MENU_T1HOT_PAR,		//!< Submenú para establecer temp1 hot
		ST_MENU_T2HOT_PAR,		//!< Submenú para establecer temp2 hot

		ST_MENU_FACTORYRESET,	//!< Submenú para restablecimiento a parámetros por default

		MENU_COUNT				//!< reserved
	};


	enum Transitions {
		TR_IDLE_MAIN = 0,

		TR_MAIN_MAIN,		// force st_main_on_entering() execution
		TR_MAIN_IDLE,		// key: 0
		TR_MAIN_WHEELCAL,	// key: 1
		TR_MAIN_PHCAL,
		TR_MAIN_PFCAL,
		TR_MAIN_T1CAL,
		TR_MAIN_T2CAL,
		TR_MAIN_ALPHACAL,

		TR_MAIN_MVMAX,
		TR_MAIN_BVMAX,
		TR_MAIN_BVMIN,
		TR_MAIN_PHLIM,
		TR_MAIN_PFLIM,
		TR_MAIN_T1HOT,
		TR_MAIN_T2HOT,

		TR_MAIN_FRESET,		// key: 14

		TR_WHEELCAL_MAIN,
		TR_PHCAL_MAIN,
		TR_PFCAL_MAIN,
		TR_T1CAL_MAIN,
		TR_T2CAL_MAIN,
		TR_ALPHACAL_MAIN,

		TR_MVMAX_MAIN,
		TR_BVMAX_MAIN,
		TR_BVMIN_MAIN,
		TR_PHLIM_MAIN,
		TR_PFLIM_MAIN,
		TR_T1HOT_MAIN,
		TR_T2HOT_MAIN,

		TR_FRESET_MAIN,
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

		ENT_MVMAX,
		ENT_BVMAX,
		ENT_BVMIN,
		ENT_PHLIM,
		ENT_PFLIM,
		ENT_T1HOT,
		ENT_T2HOT,
	};


	enum OnLeavings {
		EXIT_IDLE = 0,
		EXIT_MAIN,

	};

	void setPages(uint8_t numPages);

	uint8_t getPage();

	uint8_t nextPage();

	uint8_t prevPage();

	void setPage(uint8_t page);

private:
	uint8_t _lcd_menu_page = 0;

	uint8_t _num_pages;

};


#endif /* MENUFSM_H_ */
