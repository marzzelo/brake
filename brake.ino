//
//
//				  ____    _    _   _  ____ ___    ____  _____   _____ ____  _____ _   _  ___
//				 | __ )  / \  | \ | |/ ___/ _ \  |  _ \| ____| |  ___|  _ \| ____| \ | |/ _ \    ;
//				 |  _ \ / _ \ |  \| | |  | | | | | | | |  _|   | |_  | |_) |  _| |  \| | | | |
//				 | |_) / ___ \| |\  | |__| |_| | | |_| | |___  |  _| |  _ <| |___| |\  | |_| |
//				 |____/_/   \_\_| \_|\____\___/  |____/|_____| |_|   |_| \_\_____|_| \_|\___/
//
//																	   _   _   _   _
//						  /\  ._ _|     o ._   _    |\/|  _   _   _.    ) |_  |_  / \			;
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
#include "TM1638.h"

#define SERIAL_MONITORING

#define ZERO_PH				5
#define ZERO_PF				5
#define ZERO_MASS_VEL		5
#define ZERO_WHEEL_VEL		5

#define STB					10
#define CLK					11
#define DIO					12


// decreases terminal-messages-printing-rate (1 message every 1 sec)
// if SERIAL_MONITORING defined, returns 2 every 1ms, else returns 1.

#ifdef SERIAL_MONITORING
#define RETVAL	2
#else
#define RETVAL	1
#endif

int mon() {
	if (millis() % 1000 == 0) {
		return RETVAL;
	}
	return 0;
}



//	  _
//	 /  |  _.  _  _  _   _
//	 \_ | (_| _> _> (/_ _>
//
BankButtons 		*bankButtons;
BankLeds 			*bankLeds;
BankAnalogInputs 	*bankInputs;
BankKeyPad 			*bankKp;
MainFSM 			*brake;
MenuFSM 			*menu;
TM1638				*tm1638;
MyTasker 			*tasker;

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
Printer printer(60);



//	  __
//	 (_   _ _|_     ._
//	 __) (/_ |_ |_| |_)
//					|
void setup() {

	// Serial Port
	Serial.begin(115200);
	while (!Serial) {}	// clears serial buff

	bankButtons = 	new BankButtons(onBtn0, onBtn1, onBtn2, onBtn3);
	bankLeds = 		new BankLeds();
	bankInputs = 	new BankAnalogInputs(checkAngle, 500, 5);
	bankKp = 		new BankKeyPad(keyPadPressedHandler, keyPadDataReadyHandler);
	brake = 		new MainFSM(mainTransitions, mainOnEnterings, mainOnLeavings, nullptr);
	menu =	 		new MenuFSM(menuTransitions, menuOnEnterings, menuOnLeavings);
	tm1638 = 		new TM1638(STB, CLK, DIO);
	tasker = 		new MyTasker(Task1ms, Task10ms, Task100ms, NULL);

	// Timer
	Timer1.stop();
	Timer1.initialize(1000);
	Timer1.attachInterrupt(T1_ISR);
	Timer1.start();


	// State Machines GO IDLE!
	brake->SetState(MainFSM::ST_IDLE, false, true);
	menu->SetState(MenuFSM::ST_MENU_IDLE, false, false);

//	bank.eePreset();			// default calibration/parameter values

//	bankButtons->reset();		// clears buttons buffer
	bankInputs->loadSettings();	// Loads calibration/test parameters from EEprom
	bankInputs->start(); 		// Enables DAQ
}


//
//	 |   _   _  ._
//	 |_ (_) (_) |_)
//				|
void loop() {

	menu->Update();				// compute main menu FSM transitions

	brake->Update();			// compute system FSM transitions

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
//	bankButtons->setPressed(2);
	bankLeds->beep();
	double offset = - bankInputs->encoderRead().position * 360.0 / 2000.0;
	Serial << "\n offset: " << offset;
	bankInputs->setAngleOffset(offset);
}

void onBtn3() {
	bankButtons->setPressed(3);
}


/******************************************
 * KEY PAD Callbacks
 ******************************************/
void keyPadDataReadyHandler() {
	if (!bankKp->checkCommands)
			return;

	bankLeds->beep(100, 1, 1);

	int cc = bankKp->getCommand();

	if (cc < 16) {
		bankKp->ev_key[cc] = true;
//		Serial << "\nkey: " << cc;
	} else if (cc < bankKp->CmdEnum::_END) {
		bankKp->ev_cmd[cc - 16] = true;
//		Serial << "\ncomando: " << cc;
	} else {
		Serial << "\ncomando inválido: " << cc;
	}

	bankKp->start();

}

void keyPadPressedHandler(char key) {
	Serial << ((key == '*') ? bankKp->getAsterisk() : key);
	bankLeds->display(key);
	bankLeds->beep(20, 1, 1);
}


/******************************************
 * Encoder Callback
 ******************************************/
void checkAngle() {
	bankInputs->encoder->tick(); // just call tick() to check the state.
}


/******************************************
 * TASKER Callbacks
 ******************************************/
void Task1ms() {
	bankButtons->update();
	bankLeds->update();
	bankKp->update();
}

void Task10ms() {
	bankInputs->update();
}

void Task100ms() {
	//
}

/******************************************
 * TIMER 1  ISR
 ******************************************/
void T1_ISR(void) {
	tasker->update();
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

