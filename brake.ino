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
#include "SuperLed.h"
#include "Button.h"
#include "TimerOne.h"
#include "KeyPadRx.h"
#include "FreqCount.h"  // contador de pulsos  [+0V --> +5V]
#include "MyTasker.h"
#include "EEPROM.h"
#include "EEpromPlus.h"
#include "StateMachineLib.h"

#include "LiquidCrystal.h"

#include "Brake.h"

void saveSettings() {
	int eeAddress = 0;  // Bank 0/3 : Cell 0x000/0x3FF
	EEPROM.put(eeAddress, calFactors);

	eeAddress += sizeof(calFactors);
	EEPROM.put(eeAddress, testParms);
}

void loadSettings() {
	int eeAddress = 0;
	EEPROM.get(0, calFactors);

	eeAddress += sizeof(calFactors);
	EEPROM.get(eeAddress, testParms);
}

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

	// SuperLeds - el Timer debe iniciarse DESPUES de iniciar SuperLeds.

	led[0] = new SuperLed(LED0, 10, 40, 0);
	led[1] = new SuperLed(LED1, 10, 40, 0);
	led[2] = new SuperLed(LED2, 10, 40, 0);
	led[3] = new SuperLed(LED3, 10, 40, 0);
	led[4] = new SuperLed(LED4, 10, 40, 0);
	led[5] = new SuperLed(LED5, 10, 40, 0);
	led[6] = new SuperLed(LED6, 10, 40, 0);
	led[7] = new SuperLed(LED7, 10, 40, 0);

	buzz = new SuperLed(BUZZ, 6, 1, 0);
	buzz->setCycles(1);

	// Buttons
	// @formatter:off
	btn[0] = new Button(BTN0, []() { btn0_pressed = true; }, onBtn0Long);
	btn[1] = new Button(BTN1, []() { btn1_pressed = true; }, onBtn1Long);
	btn[2] = new Button(BTN2, []() { btn2_pressed = true; }, onBtn2Long);
	btn[3] = new Button(BTN3, []() { btn3_pressed = true; }, onBtn3Long);	// @formatter:on

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

	// Analog input
	analogReference(DEFAULT);  // 5.0V

	FreqCount.begin(PERIOD);

	//state_reset();
	Serial.println(F("\n\nBrake Test"));

	setupFSM();
	setupMENU();

	FSM.SetState(ST_IDLE, false, true);
	MENU.SetState(ST_MENU_IDLE, false, true);

//	lcd.begin(16, 2);
//
//	lcd.setCursor(0, 0);
//	lcd.print(F("TEST 123"));

	/*********************************
	 * CALIBRATION
	 *********************************/
//	testParms.max_mass_vel = 500.28f;  // 500 rpm
//	testParms.brake_mass_vel_min = 410.28f;  // 420 rpm nominal
//	testParms.brake_mass_vel_max = 430.28f;
//	testParms.landing_wheel_vel = 500.2f;
//	testParms.ph_threshold = 512.28f;
//	testParms.pf_threshold = 100.28f;
//
//	calFactors.ka_mass = 1.0f;		// ka_mass = 1 rpm/Hz
//	calFactors.ka_wheel = 1.0f;
//	calFactors.ka_ph = 1.0f;
//	calFactors.ka_pf = 1.0f;
//
//	int eeAddress = 0;  // Bank 0/3 : Cell 0x000/0x3FF
//	EEPROM.put(eeAddress, calFactors);
//
//	eeAddress += sizeof(calFactors);
//	EEPROM.put(eeAddress, testParms);

	TestParms testParms1;
	CalFactors calFactors1;

	loadSettings();

//	EEpromPlus::EEshow(32, 1, 'm');

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

	Mv = FreqCount.read() * 1000 / PERIOD;

	Mv_eq_0 = Mv <= ZERO_MASS_VEL;
	Mv_gt_0 = Mv > ZERO_MASS_VEL;
	Mv_gt_MAX = Mv >= testParms.max_mass_vel;
	Mv_le_BRAKEv_max = Mv <= testParms.brake_mass_vel_max;
	Mv_ge_BRAKEv_min = Mv >= testParms.brake_mass_vel_min;

	Wv_eq_0 = wheel_daq_value <= ZERO_WHEEL_VEL;
	Wv_ge_LANDINGv = wheel_daq_value
			>= testParms.landing_wheel_vel / calFactors.ka_wheel;
	Wv_gt_0 = wheel_daq_value > ZERO_WHEEL_VEL;

	Ph_gt_0 = ph_daq_value >= ZERO_PH;
	Ph_ge_Ph1 = ph_daq_value >= testParms.ph_threshold / calFactors.ka_ph;

	Pf_gt_0 = pf_daq_value > ZERO_PF;
	Pf_ge_Pf1 = pf_daq_value >= testParms.pf_threshold / calFactors.ka_pf;

	timeOut = (millis() - _t0) > _dt;

	daq_ready = false;  // data read flag
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

	if (FreqCount.available() && daq_ready) {
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
		buzz->start(10, 1, 1);

		int cc = getCmd(keyBuff, cmdTable);

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
 ***************************/
//	if (keyboard->dataReady()) {
//
//		int cc = getCmd(str, cmdTable);
//
//		switch (cc) {
//		case READ:
//			//Serial << "Reading...\n\n";
//			daqEnabled = true;
//			break;
//		case WRITE:
//			Serial << "Writing...";
//			break;
//
//		case START:
//			Serial << "Starting...";
//			break;
//
//		case STOP:
//			Serial << "Stopping...";
//			break;
//
//		default:
//			Serial << "Unknown command";
//			break;
//		}
//
//		keyboard->start();
//	}
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
void onBtn0Long() {
	Serial << F("\nButton0 Long Pressed!");
	led[0]->start();
}

void onBtn1Long() {
	Serial << F("\nButton1 Long Pressed!");
	led[1]->start();
}

void onBtn2Long() {
	Serial << F("\nButton2 Long Pressed!");
	led[2]->start();
}

void onBtn3Long() {
	Serial << F("\nButton3 Long Pressed!");
	led[3]->start();
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
	buzz->start(2, 1, 1);
	//Serial << F("\nkeypressed: ") << key;
}

/******************************************
 * TASKER Callbacks
 ******************************************/
void Task1ms() {
	for (int btnIndex = 0; btnIndex < 4; ++btnIndex) {
		btn[btnIndex]->update();
	}
}

void Task10ms() {
	for (int ledIndex = 0; ledIndex < 8; ++ledIndex) {
		led[ledIndex]->update();
	}

	buzz->update();
}

void Task100ms() {
	if (daq_enabled) {
		wheel_daq_value = analogRead(INPUT_WHEEL);
		ph_daq_value = analogRead(INPUT_PH);
		pf_daq_value = analogRead(INPUT_PF);

		daq_ready = true;
	}
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
	daq_ready = false;
	eventsChecked = false;
	daq_enabled = true;
	keypad_data_ready = false;

	btn0_pressed = false;
	btn1_pressed = false;
	btn2_pressed = false;
	btn3_pressed = false;

	ledsOff();
	led[0]->set(HIGH);

//keyboard->start();
	keyPadRx->start();

	FreqCount.begin(PERIOD);

}

void ledsOff() {
	for (int ledIndex = 0; ledIndex < 8; ++ledIndex) {
		led[ledIndex]->set(LOW);
	}
}
