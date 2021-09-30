#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2021-09-30 13:57:33

#include "Arduino.h"
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
extern bool cmd_menu_sent;
extern bool ev_key[];
extern bool ev_cmd[];
extern bool btn_pressed[];
extern bool Mv_gt_0;
extern bool Mv_gt_MAX;
extern bool Mv_ge_BRAKEv_min;
extern bool Mv_le_BRAKEv_max;
extern bool Mv_eq_0;
extern bool Wv_gt_0;
extern bool Wv_ge_LANDINGv;
extern bool Wv_eq_0;
extern bool Ph_gt_0;
extern bool Ph_ge_Ph1;
extern bool Pf_gt_0;
extern bool Pf_ge_Pf1;
extern bool T1_ge_Thot;
extern bool T2_ge_Thot;
extern bool reset_requested;
extern bool timeOut;
extern bool eventsChecked;
extern BankLeds bankLeds;
extern BankAnalogInputs bankInputs;
extern BankKeyPad bankKp;
extern KeyPadRX* keyPadRx;
extern MainFSM* brake;
extern MenuFSM* menu;
#define T250MS 	25
#define T500MS 	50
#define T1S 	100
#define ZERO_PH				5
#define ZERO_PF				5
#define ZERO_MASS_VEL		5
#define ZERO_WHEEL_VEL		5
extern char str[];
extern Rx* keyboard;
extern volatile bool dataReady;
extern bool checkCommands;
extern volatile bool keypad_data_ready;
extern char _t250ms;
extern unsigned long _t0;
extern MyTasker* tasker;
extern const char* cmdTable[];
extern const char* cmdTable[];
extern const char* cmdTable[];
extern LiquidCrystal lcd;
extern Printer printer;
#include "MainTransitions.h"
#include "MainOnEnterings.h"
#include "MainOnLeavings.h"
#include "MenuTransitions.h"
#include "MenuOnEnterings.h"
#include "MenuOnLeavings.h"

void setup() ;
void loop() ;
void checkEvents() ;
void setTimeOut(unsigned long dt) ;
void checkKeyPad() ;
int getCmd(char *strCmd, const char *table[]) ;
void onBtn0() ;
void onBtn1() ;
void onBtn2() ;
void onBtn3() ;
void dataReadyHandler() ;
void keyPressedHandler(char key) ;
void keyPadDataReadyHandler() ;
void keyPadPressedHandler(char key) ;
void Task1ms() ;
void Task10ms() ;
void Task100ms() ;
void T1_ISR(void) ;
void state_reset() ;
void checkAngle() ;


#include "brake.ino"

#endif
