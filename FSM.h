/*
 * FSM.h
 *
 *  Created on: 27 ago. 2021
 *      Author: mvald
 *      https://www.luisllamas.es/libreria-arduino-statemachine/
 */

#ifndef FSM_H_
#define FSM_H_

/******************************************************************
 * 								F S M
 ******************************************************************/

extern void state_reset();
extern void setTimeOut(unsigned long dt);

// STATE LEAVING

// STATES
enum State {
	ST_IDLE,			// esperando input de inicio de ensayo
	ST_CHECKING_COND,	// esperando condiciones correctas de inicio
	ST_COND_OK,			// esperando inicio de movimiento de volanta
	ST_SPEEDING, 		// volanta acelerando, esperando velocidad máxima
	ST_MAX_VEL, 		// velocidad maxima alcanzada, esperando aterrizaje.
	ST_LANDING, 		// aterrizaje iniciado, esperando presión de frenado
	ST_LANDED,			// presión alcanzada, esperando velocidad de frenado
	ST_BRAKING_VEL, 	// velocidad de freno alcanzada, esperando frenado
	ST_BRAKING,			// frenado iniciado, esperando detención total.
	ST_TEST_COMPLETE,	// test finalizado, esperando reset o timeout para IDLE.
	ST_TEST_ERROR,		// test Abortado, esperando reset o timeout para IDLE.
	ST_COUNT
};

#define N_TRANSITIONS	25

StateMachine FSM(ST_COUNT, N_TRANSITIONS);

///////////////// INPUTS //////////////////
bool btn0_p;
bool btn1_p;
bool btn2_p;
bool btn3_p;

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

bool reset_requested;

bool timeOut;
bool eventsChecked;

//////////////////////////////////////////////////////////////////
// STATES ON_TRANSITION
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// STATES ON_ENTERING
//////////////////////////////////////////////////////////////////
void ent_idle() {
// Flags preset
	buzz->start();
	state_reset();
	keyPadEnabled = true;
	Serial << F("\n\n**************************************");
	Serial << F("\n*  IDLE <Press START para comenzar>  *");
	Serial << F("\n**************************************\n\n");
//	lcd.setCursor(0, 1);
//	lcd.print("IDLE            ");
}

void ent_checking_cond() {
	daq_enabled = true;
	daq_ready = false;

	led[7]->start();  // preset ERROR LAMP
	buzz->start();
	Serial
			<< F(
					"\nST_CHECKING_COND... [Esperando condiciones de inicio: Mv=0, Wv=0, Ph=0, Pf=0]");

//	lcd.setCursor(0, 1);
//	lcd.print("ST_CHECKING_COND");
}

void ent_cond_ok() {
	led[7]->set(LOW);

	Serial << F("\nST_COND_OK... [Esperando Mv > 0]");
	buzz->start();

//	lcd.setCursor(0, 1);
//	lcd.print("ST_COND_OK      ");
}

void ent_speeding() {
	buzz->start();
	led[0]->start();

	Serial << F("\n\nST_SPEEDING... [Esperando Mv >= ") << _DEC(testParms.max_mass_vel)
			<< F(" rpm]");

//	lcd.setCursor(0, 1);
//	lcd.print("ST_SPEEDING    ");
}

void ent_max_vel() {
	buzz->start();

	led[0]->stop();
	led[1]->start();

	Serial << F("\n\nST_MAX_VEL... [Esperando Wv > ") << _DEC(testParms.landing_wheel_vel)
			<< F(" m/s]");
}

void ent_landing() {
	buzz->start();

	led[1]->stop();
	led[2]->start();

	Serial << F("\n\nST_LANDING... [Esperando PH >= ") << _DEC(testParms.ph_threshold)
			<< F(" bar]");
}

void ent_landed() {
	buzz->start();

	led[2]->stop();
	led[3]->start();

	Serial << F("\n\nST_LANDED... [Esperando ") << _DEC(testParms.brake_mass_vel_min)
			<< " < Mv < " << _DEC(testParms.brake_mass_vel_max) << "]";
}

void ent_braking_vel() {
	buzz->start();

	led[3]->stop();
	led[4]->start();
	Serial << F("\n\nST_BRAKING_VEL... [Esperando PF >= ") << _DEC(testParms.pf_threshold)
			<< F(" bar]");
}

void ent_braking() {
	buzz->start();

	led[4]->stop();
	led[5]->start();
	Serial << F("\n\nST_BRAKING... [Esperando Wv = 0 & Mv = 0]");
}

void ent_test_error() {
	buzz->start();

	for (int ledIndex = 0; ledIndex < 8; ++ledIndex) {
		led[ledIndex]->set(HIGH);
	}
	Serial << F("\n**TEST ERROR** <RESET> para REINICIAR");
}

void ent_test_complete() {
	buzz->start();

	for (int ledIndex = 0; ledIndex < 8; ++ledIndex) {
		led[ledIndex]->start();
	}
	Serial << F("\n**TEST FINALIZADO** <RESET> para REINICIAR");
}


//////////////////////////////////////////////////////////////////
// 					T R A N S I T I O N S
//////////////////////////////////////////////////////////////////

void setupFSM() {

	//---------------------------------------------------------------------------
	FSM.AddTransition(ST_IDLE, ST_CHECKING_COND, []() {
		return btn0_p;
	});

	// ---------------------------------------------------------------------------
	FSM.AddTransition(ST_CHECKING_COND, ST_COND_OK, []() {

		bool cond_ok = true;

		if (Mv_gt_0) {
			cond_ok = false;
			Serial << F("\n** DETENER MASA (") << _FLOAT(Mv, 3) << F(" rpm)");
		}

		if (Wv_gt_0) {
			cond_ok = false;
			Serial << F("\n** DETENER RUEDA (") << _FLOAT(wheel_daq_value*calFactors.ka_wheel, 3) << F(" rpm)");
}

		if (Ph_gt_0) {
			cond_ok = false;
			Serial << F("\n** REDUCIR PH (") << _FLOAT(ph_daq_value*calFactors.ka_ph, 3)
					<< F(" bar)");
		}

		if (Pf_gt_0) {
			cond_ok = false;
			Serial << F("\n** REDUCIR PF (") << _FLOAT(pf_daq_value*calFactors.ka_pf, 3)
					<< F(" bar)");
		}

		if (!cond_ok) {
			Serial << F("\n----------------------");
		}

		return cond_ok;
	});

	FSM.AddTransition(ST_CHECKING_COND, ST_IDLE, []() {
		if (timeOut) {
			Serial << F("\nTimeout");
			return true;
		}
		if (reset_requested) {
			reset_requested = false;
			return true;
		}
		return btn3_p;
	});

	//---------------------------------------------------------------------------
	FSM.AddTransition(ST_COND_OK, ST_SPEEDING,
			[]() {
				Serial << F("\nST_COND_OK> mass vel: ") << _FLOAT(Mv, 3)
						<< F(" ** INICIAR GIRO **");
				return (Mv_gt_0);
			});

	FSM.AddTransition(ST_COND_OK, ST_IDLE, []() {
		if (timeOut) {
			Serial << F("\nTimeout");
			return true;
		}
		return btn3_p;
	});

	//---------------------------------------------------------------------------
	FSM.AddTransition(ST_SPEEDING, ST_MAX_VEL,
			[]() {
				Serial << F("\nST_SPEEDING> Mv: ") << _FLOAT(Mv, 3)
						<< F(" ** ACELERAR a 500 rpm **");
				return Mv_gt_MAX;
			});

	FSM.AddTransition(ST_SPEEDING, ST_IDLE, []() {
		if (timeOut) {
			Serial << F("\nTimeout");
			return true;
		}
		return btn3_p;
	});

	//---------------------------------------------------------------------------
	FSM.AddTransition(ST_MAX_VEL, ST_LANDING,
			[]() {
				Serial << F("\nST_MAX_VEL> Mv: ") << _FLOAT(Mv, 3) << F(", Wv: ")
						<< _FLOAT(wheel_daq_value*calFactors.ka_wheel, 3)
						<< F(" ** ATERRIZAR RUEDA ***");
				return Wv_ge_LANDINGv;
			});

	FSM.AddTransition(ST_MAX_VEL, ST_SPEEDING, []() {
		return Mv_le_BRAKEv_max;
	});

	FSM.AddTransition(ST_MAX_VEL, ST_IDLE, []() {
		return btn3_p;
	});

	//---------------------------------------------------------------------------
	FSM.AddTransition(ST_LANDING, ST_LANDED,
			[]() {
				Serial << F("\nLANDING> Mv: ") << _FLOAT(Mv, 3) << F(", Wv: ")
						<< _FLOAT(wheel_daq_value*calFactors.ka_wheel, 3) << F(", PH: ")
						<< _FLOAT(ph_daq_value*calFactors.ka_ph, 3) << F(" ** AUMENTAR Ph a ")
						<< _FLOAT(testParms.ph_threshold, 3) << " ***";
				return Ph_ge_Ph1;
			});

	FSM.AddTransition(ST_LANDING, ST_TEST_ERROR, []() {
		return Mv_le_BRAKEv_max;
	});

	FSM.AddTransition(ST_LANDING, ST_IDLE, []() {
		return btn3_p;
	});

	//---------------------------------------------------------------------------
	FSM.AddTransition(ST_LANDED, ST_BRAKING_VEL,
			[]() {
				if (!Mv_le_BRAKEv_max) {
					Serial << F("\nST_LANDED> Mv: ") << _FLOAT(Mv, 3)
							<< F(" ** DISMINUIR VELOCIDAD **");
				}
				if (!Mv_ge_BRAKEv_min) {
					Serial << F("\nST_LANDED> Mv: ") << _FLOAT(Mv, 3)
							<< F(" ** ACELERAR **");
				}
				return (Mv_le_BRAKEv_max && Mv_ge_BRAKEv_min);
			});

	FSM.AddTransition(ST_LANDED, ST_IDLE, []() {
		return btn3_p;
	});

	//---------------------------------------------------------------------------
	FSM.AddTransition(ST_BRAKING_VEL, ST_BRAKING,
			[]() {
				Serial << F("\nST_BRAKING_VEL> PF: ") << _FLOAT(pf_daq_value*calFactors.ka_pf, 3)
						<< " ***### APLICAR FRENO ###***";

				return Pf_ge_Pf1;
			});

	FSM.AddTransition(ST_BRAKING_VEL, ST_LANDED, []() {

		if (!(Mv_le_BRAKEv_max && Mv_ge_BRAKEv_min)) {
			buzz->start();
			return true;
		}
		return false;
	});

	FSM.AddTransition(ST_BRAKING_VEL, ST_IDLE, []() {
		return btn3_p;
	});

	//---------------------------------------------------------------------------
	FSM.AddTransition(ST_BRAKING, ST_TEST_COMPLETE,
			[]() {
				Serial << F("\nST_BRAKING> Mv: ") << _FLOAT(Mv, 3) << ", Wv: "
						<< _FLOAT(wheel_daq_value*calFactors.ka_wheel, 3)
						<< " ** MANTENER Pf HASTA DETENER ** ";

				return (Mv_eq_0 && Wv_eq_0);
			});

	FSM.AddTransition(ST_BRAKING, ST_IDLE, []() {
		return btn3_p;
	});

	//---------------------------------------------------------------------------
	FSM.AddTransition(ST_TEST_COMPLETE, IDLE, []() {
		return btn3_p;
	});

	//---------------------------------------------------------------------------
	FSM.AddTransition(ST_TEST_ERROR, IDLE, []() {
		return btn3_p;
	});

	//////////////////////////////////////////////////////////////////
	// ON_ENTERING
	//////////////////////////////////////////////////////////////////
	FSM.SetOnEntering(ST_IDLE, ent_idle);
	FSM.SetOnEntering(ST_CHECKING_COND, ent_checking_cond);
	FSM.SetOnEntering(ST_COND_OK, ent_cond_ok);
	FSM.SetOnEntering(ST_SPEEDING, ent_speeding);
	FSM.SetOnEntering(ST_MAX_VEL, ent_max_vel);
	FSM.SetOnEntering(ST_LANDING, ent_landing);
	FSM.SetOnEntering(ST_LANDED, ent_landed);
	FSM.SetOnEntering(ST_BRAKING_VEL, ent_braking_vel);
	FSM.SetOnEntering(ST_BRAKING, ent_braking);
	FSM.SetOnEntering(ST_TEST_ERROR, ent_test_error);
	FSM.SetOnEntering(ST_TEST_COMPLETE, ent_test_complete);

	//////////////////////////////////////////////////////////////////
	// ON_LEAVING
	//////////////////////////////////////////////////////////////////
	FSM.SetOnLeaving(ST_IDLE, []() {
		setTimeOut(120000);
		keyPadEnabled = false;
		Serial << F("\n...leaving ST_IDLE");
	});
	FSM.SetOnLeaving(ST_CHECKING_COND, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_CHECKING_COND");
	});
	FSM.SetOnLeaving(ST_COND_OK, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_COND_OK");
	});
	FSM.SetOnLeaving(ST_SPEEDING, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_SPEEDING");
	});
	FSM.SetOnLeaving(ST_MAX_VEL, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_MAX_VEL");
	});
	FSM.SetOnLeaving(ST_LANDING, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_LANDING");
	});
	FSM.SetOnLeaving(ST_LANDED, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_LANDED");
	});
	FSM.SetOnLeaving(ST_BRAKING_VEL, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_BRAKING_VEL");
	});
	FSM.SetOnLeaving(ST_BRAKING, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_BRAKING");
	});
	FSM.SetOnLeaving(ST_TEST_ERROR, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_TEST_ERROR");
	});
	FSM.SetOnLeaving(ST_TEST_COMPLETE, []() {
		setTimeOut(120000);
		Serial << F("\n...leaving ST_TEST_COMPLETE");
	});
}

#endif /* FSM_H_ */
