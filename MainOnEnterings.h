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
	bankLeds->display('P');

	bankKp->checkCommands = true;
	bankKp->start();

	bankButtons->reset();

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
	bankLeds->blink('E');
	bankLeds->relayStart(0);
	bankLeds->beep();

	Serial << "\nST_CHECKING_COND... [Esperando condiciones de inicio: Mv=0, Wv=0, Ph=0, Pf=0, T1&T2<Thot]";
}

void ent_condok() {
	Serial << "\nST_COND_OK... [Esperando Mv > 0]";
	bankLeds->display(0);
	bankLeds->relayOn(0);
	bankLeds->beep();
}

void ent_speeding() {
	bankLeds->beep();
	bankLeds->display(1);
	bankLeds->relayOff(0);
	bankLeds->relayOn(1);

	Serial << "\n\nST_SPEEDING... [Esperando Mv >= "
			<< _DEC(bankInputs->testParms.max_mass_vel) << " rpm]";
}

void ent_maxvel() {
	bankLeds->beep();
	bankLeds->display(2);
	bankLeds->relayOff(1);
	bankLeds->relayOn(2);

	Serial << "\n\nST_MAX_VEL... [Esperando Wv > "
			<< _DEC(bankInputs->testParms.landing_wheel_vel) << " m/s]";
}

void ent_landing() {
	bankLeds->beep();
	bankLeds->display(3);
	bankLeds->relayOff(2);
	bankLeds->relayOn(3);

	Serial << "\n\nST_LANDING... [Esperando PH >= "
			<< _DEC(bankInputs->testParms.ph_threshold) << " bar]";
}

void ent_landed() {
	bankLeds->beep();
	bankLeds->display(4);
	bankLeds->relayOff(3);
	bankLeds->relayOn(4);

	Serial << "\n\nST_LANDED... [Esperando "
			<< _DEC(bankInputs->testParms.brake_mass_vel_min) << " < Mv < "
			<< _DEC(bankInputs->testParms.brake_mass_vel_max) << "]";
}

void ent_brakingvel() {
	bankLeds->beep();
	bankLeds->display(5);
	bankLeds->relayOff(4);
	bankLeds->relayOn(5);

	Serial << "\n\nST_BRAKING_VEL... [Esperando PF >= "
			<< _DEC(bankInputs->testParms.pf_threshold) << " bar]";
}

void ent_braking() {
	bankLeds->beep();
	bankLeds->display(6);
	bankLeds->relayOff(5);
	bankLeds->relayOn(6);
	bankInputs->startCounting();

	Serial << "\n\nST_BRAKING... [Esperando Wv = 0 & Mv = 0]";
}

void ent_error() {
	bankLeds->beep();
	bankLeds->blink('E');
	bankLeds->relayOffAll();
	bankLeds->relayStart(7);

	if (!bankInputs->check(PF_GT_0)) {
		Serial << "\nPf demasiado baja. Test abortado";
	} else if (bankInputs->check(MV_LE_BRAKEV_MAX)) {
		Serial << "\nPérdida de velocidad de Masa. Test abortado";
	}

	Serial << "\n**TEST ERROR** <RESET> para REINICIAR";
}

void ent_complete() {
	bankLeds->beep();
	bankLeds->blink('F');
	bankLeds->relayOffAll();
	bankLeds->relayOn(7);

	printer.print_header("TEST FINALIZADO - RESULTADOS");

	printer.make_item("Tiempo de Frenado t: %s s", tf);
	printer.make_item("Distancia de Frenado d: %s m",
			bankInputs->getDistance());
	printer.make_item("Temperatura final T1: %s °C", bankInputs->getT1());
	printer.make_item("Temperatura final T2: %s °C", bankInputs->getT2());
	printer.print_separator();

	Serial << "\n\n<RESET> para REINICIAR";
}

void ent_monitoring() {
	bankLeds->blink('M');
	// bankLeds->relayStartAll();
	bankLeds->beep();
	bankInputs->startCounting();

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
