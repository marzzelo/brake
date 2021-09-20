/* ===============================================================================================
 * 					       BANCO DE FRENO - FIRMWARE ARDUINO MEGA 2560
 * ===============================================================================================*/
/**
 * # OPERACIÓN
 *
 * ## DISPLAY: P (IDLE)
 * - Presionar START para comenzar el ensayo
 *
 * ## DISPLAY: E (ST_CHECKING_COND)
 * - Verificar las siguientes condiciones:
 *   - Velocidad de masa nula
 *   - Velocidad de rueda nula
 *   - Presiones de horquilla y freno nulas
 *   - Temperaturas por debajo del límite caliente
 *
 * ## DISPLAY: 0 (ST_COND_OK)
 * - Arrancar masa
 *
 * ## DISPLAY: 1 (ST_SPEEDING)
 * - Acelerar masa hasta alcanzar Vmax
 *
 * ## DISPLAY: 2 (ST_MAX_VEL)
 * - Aterrizar rueda
 *
 * ## DISPLAY: 3 (ST_LANDING)
 * - Aumentar presión de horquilla hasta la presión nominal del ensayo (simula peso del avión)
 * - ERROR: si la velocidad de masa disminuye por debajo de la velocidad de frenado antes de aplicar el freno.
 *
 * ## DISPLAY: 4 (ST_LANDED)
 * - Disminuir o Aumentar velocidad de masa hasta ingresar al rango de frenado MvBmin < Mv < MvBmax
 *
 * ## DISPLAY: 5 (ST_BRAKING_VEL)
 * - Aplicar freno
 * - REGRESO A [4]: si la velocidad de masa sale del rango de frenado antes de que la presión de freno alcance la presión nominal
 *
 * ## DISPLAY: 6 (ST_BRAKING)
 * - Esperar la detención total de la masa y la rueda
 * - ERROR: si la presión de freno disminuye demasiado antes de la detención total de la rueda y masa
 *
 * ## DISPLAY: F (ST_TEST_COMPLETE)
 * - Fin del ensayo, presionar Start para reiniciar.
 *
 *************************************************************************************************/
#define fadea
#define debug

#include "Rx.h"
#include "CmdSplitter.h"
#include "Streaming.h"
#include "TimerOne.h"
#include "KeyPadRx.h"
#include "MyTasker.h"
#include "StateMachineLib.h"
#include "LiquidCrystal.h"

#include "BankButtons.h"
#include "BankLeds.h"
#include "Bank.h"
#include "BankAnalogInputs.h"
#include "BankKeyPad.h"

void onBtn0();
void onBtn1();
void onBtn2();
void onBtn3();

Bank bank;

BankButtons bankButtons(onBtn0, onBtn1, onBtn2, onBtn3);

BankLeds bankLeds;

BankAnalogInputs bankInputs(500, 5);

BankKeyPad bankKp;

KeyPadRX *keyPadRx = bankKp.getKeyPadRX();

/*********************************
 * TIMER ACCUMULATORS
 *********************************/
#define T250MS 	25
#define T500MS 	50
#define T1S 	100

#define ZERO_PH				5
#define ZERO_PF				5
#define ZERO_MASS_VEL		5
#define ZERO_WHEEL_VEL		5

/*********************************
 * RX - PC KEYBOARD OR PROCESSING
 *********************************/
char str[40];  // comandos desde keyboard o Processing
Rx *keyboard = new Rx(str, 40);
volatile bool dataReady = false;

bool checkCommands = true;
volatile bool keypad_data_ready;

/***************************
 * TIMERS
 ***************************/
char _t250ms, _t500ms, _t1s;
unsigned long _t0, _dt;

/***************************
 * TASKER
 ***************************/
MyTasker *tasker;

/***************************
 * COMMAND TABLE
 ***************************/

// @formatter:off
char const *cmdTable[] = {
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15",
		"*0", "*1", "*2", "*3", "*4", "*5", "*6", "*7", "*8", "*9" };

enum _cmdEnum {
	KEY0, KEY1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9, KEY10, KEY11, KEY12, KEY13, KEY14, KEY15,
	CMD0, CMD1, CMD2, CMD3, CMD4, CMD5, CMD6, CMD7, CMD8, CMD9,
	_END
} cmdEnum;                       // @formatter:on
//
//

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

uint16_t Mv;
bool cmd_menu_sent;
bool ev_key[16] = { false };
bool ev_cmd[10] = { false };

/*************************************************************
 * 							F S M
 *************************************************************/
#include "FSM.h"

#include "MENU.h"

/*************************************************************
 * 							SETUP
 *************************************************************/
void setup() {

	Serial.begin(115200);
	while (!Serial) {
	}

	tasker = new MyTasker(Task1ms, Task10ms, Task100ms, NULL);

	// Keyboard handlers
	keyboard->setDataReadyHandler(dataReadyHandler);
	keyboard->setKeyPressedHandler(keyPressedHandler);

	// Timer
	Timer1.stop();
	Timer1.initialize(1000);
	Timer1.attachInterrupt(T1_ISR);
	Timer1.start();

	// Counters
	_t250ms = T250MS;
	_t500ms = T500MS;
	_t1s = T1S;

	bank.setup();	// Establece pinModes para relays. Apaga relays y led built-in

	setupFSM();		// Carga transiciones, enterings & leavings de states de la máquina principal.
	setupMENU();	// Carga transiciones, enterings & leavings de states de la máquina de Menús.

	FSM.SetState(ST_IDLE, false, true);
	MENU.SetState(ST_MENU_IDLE, false, false);

//	lcd.begin(16, 2);
//
//	lcd.setCursor(0, 0);
//	lcd.print(F("TEST 123"));

//	bank.eePreset();

	bank.loadSettings();	// Carga datos de calibración y parámetros de ensayo desde EEprom

}

//////////////////////////////////////////////////////////////////
// EVENT CHECKING
//////////////////////////////////////////////////////////////////
/**
 * Se ejecuta antes de llamar a checkEvents de la FSM principal.
 * A partir de las lecturas de las entradas analógicas habilitadas, calcula varias variables
 * booleanas que serán utilizadas en la FSM principal para generar las transiciones.
 */
void checkEvents() {

	for (int btnIndex = 0; btnIndex < 4; ++btnIndex)
		btn_pressed[btnIndex] = bankButtons.read(btnIndex);  // read() clears pressed state.

	// Velocidad de masa - available() es comprobado previamente en loop: if(bankInputs.ready())
	Mv = bankInputs.getRpm();
	Mv_eq_0 = Mv <= ZERO_MASS_VEL;
	Mv_gt_0 = Mv > ZERO_MASS_VEL;
	Mv_gt_MAX = Mv >= bank.testParms.max_mass_vel;
	Mv_le_BRAKEv_max = Mv <= bank.testParms.brake_mass_vel_max;
	Mv_ge_BRAKEv_min = Mv >= bank.testParms.brake_mass_vel_min;

	// Velocidad de rueda
	Wv_eq_0 = bankInputs.wheel_daq_value <= ZERO_WHEEL_VEL;
	Wv_ge_LANDINGv = bankInputs.wheel_daq_value	>= bank.testParms.landing_wheel_vel / bank.calFactors.ka_wheel;
	Wv_gt_0 = bankInputs.wheel_daq_value > ZERO_WHEEL_VEL;

	// Presiones
	Ph_gt_0 = bankInputs.ph_daq_value >= ZERO_PH;
	Ph_ge_Ph1 = bankInputs.ph_daq_value >= bank.testParms.ph_threshold / bank.calFactors.ka_ph;
	Pf_gt_0 = bankInputs.pf_daq_value > ZERO_PF;
	Pf_ge_Pf1 = bankInputs.pf_daq_value >= bank.testParms.pf_threshold / bank.calFactors.ka_pf;

	// Temperaturas
	T1_ge_Thot = bankInputs.t1_daq_value >= bank.testParms.t1_hot / bank.calFactors.ka_t1;
	T2_ge_Thot = bankInputs.t2_daq_value >= bank.testParms.t2_hot / bank.calFactors.ka_t2;

	// Tiempo
	timeOut = (millis() - _t0) > _dt;

	// Activar DAQ e inicializar FreqCounter
	bankInputs.start();
}

void setTimeOut(unsigned long dt) {
	_t0 = millis();
	_dt = dt;
}


/*********************************************************************************
 * 										LOOP
 *********************************************************************************/
void loop() {

	checkKeyPad();	// Check comandos por teclado - comenzados con "*"  (ej.: "*5#")

	MENU.Update();	// chequea FSM de Menú principal

	if (bankInputs.ready()) {	// Datos digitalizados y conteo de pulsos disponible ?
		checkEvents();			// Actualizar booleans

		FSM.Update();			// Generar transiciones (si corresponde, según checkEvents()
	}

}

/**
 * KEYPAD COMMANDS
 * Verifica si hay un comando terminado con # en el buffer del keyPad.
 * En caso afirmativo, setea las flags ev_key[n] o ev_cmd[n].
 * Las flags deberán ser reseteadas cuando sean servidas.
 */
void checkKeyPad() {
	if (!checkCommands)
		return;

	if (keyPadRx->dataReady()) {
		bankLeds.beep(100, 1, 1);

		int cc = getCmd(bankKp.getBuff(), cmdTable);

		if (cc < 16) {
			Serial << "\nkey: " << cc;
			ev_key[cc] = true;
		} else if (cc < _END) {
			Serial << "\nevt: " << (cc - 16);
			ev_cmd[cc - 16] = true;
		} else {
			Serial << "\ncomando inválido: " << cc;
		}

		keyPadRx->start();

	}
}

/***************************
 * Keyboard COMMAND available
 ***************************
 if (keyboard->dataReady()) {

 int cc = getCmd(str, cmdTable);

 switch (cc) {
 case READ:
 //Serial << "Reading...\n\n";
 daqEnabled = true;
 break;
 case WRITE:
 Serial << "Writing...";
 break;

 case START:
 Serial << "Starting...";
 break;

 case STOP:
 Serial << "Stopping...";
 break;

 default:
 Serial << "Unknown command";
 break;
 }

 keyboard->start();
 }

 */

/***************************
 * Obtain command offset (keyboard)
 ***************************/
int getCmd(char *strCmd, const char *table[]) {
	int p;

	for (p = 0; p <= _END; p++) {
		if (strcmp(strCmd, table[p]) == 0) {
			break;
		}
	}
	return p;
}

/******************************************
 * BUTTONS Callbacks
 ******************************************/
void onBtn0() {
	bankButtons.setPressed(0);
}

void onBtn1() {
	bankButtons.setPressed(1);
}

void onBtn2() {
	bankButtons.setPressed(2);
}

void onBtn3() {
	bankButtons.setPressed(3);
}

/******************************************
 * PC KEYBOARD Callbacks
 ******************************************/
void dataReadyHandler() {

}

void keyPressedHandler(char key) {

}

/******************************************
 * KEY PAD Callbacks
 ******************************************/
void keyPadDataReadyHandler() {
	keypad_data_ready = true;
}

void keyPadPressedHandler(char key) {
	Serial << ((key == '*') ? keyPadRx->getAsterisk() : key);
	bankLeds.display(key);
	bankLeds.beep(20, 1, 1);
}

/******************************************
 * TASKER Callbacks
 ******************************************/
void Task1ms() {
	bankButtons.update();
	bankLeds.update();
}

void Task10ms() {

}

void Task100ms() {
	bankInputs.update();
}

/******************************************
 * TIMER 1  ISR
 ******************************************/
void T1_ISR(void) {
//keyboard->update();
	keyPadRx->update();
	tasker->update();
}

/******************************************
 * ROUTINES
 ******************************************/
void state_reset() {
	bankInputs.start();
	eventsChecked = false;

	keypad_data_ready = false;


	for (int btnIndex = 0; btnIndex < 4; ++btnIndex)
		btn_pressed[btnIndex] = false;

	bankLeds.relayOffAll();

//keyboard->start();
	keyPadRx->start();
}

