/*
 * MenuFSM.cpp
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


#include "MenuFSM.h"


MenuFSM::MenuFSM(bool (*transitions[])(void), void (*onEnterings[])(void), void (*onLeavings[])(void))
		: StateMachine(MenuState::MENU_COUNT, N_TRANSITIONS) {


//	  __                  ___
//	 (_ _|_  _. _|_  _     | ._ _. ._   _ o _|_ o  _  ._   _
//	 __) |_ (_|  |_ (/_    | | (_| | | _> |  |_ | (_) | | _>
//

	/////////////////////////////
	// IDLE TO MAIN (index: 0)
	/////////////////////////////
	AddTransition(ST_MENU_IDLE, ST_MENU_MAIN, transitions[TR_IDLE_MAIN]);

	/////////////////////////////
	// FACTORES DE CALIBRACIÓN
	/////////////////////////////
	AddTransition(ST_MENU_MAIN, ST_MENU_IDLE, transitions[TR_MAIN_IDLE]);

	AddTransition(ST_MENU_MAIN, ST_MENU_WHEEL_CAL, transitions[TR_MAIN_WHEELCAL]);

	AddTransition(ST_MENU_MAIN, ST_MENU_PH_CAL, transitions[TR_MAIN_PHCAL]);

	AddTransition(ST_MENU_MAIN, ST_MENU_PF_CAL, transitions[TR_MAIN_PFCAL]);

	AddTransition(ST_MENU_MAIN, ST_MENU_T1_CAL, transitions[TR_MAIN_T1CAL]);

	AddTransition(ST_MENU_MAIN, ST_MENU_T1_CAL, transitions[TR_MAIN_T2CAL]);

	AddTransition(ST_MENU_MAIN, ST_MENU_ALPHA_CAL, transitions[TR_MAIN_ALPHACAL]);


	/////////////////////////////
	// PARÁMETROS DE ENSAYO
	/////////////////////////////
	AddTransition(ST_MENU_MAIN, ST_MENU_MVMAX_PAR, transitions[TR_MAIN_MVMAX]);

	AddTransition(ST_MENU_MAIN, ST_MENU_BVMAX_PAR, transitions[TR_MAIN_BVMAX]);

	AddTransition(ST_MENU_MAIN, ST_MENU_BVMIN_PAR, transitions[TR_MAIN_BVMIN]);

	AddTransition(ST_MENU_MAIN, ST_MENU_PHLIM_PAR, transitions[TR_MAIN_PHLIM]);

	AddTransition(ST_MENU_MAIN, ST_MENU_PFLIM_PAR, transitions[TR_MAIN_PFLIM]);

	AddTransition(ST_MENU_MAIN, ST_MENU_T1HOT_PAR, transitions[TR_MAIN_MVMAX]);

	AddTransition(ST_MENU_MAIN, ST_MENU_T2HOT_PAR, transitions[TR_MAIN_MVMAX]);


	//////////////////////////////////////////
	// RETORNOS A MAIN MENU
	//////////////////////////////////////////
	AddTransition(ST_MENU_WHEEL_CAL, ST_MENU_MAIN, transitions[TR_WHEELCAL_MAIN]);

	AddTransition(ST_MENU_PH_CAL, ST_MENU_MAIN, transitions[TR_PHCAL_MAIN]);

	AddTransition(ST_MENU_PF_CAL, ST_MENU_MAIN, transitions[TR_PFCAL_MAIN]);

	AddTransition(ST_MENU_T1_CAL, ST_MENU_MAIN, transitions[TR_T1CAL_MAIN]);

	AddTransition(ST_MENU_T2_CAL, ST_MENU_MAIN, transitions[TR_T2CAL_MAIN]);

	AddTransition(ST_MENU_ALPHA_CAL, ST_MENU_MAIN, transitions[TR_ALPHACAL_MAIN]);



	AddTransition(ST_MENU_MVMAX_PAR, ST_MENU_MAIN, transitions[TR_MVMAX_MAIN]);

	AddTransition(ST_MENU_BVMAX_PAR, ST_MENU_MAIN, transitions[TR_BVMAX_MAIN]);

	AddTransition(ST_MENU_BVMIN_PAR, ST_MENU_MAIN, transitions[TR_BVMIN_MAIN]);

	AddTransition(ST_MENU_PHLIM_PAR, ST_MENU_MAIN, transitions[TR_PHLIM_MAIN]);

	AddTransition(ST_MENU_PFLIM_PAR, ST_MENU_MAIN, transitions[TR_PFLIM_MAIN]);

	AddTransition(ST_MENU_T1HOT_PAR, ST_MENU_MAIN, transitions[TR_T1HOT_MAIN]);

	AddTransition(ST_MENU_T2HOT_PAR, ST_MENU_MAIN, transitions[TR_T2HOT_MAIN]);



//	  _         _
//	 / \ ._    |_ ._ _|_  _  ._ o ._   _
//	 \_/ | |   |_ | | |_ (/_ |  | | | (_|
//	                                   _|
	SetOnEntering(ST_MENU_IDLE, onEnterings[ENT_IDLE]);

	SetOnEntering(ST_MENU_MAIN, onEnterings[ENT_MAIN]);

	//----- AJUSTES DE CALIBRACIÓN ---------------------------------------------------------

	SetOnEntering (ST_MENU_WHEEL_CAL, onEnterings[ENT_WHEELCAL]);

	SetOnEntering (ST_MENU_PH_CAL, onEnterings[ENT_PHCAL]);

	SetOnEntering ( ST_MENU_PF_CAL, onEnterings[ENT_PFCAL]);

	SetOnEntering ( ST_MENU_T1_CAL, onEnterings[ENT_T1CAL]);

	SetOnEntering ( ST_MENU_T2_CAL, onEnterings[ENT_T2CAL]);

	SetOnEntering ( ST_MENU_ALPHA_CAL, onEnterings[ENT_ALPHACAL]);

	//----- PARÁMETROS --------------------------------------------------------------------

	SetOnEntering (ST_MENU_MVMAX_PAR, onEnterings[ENT_MVMAX]);

	SetOnEntering (ST_MENU_BVMAX_PAR, onEnterings[ENT_BVMAX]);

	SetOnEntering (ST_MENU_BVMIN_PAR, onEnterings[ENT_BVMIN]);

	SetOnEntering (ST_MENU_PHLIM_PAR, onEnterings[ENT_PHLIM]);

	SetOnEntering (ST_MENU_PFLIM_PAR, onEnterings[ENT_PFLIM]);

	SetOnEntering (ST_MENU_T1HOT_PAR, onEnterings[ENT_T1HOT]);

	SetOnEntering (ST_MENU_T2HOT_PAR, onEnterings[ENT_T2HOT]);


//	  _
//	 / \ ._    |   _   _.    o ._   _
//	 \_/ | |   |_ (/_ (_| \/ | | | (_|
//
	SetOnLeaving(ST_MENU_IDLE, onLeavings[EXIT_IDLE]);

	SetOnLeaving (ST_MENU_MAIN, onLeavings[EXIT_MAIN]);


	// GO IDLE!
	SetState(MenuFSM::ST_MENU_IDLE, false, false);

}


