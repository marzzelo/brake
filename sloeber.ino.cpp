#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2021-09-06 10:48:43

#include "Arduino.h"
#define debug
#define fadea
#include "Rx.h"
#include "CmdSplitter.h"
#include "Streaming.h"
#include "SuperLed.h"
#include "TimerOne.h"
#include "KeyPadRx.h"
#include "FreqCount.h"
#include "MyTasker.h"
#include "EEPROM.h"
#include "EEpromPlus.h"
#include "StateMachineLib.h"
#include "LiquidCrystal.h"
#include "BankButtons.h"
#include "Bank.h"
#include "BankAnalogInputs.h"
extern Bank bank;
extern BankButtons bankButtons;
#define INPUT_MASS		47
#define LED0	22
#define LED1	24
#define LED2	26
#define LED3	28
#define LED4	30
#define LED5	32
#define LED6	34
#define LED7	36
#define BUZZ	38
#define KP_ROW0		A9
#define KP_ROW1		A12
#define KP_ROW2		A11
#define KP_ROW3		A14
#define KP_COL0		A8
#define KP_COL1		A10
#define KP_COL2		A13
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
extern char keyBuff[];
extern const byte ROWS;
extern const byte COLS;
extern char keys[];
extern char keys[];
extern byte rowPins[];
extern byte colPins[];
extern Keypad* keypad;
extern KeyPadRX* keyPadRx;
extern bool keyPadEnabled;
extern volatile bool keypad_data_ready;
extern const int PERIOD;
extern volatile uint16_t Mv;
extern char _t250ms;
extern unsigned long _t0;
extern SuperLed* led[];
extern SuperLed* buzz;
extern MyTasker* tasker;
extern const char* cmdTable[];
extern const char* cmdTable[];
extern const char* cmdTable[];
extern bool cmd_menu_sent;
extern bool btn0_pressed;
extern bool btn1_pressed;
extern bool btn2_pressed;
extern bool btn3_pressed;
extern LiquidCrystal lcd;
#include "FSM.h"
#include "MENU.h"

void setup() ;
void checkEvents() ;
void setTimeOut(unsigned long dt) ;
void loop() ;
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
void ledsOff() ;


#include "brake.ino"

#endif
