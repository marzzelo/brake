/*
 * MainTransitions.h
 *
 *  Created on: 30 sep. 2021
 *      Author: valdez
 */

#ifndef MAINTRANSITIONS_H_
#define MAINTRANSITIONS_H_

/*
  _____                    _ _   _
 |_   _| __ __ _ _ __  ___(_) |_(_) ___  _ __  ___
   | || '__/ _` | '_ \/ __| | __| |/ _ \| '_ \/ __|
   | || | | (_| | | | \__ \ | |_| | (_) | | | \__ \
   |_||_|  \__,_|_| |_|___/_|\__|_|\___/|_| |_|___/

*/

extern BankButtons bankButtons;


uint16_t Mv;
uint16_t Wv;
uint16_t Ph;
uint16_t T1;
uint16_t T2;
uint16_t Pf;

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

bool timeOut;


unsigned long _t0, _dt;



void setTimeOut(unsigned long dt) {
	_t0 = millis();
	_dt = dt;
}


//////////////////////////////////////////////////////////////////
// EVENT CHECKING
//////////////////////////////////////////////////////////////////
/**
 * A partir de las lecturas de las entradas analógicas habilitadas, calcula varias variables
 * booleanas que serán utilizadas en la FSM principal para generar las transiciones.
 */
void checkEvents() {

	if (bankButtons.read(1)) {
		bankLeds.beep();
		bankInputs.nextDisplayVar();
	}

	// Velocidad de masa - available() es comprobado previamente en loop: if(bankInputs.ready())
	Mv = 					bankInputs.getRpm();
	Wv = 					bankInputs.getWv();
	Ph = 					bankInputs.getPh();
	Pf = 					bankInputs.getPf();
	T1 = 					bankInputs.getT1();
	T2 = 					bankInputs.getT2();


	Mv_eq_0 = 				Mv <= ZERO_MASS_VEL;
	Mv_gt_0 = 				Mv > ZERO_MASS_VEL;
	Mv_gt_MAX = 			Mv >= bankInputs.testParms.max_mass_vel;
	Mv_le_BRAKEv_max = 		Mv <= bankInputs.testParms.brake_mass_vel_max;
	Mv_ge_BRAKEv_min = 		Mv >= bankInputs.testParms.brake_mass_vel_min;


	// Velocidad de rueda
	Wv_eq_0 = 				Wv <= ZERO_WHEEL_VEL;
	Wv_ge_LANDINGv = 		Wv >= bankInputs.testParms.landing_wheel_vel;
	Wv_gt_0 = 				Wv > ZERO_WHEEL_VEL;

	// Presiones
	Ph_gt_0 = 				Ph >= ZERO_PH;
	Ph_ge_Ph1 = 			Ph	>= bankInputs.testParms.ph_threshold;

	Pf_gt_0 = 				Pf > ZERO_PF;
	Pf_ge_Pf1 = 			Pf >= bankInputs.testParms.pf_threshold;;

	// Temperaturas
	T1_ge_Thot = 			T1 >= bankInputs.testParms.t1_hot;
	T2_ge_Thot = 			T2 >= bankInputs.testParms.t2_hot;


	// Tiempo
	timeOut = 				(millis() - _t0) > _dt;

	// Activar DAQ e inicializar FreqCounter
	bankInputs.start();
}

/*****************************************************
 *
 * Transitions from ST_IDLE
 *
 *****************************************************/
/**
 * ## Presionando el botón START o keyPad *2 se inicia el ensayo.
 */
bool tr_idle_checking() {
	if (bankKp->ev_key[2]) {		// cmd[2]:  *2#
		bankKp->ev_key[2] = false;
		return true;
	}

	return bankButtons.read(0);
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
bool tr_checking_condok() {	// 0

	bool cond_ok = true;

	if (bankInputs.check(MV_GT_0)) {
		cond_ok = false;

		Serial << "\n** DETENER MASA [" << _FLOAT(Mv, 3) << " rpm]";
	}

	if (bankInputs.check(WV_GT_0)) {
		cond_ok = false;
		Serial << "\n** DETENER RUEDA [" << _FLOAT(Wv, 3) << " rpm]";
	}

	if (Ph_gt_0) {
		cond_ok = false;
		Serial << "\n** REDUCIR PH [" << _FLOAT(Ph, 3) << " bar]";
	}

	if (Pf_gt_0) {
		cond_ok = false;
		Serial << "\n** REDUCIR PF [" << _FLOAT(Pf, 3) << " bar]";
	}

	if (T1_ge_Thot || T2_ge_Thot) {
		cond_ok = false;
		Serial << "\n** Temp Alta [T1: " << _FLOAT(T1, 3) << " °C]";
		Serial << " [T2: " << _FLOAT(T2, 3) << " °C]";
	}

	if (!cond_ok) {
		Serial << "\n----------------------";
	}

	return cond_ok;
}



bool tr_idle_monitoring() {
	if (bankKp->ev_key[5]) {
		bankKp->ev_key[5] = false;
		return true;
	}
	return false;
}


/**
 * ## Presionando RESET desde cualquier estado se retorna al inicio (IDLE)
 */
bool tr_any_idle() { // 1
	if (timeOut) {
		bankLeds.beep();
		Serial << "\nTimeout";
		return true;
	}

	return bankButtons.read(3);
}


/*****************************************************
 *
 * Transitions from ST_COND_OK
 *
 *****************************************************/
/**
 * ## Cumplidas las condiciones, esperar inicio de giro de masa
 */
bool tr_condok_speeding() {
	Serial << "\nST_COND_OK> mass vel: " << _FLOAT(Mv, 3)
	<< " ** INICIAR GIRO **";
	return Mv_gt_0;
}


/*****************************************************
 *
 * Transitions from ST_SPEEDING
 *
 *****************************************************/
/**
 * ## Se alcanza la máxima velocidad de masa.
 */
bool tr_speeding_maxvel() {

	Serial << "\nST_SPEEDING> Mv: " << _FLOAT(Mv, 3)
	<< " ** ACELERAR a 500 rpm **";
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
bool tr_maxvel_landing() {
	Serial << "\nST_MAX_VEL> Mv: " << _FLOAT(Mv, 3);
	Serial << ", Wv: " << _FLOAT(Wv, 3) << " ** ATERRIZAR RUEDA ***";
	return Wv_ge_LANDINGv;
}



/**
 * ## Se retorna al estado SPEEDING si la velocidad de masa cae por debajo de la velocidad máxima
 */
bool tr_maxvel_speeding() {
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
bool tr_landing_landed() {
	Serial << "\nLANDING> Mv: " << _FLOAT(Mv, 3);
	Serial << ", Wv: " << _FLOAT(Wv, 3);
	Serial << ", PH: " << _FLOAT(Ph, 3) << " ** AUMENTAR Ph a "
	<< _FLOAT(bankInputs.testParms.ph_threshold, 3) << " ***";
	return Ph_ge_Ph1;
}





/**
 * ## Se ingresa al estado de ERROR si la velocidad de masa disminuye por debajo de la velocidad de frenado
 * antes de aplicar el freno.
 */
bool tr_landing_error() {
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
bool tr_landed_brakingvel() {

	if (!Mv_le_BRAKEv_max) {
		Serial << "\nST_LANDED> Mv: " << _FLOAT(Mv, 3);
		Serial << " ** DISMINUIR VELOCIDAD **";
	}
	if (!Mv_ge_BRAKEv_min) {
		Serial << "\nST_LANDED> Mv: ";
		Serial << _FLOAT(Mv, 3) << " ** ACELERAR **";
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
bool tr_brakingvel_braking() {
	Serial << "\nST_BRAKING_VEL> PF: " << _FLOAT(Pf, 3);
	Serial << " ***### APLICAR FRENO ###***";

	return Pf_ge_Pf1;
}





/**
 * ## Se regresa al estado LANDED si la velocidad de masa sale del rango de frenado antes de que la presión de freno alcance la presión nominal
 */
bool tr_brakingvel_landed() {
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
bool tr_braking_complete() {
	double t = bankInputs.getTime();

	Serial << "\nST_BRAKING> Mv: " << _FLOAT(Mv, 3);
	Serial << ", Wv: " << _FLOAT(Wv, 3) << ", d: " << _FLOAT(bankInputs.getDistance(), 3);
	Serial << ", t: " << _FLOAT(t, 3);
	Serial << " ** MANTENER Pf HASTA DETENER ** ";

	return (Mv_eq_0 && Wv_eq_0);
}





/**
 * ## Se disminuyó demasiado la presión de freno durante el frenado
 */
bool tr_braking_error() {
	if (!Pf_gt_0) {
		Serial << "\nPresión de freno baja durante el frenado";
		return true;
	}
	return false;
}




/*****************************************************
 *
 * Transitions from ST_MONITORING
 *
 *****************************************************/
/**
 * ## Condiciones nominales para el inicio del ensayo.
 * - Masa detenida
 */
bool tr_monitoring_idle() {
	Serial << "\n" << bankInputs.getTime();
	Serial << "\t" << Mv;
	Serial << "\t" << Wv;
	Serial << "\t" << Ph;
	Serial << "\t" << Pf;
	Serial << "\t" << T1;
	Serial << "\t" << T2;
	Serial << "\t" << bankInputs.getDistance();
	Serial << "\t" << bankInputs.encoderRead().angle;
	Serial << "\t Display:" << bankInputs.getDisplayVar();

	return bankButtons.read(3);
}



/**
 * Array of transition functions.
 * NOTE: ORDER MUST FOLLOW THE ENUM  MainFSM::Transitions
 */
bool (*mainTransitions[])(void) = {

	tr_idle_checking,
	tr_idle_monitoring,
	tr_checking_condok,
	tr_any_idle,
	tr_condok_speeding,
	tr_speeding_maxvel,
	tr_maxvel_landing,
	tr_maxvel_speeding,
	tr_landing_landed,
	tr_landing_error,
	tr_landed_brakingvel,
	tr_brakingvel_braking,
	tr_brakingvel_landed,
	tr_braking_complete,
	tr_braking_error,
	tr_monitoring_idle,

};


#endif /* MAINTRANSITIONS_H_ */
