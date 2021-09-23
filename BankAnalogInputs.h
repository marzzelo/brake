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

#define INPUT_MASS		47	// Frequency Counter
#define INPUT_PF		A0	// Analog input
#define INPUT_WHEEL		A1	// Analog input
#define INPUT_PH		A2	// Analog input
#define INPUT_T1		A3	// Temp 1
#define INPUT_T2		A4	// Temp 2

/*********************************
 * ROTARY ENCODER PINS
 *********************************/
#define PIN_IN1 		18	// SELECT PINS 2, 3, 18, 19, 20 or 21
#define PIN_IN2 		19	// SELECT PINS 2, 3, 18, 19, 20 or 21


/**
 * Controla todas las entradas anal�gicas del Banco de Freno, incluyendo:
 * - Velocidad de la rueda
 * - Presi�n de horquilla
 * - Presi�n de freno
 *
 * Controla tambi�n las entradas de pulsos, incluyendo:
 * - Velocidad de Masa (lector �ptico)
 */
class BankAnalogInputs {

private:
	int _period; 						//!< Periodo de muestreo de pulsos (FreqCount.h)

	int _pos = 0;  						//!< Rotary Encoder Position;

	volatile bool _daq_ready;
	volatile bool _daq_enabled;
	int _filter;
	volatile double _distance;			//<! Distancia recorrida durante el frenado
	double _t0 = 0;
	bool _counting;
	uint32_t _freqBuff[8] = {0};

	void (*_checkPosition)();

public:
	RotaryEncoder *encoder = nullptr;

	struct EncoderData {
		long position;
		int angle;
		int direction;
		unsigned long rpm;
	};

	EncoderData encoderData;

	volatile uint16_t wheel_daq_value;	//<! Velocidad de rueda (calibrar mediante men�)
	volatile uint16_t ph_daq_value;		//<! Presi�n de horquilla (calibrar mediante men�)
	volatile uint16_t pf_daq_value;		//<! Presi�n de freno (calibrar mediante men�)
	volatile uint16_t t1_daq_value;		//<! Temperatura 1
	volatile uint16_t t2_daq_value;		//<! Temperatura 2

	BankAnalogInputs(void (*checkPosition)(), int period = 1000, int filter = 1);

	void enable() { _daq_enabled = true; }

	void start() { _daq_enabled = true; _daq_ready = false; FreqCount.begin(_period); }

	bool ready() { return (FreqCount.available() && _daq_ready); }

	uint32_t getRpm();

	double getDistance();

	void startCounting();

	double stopCounting();

	void resetTimer();

	double getTime();

	void update();

	EncoderData encoderRead();

};

#endif /* BANKANALOGINPUTS_H_ */
