/*
 * BankAnalogInputs.h
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

/*
					 _                _               ___                   _
					/ \   _ __   __ _| | ___   __ _  |_ _|_ __  _ __  _   _| |_ ___
				   / _ \ | '_ \ / _` | |/ _ \ / _` |  | || '_ \| '_ \| | | | __/ __|
				  / ___ \| | | | (_| | | (_) | (_| |  | || | | | |_) | |_| | |_\__ \
				 /_/   \_\_| |_|\__,_|_|\___/ \__, | |___|_| |_| .__/ \__,_|\__|___/
											  |___/            |_|

 */

#ifndef BANKANALOGINPUTS_H_
#define BANKANALOGINPUTS_H_

#include "Arduino.h"
#include "FreqCount.h"  	// contador de pulsos  [+0V --> +5V]
#include "RotaryEncoder.h"
#include "EEPROM.h"
#include "EEpromPlus.h"


#define INPUT_MASS		47	// Frequency Counter
#define INPUT_PF		A0	// Presión de Freno
#define INPUT_WHEEL		A1	// Velocidad de Rueda
#define INPUT_PH		A2	// Presión de Horquilla
#define INPUT_T1		A3	// Temp 1
#define INPUT_T2		A4	// Temp 2

/*********************************
 * ROTARY ENCODER PINS
 *********************************/
#define PIN_IN1 		18	// Green // SELECT PINS 2, 3, 18, 19, 20 or 21
#define PIN_IN2 		19	// White // SELECT PINS 2, 3, 18, 19, 20 or 21


#define ZERO_PH				5
#define ZERO_PF				5
#define ZERO_MASS_VEL		5
#define ZERO_WHEEL_VEL		5

enum Condition {
	MV_GT_0,
	MV_GT_MAX,
	MV_GE_BRAKEV_MIN,
	MV_LE_BRAKEV_MAX,
	MV_EQ_0,
	WV_GT_0,
	WV_GE_LANDINGV,
	WV_EQ_0,
	PH_GT_0,
	PH_GE_PH1,
	PF_GT_0,
	PF_GE_PF1,
	T1_GE_THOT,
	T2_GE_THOT,
	TIMEOUT
};

/**
 * Controla todas las entradas analógicas del Banco de Freno, incluyendo:
 * - Velocidad de la rueda
 * - Presión de horquilla
 * - Presión de freno
 * - Posición, velocidad y dirección del encoder
 * - Velocidad de Masa (lector óptico)
 */
class BankAnalogInputs {

public:
	enum VarNames {
		MASS, ANGLE, WHEEL, PH, PF, T1, T2, ERROR, END
	};

private:
	struct TestParms {
		double max_mass_vel;
		double brake_mass_vel_min;
		double brake_mass_vel_max;
		double landing_wheel_vel;
		double ph_threshold;
		double pf_threshold;
		double t1_hot;		//!< too high to begin test
		double t2_hot;		//!< too high to begin test
	};

	struct CalFactors {
		double ka_mass;
		double ka_wheel;
		double ka_ph;
		double ka_pf;
		double ka_t1;
		double ka_t2;
		double kb_alpha;
	};

	int relay[4];

	struct EncoderData {
		long position;
		double angle;
		int direction;
		unsigned long rpm;
	};


	int _period; 						//!< Periodo de muestreo de pulsos (FreqCount.h)
	int _pos = 0;  						//!< Rotary Encoder Position;
	volatile bool _daq_ready;
	volatile bool _daq_enabled;
	int _filter;
	volatile double _distance;			//<! Distancia recorrida durante el frenado
	double _t0 = 0;
	double _dt = 0;
	bool _counting;
	uint32_t _freqBuff[8] = {0};
	double _angleOffset = 0.0;
	void (*_checkPosition)();
	VarNames _display_var = VarNames::ANGLE;
	EncoderData _encoderData;
	double _last_rpm = 28.0;				//<! buffers rpm
	char _buff[5];


public:
	TestParms testParms;
	CalFactors calFactors;
	RotaryEncoder *encoder = nullptr;

	volatile double mass_rpm;			//<! velocidad volanta
//	volatile double angle;				//<! posición angular encoder --> use encoderRead().angle
	volatile double wheel_daq_value;	//<! Velocidad de rueda (calibrar mediante menú)
	volatile double ph_daq_value;		//<! Presión de horquilla (calibrar mediante menú)
	volatile double pf_daq_value;		//<! Presión de freno (calibrar mediante menú)
	volatile double t1_daq_value;		//<! Temperatura 1
	volatile double t2_daq_value;		//<! Temperatura 2


	BankAnalogInputs(void (*checkPosition)(), int period = 1000, int filter = 1);

	void setup();
	void saveSettings();
	void loadSettings();
	void eePreset();

	void enable();
	void start();
	bool ready();
	bool check(Condition cond);
	void setTimeOut(unsigned long dt);

	double getDistance();		//<! distancia recorrida en metros (desde el inicio del frenado)
	double getTime();
	EncoderData encoderRead();	//<! posición angular, rpm, dirección (encoder)
	double getRpm();			//<! velocidad volanta en rpm
	double getWv();				//<! velocidad de rueda en m/s
	double getPf();
	double getPh();
	double getT1();
	double getT2();

	void startCounting();
	double stopCounting();

	void update();
	void setAngleOffset(double angleOffset);

	void setDisplayVarIndex(int index);
	int getDisplayVarIndex();
	char *nextDisplayVar();		// returns DisplayVarName
	double getDisplayVarValue();
	char *getDisplayVarName();

};


#endif /* BANKANALOGINPUTS_H_ */
