/*
 * MainFSM.h
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


#ifndef MAINFSM_H_
#define MAINFSM_H_

#include "StateMachineLib.h"
#include "BankButtons.h"
#include "BankLeds.h"
#include "BankAnalogInputs.h"
#include "Streaming.h"
#include "Arduino.h"


#define N_TRANSITIONS	35

/**
 * # M�quina de estados principal
 */
class MainFSM: public StateMachine {
public:
	MainFSM(bool (*transitions[])(void), void (*onEnterings[])(void), void (*onLeavings[])(void));

	bool Update();


	/**
	 * # Estados para la FSM principal del sistema
	 */
	enum State {
		ST_IDLE,			//!< esperando input de inicio de ensayo
		ST_CHECKING_COND,	//!< esperando condiciones correctas de inicio
		ST_COND_OK,			//!< esperando inicio de movimiento de volanta
		ST_SPEEDING, 		//!< volanta acelerando, esperando velocidad máxima
		ST_MAX_VEL, 		//!< velocidad maxima alcanzada, esperando aterrizaje.
		ST_LANDING, 		//!< aterrizaje iniciado, esperando presión de frenado
		ST_LANDED,			//!< presión alcanzada, esperando velocidad de frenado
		ST_BRAKING_VEL, 	//!< velocidad de freno alcanzada, esperando frenado
		ST_BRAKING,			//!< frenado iniciado, esperando detención total.
		ST_TEST_COMPLETE,	//!< test finalizado, esperando reset o timeout para IDLE.
		ST_TEST_ERROR,		//!< test Abortado, esperando reset o timeout para IDLE.
		ST_MONITORING,		//!< outputs all analog sensors to Serial port

		COUNT
	};



	enum Transitions {
		TR_IDLE_CHECKING = 0,
		TR_IDLE_MONITORING,
		TR_CHECKING_CONDOK,
		TR_ANY_IDLE,
		TR_CONDOK_SPEEDING,
		TR_SPEEDING_MAXVEL,
		TR_MAXVEL_LANDING,
		TR_MAXVEL_SPEEDING,
		TR_LANDING_LANDED,
		TR_LANDING_ERROR,
		TR_LANDED_BRAKINGVEL,
		TR_BRAKINGVEL_BRAKING,
		TR_BRAKINGVEL_LANDED,
		TR_BRAKING_COMPLETE,
		TR_BRAKING_ERROR,
		TR_MONITORING_IDLE,
	};


	enum OnEnterings {
		ENT_IDLE,
		ENT_CHECKING,
		ENT_CONDOK,
		ENT_SPEEDING,
		ENT_MAXVEL,
		ENT_LANDING,
		ENT_LANDED,
		ENT_BRAKINGVEL,
		ENT_BRAKING,
		ENT_ERROR,
		ENT_COMPLETE,
		ENT_MONITORING,
	};


	enum OnLeavings {
		EXIT_IDLE,
		EXIT_ANY,
		EXIT_MONITORING,

	};

};



#endif /* MAINFSM_H_ */
