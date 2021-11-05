/*
 * MainOnEnterings.h
 *
 *  Created on: 30 sep. 2021
 *      Author: valdez
 */

#ifndef MAINONENTERINGS_H_
#define MAINONENTERINGS_H_

#define PRINT_HEADER() 		PDAQS("\nsep=\t\nt[s]\tSTATE\tMv\tWv\tPh\tPf\tT1\tT2\td\tangle")

#define BUFF_SIZE	80

char str[BUFF_SIZE] = { 0 };



void ent_idle() {
	bankLeds->beep();
	bankLeds->relayOffAll();
	dprint = false;

	bankKp->clearAll();
	bankKp->checkCommands = true;
	bankKp->start();

	bankButtons->reset();
	bankInputs->reset();


	tm1638->setShiftVel(TM1638::VEL_FAST);
	tm1638->setBrightness(TM1638::BRI_INTENSE);
	tm1638->dispstr("Banco de Freno - " VERSION);
	tm1638->ledsOff();

	matrix->setEffects(PA_CENTER, 25, 4000, PA_PRINT, PA_SCROLL_UP);
	matrix->setMessage("loading...");

	printer.print_item("Presionar START para comenzar");
	printer.print_item("o ingresar   1     para Configurar ensayo");
	printer.print_item("             2     para Comenzar ensayo");
	printer.print_item("             3     para Monitoreo de variables");
	printer.print_separator();

	char *l4[] = {"BANCO DE FRENO " VERSION,
			"1# Configuración",
			"2# Comenzar Test",
			"3# Monitoreo Señales"};

	lcd->write4l(l4);

	PRINTSA("\n -> IDLE");
}


void ent_checking() {
	bankInputs->start();
	bankLeds->relayStart(0);
	bankLeds->beep();

	tm1638->dispstr("Checking");

	PRINTS("\nST_CHECKING_COND... [Esperando condiciones de inicio: Mv=0, Wv=0, Ph=0, Pf=0, T1&T2<Thot]");
	lcd->clear();

	char *l4[] = {"Poner todo a cero   ",
			            "Mv, Wv, Ph, T1, T2  ",
						"",
				        "RESET cancela ensayo"};

	lcd->write4l(l4);

	matrix->setEffects(PA_CENTER, 25, 4000, PA_PRINT, PA_PRINT);
	matrix->setMessage("S T A R T");

	PRINTSA("\n -> CHECKING_CONDITINOS");
}


void ent_condok() {
	PRINTS("\nST_COND_OK... [Esperando Mv > 0]");
	bankLeds->relayOnly(0);
	bankLeds->beep();

	PRINT_HEADER();
	dprint = true;

	tm1638->dispstr("Iniciar ");
	matrix->setEffects(PA_CENTER, 25, 4000, PA_MESH, PA_SCROLL_RIGHT);
	matrix->setMessage("Iniciar Giro");

//	      		         $$$$$$$$$$$$$$$$$$$$
	char *l4[] = {"",
						"Iniciar Giro de Masa"};

	lcd->write4l(l4, 2);

	PRINTSA("\n -> COND_OK");
}

void ent_speeding() {
	bankLeds->beep();
	bankLeds->relayOnly(1);

	bankInputs->setDisplayVarIndex(BankAnalogInputs::MASS);

	tm1638->ledOnly(BankAnalogInputs::MASS);
	tm1638->dispstr("Acelerar");

//	                     $$$$$$$$$$$$$$$$$$$$
	char *l4[] = {"",
				        "Acelerar hasta Vmáx"};

	lcd->write4l(l4, 2);
	matrix->setEffects(PA_CENTER, 25, 4000, PA_PRINT, PA_PRINT);

	PRINTSA("\n -> SPEEDING");
}

void ent_maxvel() {
	bankLeds->beep();
	bankLeds->relayOnly(2);

	displayVar(BankAnalogInputs::WHEEL);
	PRINT("\n\nST_MAX_VEL... [Esperando Wv > ", bankInputs->testParms.landing_wheel_vel);
	PRINTS(" m/s]");

//	                     $$$$$$$$$$$$$$$$$$$$
	char *l4[] = {"",
						"Esperando aterrizaje"};

	lcd->write4l(l4, 2);

	PRINTSA("\n -> MAX_VEL");
}

void ent_landing() {
	bankLeds->beep();
	bankLeds->relayOnly(3);

	displayVar(BankAnalogInputs::PH);

	PRINT("\n\nST_LANDING... [Esperando PH >= ", bankInputs->testParms.ph_threshold);
	PRINTS(" bar]");

//	                     $$$$$$$$$$$$$$$$$$$$
	char *l4[] = {"",
						"Esperando Ph máx"};

	lcd->write4l(l4, 2);

	PRINTSA("\n -> LANDING");
}

void ent_landed() {
	bankLeds->beep();
	bankLeds->relayOnly(4);

	displayVar(BankAnalogInputs::MASS);

	PRINT("\n\nST_LANDED... [Esperando ", bankInputs->testParms.brake_mass_vel_min);
	PRINT(" < Mv < ", bankInputs->testParms.brake_mass_vel_max);
	PRINTS("]");

//	                     $$$$$$$$$$$$$$$$$$$$
	char *l4[] = {"",
						"Esperando Velocidad",
	                    "de frenado"};

	lcd->write4l(l4, 3);

	PRINTSA("\n -> LANDED");
}

void ent_brakingvel() {
	bankLeds->beep();
	bankLeds->relayOnly(5);

	displayVar(BankAnalogInputs::PF);

	PRINT("\n\nST_BRAKING_VEL... [Esperando PF >= ", bankInputs->testParms.pf_threshold);
	PRINTS(" bar]");

//	                     $$$$$$$$$$$$$$$$$$$$
	char *l4[] = {"",
						"-- APLICAR FRENO --"};

	lcd->write4l(l4, 2);

	PRINTSA("\n -> BRAKING_VEL");
}

void ent_braking() {
	bankLeds->beep();
	bankLeds->relayOnly(6);
	bankInputs->startCounting();

	displayVar(BankAnalogInputs::WHEEL);

	matrix->setEffects(PA_CENTER, 25, 4000, PA_SCROLL_DOWN_RIGHT, PA_SCROLL_UP_RIGHT);
	matrix->setMessage("MANTENER Pf");

	PRINTS("\n\nST_BRAKING... [Esperando Wv = 0 & Mv = 0]");

//	                     $$$$$$$$$$$$$$$$$$$$
	char *l4[] = {"",
						"-- << FRENANDO >> --",
						"Esperando detención"};

	lcd->write4l(l4, 3);

	PRINTSA("\n -> BRAKING");
}

void ent_error() {
	bankLeds->beep();
	bankLeds->relayOffAll();
	bankLeds->relayStart(7);
	tm1638->ledsOff();
	matrix->setEffects(PA_CENTER, 25, 4000, PA_PRINT, PA_PRINT);

	PRINTS("\n**TEST ERROR** <RESET> para REINICIAR");

//	                     $$$$$$$$$$$$$$$$$$$$
	char *l4[] = {"",
						"ERROR DURANTE ENSAYO"};

	lcd->write4l(l4, 2);

	PRINTSA("\n -> ERROR");
}

void ent_complete() {

	bankLeds->beep();
	bankLeds->relayOnly(7);
	tm1638->ledsOff();

	bankInputs->stopCounting();
	bankInputs->setTimeOut(3000);

	double dist = bankInputs->getDistance();

	tm1638->dispstr("-FINAL- ");
	matrix->setEffects(PA_CENTER, 25, 4000, PA_MESH, PA_WIPE_CURSOR);
	matrix->setMessage("--- FINAL ---");

	char buff_tf[10] = { 0 };
	char buff_dist[10] = { 0 };

	dtostrf(tf, 0, 1, buff_tf);
	dtostrf(dist, 0, 1, buff_dist);

	snprintf(str, 20, "Tiempo = %s", buff_tf);
	snprintf(str+40, 20, "Dist = %s", buff_dist);

//	                     $$$$$$$$$$$$$$$$$$$$
	char *l4[] = {"--<< FINALIZADO >>--",
						str,
						str+40,
						"RESET para reiniciar"};

	lcd->write4l(l4, (unsigned)4);
	resultIndex = 0;

	PRINTSA("\n -> COMPLETE");
}



void ent_monitoring() {
	bankLeds->beep();
	bankInputs->startCounting();
	bankInputs->setTimeOut(500);

	dprint = true;

	bankInputs->setDisplayVarIndex(BankAnalogInputs::MASS);
	displayVar();

	PRINT_HEADER();

//	                     $$$$$$$$$$$$$$$$$$$$
	char *l4[] = {"---  Monitoreo  ---",
						"SELECT: Sig. parám.",
						"RESET: terminar",
						"SWITCHES: elegir var"};

	lcd->write4l(l4, 4);
	matrix->setEffects(PA_CENTER, 25, 4000, PA_PRINT, PA_PRINT);

	PRINTSA("\n -> MONITORING");
}

/**
 * Array of on-entering functions.
 * NOTE: ORDER MUST FOLLOW THE ENUM  MainFSM::OnEnterings
 */
void (*mainOnEnterings[])(void) = {
	ent_idle,
	ent_checking,
	ent_condok,
	ent_speeding,
	ent_maxvel,
	ent_landing,
	ent_landed,
	ent_brakingvel,
	ent_braking,
	ent_error,
	ent_complete,
	ent_monitoring,
};

#endif /* MAINONENTERINGS_H_ */
