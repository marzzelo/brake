/*
 * BankAnalogInputs.h
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#ifndef BANKANALOGINPUTS_H_
#define BANKANALOGINPUTS_H_

#include "Arduino.h"

#define INPUT_PF		A0	// Analog input
#define INPUT_WHEEL		A1	// Analog input
#define INPUT_PH		A2	// Analog input

class BankAnalogInputs {

private:
	volatile bool _daq_ready;
	volatile bool _daq_enabled;

public:

	volatile uint16_t wheel_daq_value;
	volatile uint16_t ph_daq_value;
	volatile uint16_t pf_daq_value;

	BankAnalogInputs();

	void enable() { _daq_enabled = true; }

	void start() { _daq_enabled = true; _daq_ready = false; }

	bool ready() { return _daq_ready; }

	void update();
};

#endif /* BANKANALOGINPUTS_H_ */
