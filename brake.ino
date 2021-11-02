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

#include "PrintMacros.h"

#include "Rx.h"
#include "CmdSplitter.h"
#include "Streaming.h"
#include "TimerOne.h"
#include "KeyPadRx.h"
#include "MyTasker.h"
#include "StateMachineLib.h"
#include "HC20040IC.h"
#include "bankButtons.h"
#include "BankLeds.h"
#include "BankAnalogInputs.h"
#include "BankKeyPad.h"
#include "MenuFSM.h"
#include "MainFSM.h"
#include "Printer.h"
#include "TM1638.h"
#include "Matrix.h"
#include "MMFilter.h"
#include "Timer.h"

#define VERSION				"1.0.0 Beta"

#define DISPLAY_MESSAGES_PERIOD		500
#define SERIAL_DAQ_PERIOD			250
#define TEST_PERIOD					500
#define BANNER_PERIOD				6000


#define ZERO_PH				15
#define ZERO_PF				15
#define ZERO_MASS_VEL		15
#define ZERO_WHEEL_VEL		15

#define STB					10
#define CLK					11
#define DIO					12

#define MAX_DEVICES 		8
#define CS_PIN    			53  // DATA: 51,  CLK: 52

#define LCD_PIN_CONFIG		14,15,16,17,18,19



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
Matrix	 			*matrix;
HC20040IC			*lcd;

Timer				*timerDaq, *timerDisplay, *timerTest, *timerBanner;
Printer printer(60);

bool dprint = false;



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
	menu =	 		new MenuFSM(menuTransitions, menuOnEnterings, menuOnLeavings, ARRAY_SIZE(lines));
	tm1638 = 		new TM1638(STB, CLK, DIO);
	tasker = 		new MyTasker(Task1ms, Task10ms, Task100ms, NULL);
	matrix = 		new Matrix(CS_PIN, MAX_DEVICES);
	lcd = 			new HC20040IC(7,  6,      5,4,3,2);

	timerDaq = 		new Timer(SERIAL_DAQ_PERIOD);
	timerDisplay = 	new Timer(DISPLAY_MESSAGES_PERIOD);
	timerTest = 	new Timer(TEST_PERIOD);
	timerBanner =   new Timer(BANNER_PERIOD);

	// Timer
	Timer1.stop();
	Timer1.initialize(1000);
	Timer1.attachInterrupt(T1_ISR);
	Timer1.start();

	brake->SetState(MainFSM::ST_IDLE, false, true);
	menu->SetState(MenuFSM::ST_MENU_IDLE, false, false);

	bankInputs->loadSettings();	// Loads calibration/test parameters from EEprom
	bankInputs->start(); 		// Enables DAQ
	bankInputs->setTimeOut(250);

}

//
//	 |   _   _  ._
//	 |_ (_) (_) |_)
//				|
void loop() {

	menu->Update();				// compute main menu FSM transitions

	brake->Update();			// compute system FSM transitions

	daqprint();
}



void daqprint() {
	if (!dprint) return;

	if (timerDaq->read()) {
		NPDAQX(bankInputs->getTime());
		TPDAQX(brake->GetState());
		TPDAQX(bankInputs->getRpm());
		TPDAQX(bankInputs->getWv());
		TPDAQX(bankInputs->getPh());
		TPDAQX(bankInputs->getPf());
		TPDAQX(bankInputs->getT1());
		TPDAQX(bankInputs->getT2());
		TPDAQX(bankInputs->getDistance());
		TPDAQX(bankInputs->encoderRead().angle);
	}

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
	NPRINT("offset: ", offset);
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
		NPRINT("comando inválido: ", cc);
	}

	bankKp->start();

}

void keyPadPressedHandler(char key) {
	Serial << ((key == '*') ? bankKp->getAsterisk() : key);
	lcd->cwrite((key == '*') ? bankKp->getAsterisk() : key);
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
	matrix->update();
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

