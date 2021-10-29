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
extern BankAnalogInputs *bankInputs;
extern BankLeds *bankLeds;


/**
 * Array of on-leaving functions.
 * NOTE: ORDER MUST FOLLOW THE ENUM  MainFSM::OnLeavings
 */
void (*mainOnLeavings[])(void) = {
	[]() {
		// LEAVING IDLE
		bankInputs->setTimeOut(120000);
//		bankKp->checkCommands = false;
	},

	// LEAVING OTHER
	[]() {
		bankInputs->setTimeOut(120000);
	},

	// LEAVING MONITORING
	[] () {
		bankLeds->relayStopAll();
		dprint = false;
		//matrix->setEffects(PA_LEFT, 35, 1000, PA_SCROLL_LEFT, PA_SCROLL_UP);
	}
};

#endif /* MAINONLEAVINGS_H_ */
