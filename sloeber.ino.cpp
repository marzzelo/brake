#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2021-11-03 12:54:31

#include "Arduino.h"
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
#define VERSION				"1.0.0"
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
#define CS_PIN    			53
#define LCD_PIN_CONFIG		14,15,16,17,18,19
extern BankButtons* bankButtons;
extern BankLeds* bankLeds;
extern BankAnalogInputs* bankInputs;
extern BankKeyPad* bankKp;
extern MainFSM* brake;
extern MenuFSM* menu;
extern TM1638* tm1638;
extern MyTasker* tasker;
extern Matrix* matrix;
extern HC20040IC* lcd;
extern Timer* timerDaq;
extern Printer printer;
extern bool dprint;
#include "MainTransitions.h"
#include "MainOnEnterings.h"
#include "MainOnLeavings.h"
#include "MenuTransitions.h"
#include "MenuOnEnterings.h"
#include "MenuOnLeavings.h"

void setup() ;
void loop() ;
void daqprint() ;
void onBtn0() ;
void onLongBtn0() ;
void onBtn1() ;
void onLongBtn1() ;
void keyPadDataReadyHandler() ;
void keyPadPressedHandler(char key) ;
void checkAngle() ;
void Task1ms() ;
void Task10ms() ;
void Task100ms() ;
void T1_ISR(void) ;


#include "brake.ino"

#endif
