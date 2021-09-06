/* ===============================================================================================
 * 					       BANCO DE FRENO - FIRMWARE ARDUINO MEGA 2560
 * ===============================================================================================
 *
 *
 *
 *
 *
 *************************************************************************************************/

#define debug
#define fadea

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

BankAnalogInputs bankInputs(200);

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


bool keyPadEnabled = true;
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
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
		"*0", "*1", "*2", "*3", "*4", "*5", "*6", "*7", "*8", "*9" };

enum _cmdEnum {
	KEY0, KEY1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9,
	CMD0, CMD1, CMD2, CMD3, CMD4, CMD5, CMD6, CMD7, CMD8, CMD9,
	_END
} cmdEnum;                   // @formatter:on
//
//
bool cmd_menu_sent;

bool btn0_pressed = false;
bool btn1_pressed = false;
bool btn2_pressed = false;
bool btn3_pressed = false;

uint16_t Mv;

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

/******************************************************************
 * 								F S M
 ******************************************************************/

#include "FSM.h"

#include "MENU.h"

/*********************************************************************************
 * 										SETUP
 *********************************************************************************/
void setup() {

	Serial.begin(115200);
	while (!Serial) {
	}

	tasker = new MyTasker(Task1ms, Task10ms, Task100ms, NULL);

	// Keyboard handlers
	keyboard->setDataReadyHandler(dataReadyHandler);
	keyboard->setKeyPressedHandler(keyPressedHandler);

	// KeyPad handlers
	keyPadRx->setDataReadyHandler(keyPadDataReadyHandler);
	keyPadRx->setKeyPressedHandler(keyPadPressedHandler);

	// Timer
	Timer1.stop();
	Timer1.initialize(1000);
	Timer1.attachInterrupt(T1_ISR);
	Timer1.start();

	// Counters
	_t250ms = T250MS;
	_t500ms = T500MS;
	_t1s = T1S;

	// turn built-in led off
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	Serial.println(F("\n\nBrake Test"));

	setupFSM();
	setupMENU();

	FSM.SetState(ST_IDLE, false, true);
	MENU.SetState(ST_MENU_IDLE, false, true);

//	lcd.begin(16, 2);
//
//	lcd.setCursor(0, 0);
//	lcd.print(F("TEST 123"));


	bank.loadSettings();

}

//////////////////////////////////////////////////////////////////
// EVENT CHECKING
//////////////////////////////////////////////////////////////////
void checkEvents() {

	btn0_p = btn0_pressed;
	btn0_pressed = false;

	btn1_p = btn1_pressed;
	btn1_pressed = false;

	btn2_p = btn2_pressed;
	btn2_pressed = false;

	btn3_p = btn3_pressed;
	btn3_pressed = false;

	Mv = bankInputs.getRpm();  // available() already checked!

	Mv_eq_0 = Mv <= ZERO_MASS_VEL;
	Mv_gt_0 = Mv > ZERO_MASS_VEL;
	Mv_gt_MAX = Mv >= bank.testParms.max_mass_vel;
	Mv_le_BRAKEv_max = Mv <= bank.testParms.brake_mass_vel_max;
	Mv_ge_BRAKEv_min = Mv >= bank.testParms.brake_mass_vel_min;

	Wv_eq_0 = bankInputs.wheel_daq_value <= ZERO_WHEEL_VEL;
	Wv_ge_LANDINGv = bankInputs.wheel_daq_value
			>= bank.testParms.landing_wheel_vel / bank.calFactors.ka_wheel;
	Wv_gt_0 = bankInputs.wheel_daq_value > ZERO_WHEEL_VEL;

	Ph_gt_0 = bankInputs.ph_daq_value >= ZERO_PH;
	Ph_ge_Ph1 = bankInputs.ph_daq_value
			>= bank.testParms.ph_threshold / bank.calFactors.ka_ph;

	Pf_gt_0 = bankInputs.pf_daq_value > ZERO_PF;
	Pf_ge_Pf1 = bankInputs.pf_daq_value
			>= bank.testParms.pf_threshold / bank.calFactors.ka_pf;

	timeOut = (millis() - _t0) > _dt;

	bankInputs.start();
}

// TODO: Convert to macro
void setTimeOut(unsigned long dt) {
	_t0 = millis();
	_dt = dt;
}

/*********************************************************************************
 * 										LOOP
 *********************************************************************************/
void loop() {

	checkKeyPad();

	MENU.Update();

	if (bankInputs.ready()) {
		checkEvents();

		FSM.Update();
	}

}

/***************************
 * KEYPAD COMMANDS
 ***************************/
void checkKeyPad() {
	if (!keyPadEnabled)
		return;

	if (keyPadRx->dataReady()) {
		bankLeds.beep(10, 1, 1);

		int cc = getCmd(bankKp.getBuff(), cmdTable);

		if (cc < 10) {
			Serial << "\nkey: " << cc;
			ev_key[cc] = true;
		} else {
			Serial << "\nevt: " << (cc - 10);
			ev_cmd[cc - 10] = true;
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
 * Obtain command offset
 ***************************/
int getCmd(char *strCmd, const char *table[]) {
	int p;

	for (p = 0; p < _END; p++) {
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
	btn0_pressed = true;
}

void onBtn1() {
	btn1_pressed = true;
}

void onBtn2() {
	btn2_pressed = true;
}

void onBtn3() {
	btn3_pressed = true;
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
	bankLeds.beep(2, 1, 1);
}

/******************************************
 * TASKER Callbacks
 ******************************************/
void Task1ms() {

	bankButtons.update();
}

void Task10ms() {
	bankLeds.update();
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

	btn0_pressed = false;
	btn1_pressed = false;
	btn2_pressed = false;
	btn3_pressed = false;

	bankLeds.ledOffAll();
	bankLeds.ledOn(0);

//keyboard->start();
	keyPadRx->start();
}

