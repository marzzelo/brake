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

extern BankButtons *bankButtons;

double tf;	//<! final braking time

extern bool mon();

/*****************************************************
 *
 * Transitions from ST_IDLE
 *
 *****************************************************/
/**
 * ## Presionando el bot�n START o keyPad 2 se inicia el ensayo.
 */
bool tr_idle_checking() {
	return bankButtons->read(0) || bankKp->readKey(2);
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
 * - Presi�n de horquilla nula
 * - Presi�n de freno nula
 * - Temperaturas por debajo del l�mite caliente
 */
bool tr_checking_condok() {

	bool cond_ok = true;
	bool ttd = mon();	// limit message printing to every 1 second

	if (bankInputs->check(MV_GT_0)) {
		cond_ok = false;
		if (ttd) {
			Serial << "\n** DETENER MASA [" << _FLOAT(bankInputs->getRpm(), 3) << " rpm]";
			tm1638->dispstr(" MV GT 0");
		}
	}

	if (bankInputs->check(WV_GT_0)) {
		cond_ok = false;
		if (ttd) {
			Serial << "\n** DETENER RUEDA [" << _FLOAT(bankInputs->getWv(), 3) << " rpm]";
			tm1638->dispstr(" WV GT 0");
		}
	}

	if (bankInputs->check(PH_GT_0)) {
		cond_ok = false;
		if (ttd) {
			Serial << "\n** REDUCIR PH [" << _FLOAT(bankInputs->getPh(), 3) << " bar]";
			tm1638->dispstr(" PH GT 0");
		}
	}

	if (bankInputs->check(PF_GT_0)) {
		cond_ok = false;
		if (ttd) {
			Serial << "\n** REDUCIR PF [" << _FLOAT(bankInputs->getPf(), 3) << " bar]";
			tm1638->dispstr(" PF GT 0");
		}
	}

	if (bankInputs->check(T1_GE_THOT) || bankInputs->check(T2_GE_THOT)) {
		cond_ok = false;

		if (ttd) {
			Serial << "\n** Temp Alta [T1: " << _FLOAT(bankInputs->getT1(), 3) << " �C]";
			Serial << " [T2: " << _FLOAT(bankInputs->getT2(), 3) << " �C]";
			tm1638->dispstr("TMP ALTA");
		}
	}

	if (!cond_ok) {
		if (ttd) Serial << "\n----------------------";
	}

	return cond_ok;
}


/**
 * ## Presionando keypad "5" se ingresa a monitoreo de variables
 */
bool tr_idle_monitoring() {
	return bankKp->readKey(5);
}


/**
 * ## Presionando RESET (desde cualquier estado) se retorna al inicio (IDLE)
 */
bool tr_any_idle() {
	if (bankInputs->check(TIMEOUT)) {
		bankLeds->beep();
		if (mon()) Serial << "\nTimeout";
		return true;
	}

	return bankButtons->read(3);
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
	if (mon()) {
		Serial << "\nST_COND_OK> mass vel: " << _FLOAT(bankInputs->getRpm(), 3) << " ** INICIAR GIRO **";
		tm1638->dispstr("INICIAR ");
	}
	return bankInputs->check(MV_GT_0);
}


/*****************************************************
 *
 * Transitions from ST_SPEEDING
 *
 *****************************************************/
/**
 * ## Se alcanza la m�xima velocidad de masa.
 */
bool tr_speeding_maxvel() {

	if (mon()) {
		Serial << "\nST_SPEEDING> Mv: " << _FLOAT(bankInputs->getRpm(), 3) << " ** ACELERAR a 500 rpm **";
		tm1638->dispmix("ACEL", bankInputs->getRpm());
	}
	return bankInputs->check(MV_GT_MAX);
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
	if (mon()) {
		Serial << "\nST_MAX_VEL> Mv: " << _FLOAT(bankInputs->getRpm(), 3);
		Serial << ", Wv: " << _FLOAT(bankInputs->getWv(), 3) << " ** ATERRIZAR RUEDA ***";
		tm1638->dispmix("ATERRIZA", bankInputs->getWv());
	}
	return bankInputs->check(WV_GE_LANDINGV);
}



/**
 * ## Se retorna al estado SPEEDING si la velocidad de masa cae por debajo de la velocidad m�xima
 */
bool tr_maxvel_speeding() {
	return bankInputs->check(MV_LE_BRAKEV_MAX);
}




/*****************************************************
 *
 * Transitions from ST_LANDING
 *
 *****************************************************/
/**
 * ## Se alcanza el estado LANDED cuando la presi�n de horquilla alcanza o supera el valor nominal de aterrizaje.
 */
bool tr_landing_landed() {
	if (mon()) {
		Serial << "\nLANDING> Mv: " << _FLOAT(bankInputs->getRpm(), 3);
		Serial << ", Wv: " << _FLOAT(bankInputs->getWv(), 3);
		Serial << ", PH: " << _FLOAT(bankInputs->getPh(), 3) << " ** AUMENTAR Ph a " << _FLOAT(bankInputs->testParms.ph_threshold, 3) << " ***";
	}

	return bankInputs->check(PH_GE_PH1);
}





/**
 * ## Se ingresa al estado de ERROR si la velocidad de masa disminuye por debajo de la velocidad de frenado
 * antes de aplicar el freno.
 */
bool tr_landing_error() {
	if (bankInputs->check(MV_LE_BRAKEV_MAX)) {
		if (mon()) Serial << "\nERROR: velocidad de masa baja antes de frenar";
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
 * - Los l�mites superior e inferior de la zona son establecidos mediante el men� del sistema.
 * - Los valores nominales aproximados est�n alrededor de las 420 rpm
 */
bool tr_landed_brakingvel() {

	if (!bankInputs->check(MV_LE_BRAKEV_MAX)) {
		if (mon()) {
			Serial << "\nST_LANDED> Mv: " << _FLOAT(bankInputs->getRpm(), 3);
			Serial << " ** DISMINUIR VELOCIDAD **";
		}
	}
	if (!bankInputs->check(MV_GE_BRAKEV_MIN)) {
		if (mon()) {
			Serial << "\nST_LANDED> Mv: ";
			Serial << _FLOAT(bankInputs->getRpm(), 3) << " ** ACELERAR **";
		}
	}
	return (bankInputs->check(MV_LE_BRAKEV_MAX) && bankInputs->check(MV_GE_BRAKEV_MIN));
}





/*****************************************************
 *
 * Transitions from ST_BRAKING_VEL
 *
 *****************************************************/
/**
 * ## Se ingresa a BRAKING cuando la presi�n de freno alcanza la presi�n nominal del ensayo.
 * - El valor nominal de presi�n de freno puede fijarse desde el men� del sistema.
 */
bool tr_brakingvel_braking() {
	if (mon()) {
		Serial << "\nST_BRAKING_VEL> PF: " << _FLOAT(bankInputs->getPf(), 3);
		Serial << " ***### APLICAR FRENO ###***";
	}

	return bankInputs->check(PF_GE_PF1);
}





/**
 * ## Se regresa al estado LANDED si la velocidad de masa sale del rango de frenado antes de que la presi�n de freno alcance la presi�n nominal
 */
bool tr_brakingvel_landed() {
	if (!(bankInputs->check(MV_LE_BRAKEV_MAX) && bankInputs->check(MV_GE_BRAKEV_MIN))) {
		bankLeds->beep();
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
	tf = bankInputs->getTime();
	if (mon()) {
		Serial << "\nST_BRAKING> Mv: " << _FLOAT(bankInputs->getRpm(), 3);
		Serial << ", Wv: " << _FLOAT(bankInputs->getWv(), 3) << ", d: " << _FLOAT(bankInputs->getDistance(), 3);
		Serial << ", t: " << _FLOAT(tf, 3);
		Serial << " ** MANTENER Pf HASTA DETENER ** ";
		Serial << " (Pf: " << _FLOAT(bankInputs->getPf(), 3) << ")";
	}
	return (bankInputs->check(MV_EQ_0) && bankInputs->check(WV_EQ_0));
}





/**
 * ## Se disminuy� demasiado la presi�n de freno durante el frenado
 */
bool tr_braking_error() {
	if (!bankInputs->check(PF_GE_PF1)) {
		if (mon()) Serial << "\nPresi�n de freno baja durante el frenado";
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

	char buff[5];

	if (bankButtons->read(1)) {
		bankLeds->beep();

		tm1638->dispstr(bankInputs->nextDisplayVar());
	}

	if (millis() % 100 == 0) {
		Serial << "\n" << bankInputs->getTime();
		Serial << "\t" << bankInputs->getRpm();
		Serial << "\t" << bankInputs->getWv();
		Serial << "\t" << bankInputs->getPh();
		Serial << "\t" << bankInputs->getPf();
		Serial << "\t" << bankInputs->getT1();
		Serial << "\t" << bankInputs->getT2();
		Serial << "\t" << bankInputs->getDistance();
		Serial << "\t" << bankInputs->encoderRead().angle;
		Serial << "\t" << bankInputs->getDisplayVar();	// debug only

		dtostrf(bankInputs->getDisplayVar(), 4, 0, buff);
		tm1638->dispstr(buff, 4);
	}
	return bankButtons->read(3);
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
