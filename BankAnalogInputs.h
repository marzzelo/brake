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

#define INPUT_MASS		47	// Frequency Counter
#define INPUT_PF		A0	// Analog input
#define INPUT_WHEEL		A1	// Analog input
#define INPUT_PH		A2	// Analog input
#define INPUT_T1		A3	// Temp 1
#define INPUT_T2		A4	// Temp 2


/**
 * Controla todas las entradas analógicas del Banco de Freno, incluyendo:
 * - Velocidad de la rueda
 * - Presión de horquilla
 * - Presión de freno
 *
 * Controla también las entradas de pulsos, incluyendo:
 * - Velocidad de Masa (lector óptico)
 */
class BankAnalogInputs {

private:
	int _period; //!< Periodo de muestreo de pulsos (FreqCount.h)

	volatile bool _daq_ready;
	volatile bool _daq_enabled;
	int _filter;
	volatile double _distance;			//<! Distancia recorrida durante el frenado
	bool _counting;

//public:   // debug only
	uint32_t _freqBuff[8] = {0};

public:

	volatile uint16_t wheel_daq_value;	//<! Velocidad de rueda (calibrar mediante menú)
	volatile uint16_t ph_daq_value;		//<! Presión de horquilla (calibrar mediante menú)
	volatile uint16_t pf_daq_value;		//<! Presión de freno (calibrar mediante menú)
	volatile uint16_t t1_daq_value;		//<! Temperatura 1
	volatile uint16_t t2_daq_value;		//<! Temperatura 2

	BankAnalogInputs(int period = 1000, int filter = 1);

	void enable() { _daq_enabled = true; }

	void start() { _daq_enabled = true; _daq_ready = false; FreqCount.begin(_period); }

	bool ready() { return (FreqCount.available() && _daq_ready); }

	uint32_t getRpm();

	double getDistance();

	void startCounting();

	double stopCounting();

	void update();
};

#endif /* BANKANALOGINPUTS_H_ */
