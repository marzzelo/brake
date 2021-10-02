//
//
//				  ____    _    _   _  ____ ___    ____  _____   _____ ____  _____ _   _  ___
//				 | __ )  / \  | \ | |/ ___/ _ \  |  _ \| ____| |  ___|  _ \| ____| \ | |/ _ \
//				 |  _ \ / _ \ |  \| | |  | | | | | | | |  _|   | |_  | |_) |  _| |  \| | | | |
//				 | |_) / ___ \| |\  | |__| |_| | | |_| | |___  |  _| |  _ <| |___| |\  | |_| |
//				 |____/_/   \_\_| \_|\____\___/  |____/|_____| |_|   |_| \_\_____|_| \_|\___/
//
//																	   _   _   _   _
//						  /\  ._ _|     o ._   _    |\/|  _   _   _.    ) |_  |_  / \
//						 /--\ | (_| |_| | | | (_)   |  | (/_ (_| (_|   /_  _) |_) \_/
//															  _|

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
#include "BankAnalogInputs.h"
#include "BankKeyPad.h"
#include "MenuFSM.h"
#include "MainFSM.h"
#include "Printer.h"

bool cmd_menu_sent;
bool ev_key[16] = { false };
bool ev_cmd[10] = { false };

bool btn_pressed[4];

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

bool T1_ge_Thot;
bool T2_ge_Thot;

bool reset_requested;
bool timeOut;
bool eventsChecked;


//	  _
//	 /  |  _.  _  _  _   _
//	 \_ | (_| _> _> (/_ _>
//
BankButtons bankButtons(onBtn0, onBtn1, onBtn2, onBtn3);
BankLeds bankLeds;
BankAnalogInputs bankInputs(checkAngle, 500, 5);
BankKeyPad bankKp;
KeyPadRX *keyPadRx = bankKp.getKeyPadRX();
MainFSM *brake = new MainFSM(mainTransitions, mainOnEnterings, mainOnLeavings);
MenuFSM *menu = new MenuFSM(menuTransitions, menuOnEnterings, menuOnLeavings);


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
//char str[40];  // comandos desde keyboard o Processing
//Rx *keyboard = new Rx(str, 40);
//volatile bool dataReady = false;

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
} cmdEnum;                         // @formatter:on
//
//
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

Printer printer(60);



//	  __
//	 (_   _ _|_     ._
//	 __) (/_ |_ |_| |_)
//					|
void setup() {

	Serial.begin(115200);
	while (!Serial) {
	}

	tasker = new MyTasker(Task1ms, Task10ms, Task100ms, NULL);

	// Keyboard handlers
//	keyboard->setDataReadyHandler(dataReadyHandler);
//	keyboard->setKeyPressedHandler(keyPressedHandler);

	// Timer
	Timer1.stop();
	Timer1.initialize(1000);
	Timer1.attachInterrupt(T1_ISR);
	Timer1.start();

	// Counters
	_t250ms = T250MS;
	_t500ms = T500MS;
	_t1s = T1S;

	// State Machines
	brake->SetState(MainFSM::ST_IDLE, false, true);
	menu->SetState(MenuFSM::ST_MENU_IDLE, false, false);


//	lcd.begin(16, 2);
//
//	lcd.setCursor(0, 0);
//	lcd.print(F("TEST 123"));

//	bank.eePreset();

	bankInputs.loadSettings();// Carga datos de calibraci�n y par�metros de ensayo desde EEprom
	bankInputs.start();
}




//
//	 |   _   _  ._
//	 |_ (_) (_) |_)
//				|
void loop() {

	checkKeyPad();// Check comandos por teclado - comenzados con "*"  (ej.: "*5#")

	menu->Update();	// chequea FSM de Men� principal

	if (bankInputs.ready()) {// Datos digitalizados y conteo de pulsos disponible ?
		checkEvents();			// Actualizar booleans

//		FSM.Update();			// Generar transiciones (si corresponde, seg�n checkEvents()
		brake->Update();
	}

}



//////////////////////////////////////////////////////////////////
// EVENT CHECKING
//////////////////////////////////////////////////////////////////
/**
 * Se ejecuta antes de llamar a checkEvents de la FSM principal.
 * A partir de las lecturas de las entradas anal�gicas habilitadas, calcula varias variables
 * booleanas que ser�n utilizadas en la FSM principal para generar las transiciones.
 */
void checkEvents() {

	for (int btnIndex = 0; btnIndex < 4; ++btnIndex)
		btn_pressed[btnIndex] = bankButtons.read(btnIndex); // read() clears pressed state.

	if (btn_pressed[1]) {
		bankInputs.nextDisplayVar();
	}

	// Velocidad de masa - available() es comprobado previamente en loop: if(bankInputs.ready())
	uint16_t Mv = bankInputs.getRpm();
	Mv_eq_0 = Mv <= ZERO_MASS_VEL;
	Mv_gt_0 = Mv > ZERO_MASS_VEL;
	Mv_gt_MAX = Mv >= bankInputs.testParms.max_mass_vel;
	Mv_le_BRAKEv_max = Mv <= bankInputs.testParms.brake_mass_vel_max;
	Mv_ge_BRAKEv_min = Mv >= bankInputs.testParms.brake_mass_vel_min;

	// Velocidad de rueda
	Wv_eq_0 = bankInputs.wheel_daq_value <= ZERO_WHEEL_VEL;
	Wv_ge_LANDINGv = bankInputs.wheel_daq_value
			>= bankInputs.testParms.landing_wheel_vel
					/ bankInputs.calFactors.ka_wheel;
	Wv_gt_0 = bankInputs.wheel_daq_value > ZERO_WHEEL_VEL;

	// Presiones
	Ph_gt_0 = bankInputs.ph_daq_value >= ZERO_PH;
	Ph_ge_Ph1 = bankInputs.ph_daq_value
			>= bankInputs.testParms.ph_threshold / bankInputs.calFactors.ka_ph;
	Pf_gt_0 = bankInputs.pf_daq_value > ZERO_PF;
	Pf_ge_Pf1 = bankInputs.pf_daq_value
			>= bankInputs.testParms.pf_threshold / bankInputs.calFactors.ka_pf;

	// Temperaturas
	T1_ge_Thot = bankInputs.t1_daq_value
			>= bankInputs.testParms.t1_hot / bankInputs.calFactors.ka_t1;
	T2_ge_Thot = bankInputs.t2_daq_value
			>= bankInputs.testParms.t2_hot / bankInputs.calFactors.ka_t2;

	// Tiempo
	timeOut = (millis() - _t0) > _dt;

	// Activar DAQ e inicializar FreqCounter
	bankInputs.start();
}

void setTimeOut(unsigned long dt) {
	_t0 = millis();
	_dt = dt;
}

/**
 * KEYPAD COMMANDS
 * Verifica si hay un comando terminado con # en el buffer del keyPad.
 * En caso afirmativo, setea las flags ev_key[n] o ev_cmd[n].
 * Las flags deber�n ser reseteadas cuando sean servidas.
 */
void checkKeyPad() {
	if (!checkCommands)
		return;

	if (keyPadRx->dataReady()) {
		bankLeds.beep(100, 1, 1);

		int cc = getCmd(bankKp.getBuff(), cmdTable);

		if (cc < 16) {
//			Serial << "\nkey: " << cc;
			ev_key[cc] = true;
		} else if (cc < _END) {
//			Serial << "\nevt: " << (cc - 16);
			ev_cmd[cc - 16] = true;
		} else {
			Serial << "\ncomando inv�lido: " << cc;
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




//	  _                           _
//	 |_)    _|_ _|_  _  ._   _   /   _. | | |_   _.  _ |   _
//	 |_) |_| |_  |_ (_) | | _>   \_ (_| | | |_) (_| (_ |< _>
//
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

void checkAngle() {
	bankInputs.encoder->tick(); // just call tick() to check the state.
}


//	  _  __         _
//	 |_ (_  |\/|   /   _. | | |_   _.  _ |   _
//	 |  __) |  |   \_ (_| | | |_) (_| (_ |< _>
//
#include "MainTransitions.h"

#include "MainOnEnterings.h"

#include "MainOnLeavings.h"

#include "MenuTransitions.h"

#include "MenuOnEnterings.h"

#include "MenuOnLeavings.h"


/*
 *
 * # OPERACI�N
 *
 * ## DISPLAY: P (IDLE)
 * - Presionar START para comenzar el ensayo
 *
 * ## DISPLAY: E (ST_CHECKING_COND)
 * - Verificar las siguientes condiciones:
 *   - Velocidad de masa nula
 *   - Velocidad de rueda nula
 *   - Presiones de horquilla y freno nulas
 *   - Temperaturas por debajo del l�mite caliente
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
 * - Aumentar presi�n de horquilla hasta la presi�n nominal del ensayo (simula peso del avi�n)
 * - ERROR: si la velocidad de masa disminuye por debajo de la velocidad de frenado antes de aplicar el freno.
 *
 * ## DISPLAY: 4 (ST_LANDED)
 * - Disminuir o Aumentar velocidad de masa hasta ingresar al rango de frenado MvBmin < Mv < MvBmax
 *
 * ## DISPLAY: 5 (ST_BRAKING_VEL)
 * - Aplicar freno
 * - REGRESO A [4]: si la velocidad de masa sale del rango de frenado antes de que la presi�n de freno alcance la presi�n nominal
 *
 * ## DISPLAY: 6 (ST_BRAKING)
 * - Esperar la detenci�n total de la masa y la rueda
 * - ERROR: si la presi�n de freno disminuye demasiado antes de la detenci�n total de la rueda y masa
 *
 * ## DISPLAY: F (ST_TEST_COMPLETE)
 * - Fin del ensayo, presionar Start para reiniciar.
 *
 *************************************************************************************************/
