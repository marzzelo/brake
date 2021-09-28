/*
 * BankAnalogInputs.h
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#ifndef BANKANALOGINPUTS_H_
#define BANKANALOGINPUTS_H_

#include "Arduino.h"
#include "FreqCount.h"  // contador de pulsos  [+0V --> +5V]
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


/**
 * Controla todas las entradas analógicas del Banco de Freno, incluyendo:
 * - Velocidad de la rueda
 * - Presión de horquilla
 * - Presión de freno
 *
 * Controla también la entrada de pulsos:
 * - Velocidad de Masa (lector óptico)
 */
class BankAnalogInputs {

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
		int angle;
		int direction;
		unsigned long rpm;
	};

	enum Reference {
		MASS = 1, ANGLE, WHEEL, PH, PF, T1, T2, END
	};


	int _period; 						//!< Periodo de muestreo de pulsos (FreqCount.h)
	int _pos = 0;  						//!< Rotary Encoder Position;
	volatile bool _daq_ready;
	volatile bool _daq_enabled;
	int _filter;
	volatile double _distance;			//<! Distancia recorrida durante el frenado
	double _t0 = 0;
	bool _counting;
	uint32_t _freqBuff[8] = {0};
	int _angleOffset = 0;
	void (*_checkPosition)();
	Reference _display_var = Reference::ANGLE;;
	EncoderData _encoderData;


public:
	TestParms testParms;
	CalFactors calFactors;

	void setup();
	void saveSettings();
	void loadSettings();
	void eePreset();

	RotaryEncoder *encoder = nullptr;

	volatile uint16_t mass_rpm;			//<! velocidad volanta
	volatile uint16_t angle;			//<! posición angular encoder
	volatile uint16_t wheel_daq_value;	//<! Velocidad de rueda (calibrar mediante menú)
	volatile uint16_t ph_daq_value;		//<! Presión de horquilla (calibrar mediante menú)
	volatile uint16_t pf_daq_value;		//<! Presión de freno (calibrar mediante menú)
	volatile uint16_t t1_daq_value;		//<! Temperatura 1
	volatile uint16_t t2_daq_value;		//<! Temperatura 2

	BankAnalogInputs(void (*checkPosition)(), int period = 1000, int filter = 1);

	void enable() { _daq_enabled = true; }
	void start() { _daq_enabled = true; _daq_ready = false; FreqCount.begin(_period); }
	bool ready() { return (FreqCount.available() && _daq_ready); }

	double getDistance();		//<! distancia recorrida en metros (desde el inicio del frenado)
	double getTime();
	EncoderData encoderRead();	//<! posición angular, rpm, dirección (encoder)
	uint32_t getRpm();			//<! velocidad volanta en rpm
	uint16_t getWv();			//<! velocidad de rueda en m/s
	uint16_t getPf();
	uint16_t getPh();
	uint16_t getT1();
	uint16_t getT2();

	void startCounting();
	double stopCounting();
	void resetTimer();
	void update();
	void setAngleOffset(int angleOffset) { _angleOffset = angleOffset; }
	void nextDisplayVar();
	uint16_t getDisplayVar();

};

#endif /* BANKANALOGINPUTS_H_ */
