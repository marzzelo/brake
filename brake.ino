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




//	  _
//	 /  |  _.  _  _  _   _
//	 \_ | (_| _> _> (/_ _>
//
BankButtons bankButtons(onBtn0, onBtn1, onBtn2, onBtn3);
BankLeds bankLeds;
BankAnalogInputs bankInputs(checkAngle, 500, 5);
BankKeyPad *bankKp = new BankKeyPad(keyPadPressedHandler, keyPadDataReadyHandler, &bankLeds);

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


bool eventsChecked;

//	  __
//	 (_   _ _|_     ._
//	 __) (/_ |_ |_| |_)
//					|
void setup() {

	Serial.begin(115200);
	while (!Serial) {
	}

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

//	bank.eePreset();

	bankInputs.loadSettings();	// Carga datos de calibración y parámetros de ensayo desde EEprom
	bankInputs.start(); 		// Inicia el contador de pulsos
}




//
//	 |   _   _  ._
//	 |_ (_) (_) |_)
//				|
void loop() {

	bankKp->check();			// Check comandos por teclado - comenzados con "*"  (ej.: "*5#")

	menu->Update();				// chequea FSM de Menú principal

	if (bankInputs.ready()) {	// Datos digitalizados y conteo de pulsos disponible?

		checkEvents();			// Adquirir datos y actualizar condiciones booleanas
		brake->Update();		// Avanzar máquina de estados principal

	}

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
	bankKp->setDataReady(true);
}

void keyPadPressedHandler(char key) {
	Serial << ((key == '*') ? bankKp->getAsterisk() : key);
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

	bankKp->update();
	tasker->update();

}


// Encoder callback
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
