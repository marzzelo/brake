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

class BankAnalogInputs {

private:
	int _PERIOD; // Periodo de muestreo de pulsos (FreqCount.h)

	volatile bool _daq_ready;
	volatile bool _daq_enabled;

public:

	volatile uint16_t wheel_daq_value;
	volatile uint16_t ph_daq_value;
	volatile uint16_t pf_daq_value;

	BankAnalogInputs(int period = 200);

	void enable() { _daq_enabled = true; }

	void start() { _daq_enabled = true; _daq_ready = false; FreqCount.begin(_PERIOD); }

	bool ready() { return (FreqCount.available() && _daq_ready); }

	uint32_t getRpm() { return FreqCount.read() * 1000 / _PERIOD; }

	void update();
};

#endif /* BANKANALOGINPUTS_H_ */
