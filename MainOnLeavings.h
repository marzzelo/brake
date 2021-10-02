/*
 * MainOnLeavings.h
 *
 *  Created on: 30 sep. 2021
 *      Author: valdez
 */

#ifndef MAINONLEAVINGS_H_
#define MAINONLEAVINGS_H_


extern void state_reset();
extern void setTimeOut(unsigned long dt);
extern BankAnalogInputs bankInputs;
extern BankLeds bankLeds;
extern MyLed7 *led7s;


/**
 * Array of on-leaving functions.
 * NOTE: ORDER MUST FOLLOW THE ENUM  MainFSM::OnLeavings
 */
void (*mainOnLeavings[])(void) = {
	[]() {
		// LEAVING IDLE
		setTimeOut(120000);
		bankKp->checkCommands(false);
	},

	// LEAVING OTHER
	[]() {
		setTimeOut(120000);
	},

	// LEAVING MONITORING
	[] () {
		bankLeds.relayStopAll();
	}
};

#endif /* MAINONLEAVINGS_H_ */
