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
#include "Confirmator.h"

#define SERIAL_MONITORING

#define ZERO_PH				5
#define ZERO_PF				5
#define ZERO_MASS_VEL		5
#define ZERO_WHEEL_VEL		5

#define STB					10
#define CLK					11
#define DIO					12


// decreases terminal-messages-printing-rate (1 message every 1 sec)
int mon() {
	return (millis() % 1000 == 0);
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
Confirmator			*confirmator;

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

	bankButtons = 	new BankButtons(onBtn0, onLongBtn0, onBtn1, onLongBtn1);
	bankLeds = 		new BankLeds();
	bankInputs = 	new BankAnalogInputs(checkAngle, 500, 5);
	bankKp = 		new BankKeyPad(keyPadPressedHandler, keyPadDataReadyHandler);
	brake = 		new MainFSM(mainTransitions, mainOnEnterings, mainOnLeavings, nullptr);
	menu =	 		new MenuFSM(menuTransitions, menuOnEnterings, menuOnLeavings);
	tm1638 = 		new TM1638(STB, CLK, DIO);
	tasker = 		new MyTasker(Task1ms, Task10ms, Task100ms, NULL);
	confirmator =	new Confirmator();

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
//	Serial << "\nbtn0 pressed!";
}

void onLongBtn0() {
	bankButtons->setLongPressed(0);
//	Serial << "\nbtn0 long-pressed!";
}

void onBtn1() {
	bankButtons->setPressed(1);
//	Serial << "\nbtn1 pressed!";
}

void onLongBtn1() {
	bankLeds->beep();
	double offset = - bankInputs->encoderRead().position * 360.0 / 2000.0;
	Serial << "\noffset: " << offset;
	bankInputs->setAngleOffset(offset);
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
		Serial << "\ncomando inv�lido: " << cc;
	}

	bankKp->start();

}

void keyPadPressedHandler(char key) {
	Serial << ((key == '*') ? bankKp->getAsterisk() : key);
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
	tm1638->update();
}

void Task100ms() {

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

