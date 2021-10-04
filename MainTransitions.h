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

extern void state_reset();


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
bool tr_checking_condok() {	// 0

	bool cond_ok = true;

	if (Mv_gt_0) {
		cond_ok = false;
		uint32_t rpm = bankInputs.getRpm();
		Serial << "\n** DETENER MASA [" << _FLOAT(rpm, 3) << " rpm]";
	}

	if (Wv_gt_0) {
		cond_ok = false;
		uint16_t wv = bankInputs.getWv();
		Serial << "\n** DETENER RUEDA [" << _FLOAT(wv, 3) << " rpm]";
	}

	if (Ph_gt_0) {
		cond_ok = false;
		uint16_t ph = bankInputs.getPh();
		Serial << "\n** REDUCIR PH [" << _FLOAT(ph, 3) << " bar]";
	}

	if (Pf_gt_0) {
		cond_ok = false;
		uint16_t pf = bankInputs.getPf();
		Serial << "\n** REDUCIR PF [" << _FLOAT(pf, 3) << " bar]";
	}

	if (T1_ge_Thot || T2_ge_Thot) {
		cond_ok = false;
		uint16_t T1 = bankInputs.getT1();
		uint16_t T2 = bankInputs.getT2();
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
	return btn_pressed[3];
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
	uint16_t Mv = bankInputs.getRpm();
	Serial << "\nST_COND_OK> mass vel: " << _FLOAT(Mv, 3)
	<< " ** INICIAR GIRO **";
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
bool tr_speeding_maxvel() {
	uint16_t Mv = bankInputs.getRpm();

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
	uint16_t Mv = bankInputs.getRpm();
	uint16_t wv = bankInputs.getWv();

	Serial << "\nST_MAX_VEL> Mv: " << _FLOAT(Mv, 3);
	Serial << ", Wv: " << _FLOAT(wv, 3) << " ** ATERRIZAR RUEDA ***";
	return Wv_ge_LANDINGv;
}



/**
 * ## Se retorna al estado SPEEDING si la velocidad de masa cae por debajo de la velocidad máxima
 */
bool tr_maxvel_speeding() { // 5
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
bool tr_landing_landed() { // 6
	uint16_t wv = bankInputs.getWv();
	uint16_t ph = bankInputs.getPh();
	uint16_t Mv = bankInputs.getRpm();

	Serial << "\nLANDING> Mv: " << _FLOAT(Mv, 3);
	Serial << ", Wv: " << _FLOAT(wv, 3);
	Serial << ", PH: " << _FLOAT(ph, 3) << " ** AUMENTAR Ph a "
	<< _FLOAT(bankInputs.testParms.ph_threshold, 3) << " ***";
	return Ph_ge_Ph1;
}





/**
 * ## Se ingresa al estado de ERROR si la velocidad de masa disminuye por debajo de la velocidad de frenado
 * antes de aplicar el freno.
 */
bool tr_landing_error() { // 7
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
bool tr_landed_brakingvel() { // 8
	uint16_t Mv = bankInputs.getRpm();

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
bool tr_brakingvel_braking() { // 9
	uint16_t pf = bankInputs.getPf();
	Serial << "\nST_BRAKING_VEL> PF: " << _FLOAT(pf, 3);
	Serial << " ***### APLICAR FRENO ###***";

	return Pf_ge_Pf1;
}





/**
 * ## Se regresa al estado LANDED si la velocidad de masa sale del rango de frenado antes de que la presión de freno alcance la presión nominal
 */
bool tr_brakingvel_landed() { // 10
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
bool tr_braking_complete() { // 11
	uint16_t wv = bankInputs.getWv();
	double t = bankInputs.getTime();
	uint16_t Mv = bankInputs.getRpm();

	Serial << "\nST_BRAKING> Mv: " << _FLOAT(Mv, 3);
	Serial << ", Wv: " << _FLOAT(wv, 3) << ", d: " << _FLOAT(bankInputs.getDistance(), 3);
	Serial << ", t: " << _FLOAT(t, 3);
	Serial << " ** MANTENER Pf HASTA DETENER ** ";

	return (Mv_eq_0 && Wv_eq_0);
}





/**
 * ## Se disminuyó demasiado la presión de freno durante el frenado
 */
bool tr_braking_error() { // 12
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
bool tr_monitoring_idle() { // 13
	Serial << "\n" << bankInputs.getTime();
	Serial << "\t" << bankInputs.getRpm();
	Serial << "\t" << bankInputs.getWv();
	Serial << "\t" << bankInputs.getPh();
	Serial << "\t" << bankInputs.getPf();
	Serial << "\t" << bankInputs.getT1();
	Serial << "\t" << bankInputs.getT2();
	Serial << "\t" << bankInputs.getDistance();
	Serial << "\t" << bankInputs.encoderRead().angle;
	Serial << "\t Display:" << bankInputs.getDisplayVar();

	return btn_pressed[3];
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
