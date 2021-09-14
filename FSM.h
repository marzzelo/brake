/*
 * FSM.h
 *
 *  Created on: 27 ago. 2021
 *      Author: mvald
 *      https://www.luisllamas.es/libreria-arduino-statemachine/
 */

#ifndef FSM_H_
#define FSM_H_

/******************************************************************
 * 								F S M
 ******************************************************************/

extern void state_reset();
extern void setTimeOut(unsigned long dt);
extern BankAnalogInputs bankInputs;
extern BankLeds bankLeds;
extern MyLed7 *led7s;
extern uint16_t Mv;


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
	ST_COUNT
};

#define N_TRANSITIONS	30

StateMachine FSM(ST_COUNT, N_TRANSITIONS);

///////////////// INPUTS //////////////////
bool btn_pressed[4];

bool Mv_gt_0;
bool Mv_gt_MAX;
bool Mv_ge_BRAKEv_min;
bool Mv_le_BRAKEv_max;
bool Mv_eq_0;

bool Wv_gt_0;
bool Wv_ge_LANDINGv;
bool Wv_eq_0;

bool Ph_gt_0;
bool Ph_ge_Ph1;

bool Pf_gt_0;
bool Pf_ge_Pf1;

bool T1_ge_Thot;
bool T2_ge_Thot;

bool reset_requested;

bool timeOut;
bool eventsChecked;

//////////////////////////////////////////////////////////////////
// STATES ON_TRANSITION
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// STATES ON_ENTERING
//////////////////////////////////////////////////////////////////
void ent_idle() {
	bankLeds.beep();
	state_reset();
	checkCommands = true;
	Serial << F("\n\n**************************************");
	Serial << F("\n* Presionar START para comenzar      *");
	Serial << F("\n* Ingresar *1<enter> para Menú       *");
	Serial << F("\n**************************************\n\n");

	bankLeds.display('P');
}

void ent_checking_cond() {
	bankInputs.start();
	bankLeds.blink('E');
	bankLeds.beep();

	Serial << F("\nST_CHECKING_COND... [Esperando condiciones de inicio: Mv=0, Wv=0, Ph=0, Pf=0]");
}

void ent_cond_ok() {
	Serial << F("\nST_COND_OK... [Esperando Mv > 0]");
	bankLeds.display(0);
	bankLeds.beep();
}

void ent_speeding() {
	bankLeds.beep();
	bankLeds.display(1);

	Serial << F("\n\nST_SPEEDING... [Esperando Mv >= ")
			<< _DEC(bank.testParms.max_mass_vel) << F(" rpm]");
}

void ent_max_vel() {
	bankLeds.beep();
	bankLeds.display(2);

	Serial << F("\n\nST_MAX_VEL... [Esperando Wv > ")
			<< _DEC(bank.testParms.landing_wheel_vel) << F(" m/s]");
}

void ent_landing() {
	bankLeds.beep();
	bankLeds.display(3);

	Serial << F("\n\nST_LANDING... [Esperando PH >= ")
			<< _DEC(bank.testParms.ph_threshold) << F(" bar]");
}

void ent_landed() {
	bankLeds.beep();
	bankLeds.display(4);

	Serial << F("\n\nST_LANDED... [Esperando ")
			<< _DEC(bank.testParms.brake_mass_vel_min) << " < Mv < "
			<< _DEC(bank.testParms.brake_mass_vel_max) << "]";
}

void ent_braking_vel() {
	bankLeds.beep();
	bankLeds.display(5);
	Serial << F("\n\nST_BRAKING_VEL... [Esperando PF >= ")
			<< _DEC(bank.testParms.pf_threshold) << F(" bar]");
}

void ent_braking() {
	bankLeds.beep();
	bankLeds.display(6);

	Serial << F("\n\nST_BRAKING... [Esperando Wv = 0 & Mv = 0]");
}

void ent_test_error() {
	bankLeds.beep();
	bankLeds.blink('E');

	if (!Pf_gt_0) {
		Serial << F("\nPf demasiado baja. Test abortado");
	} else if (Mv_le_BRAKEv_max) {
		Serial << F("\nPérdida de velocidad de Masa. Test abortado");
	}

	Serial << F("\n**TEST ERROR** <RESET> para REINICIAR");
}

void ent_test_complete() {
	bankLeds.beep();
	bankLeds.blink('F');

	Serial << F("\n**TEST FINALIZADO** <RESET> para REINICIAR");
}

//////////////////////////////////////////////////////////////////
//
// 					T R A N S I T I O N S
//
//////////////////////////////////////////////////////////////////

/*****************************************************
 *
 * Transitions from ST_IDLE
 *
 *****************************************************/
/**
 * ## Presionando el botón START se inicia el ensayo.
 */
bool from_idle_to_checking_cond() {
	return btn_pressed[0];
}

/*****************************************************
 *
 * Transitions from ST_CHECKING_COND
 *
 *****************************************************/
/**
 * ## Condiciones nominales para el inicio del ensayo.
 * - Masa detenida
 * - Rueda detenida
 * - Presión de horquilla nula
 * - Presión de freno nula
 * - Temperaturas por debajo del límite caliente
 */
bool from_checking_to_cond_ok() {

	bool cond_ok = true;

	if (Mv_gt_0) {
		cond_ok = false;
		uint32_t rpm = bankInputs.getRpm();
		Serial << F("\n** DETENER MASA [") << _FLOAT(rpm, 3) << " rpm]";
	}

	if (Wv_gt_0) {
		cond_ok = false;
		uint16_t wv = bankInputs.wheel_daq_value * bank.calFactors.ka_wheel;
		Serial << F("\n** DETENER RUEDA [") << _FLOAT(wv, 3) << " rpm]";
	}

	if (Ph_gt_0) {
		cond_ok = false;
		uint16_t ph = bankInputs.ph_daq_value * bank.calFactors.ka_ph;
		Serial << F("\n** REDUCIR PH [") << _FLOAT(ph, 3) << F(" bar]");
	}

	if (Pf_gt_0) {
		cond_ok = false;
		uint16_t pf = bankInputs.pf_daq_value * bank.calFactors.ka_pf;
		Serial << F("\n** REDUCIR PF [") << _FLOAT(pf, 3) << F(" bar]");
	}

	if (T1_ge_Thot || T2_ge_Thot) {
		cond_ok = false;
		uint16_t T1 = bankInputs.t1_daq_value * bank.calFactors.ka_t1;
		uint16_t T2 = bankInputs.t2_daq_value * bank.calFactors.ka_t2;
		Serial << F("\n** Temp Alta [T1: ") << _FLOAT(T1, 3) << F(" °C]");
		Serial << F(" [T2: ") << _FLOAT(T2, 3) << F(" °C]");
	}

	if (!cond_ok) {
		Serial << F("\n----------------------");
	}

	return cond_ok;
}

/**
 * ## Presionando RESET desde cualquier estado se retorna al inicio (IDLE)
 */
bool any_to_idle() {
	{
		if (timeOut) {
			bankLeds.beep();
			Serial << F("\nTimeout");
			return true;
		}
		return btn_pressed[3];
	}
}

/*****************************************************
 *
 * Transitions from ST_COND_OK
 *
 *****************************************************/
/**
 * ## Cumplidas las condiciones, esperar inicio de giro de masa
 */
bool from_cond_ok_to_speeding() {
	Serial << F("\nST_COND_OK> mass vel: ") << _FLOAT(Mv, 3)
			<< F(" ** INICIAR GIRO **");
	return (Mv_gt_0);
}

/*****************************************************
 *
 * Transitions from ST_SPEEDING
 *
 *****************************************************/
/**
 * ## Se alcanza la máxima velocidad de masa.
 */
bool from_speeding_to_max_vel() {
	Serial << F("\nST_SPEEDING> Mv: ") << _FLOAT(Mv, 3)
			<< F(" ** ACELERAR a 500 rpm **");
	return Mv_gt_MAX;
}

/*****************************************************
 *
 * Transitions from ST_MAX_VEL
 *
 *****************************************************/
/**
 * ## Se alcanza el estado LANDING cuando la velocidad de rueda alcanza el valor nominal fijado en el menu del sistema.
 */
bool from_max_vel_to_landing() {
	uint16_t wv = bankInputs.wheel_daq_value * bank.calFactors.ka_wheel;
	Serial << F("\nST_MAX_VEL> Mv: ") << _FLOAT(Mv, 3);
	Serial << F(", Wv: ") << _FLOAT(wv, 3) << F(" ** ATERRIZAR RUEDA ***");
	return Wv_ge_LANDINGv;
}

/**
 * ## Se retorna al estado SPEEDING si la velocidad de masa cae por debajo de la velocidad máxima
 */
bool from_max_vel_to_speeding() {
	return Mv_le_BRAKEv_max;
}

/*****************************************************
 *
 * Transitions from ST_LANDING
 *
 *****************************************************/
/**
 * ## Se alcanza el estado LANDED cuando la presión de horquilla alcanza o supera el valor nominal de aterrizaje.
 */
bool from_landing_to_landed() {
	uint16_t wv = bankInputs.wheel_daq_value * bank.calFactors.ka_wheel;
	uint16_t ph = bankInputs.ph_daq_value * bank.calFactors.ka_ph;

	Serial << F("\nLANDING> Mv: ") << _FLOAT(Mv, 3);
	Serial << F(", Wv: ") << _FLOAT(wv, 3);
	Serial << F(", PH: ") << _FLOAT(ph, 3) << F(" ** AUMENTAR Ph a ")
			<< _FLOAT(bank.testParms.ph_threshold, 3) << " ***";
	return Ph_ge_Ph1;
}

/**
 * ## Se ingresa al estado de ERROR si la velocidad de masa disminuye por debajo de la velocidad de frenado
 * antes de aplicar el freno.
 */
bool from_landing_to_error() {
	if (Mv_le_BRAKEv_max) {
		Serial << "\nERROR: velocidad de masa baja antes de frenar";
		return true;
	}
	return false;
}

/*****************************************************
 *
 * Transitions from ST_LANDED
 *
 *****************************************************/
/**
 * ## Se ingresa al estado BRAKING_VEL cuando la velocidad de masa ingresa a la zona de frenado.
 * - Los límites superior e inferior de la zona son establecidos mediante el menú del sistema.
 * - Los valores nominales aproximados están alrededor de las 420 rpm
 */
bool from_landed_to_braking_vel() {
	if (!Mv_le_BRAKEv_max) {
		Serial << F("\nST_LANDED> Mv: ") << _FLOAT(Mv, 3);
		Serial << F(" ** DISMINUIR VELOCIDAD **");
	}
	if (!Mv_ge_BRAKEv_min) {
		Serial << F("\nST_LANDED> Mv: ");
		Serial << _FLOAT(Mv, 3) << F(" ** ACELERAR **");
	}
	return (Mv_le_BRAKEv_max && Mv_ge_BRAKEv_min);
}

/*****************************************************
 *
 * Transitions from ST_BRAKING_VEL
 *
 *****************************************************/
/**
 * ## Se ingresa a BRAKING cuando la presión de freno alcanza la presión nominal del ensayo.
 * - El valor nominal de presión de freno puede fijarse desde el menú del sistema.
 */
bool from_braking_vel_to_braking() {
	uint16_t pf = bankInputs.pf_daq_value * bank.calFactors.ka_pf;
	Serial << F("\nST_BRAKING_VEL> PF: ") << _FLOAT(pf, 3);
	Serial << " ***### APLICAR FRENO ###***";

	return Pf_ge_Pf1;
}

/**
 * ## Se regresa al estado LANDED si la velocidad de masa sale del rango de frenado antes de que la presión de freno alcance la presión nominal
 */
bool from_braking_vel_to_landed() {
	if (!(Mv_le_BRAKEv_max && Mv_ge_BRAKEv_min)) {
		bankLeds.beep();
		return true;
	}
	return false;
}

/*****************************************************
 *
 * Transitions from ST_BRAKING
 *
 *****************************************************/
/**
 * ## Se ingresa a TEST_COMPLETE cuando las velocidades de masa y de rueda se anulan.
 */
bool from_braking_to_complete() {
	uint16_t wv = bankInputs.wheel_daq_value * bank.calFactors.ka_wheel;
	Serial << F("\nST_BRAKING> Mv: ") << _FLOAT(Mv, 3);
	Serial << ", Wv: " << _FLOAT(wv, 3) << " ** MANTENER Pf HASTA DETENER ** ";

	return (Mv_eq_0 && Wv_eq_0);
}

/**
 * ## Se disminuyó demasiado la presión de freno durante el frenado
 */
bool from_braking_to_error() {
	if (!Pf_gt_0) {
		Serial << "\nPresión de freno baja durante el frenado";
		return true;
	}
	return false;
}

/*****************************************************
 *
 * Transitions from ST_TEST_COMPLETE
 *
 *****************************************************/

/**
 * Asignar callbacks de entrada, transición y salida
 */
void setupFSM() {

	FSM.AddTransition(ST_IDLE, ST_CHECKING_COND, from_idle_to_checking_cond);

	//-----------------------------------------------------------------------

	FSM.AddTransition(ST_CHECKING_COND, ST_COND_OK, from_checking_to_cond_ok);

	FSM.AddTransition(ST_CHECKING_COND, ST_IDLE, any_to_idle);

	//-----------------------------------------------------------------------

	FSM.AddTransition(ST_COND_OK, ST_SPEEDING, from_cond_ok_to_speeding);

	FSM.AddTransition(ST_COND_OK, ST_IDLE, any_to_idle);

	//-----------------------------------------------------------------------

	FSM.AddTransition(ST_SPEEDING, ST_MAX_VEL, from_speeding_to_max_vel);

	FSM.AddTransition(ST_SPEEDING, ST_IDLE, any_to_idle);

	//-----------------------------------------------------------------------

	FSM.AddTransition(ST_MAX_VEL, ST_LANDING, from_max_vel_to_landing);

	FSM.AddTransition(ST_MAX_VEL, ST_SPEEDING, from_max_vel_to_speeding);

	FSM.AddTransition(ST_MAX_VEL, ST_IDLE, any_to_idle);

	//-----------------------------------------------------------------------

	FSM.AddTransition(ST_LANDING, ST_LANDED, from_landing_to_landed);

	FSM.AddTransition(ST_LANDING, ST_TEST_ERROR, from_landing_to_error);

	FSM.AddTransition(ST_LANDING, ST_IDLE, any_to_idle);

	//-----------------------------------------------------------------------

	FSM.AddTransition(ST_LANDED, ST_BRAKING_VEL, from_landed_to_braking_vel);

	FSM.AddTransition(ST_LANDED, ST_IDLE, any_to_idle);

	//-----------------------------------------------------------------------

	FSM.AddTransition(ST_BRAKING_VEL, ST_BRAKING, from_braking_vel_to_braking);

	FSM.AddTransition(ST_BRAKING_VEL, ST_LANDED, from_braking_vel_to_landed);

	FSM.AddTransition(ST_BRAKING_VEL, ST_IDLE, any_to_idle);

	//-----------------------------------------------------------------------

	FSM.AddTransition(ST_BRAKING, ST_TEST_COMPLETE, from_braking_to_complete);

	FSM.AddTransition(ST_BRAKING, ST_TEST_ERROR, from_braking_to_error);

	FSM.AddTransition(ST_BRAKING, ST_IDLE, any_to_idle);

	//-----------------------------------------------------------------------

	FSM.AddTransition(ST_TEST_COMPLETE, IDLE, any_to_idle);

	//-----------------------------------------------------------------------

	FSM.AddTransition(ST_TEST_ERROR, IDLE, any_to_idle);

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	//
	// ON_ENTERING
	//
	//////////////////////////////////////////////////////////////////
	FSM.SetOnEntering(ST_IDLE, ent_idle);
	FSM.SetOnEntering(ST_CHECKING_COND, ent_checking_cond);
	FSM.SetOnEntering(ST_COND_OK, ent_cond_ok);
	FSM.SetOnEntering(ST_SPEEDING, ent_speeding);
	FSM.SetOnEntering(ST_MAX_VEL, ent_max_vel);
	FSM.SetOnEntering(ST_LANDING, ent_landing);
	FSM.SetOnEntering(ST_LANDED, ent_landed);
	FSM.SetOnEntering(ST_BRAKING_VEL, ent_braking_vel);
	FSM.SetOnEntering(ST_BRAKING, ent_braking);
	FSM.SetOnEntering(ST_TEST_ERROR, ent_test_error);
	FSM.SetOnEntering(ST_TEST_COMPLETE, ent_test_complete);

	//////////////////////////////////////////////////////////////////
	//
	// ON_LEAVING
	//
	//////////////////////////////////////////////////////////////////
	FSM.SetOnLeaving(ST_IDLE, []() {
		setTimeOut(120000);
		checkCommands = false;
		Serial << F("\n...leaving ST_IDLE");
	});
	FSM.SetOnLeaving(ST_CHECKING_COND, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_CHECKING_COND");
	});
	FSM.SetOnLeaving(ST_COND_OK, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_COND_OK");
	});
	FSM.SetOnLeaving(ST_SPEEDING, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_SPEEDING");
	});
	FSM.SetOnLeaving(ST_MAX_VEL, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_MAX_VEL");
	});
	FSM.SetOnLeaving(ST_LANDING, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_LANDING");
	});
	FSM.SetOnLeaving(ST_LANDED, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_LANDED");
	});
	FSM.SetOnLeaving(ST_BRAKING_VEL, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_BRAKING_VEL");
	});
	FSM.SetOnLeaving(ST_BRAKING, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_BRAKING");
	});
	FSM.SetOnLeaving(ST_TEST_ERROR, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_TEST_ERROR");
	});
	FSM.SetOnLeaving(ST_TEST_COMPLETE, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_TEST_COMPLETE");
	});
}

#endif /* FSM_H_ */
