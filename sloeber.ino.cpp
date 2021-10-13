#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2021-10-13 14:03:18

#include "Arduino.h"
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
extern BankButtons* bankButtons;
extern BankLeds* bankLeds;
extern BankAnalogInputs* bankInputs;
extern BankKeyPad* bankKp;
extern MainFSM* brake;
extern MenuFSM* menu;
extern LiquidCrystal lcd;
extern Printer printer;
extern MyTasker* tasker;
#define T250MS 	25
#define T500MS 	50
#define T1S 	100
#define ZERO_PH				5
#define ZERO_PF				5
#define ZERO_MASS_VEL		5
#define ZERO_WHEEL_VEL		5
extern char _t250ms;
#include "MainTransitions.h"
#include "MainOnEnterings.h"
#include "MainOnLeavings.h"
#include "MenuTransitions.h"
#include "MenuOnEnterings.h"
#include "MenuOnLeavings.h"

bool mon() ;
void setup() ;
void loop() ;
void onBtn0() ;
void onBtn1() ;
void onBtn2() ;
void onBtn3() ;
void keyPadDataReadyHandler() ;
void keyPadPressedHandler(char key) ;
void Task1ms() ;
void Task10ms() ;
void Task100ms() ;
void T1_ISR(void) ;
void checkAngle() ;


#include "brake.ino"

#endif
