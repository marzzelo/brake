/*
 * MainOnEnterings.h
 *
 *  Created on: 30 sep. 2021
 *      Author: valdez
 */

#ifndef MAINONENTERINGS_H_
#define MAINONENTERINGS_H_

void ent_idle() {
	bankLeds->beep();
	bankLeds->relayOffAll();

	bankKp->checkCommands = true;
	bankKp->start();

	bankButtons->reset();

	tm1638->setShiftVel(TM1638::VEL_FAST);
	tm1638->setBrightness(TM1638::BRI_INTENSE);
	tm1638->dispstr("FAdEA-21 bAnco dE FrEno - EnSAYOS EStructurALES");
	tm1638->ledsOff();


	printer.print_header("Brake Test v1.0 release 10/2021", true);
	printer.print_item("Presionar START para comenzar");
	printer.print_item("o ingresar   1     para Configurar ensayo");
	printer.print_item("             2     para Comenzar ensayo");
	printer.print_item("             5     para Monitoreo de variables");
	printer.print_separator();
	Serial << "\n\n==> ";
}


void ent_checking() {
	bankInputs->start();
	bankLeds->relayStart(0);
	bankLeds->beep();

	tm1638->dispstr("Checking");

	Serial << "\nST_CHECKING_COND... [Esperando condiciones de inicio: Mv=0, Wv=0, Ph=0, Pf=0, T1&T2<Thot]";
}

void ent_condok() {
	Serial << "\nST_COND_OK... [Esperando Mv > 0]";
	bankLeds->relayOnly(0);
	bankLeds->beep();
	confirmator->reset(3);

	tm1638->dispstr("Iniciar ");
}

void ent_speeding() {
	bankLeds->beep();
	bankLeds->relayOnly(1);

	bankInputs->setDisplayVarIndex(BankAnalogInputs::MASS);

	tm1638->ledOnly(BankAnalogInputs::MASS);
	tm1638->dispstr("Acelerar");

	Serial << "\n\nST_SPEEDING... [Esperando Mv >= "
			<< _DEC(bankInputs->testParms.max_mass_vel) << " rpm]";
}

void ent_maxvel() {
	bankLeds->beep();
	bankLeds->relayOnly(2);

	displayVar(BankAnalogInputs::WHEEL);

	Serial << "\n\nST_MAX_VEL... [Esperando Wv > "
			<< _DEC(bankInputs->testParms.landing_wheel_vel) << " m/s]";
}

void ent_landing() {
	bankLeds->beep();
	bankLeds->relayOnly(3);

	displayVar(BankAnalogInputs::PH);

	Serial << "\n\nST_LANDING... [Esperando PH >= "
			<< _DEC(bankInputs->testParms.ph_threshold) << " bar]";
}

void ent_landed() {
	bankLeds->beep();
	bankLeds->relayOnly(4);

	displayVar(BankAnalogInputs::MASS);

	Serial << "\n\nST_LANDED... [Esperando "
			<< _DEC(bankInputs->testParms.brake_mass_vel_min) << " < Mv < "
			<< _DEC(bankInputs->testParms.brake_mass_vel_max) << "]";
}

void ent_brakingvel() {
	bankLeds->beep();
	bankLeds->relayOnly(5);

	displayVar(BankAnalogInputs::PF);

	Serial << "\n\nST_BRAKING_VEL... [Esperando PF >= "
			<< _DEC(bankInputs->testParms.pf_threshold) << " bar]";
}

void ent_braking() {
	bankLeds->beep();
	bankLeds->relayOnly(6);
	bankInputs->startCounting();

	displayVar(BankAnalogInputs::WHEEL);

	Serial << "\n\nST_BRAKING... [Esperando Wv = 0 & Mv = 0]";
}

void ent_error() {
	bankLeds->beep();
	bankLeds->relayOffAll();
	bankLeds->relayStart(7);
	tm1638->ledOnly(BankAnalogInputs::ERROR);

	Serial << "\n**TEST ERROR** <RESET> para REINICIAR";
}

void ent_complete() {
//	char buff[48] = {"\0"};
//	char str_tf[6] = {"\0"};
//	char str_dist[6] = {"\0"};

	bankLeds->beep();
	bankLeds->relayOnly(7);
	tm1638->ledsOff();

	bankInputs->stopCounting();

//	double distance = bankInputs->getDistance();
//	double T1 = bankInputs->getT1();
//	double T2 = bankInputs->getT2();

	printer.print_header("TEST FINALIZADO");

	bankInputs->setTimeOut(3000);

//	printer.make_item("Tiempo de Frenado t: %s s", tf);
//	printer.make_item("Distancia de Frenado d: %s m", distance);
//	printer.make_item("Temperatura final T1: %s °C", T1);
//	printer.make_item("Temperatura final T2: %s °C", T2);
//	printer.print_separator();
//
//	Serial << "\n\n<RESET> para REINICIAR";
//
//	dtostrf(tf, 0, 2, str_tf);
//	dtostrf(distance, 0, 2, str_dist);
//	snprintf(buff, 48, "-FINAL- t=%s I d=%s", str_tf, str_dist);
	tm1638->dispstr("-FINAL- ");
}

void ent_monitoring() {
	bankLeds->beep();
	bankInputs->startCounting();

	bankInputs->setDisplayVarIndex(BankAnalogInputs::MASS);
	displayVar();

	Serial << "\n\n\nsep=\t\nt[s]\tMv\tWv\tPh\tPf\tT1\tT2\td\tangle";
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
