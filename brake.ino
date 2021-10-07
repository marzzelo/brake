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


#include "Rx.h"
#include "CmdSplitter.h"
#include "Streaming.h"
#include "TimerOne.h"
#include "KeyPadRx.h"
#include "MyTasker.h"
#include "StateMachineLib.h"
#include "LiquidCrystal.h"
#include "bankButtons.h"
#include "BankLeds.h"
#include "BankAnalogInputs.h"
#include "BankKeyPad.h"
#include "MenuFSM.h"
#include "MainFSM.h"
#include "Printer.h"

#define SERIAL_MONITORING


//	  _
//	 /  |  _.  _  _  _   _
//	 \_ | (_| _> _> (/_ _>
//
BankButtons *bankButtons = new BankButtons(onBtn0, onBtn1, onBtn2, onBtn3);
BankLeds *bankLeds = new BankLeds();
BankAnalogInputs *bankInputs = new BankAnalogInputs(checkAngle, 500, 5);
BankKeyPad *bankKp = new BankKeyPad(keyPadPressedHandler, keyPadDataReadyHandler, bankLeds);

MainFSM *brake = new MainFSM(mainTransitions, mainOnEnterings, mainOnLeavings);
MenuFSM *menu = new MenuFSM(menuTransitions, menuOnEnterings, menuOnLeavings);

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
Printer printer(60);
MyTasker *tasker;


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

/***************************
 * TIMERS
 ***************************/
char _t250ms, _t500ms, _t1s;

// decreases terminal-messages-printing-rate (1 message every 1 sec)
bool mon() {
#ifndef SERIAL_MONITORING
	return false;
#endif
	return (millis() % 1000 == 0);
}

//	  __
//	 (_   _ _|_     ._
//	 __) (/_ |_ |_| |_)
//					|
void setup() {

	Serial.begin(115200);
	while (!Serial) {}	// clears serial buff

	tasker = new MyTasker(Task1ms, Task10ms, Task100ms, NULL);

	// Timer
	Timer1.stop();
	Timer1.initialize(1000);
	Timer1.attachInterrupt(T1_ISR);
	Timer1.start();

	// Counters
	_t250ms = T250MS;
	_t500ms = T500MS;
	_t1s = T1S;


	// GO IDLE!
	brake->SetState(MainFSM::ST_IDLE, false, true);
	menu->SetState(MenuFSM::ST_MENU_IDLE, false, false);

//	bank.eePreset();			// default calibration/parameter values

	bankButtons->reset();		// clears buttons buffer

	bankInputs->loadSettings();	// Loads calibration/test parameters from EEprom
	bankInputs->start(); 		// Enables DAQ
}




//
//	 |   _   _  ._
//	 |_ (_) (_) |_)
//				|
void loop() {

	bankKp->check();			// Check keypad KEYS & COMMANDS

	menu->Update();				// compute main menu FSM transitions

	brake->Update();			// compute system FSM transitions

	checkButtons();

}



void checkButtons() {
	// verify btn1 pressed while in variables-monitoring state
	if (brake->GetState() == MainFSM::State::ST_MONITORING) {
		if (bankButtons->read(1)) {
			bankLeds->beep();
			bankInputs->nextDisplayVar();
		}
	}

	// reset encoder angle
	if (bankButtons->read(2)) {
		bankLeds->beep();

		double offset = - bankInputs->encoderRead().position * 360.0 / 2000.0;
		Serial << "\n offset: " << offset;
		bankInputs->setAngleOffset(offset);
	}
}


/******************************************
 * BUTTONS Callbacks
 ******************************************/
void onBtn0() {
	bankButtons->setPressed(0);
}

void onBtn1() {
	bankButtons->setPressed(1);
}

void onBtn2() {
	bankButtons->setPressed(2);
}

void onBtn3() {
	bankButtons->setPressed(3);
}


/******************************************
 * KEY PAD Callbacks
 ******************************************/
void keyPadDataReadyHandler() {
	bankKp->setDataReady(true);
}

void keyPadPressedHandler(char key) {
	Serial << ((key == '*') ? bankKp->getAsterisk() : key);
	bankLeds->display(key);
	bankLeds->beep(20, 1, 1);
}

/******************************************
 * TASKER Callbacks
 ******************************************/
void Task1ms() {
	bankButtons->update();
	bankLeds->update();
}

void Task10ms() {
	//
}

void Task100ms() {
	bankInputs->update();
}

/******************************************
 * TIMER 1  ISR
 ******************************************/
void T1_ISR(void) {
	bankKp->update();
	tasker->update();
}


// Encoder callback
void checkAngle() {
	bankInputs->encoder->tick(); // just call tick() to check the state.
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

