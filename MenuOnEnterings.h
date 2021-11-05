/*
 * MenuOnEnterings.h
 *
 *  Created on: 29 sep. 2021
 *      Author: valdez
 */

#ifndef MENUONENTERINGS_H_
#define MENUONENTERINGS_H_

char *lines[][4] = {
	{
		"1# Calibrar Vrueda",
		"2# Calibrar Ph",
		"3# Calibrar Pf",
		"4# Calibrar T1",
	},	{
		"5# Calibrar T2",
		"6# Calibrar ángulo",
		"7# Set Vmasa Máxima",
		"8# Set Vf lím super.",
	}, 	{
		"9# Set Vf lím infer.",
		"10# Set Ph nominal",
		"11# Set Pf nominal",
		"12# T1 inicial máx.",
	}, 	{
		"13# T2 inicial máx.",
		"14# Factory Reset",
		"15# Intensidad Leds",
		"0# Finalizar config.",
	}
};

char *lines2[][4] = {
		{	// #0
		//		"12345678901234567890"
				"CALIBRACION V. RUEDA",
				"Girar rueda a Vr cte",
				"Vrueda [rpm]:",
				"==>",
		},
		{	// #1
				"CALIBR. HORQUILLA",
				"Aplicar presión cte.",
				"Ph [bar]:",
				"==>",
		},
		{	// #2
				"CALIBR. PRES. FRENO",
				"Aplicar presión cte.",
				"Pf [bar]:",
				"==>",
		},
		{	// #3
				"CALIBR. TEMP 1",
				"Aplicar temp. cte.",
				"T1 [Celsius]:",
				"==>",
		},
		{	// #4
				"CALIBR. TEMP 2",
				"Aplicar temp. cte.",
				"T2 [Celsius]:",
				"==>",
		},
		{	// #5
		//		"12345678901234567890"
				"CALIBR. ANGULO",
				"Fijar ángulo de ref.",
				"Angulo [grados]:",
				"==>",
		},
		{	// #6
				"Establecer Vm Máxima",
				"                    ",
				"Vm Máx [rpm]:",
				"==>",
		},
		{	// #7
				"Set Vm Límite sup.",
				"Actual:          rpm",
				"Vm Lím Sup [rpm]:",
				"==>",
		},
		{	// #8
				"Set Vm Límite inf.",
				"Actual:          rpm",
				"Vm Lím Inf [rpm]:",
				"==>",
		},
		{	// #9
				"Set Ph nominal",
				"Actual:          bar",
				"Ph nom [bar]:",
				"==>",
		},
		{	// #10
				"Set Pf nominal",
				"Actual:          bar",
				"Pf nom [bar]:",
				"==>",
		},
		{	// #11
				"Set T1 máx inicio",
				"Actual:          Cel",
				"T1 Máx ini [Cel]:",
				"==>",
		},
		{	// #12
				"Set T2 máx inicio",
				"Actual:          Cel",
				"T2 Máx ini [Cel]:",
				"==>",
		},
		{	// #13
				"INTENSIDAD LEDS",
				"Actual:             ",
				"Nueva [0-15]:",
				"==>",
		},

};

char strval[9] = {0};

/**
 * Array of on-entering functions.
 * NOTE: ORDER MUST FOLLOW THE ENUM  MenuFSM::OnEnterings
 */
void (*menuOnEnterings[])(void) = {

	// -> IDLE  (index = 0)
	[]() {
		bankKp->asterisk('*');
		bankKp->checkCommands = true;
		brake->restart();
		brake->SetState(MainFSM::ST_IDLE, false, true);
		menu->setPage(0);
	},


	// -> MAIN MENU
	[]() {
		lcd->enableCWrite(false);	// disallows keypad echo

		bankKp->asterisk('.');
		bankKp->start();
		bankKp->checkCommands = true;
		bankKp->readKey(0);  // clear eventual buffered event;
		matrix->setEffects(PA_CENTER, 25, 4000, PA_MESH, PA_SCROLL_RIGHT);

		//AddTransition(ST_IDLE, ST_MONITORING, transitions[TR_IDLE_MONITORING]);

//		PRINTS("\n\n\n\n\n\n");
//
//		PRINTS("CONFIGURACIÓN DEL SISTEMA");
//		PRINTS("PARÁMETROS DE CALIBRACIÓN");
//
//		PRINT("\n1. Calibrar Vel Rueda [k = %s rpm/V]", bankInputs->calFactors.ka_wheel);
//		PRINT("\n2. Calibrar Ph [k = %s bar/V]", bankInputs->calFactors.ka_ph);
//		PRINT("\n3. Calibrar Pf [k = %s bar/V]", bankInputs->calFactors.ka_pf);
//		PRINT("\n4. Calibrar T1 [k = %s °C/V]", bankInputs->calFactors.ka_t1);
//		PRINT("\n5. Calibrar T2 [k = %s °C/V]", bankInputs->calFactors.ka_t2);
//		PRINT("\n6. Calibrar Ángulo a [k = %s °/V]", bankInputs->calFactors.kb_alpha);
//
//		PRINTS("PARÁMETROS DE ENSAYO");
//
//		PRINT("\n7. Vel Máx de Masa: %s rpm", bankInputs->testParms.max_mass_vel);
//		PRINT("\n8. Vel Lím Sup de Frenado: %s m/s", bankInputs->testParms.brake_mass_vel_max);
//		PRINT("\n9. Vel Lím Inf de Frenado: %s m/s", bankInputs->testParms.brake_mass_vel_min);
//		PRINT("\n10.Presión Nom de Horquilla: %s bar", bankInputs->testParms.ph_threshold);
//		PRINT("\n11.Presión Nom de Freno: %s bar", bankInputs->testParms.pf_threshold);
//		PRINT("\n12.Temperatura T1: %s °C", bankInputs->testParms.t1_hot);
//		PRINT("\n13.Temperatura T2: %s °C", bankInputs->testParms.t2_hot);
//		PRINTS("14.Factory Reset");
//
//		PRINTS("0. Salir del Menú");
//		PRINTS("\n\n==> ");

		lcd->write4l(lines[menu->getPage()]);

	},

	// -> WHEELCAL
	[ ] ( ) {
//		PRINTS("CALIBRACIÓN VEL DE RUEDA");
//		PRINTS("Girar rueda a velocidad cte. e ingresar valor real:");
//		PRINTS("\n\nVelocidad [rpm] ==> ");
		lcd->write4l(lines2[0], 4);
		lcd->enableCWrite();  // la lectura se realiza en updateParam() @ MenuTransitions.h
	},

	// -> PHCAL
	[ ] ( ) {
//		PRINTS("CALIBRACIÓN PRESIÓN DE HORQUILLA");
//		PRINTS("Aplicar presión de Horquilla cte. e ingresar valor real:");
//		PRINTS("\n\n  Presión H [bar] ==> ");
		lcd->write4l(lines2[1], 4);
		lcd->enableCWrite();  // la lectura del keypad se realiza en updateParam() @ MenuTransitions.h
	},

	// -> PFCAL
	[ ] ( ) {
//		PRINTS("CALIBRACIÓN PRESIÓN DE FRENO");
//		PRINTS("Aplicar presión de Freno cte. e ingresar valor real:");
//		PRINTS("\n\n  Presión F [bar] ==> ");
		lcd->write4l(lines2[2], 4);
		lcd->enableCWrite();  // la lectura del keypad se realiza en updateParam() @ MenuTransitions.h
	},

	// -> T1CAL
	[ ] ( ) {
//		PRINTS("CALIBRACIÓN TEMP 1");
//		PRINTS("Aplicar temperatura 1 e ingresar valor real:");
//		PRINTS("\n\n  Temperatura1 [°C] ==> ");
		lcd->write4l(lines2[3], 4);
		lcd->enableCWrite();  // la lectura del keypad se realiza en updateParam() @ MenuTransitions.h
	},

	// -> T2CAL
	[ ] ( ) {
//		PRINTS("CALIBRACIÓN TEMP 2");
//		PRINTS("Aplicar temperatura 2 e ingresar valor real:");
//		PRINTS("\n\n  Temperatura2 [°C] ==> ");
		lcd->write4l(lines2[4], 4);
		lcd->enableCWrite();  // la lectura del keypad se realiza en updateParam() @ MenuTransitions.h
	},

	// -> ALPHACAL
	[ ] ( ) {
//		PRINTS("CALIBRACIÓN ÁNGULO");
//		PRINTS("Posicionar en un ángulo conocido e ingresar el valor:");
//		PRINTS("\n\n  Ángulo [grados] ==> ");
		lcd->write4l(lines2[5], 4);
		lcd->enableCWrite();  // la lectura del keypad se realiza en updateParam() @ MenuTransitions.h
	},



	// -> MVMAX     (index = 8)
	[ ] ( ) {
//
//		PRINT("\nVELOCIDAD MÁXIMA DE MASA: %s rpm", bankInputs->testParms.max_mass_vel);
//
//		PRINTS("\n\n  Velocidad máxima [rpm] ==> ");

		dtostrf(bankInputs->testParms.max_mass_vel, 8, 1, strval);
		sprintf((char *)lines2[6][1], "Actual: %s rpm", strval);
		lcd->write4l(lines2[6], 4);
		lcd->enableCWrite();  // la lectura del keypad se realiza en updateParam() @ MenuTransitions.h
	},

	// -> VBMAX
	[ ] ( ) {
//
//		PRINT("\nLIM SUPERIOR VEL DE FRENO: %s rpm", bankInputs->testParms.brake_mass_vel_max);
//
//		PRINTS("\n\n  Límite superior [rpm] ==> ");
		dtostrf(bankInputs->testParms.brake_mass_vel_max, 8, 1, strval);
		sprintf(lines2[7][1], "Actual: %s rpm", strval);
		lcd->write4l(lines2[7], 4);
		lcd->enableCWrite();  // la lectura del keypad se realiza en updateParam() @ MenuTransitions.h
	},

	// -> VBMIN
	[ ] ( ) {
//
//		PRINT("\nLIM INFERIOR VEL DE FRENO: %s rpm", bankInputs->testParms.brake_mass_vel_min);
//
//		PRINTS("\n\n  Límite inferior [rpm] ==> ");
		dtostrf(bankInputs->testParms.brake_mass_vel_min, 8, 1, strval);
		sprintf(lines2[8][1], "Actual: %s rpm", strval);
		lcd->write4l(lines2[8], 4);
		lcd->enableCWrite();  // la lectura del keypad se realiza en updateParam() @ MenuTransitions.h
	},

	// -> PHLIM
	[ ] ( ) {
//
//		PRINT("\nPRESIÓN DE HORQUILLA NOMINAL: %s bar", bankInputs->testParms.ph_threshold);
//
//		PRINTS("\n\n  Presión nominal [bar] ==> ");
		dtostrf(bankInputs->testParms.ph_threshold, 8, 1, strval);
		sprintf(lines2[9][1], "Actual: %s bar", strval);
		lcd->write4l(lines2[9], 4);
		lcd->enableCWrite();  // la lectura del keypad se realiza en updateParam() @ MenuTransitions.h
	},

	// -> PFLIM
	[ ] ( ) {

//		PRINT("\nPRESIÓN DE FRENO NOMINAL: %s bar", bankInputs->testParms.pf_threshold);
//
//		PRINTS("\n\n  Presión nominal [bar] ==> ");
		dtostrf(bankInputs->testParms.pf_threshold, 8, 1, strval);
		sprintf(lines2[10][1], "Actual: %s bar", strval);
		lcd->write4l(lines2[10], 4);
		lcd->enableCWrite();  // la lectura del keypad se realiza en updateParam() @ MenuTransitions.h
	},

	// -> T1HOT
	[ ] ( ) {
//		PRINT("\nTEMPERATURA LÍMITE 1: %s °C", bankInputs->testParms.t1_hot);
//		PRINTS("\n\n  Temp límite_1 [°C] ==> ");
		dtostrf(bankInputs->testParms.t1_hot, 8, 1, strval);
		sprintf(lines2[11][1], "Actual: %s Cel", strval);
		lcd->write4l(lines2[11], 4);
		lcd->enableCWrite();  // la lectura del keypad se realiza en updateParam() @ MenuTransitions.h
	},

	// -> T2HOT
	[ ] ( ) {
//		PRINT("\nTEMPERATURA LÍMITE 2: %s °C", bankInputs->testParms.t2_hot);
//		PRINTS("\n\n  Temp límite_2 [°C] ==> ");
		dtostrf(bankInputs->testParms.t2_hot, 8, 1, strval);
		sprintf(lines2[12][1], "Actual: %s Cel", strval);
		lcd->write4l(lines2[12], 4);
		lcd->enableCWrite();  // la lectura del keypad se realiza en updateParam() @ MenuTransitions.h
	},

	// -> LED_INTENSITY
	[ ] ( ) {
//		dtostrf(bankInputs->testParms.t2_hot, 8, 1, strval);
		sprintf(lines2[13][1], "Actual: %d", matrix->getIntensity());
		lcd->write4l(lines2[13], 4);
		lcd->enableCWrite();  // la lectura del keypad se realiza en updateParam() @ MenuTransitions.h
	},

};




#endif /* MENUONENTERINGS_H_ */
