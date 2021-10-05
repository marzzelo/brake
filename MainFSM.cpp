/*
 * Maincpp
 *
 *  Created on: 30 sep. 2021
 *      Author: valdez
 */

//								  __  __       _         _____ ____  __  __
//								 |  \/  | __ _(_)_ __   |  ___/ ___||  \/  |
//								 | |\/| |/ _` | | '_ \  | |_  \___ \| |\/| |
//								 | |  | | (_| | | | | | |  _|  ___) | |  | |
//								 |_|  |_|\__,_|_|_| |_| |_|   |____/|_|  |_|
//


#include "MainFSM.h"

MainFSM::MainFSM(bool (*transitions[])(void), void (*onEnterings[])(void),
		void (*onLeavings[])(void)) :
		StateMachine(State::COUNT, N_TRANSITIONS) {



//	  __                  ___
//	 (_ _|_  _. _|_  _     | ._ _. ._   _ o _|_ o  _  ._   _
//	 __) |_ (_|  |_ (/_    | | (_| | | _> |  |_ | (_) | | _>
//
	AddTransition(ST_IDLE, ST_CHECKING_COND, transitions[TR_IDLE_CHECKING]);

	AddTransition(ST_IDLE, ST_MONITORING, transitions[TR_IDLE_MONITORING]);

	AddTransition(ST_CHECKING_COND, ST_COND_OK, transitions[TR_CHECKING_CONDOK]);

	AddTransition(ST_CHECKING_COND, ST_IDLE, transitions[TR_ANY_IDLE]);

	AddTransition(ST_COND_OK, ST_SPEEDING, transitions[TR_CONDOK_SPEEDING]);

	AddTransition(ST_COND_OK, ST_IDLE, transitions[TR_ANY_IDLE]);

	AddTransition(ST_SPEEDING, ST_MAX_VEL, transitions[TR_SPEEDING_MAXVEL]);

	AddTransition(ST_SPEEDING, ST_IDLE, transitions[TR_ANY_IDLE]);

	AddTransition(ST_MAX_VEL, ST_LANDING, transitions[TR_MAXVEL_LANDING]);

	AddTransition(ST_MAX_VEL, ST_SPEEDING, transitions[TR_MAXVEL_SPEEDING]);

	AddTransition(ST_MAX_VEL, ST_IDLE, transitions[TR_ANY_IDLE]);

	AddTransition(ST_LANDING, ST_LANDED, transitions[TR_LANDING_LANDED]);

	AddTransition(ST_LANDING, ST_TEST_ERROR, transitions[TR_LANDING_ERROR]);

	AddTransition(ST_LANDING, ST_IDLE, transitions[TR_ANY_IDLE]);

	AddTransition(ST_LANDED, ST_BRAKING_VEL, transitions[TR_LANDED_BRAKINGVEL]);

	AddTransition(ST_LANDED, ST_IDLE, transitions[TR_ANY_IDLE]);

	AddTransition(ST_BRAKING_VEL, ST_BRAKING, transitions[TR_BRAKINGVEL_BRAKING]);

	AddTransition(ST_BRAKING_VEL, ST_LANDED, transitions[TR_BRAKINGVEL_LANDED]);

	AddTransition(ST_BRAKING_VEL, ST_IDLE, transitions[TR_ANY_IDLE]);

	AddTransition(ST_BRAKING, ST_TEST_COMPLETE, transitions[TR_BRAKING_COMPLETE]);

	AddTransition(ST_BRAKING, ST_TEST_ERROR, transitions[TR_BRAKING_ERROR]);

	AddTransition(ST_BRAKING, ST_IDLE, transitions[TR_ANY_IDLE]);

	AddTransition(ST_TEST_COMPLETE, ST_IDLE, transitions[TR_ANY_IDLE]);

	AddTransition(ST_TEST_ERROR, ST_IDLE, transitions[TR_ANY_IDLE]);

	AddTransition(ST_MONITORING, ST_IDLE, transitions[TR_MONITORING_IDLE]);







//	  _         _
//	 / \ ._    |_ ._ _|_  _  ._ o ._   _
//	 \_/ | |   |_ | | |_ (/_ |  | | | (_|
//	                                   _|
	SetOnEntering(ST_IDLE, onEnterings[ENT_IDLE]);

	SetOnEntering(ST_CHECKING_COND, onEnterings[ENT_CHECKING]);

	SetOnEntering(ST_COND_OK, onEnterings[ENT_CONDOK]);

	SetOnEntering(ST_SPEEDING, onEnterings[ENT_SPEEDING]);

	SetOnEntering(ST_MAX_VEL, onEnterings[ENT_MAXVEL]);

	SetOnEntering(ST_LANDING, onEnterings[ENT_LANDING]);

	SetOnEntering(ST_LANDED, onEnterings[ENT_LANDED]);

	SetOnEntering(ST_BRAKING_VEL, onEnterings[ENT_BRAKINGVEL]);

	SetOnEntering(ST_BRAKING, onEnterings[ENT_BRAKING]);

	SetOnEntering(ST_TEST_ERROR, onEnterings[ENT_ERROR]);

	SetOnEntering(ST_TEST_COMPLETE, onEnterings[ENT_COMPLETE]);

	SetOnEntering(ST_MONITORING, onEnterings[ENT_MONITORING]);




//	  _
//	 / \ ._    |   _   _.    o ._   _
//	 \_/ | |   |_ (/_ (_| \/ | | | (_|
//	                                _|
	SetOnLeaving(ST_IDLE, onLeavings[EXIT_IDLE]);

	SetOnLeaving(ST_CHECKING_COND, onLeavings[EXIT_ANY]);

	SetOnLeaving(ST_COND_OK, onLeavings[EXIT_ANY]);

	SetOnLeaving(ST_SPEEDING, onLeavings[EXIT_ANY]);

	SetOnLeaving(ST_MAX_VEL, onLeavings[EXIT_ANY]);

	SetOnLeaving(ST_LANDING, onLeavings[EXIT_ANY]);

	SetOnLeaving(ST_LANDED, onLeavings[EXIT_ANY]);

	SetOnLeaving(ST_BRAKING_VEL, onLeavings[EXIT_ANY]);

	SetOnLeaving(ST_BRAKING, onLeavings[EXIT_ANY]);

	SetOnLeaving(ST_TEST_ERROR, onLeavings[EXIT_ANY]);

	SetOnLeaving(ST_TEST_COMPLETE, onLeavings[EXIT_ANY]);

	SetOnLeaving(ST_MONITORING, onLeavings[EXIT_MONITORING]);


}


//extern void checkEvents();

bool MainFSM::Update() {
	//checkEvents();			// Actualizar booleans
	return StateMachine::Update();
}



