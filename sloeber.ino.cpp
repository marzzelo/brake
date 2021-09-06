#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2021-09-06 08:24:38

#include "Arduino.h"
#define debug
#define fadea
#include "Rx.h"
#include "CmdSplitter.h"
#include "Streaming.h"
#include "SuperLed.h"
#include "Button.h"
#include "TimerOne.h"
#include "KeyPadRx.h"
#include "FreqCount.h"
#include "MyTasker.h"
#include "EEPROM.h"
#include "EEpromPlus.h"
#include "StateMachineLib.h"
#include "LiquidCrystal.h"
#include "Brake.h"

void saveSettings() ;
void loadSettings() ;
void setup() ;
void checkEvents() ;
void setTimeOut(unsigned long dt) ;
void loop() ;
void checkKeyPad() ;
int getCmd(char *strCmd, const char *table[]) ;
void onBtn0Long() ;
void onBtn1Long() ;
void onBtn2Long() ;
void onBtn3Long() ;
void dataReadyHandler() ;
void keyPressedHandler(char key) ;
void keyPadDataReadyHandler() ;
void keyPadPressedHandler(char key) ;
void Task1ms() ;
void Task10ms() ;
void Task100ms() ;
void T1_ISR(void) ;
void state_reset() ;
void ledsOff() ;


#include "brake.ino"

#endif
