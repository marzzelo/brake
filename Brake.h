/*
 * Brake.h
 *
 *  Created on: 26 ago. 2021
 *      Author: valdez
 */

#ifndef BRAKE_H_
#define BRAKE_H_

/*********************************
 * PIN DEFINITIONS
 *********************************/
#define INPUT_MASS		47	// Frequency Counter
#define INPUT_PF		A0	// Analog input
#define INPUT_WHEEL		A1	// Analog input
#define INPUT_PH		A2	// Analog input

#define LED0	22
#define LED1	24
#define LED2	26
#define LED3	28
#define LED4	30
#define LED5	32
#define LED6	34
#define LED7	36

#define BUZZ	38

#define BTN0 	31
#define BTN1 	33
#define BTN2 	35
#define BTN3 	37

#define KP_ROW0		A9
#define KP_ROW1		A12
#define KP_ROW2		A11
#define KP_ROW3		A14
#define KP_COL0		A8
#define KP_COL1		A10
#define KP_COL2		A13

/*********************************
 * TIMER ACCUMULATORS
 *********************************/
#define T250MS 	25
#define T500MS 	50
#define T1S 	100

/*********************************
 * RX - PC KEYBOARD OR PROCESSING
 *********************************/
char str[40];  // comandos desde keyboard o Processing
Rx *keyboard = new Rx(str, 40);
volatile bool dataReady = false;

/***************************
 * KEY PAD
 ***************************/
char keyBuff[40];

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS * COLS] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '*',
		'0', '#' };

byte rowPins[ROWS] = { KP_ROW0, KP_ROW1, KP_ROW2, KP_ROW3 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = { KP_COL0, KP_COL1, KP_COL2 }; //connect to the column pinouts of the keypad

Keypad *keypad = new Keypad(keys, rowPins, colPins, ROWS, COLS); // keyPad
KeyPadRX *keyPadRx = new KeyPadRX(keyBuff, 40, keypad); // Buffer-controller serial for keyPad

bool keyPadEnabled = true;
volatile bool keypad_data_ready;

/***************************
 * FREQUENCY METER
 ***************************/
const int PERIOD = 200; // Periodo de muestreo de pulsos (FreqCount.h)

/***************************
 * ANALOG INPUT
 ***************************/
volatile bool daq_ready;
volatile bool daq_enabled;

volatile uint16_t Mv;  // Mass velocity
volatile uint16_t wheel_daq_value;
volatile uint16_t ph_daq_value;
volatile uint16_t pf_daq_value;

/***************************
 * TIMERS
 ***************************/
char _t250ms, _t500ms, _t1s;
unsigned long _t0, _dt;

///***************************
// * BUTTONS
// ***************************/
Button *btn[4];

volatile bool btn0_pressed;
volatile bool btn1_pressed;
volatile bool btn2_pressed;
volatile bool btn3_pressed;

/***************************
 * DIGITAL LEDS (OUTPUT)
 ***************************/
SuperLed *led[8];
SuperLed *buzz;

/***************************
 * TASKER
 ***************************/
MyTasker *tasker;

/***************************
 * COMMAND TABLE
 ***************************/

// @formatter:off
char const *cmdTable[] = {
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
		"*0", "*1", "*2", "*3", "*4", "*5", "*6", "*7", "*8", "*9" };

enum _cmdEnum {
	KEY0, KEY1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9,
	CMD0, CMD1, CMD2, CMD3, CMD4, CMD5, CMD6, CMD7, CMD8, CMD9,
	_END
} cmdEnum;                  // @formatter:on
//
//
bool cmd_menu_sent;


/*********************************
 * 			EEPROM
 *********************************/

//struct TestParms {
//	uint16_t max_mass_vel;
//	uint16_t zero_mass_vel;
//	uint16_t brake_mass_vel_min;
//	uint16_t brake_mass_vel_max;
//
//	uint16_t zero_wheel_vel;
//	uint16_t landing_wheel_vel;
//	uint16_t ph_threshold;
//	uint16_t pf_threshold;
//
//	uint16_t zero_ph;
//	uint16_t zero_pf;
//};

struct TestParms {
	double max_mass_vel;
	double brake_mass_vel_min;
	double brake_mass_vel_max;
	double landing_wheel_vel;
	double ph_threshold;
	double pf_threshold;
};

#define ZERO_PH				5
#define ZERO_PF				5
#define ZERO_MASS_VEL		5
#define ZERO_WHEEL_VEL		5


struct CalFactors {
	double ka_mass;
	double ka_wheel;
	double ka_ph;
	double ka_pf;
};

TestParms testParms;
CalFactors calFactors;


LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

/******************************************************************
 * 								F S M
 ******************************************************************/

#include "FSM.h"

#include "MENU.h"

#endif /* BRAKE_H_ */
