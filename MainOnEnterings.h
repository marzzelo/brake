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
	tm1638->dispstr("FAdEA-21 bAnco dE FrEno - EnSAYOS EStructurALES");
	tm1638->ledsOff();

	matrix->setMessage("v1.0.0 Beta");

	printer.print_header("Brake Test v1.0 release 10/27/2021", true);
	printer.print_item("Presionar START para comenzar");
	printer.print_item("o ingresar   1     para Configurar ensayo");
	printer.print_item("             2     para Comenzar ensayo");
	printer.print_item("             5     para Monitoreo de variables");
	printer.print_separator();
	Serial << "\n\n==> ";

	const char *l4[] = {"FAdeA BANCO DE FRENO",
			"1# Configuracion",
			"2# Comenzar Test",
			"5# Monitoreo Senales"};

	lcd->write4l(l4);
}


void ent_checking() {
	bankInputs->start();
	bankLeds->relayStart(0);
	bankLeds->beep();

	tm1638->dispstr("Checking");

	PRINTS("\nST_CHECKING_COND... [Esperando condiciones de inicio: Mv=0, Wv=0, Ph=0, Pf=0, T1&T2<Thot]");
	lcd->clear();

	const char *l4[] = {"Poner todo a cero   ",
			            "Mv, Wv, Ph, T1, T2  ",
						"",
				        "RESET cancela ensayo"};

	lcd->write4l(l4);
}


void ent_condok() {
	PRINTS("\nST_COND_OK... [Esperando Mv > 0]");
	bankLeds->relayOnly(0);
	bankLeds->beep();
//	confirmator->reset(3);

	PRINT_HEADER();
	dprint = true;

	tm1638->dispstr("Iniciar ");
	matrix->setMessage("Iniciar Giro");

//	      		         $$$$$$$$$$$$$$$$$$$$
	const char *l4[] = {"",
						"Iniciar Giro de Masa"};

	lcd->write4l(l4, 2);
}

void ent_speeding() {
	bankLeds->beep();
	bankLeds->relayOnly(1);

	bankInputs->setDisplayVarIndex(BankAnalogInputs::MASS);

	tm1638->ledOnly(BankAnalogInputs::MASS);
	tm1638->dispstr("Acelerar");

//	                     $$$$$$$$$$$$$$$$$$$$
	const char *l4[] = {"",
				        "Acelerar hasta Vmax"};

	lcd->write4l(l4, 2);
}

void ent_maxvel() {
	bankLeds->beep();
	bankLeds->relayOnly(2);

	displayVar(BankAnalogInputs::WHEEL);
	PRINT("\n\nST_MAX_VEL... [Esperando Wv > ", bankInputs->testParms.landing_wheel_vel);
	PRINTS(" m/s]");

//	                     $$$$$$$$$$$$$$$$$$$$
	const char *l4[] = {"",
						"Esperando aterrizaje"};

	lcd->write4l(l4, 2);
}

void ent_landing() {
	bankLeds->beep();
	bankLeds->relayOnly(3);

	displayVar(BankAnalogInputs::PH);

	PRINT("\n\nST_LANDING... [Esperando PH >= ", bankInputs->testParms.ph_threshold);
	PRINTS(" bar]");

//	                     $$$$$$$$$$$$$$$$$$$$
	const char *l4[] = {"",
						"Esperando Ph max"};

	lcd->write4l(l4, 2);

}

void ent_landed() {
	bankLeds->beep();
	bankLeds->relayOnly(4);

	displayVar(BankAnalogInputs::MASS);

	PRINT("\n\nST_LANDED... [Esperando ", bankInputs->testParms.brake_mass_vel_min);
	PRINT(" < Mv < ", bankInputs->testParms.brake_mass_vel_max);
	PRINTS("]");

//	                     $$$$$$$$$$$$$$$$$$$$
	const char *l4[] = {"",
						"Esperando Velocidad",
	                    "de frenado"};

	lcd->write4l(l4, 3);
}

void ent_brakingvel() {
	bankLeds->beep();
	bankLeds->relayOnly(5);

	displayVar(BankAnalogInputs::PF);

	PRINT("\n\nST_BRAKING_VEL... [Esperando PF >= ", bankInputs->testParms.pf_threshold);
	PRINTS(" bar]");

//	                     $$$$$$$$$$$$$$$$$$$$
	const char *l4[] = {"",
						"-- APLICAR FRENO --"};

	lcd->write4l(l4, 2);
}

void ent_braking() {
	bankLeds->beep();
	bankLeds->relayOnly(6);
	bankInputs->startCounting();

	displayVar(BankAnalogInputs::WHEEL);
	matrix->text("-- MANTENER Pf --");
	PRINTS("\n\nST_BRAKING... [Esperando Wv = 0 & Mv = 0]");

//	                     $$$$$$$$$$$$$$$$$$$$
	const char *l4[] = {"",
						"-- << FRENANDO >> --",
						"Esperando detencion"};

	lcd->write4l(l4, 3);
}

void ent_error() {
	bankLeds->beep();
	bankLeds->relayOffAll();
	bankLeds->relayStart(7);
	tm1638->ledsOff();

	PRINTS("\n**TEST ERROR** <RESET> para REINICIAR");

//	                     $$$$$$$$$$$$$$$$$$$$
	const char *l4[] = {"",
						"ERROR DURANTE ENSAYO"};

	lcd->write4l(l4, 2);
}

void ent_complete() {

	bankLeds->beep();
	bankLeds->relayOnly(7);
	tm1638->ledsOff();

	bankInputs->stopCounting();

#if (DEBUG)
	printer.print_header("TEST FINALIZADO");
#endif

	bankInputs->setTimeOut(3000);

	double dist = bankInputs->getDistance();

	tm1638->dispstr("-FINAL- ");
	matrix->text("--- FINAL ---");

	char buff_tf[10] = { 0 };
	char buff_dist[10] = { 0 };

	dtostrf(tf, 0, 1, buff_tf);
	dtostrf(dist, 0, 1, buff_dist);

	snprintf(str, 20, "Tiempo = %s", buff_tf);
	snprintf(str+40, 20, "Dist = %s", buff_dist);

//	                     $$$$$$$$$$$$$$$$$$$$
	const char *l4[] = {"--<< FINALIZADO >>--",
						str,
						str+40,
						"RESET para reiniciar"};

	lcd->write4l(l4, 4);
}

void ent_monitoring() {
	bankLeds->beep();
	bankInputs->startCounting();
	bankInputs->setTimeOut(500);

	dprint = true;

	//matrix->setEffects(PA_CENTER, 35, 2000, PA_SCROLL_DOWN, PA_SCROLL_UP);

	bankInputs->setDisplayVarIndex(BankAnalogInputs::MASS);
	displayVar();

	PRINT_HEADER();

//	                     $$$$$$$$$$$$$$$$$$$$
	const char *l4[] = {"---  Monitoreo  ---",
						"SELECT: Sig. param.",
						"RESET: terminar",
						"BOTONES: elegir var"};

	lcd->write4l(l4, 4);
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
