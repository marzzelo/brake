/*
 * MenuOnEnterings.h
 *
 *  Created on: 29 sep. 2021
 *      Author: valdez
 */

#ifndef MENUONENTERINGS_H_
#define MENUONENTERINGS_H_

/**
 * Array of on-entering functions.
 * NOTE: ORDER MUST FOLLOW THE ENUM  MenuFSM::OnEnterings
 */
void (*menuOnEnterings[])(void) = {

	// -> IDLE  (index = 0)
	[]() {
		bankKp->asterisk('*');
		bankKp->setCheckCommands(true);
		bankLeds.display('P');
//		Serial << F("\n------\nENTERING ST_MENU_IDLE\n\n");
		brake->SetState(MainFSM::ST_IDLE, false, true);
	},


	// -> MAIN MENU
	[]() {
		bankKp->asterisk('.');
		bankKp->start();
		bankKp->setCheckCommands(true);
		bankKp->ev_key[0] = false;  // clear eventual buffered event;

		Serial << "\n\n\n\n\n\n";

		printer.print_header("CONFIGURACI�N DEL SISTEMA", false);
		printer.print_header("PAR�METROS DE CALIBRACI�N");

		printer.make_item("1. Calibrar Vel Rueda [k = %s]", bankInputs.calFactors.ka_wheel);
		printer.make_item("2. Calibrar Ph [k = %s]", bankInputs.calFactors.ka_ph);
		printer.make_item("3. Calibrar Pf [k = %s]", bankInputs.calFactors.ka_pf);
		printer.make_item("4. Calibrar T1 [k = %s]", bankInputs.calFactors.ka_t1);
		printer.make_item("5. Calibrar T2 [k = %s]", bankInputs.calFactors.ka_t2);
		printer.make_item("6. Calibrar �ngulo a [k = %s]", bankInputs.calFactors.kb_alpha);

		printer.print_header("PAR�METROS DE ENSAYO");

		printer.make_item("7. Vel M�x de Masa: %s", bankInputs.testParms.max_mass_vel);
		printer.make_item("8. Vel L�m Sup de Frenado: %s", bankInputs.testParms.brake_mass_vel_max);
		printer.make_item("9. Vel L�m Inf de Frenado: %s", bankInputs.testParms.brake_mass_vel_min);
		printer.make_item("10.Presi�n Nom de Horquilla: %s", bankInputs.testParms.ph_threshold);
		printer.make_item("11.Presi�n Nom de Freno: %s", bankInputs.testParms.pf_threshold);
		printer.make_item("12.Temperatura T1: %s", bankInputs.testParms.t1_hot);
		printer.make_item("13.Temperatura T2: %s", bankInputs.testParms.t2_hot);

		printer.print_header("0. Salir del Men�");
		Serial << "\n\n==> ";
	},

	// -> WHEELCAL
	[ ] ( ) {
		printer.print_header("CALIBRACI�N VEL DE RUEDA");
		printer.print_item("Girar rueda a velocidad cte. e ingresar valor real:");
		Serial << "\n\nVelocidad [rpm] ==> ";
	},

	// -> PHCAL
	[ ] ( ) {
		printer.print_header("CALIBRACI�N PRESI�N DE HORQUILLA");
		printer.print_item("Aplicar presi�n de Horquilla cte. e ingresar valor real:");
		Serial << "\n\n  Presi�n H [bar] ==> ";
	},

	// -> PFCAL
	[ ] ( ) {
		printer.print_header("CALIBRACI�N PRESI�N DE FRENO");
		printer.print_item("Aplicar presi�n de Freno cte. e ingresar valor real:");
		Serial << "\n\n  Presi�n F [bar] ==> ";
	},

	// -> T1CAL
	[ ] ( ) {
		printer.print_header("CALIBRACI�N TEMP 1");
		printer.print_item("Aplicar temperatura 1 e ingresar valor real:");
		Serial << "\n\n  Temperatura [�C] ==> ";
	},

	// -> T2CAL
	[ ] ( ) {
		printer.print_header("CALIBRACI�N TEMP 2");
		printer.print_item("Aplicar temperatura 2 e ingresar valor real:");
		Serial << "\n\n  Temperatura [�C] ==> ";
	},

	// -> ALPHACAL
	[ ] ( ) {
		printer.print_header("CALIBRACI�N �NGULO");
		printer.print_item("Posicionar en un �ngulo conocido e ingresar el valor:");
		Serial << "\n\n  �ngulo [grados] ==> ";
	},



	// -> MVMAX     (index = 8)
	[ ] ( ) {
		printer.print_separator();
		printer.make_item("VELOCIDAD M�XIMA DE MASA: %s rpm", bankInputs.testParms.max_mass_vel);
		printer.print_separator();
		Serial << "\n\n  Velocidad m�xima [rpm] ==> ";
	},

	// -> VBMAX
	[ ] ( ) {
		printer.print_separator();
		printer.make_item("LIM SUPERIOR VEL DE FRENO: %s rpm", bankInputs.testParms.brake_mass_vel_max);
		printer.print_separator();
		Serial << "\n\n  L�mite superior [rpm] ==> ";
	},

	// -> VBMIN
	[ ] ( ) {
		printer.print_separator();
		printer.make_item("LIM INFERIOR VEL DE FRENO: %s rpm", bankInputs.testParms.brake_mass_vel_min);
		printer.print_separator();
		Serial << "\n\n  L�mite inferior [rpm] ==> ";
	},

	// -> PHLIM
	[ ] ( ) {
		printer.print_separator();
		printer.make_item("PRESI�N DE HORQUILLA NOMINAL: %s bar", bankInputs.testParms.ph_threshold);
		printer.print_separator();
		Serial << "\n\n  Presi�n nominal [bar] ==> ";
	},

	// -> PFLIM
	[ ] ( ) {
		printer.print_separator();
		printer.make_item("PRESI�N DE FRENO NOMINAL: %s bar", bankInputs.testParms.pf_threshold);
		printer.print_separator();
		Serial << "\n\n  Presi�n nominal [bar] ==> ";
	},

	// -> T1HOT
	[ ] ( ) {
		printer.print_separator();
		printer.make_item("TEMPERATURA L�MITE 1: %s �C", bankInputs.testParms.t1_hot);
		printer.print_separator();
		Serial << "\n\n  Temp l�mite_1 [�C] ==> ";
	},

	// -> T2HOT
	[ ] ( ) {
		printer.print_separator();
		printer.make_item("TEMPERATURA L�MITE 2: %s �C", bankInputs.testParms.t2_hot);
		printer.print_separator();
		Serial << "\n\n  Temp l�mite_2 [�C] ==> ";
	},

};




#endif /* MENUONENTERINGS_H_ */
