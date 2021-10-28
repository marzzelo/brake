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


#define 		DOUBLE_BUFF_SIZE	10
#define 		MSG_BUFF_SIZE		32

extern BankButtons *bankButtons;

double tf;	//<! final braking time

char dbuff[DOUBLE_BUFF_SIZE] = { 0 };
char mbuff[MSG_BUFF_SIZE] = { 0 };

extern Timer *timerDisplay;
extern void daqprint();

/**
 * Muestra en displays TM1638 el nombre y el valor de la variable objetivo actual
 * Activa led de la variable correspondiente
 */
double displayVar(int index = NULL) {
	if (index != NULL) {
		bankInputs->setDisplayVarIndex(index);
	}

	double value = bankInputs->getDisplayVarValue();
	tm1638->dispmix(bankInputs->getDisplayVarName(), value);
	tm1638->ledOnly(bankInputs->getDisplayVarIndex());
	return value;
}

/*****************************************************
 *
 * Transitions from ST_IDLE
 *
 *****************************************************/
/**
 * ## Presionando el botón START o keyPad 2 se inicia el ensayo.
 */
bool tr_idle_checking() {
	return (bankButtons->read(0) || bankKp->readKey(2));
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
bool tr_checking_condok() {

	bool cond_ok = true;
	bool ttd = timerDisplay->read();	// limit message printing to every DATA_REFRESH_PERIOD useconds
	bool displaybussy = false;

	if (bankInputs->check(MV_GT_0)) {
		cond_ok = false;
		if (ttd) {
			PRINT("\n** DETENER MASA = ", bankInputs->getRpm());

			tm1638->dispstr("MAS GT 0");
			matrix->text("ERR VM > 0");
			displaybussy = true;
		}
	}

	if (bankInputs->check(WV_GT_0)) {
		cond_ok = false;
		if (ttd) {
			PRINT("\n** DETENER RUEDA = ", bankInputs->getWv());

			if (!displaybussy) {
				tm1638->dispstr("RUE GT 0");
				matrix->text("ERR VR > 0");
				displaybussy = true;
			}
		}
	}

	if (bankInputs->check(PH_GT_0)) {
		cond_ok = false;
		if (ttd) {
			PRINT("\n** REDUCIR PH = ", bankInputs->getPh());

			if (!displaybussy) {
				tm1638->dispstr("PH  GT 0");
				matrix->text("ERR Ph > 0");
				displaybussy = true;
			}
		}
	}

	if (bankInputs->check(PF_GT_0)) {
		cond_ok = false;
		if (ttd) {
			PRINT("\n** REDUCIR PF = ",  bankInputs->getPf());

			if (!displaybussy) {
				tm1638->dispstr("PF  GT 0");
				matrix->text("ERR Pf > 0");
				displaybussy = true;
			}
		}
	}

	if (bankInputs->check(T1_GE_THOT) || bankInputs->check(T2_GE_THOT)) {
		cond_ok = false;

		if (ttd) {
			PRINTS("\n** Temp Alta [T1 or T2]");

			if (!displaybussy) {
				tm1638->dispstr("TMP ALTA");
				matrix->text("ERR Temp ALTA");
				displaybussy = true;
			}
		}
	}

	if (bankInputs->check(ANGLE_GT_0)) {
		cond_ok = false;

		if (ttd) {
			PRINT("\nCorregir Inclinación ANG = ", bankInputs->encoderRead().angle);

			if (!displaybussy) {
				tm1638->dispstr("ANG GT 0");
				matrix->text("ERR Angulo > 0");
				displaybussy = true;
			}
		}
	}

	if (!cond_ok) {
		if (ttd)
			PRINTS("\n----------------------");
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
		PRINTS("\nTimeout");
		return true;
	}

	return (bankButtons->read(0) == BankButtons::LONG_PRESSED);
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

	if (timerDisplay->read()) {
		double rpm = bankInputs->getRpm();
		PRINT("\nmass vel = ", rpm); PRINTS(" ** INICIAR GIRO **");

		return bankInputs->check(MV_GT_0);
	}

	return false;
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

	if (timerDisplay->read()) {
		double rpm = bankInputs->getRpm();

		PRINTS("\nST_SPEEDING> ** ACELERAR a 500 rpm **");

		tm1638->dispmix("ACEL", bankInputs->getRpm());

		snprintf(mbuff, MSG_BUFF_SIZE - 1, "Acel %d rpm", round(rpm));
		matrix->text(mbuff);
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
	if (timerDisplay->read()) {
		double wheel = bankInputs->getWv();
		PRINTS(" ** ATERRIZAR RUEDA ***");

		displayVar(); // Mass vel
		snprintf(mbuff, MSG_BUFF_SIZE - 1, "Aterr %d m/s", round(wheel));
		matrix->text(mbuff);
	}
	return bankInputs->check(WV_GE_LANDINGV);
}

/**
 * ## Se retorna al estado SPEEDING si la velocidad de masa cae por debajo de la velocidad máxima
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
 * ## Se alcanza el estado LANDED cuando la presión de horquilla alcanza o supera el valor nominal de aterrizaje.
 */
bool tr_landing_landed() {
	if (timerDisplay->read()) {
		double ph = bankInputs->getPh();
		PRINT("\nLANDING> PH: ", ph);
		PRINT(" ** AUMENTAR Ph a ", bankInputs->testParms.ph_threshold);

		displayVar(); // Ph
		snprintf(mbuff, MSG_BUFF_SIZE - 1, "PH = %d bar", round(ph));
		matrix->text(mbuff);
	}

	return bankInputs->check(PH_GE_PH1);
}

/**
 * ## Se ingresa al estado de ERROR si la velocidad de masa disminuye por debajo de la velocidad de frenado
 * antes de aplicar el freno.
 */
bool tr_landing_error() {
	if (bankInputs->check(MV_LE_BRAKEV_MAX)) {
		if (timerDisplay->read()) {
			PRINTS("\nERROR: velocidad de masa baja antes de frenar");
			tm1638->dispstr("ERR. MASA");
		}
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

	if (!bankInputs->check(MV_LE_BRAKEV_MAX)) {
		if (timerDisplay->read()) {
			double rpm = bankInputs->getRpm();

			PRINT("\nST_LANDED> Mv: ", rpm);
			PRINTS(" ** DISMINUIR VELOCIDAD **");

			tm1638->dispmix("REDU", rpm);
			snprintf(mbuff, MSG_BUFF_SIZE - 1, "Reduc %d rpm", round(rpm));
			matrix->text(mbuff);
		}
	}
	if (!bankInputs->check(MV_GE_BRAKEV_MIN)) {
		if (timerDisplay->read()) {
			double rpm = bankInputs->getRpm();

			PRINT("\nST_LANDED> Mv: ", rpm);
			PRINTS(" ** ACELERAR **");

			tm1638->dispmix("ACEL", rpm);
			snprintf(mbuff, MSG_BUFF_SIZE - 1, "Acelr %d rpm", round(rpm));
			matrix->text(mbuff);
		}
	}
	return (bankInputs->check(MV_LE_BRAKEV_MAX)
			&& bankInputs->check(MV_GE_BRAKEV_MIN));
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
	if (timerDisplay->read()) {
		double pf = bankInputs->getPf();

		PRINT("\nST_BRAKING_VEL> PF: ", pf);
		PRINTS(" ***### APLICAR FRENO ###***");

		tm1638->dispmix("FREN", pf);
		snprintf(mbuff, MSG_BUFF_SIZE - 1, "FRENO %d bar", round(pf));
		matrix->text(mbuff);


	}

	return bankInputs->check(PF_GE_PF1);
}

/**
 * ## Se regresa al estado LANDED si la velocidad de masa sale del rango de frenado antes de que la presión de freno alcance la presión nominal
 */
bool tr_brakingvel_landed() {
	if (!(bankInputs->check(MV_LE_BRAKEV_MAX)
			&& bankInputs->check(MV_GE_BRAKEV_MIN))) {
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
	if (timerDisplay->read()) {
		double wheel = bankInputs->getWv();

		PRINT("\nST_BRAKING> d: ", bankInputs->getDistance());
		PRINT("\nt: ", tf);

		PRINTS(" ** MANTENER Pf HASTA DETENER ** ");
		PRINT(" (Pf: ", bankInputs->getPf());

		displayVar();  // Wheel
//		snprintf(mbuff, 16, "Rueda %d m/s", round(wheel));
//		matrix->text(mbuff);
	}
	return (bankInputs->check(WV_EQ_0));
}

bool tr_complete_idle() {
	static int _msg = 0;

	switch (_msg) {
	case 0:
		if (bankInputs->check(TIMEOUT)) {
			tm1638->dispmix("tf=", tf, 1);
			snprintf(mbuff, MSG_BUFF_SIZE - 1, "Tiempo = %d s", round(tf));
			matrix->text(mbuff);
			bankInputs->setTimeOut(3000);
			++_msg;
		}
		break;

	case 1:
		if (bankInputs->check(TIMEOUT)) {
			double dist = bankInputs->getDistance();
			tm1638->dispmix("d=", dist, 1);
			snprintf(mbuff, MSG_BUFF_SIZE - 1, "Dist = %d m", round(dist));
			matrix->text(mbuff);
			bankInputs->setTimeOut(3000);
			++_msg;
		}
		break;

	case 2:
		if (bankInputs->check(TIMEOUT)) {
			bankLeds->beep();
			tm1638->dispstr("PRESS RESET TO CONTINUE");
			matrix->setMessage("PRESS RESET TO CONTINUE");
			bankInputs->setTimeOut(8000);
			_msg = 0;
		}
		break;
	}

	return (bankButtons->read(0) == BankButtons::LONG_PRESSED);
}

/**
 * ## Se disminuyó demasiado la presión de freno durante el frenado
 */
bool tr_braking_error() {
//	if (bankInputs->check(PF_LT_PF1)) {
//		Serial << "\nPresión de freno baja durante el frenado";
//		tm1638->ledOnly(BankAnalogInputs::ERROR);
//		tm1638->dispstr("ERROR Pf");
//		return true;
//	}
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

	if (bankButtons->read(1) == BankButtons::PRESSED) {
		bankLeds->beep();
		bankInputs->nextDisplayVar();
	}

	int btn = tm1638->firstPressed();

	if (btn >= 0) {
		bankLeds->beep();
		bankInputs->setDisplayVarIndex(btn);
	}


	if (timerDisplay->read()) {
//		bankInputs->setTimeOut(250);

		double value = displayVar();
		dtostrf(value, 0, 1, dbuff);
		snprintf(mbuff, MSG_BUFF_SIZE - 1, "%s = %s", bankInputs->getDisplayVarName(), dbuff);
		matrix->text(mbuff);
	}

	return (bankButtons->read(0) == BankButtons::LONG_PRESSED);
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
	tr_complete_idle,

};

#endif /* MAINTRANSITIONS_H_ */
