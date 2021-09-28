/*
 * BankButtons.h
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#ifndef BANKBUTTONS_H_
#define BANKBUTTONS_H_

#include "Button.h"

#define BTN0 	31		//<! Start
#define BTN1 	33		//<! Select
#define BTN2 	35
#define BTN3 	37		//<! Reset

class BankButtons {

private:
	bool btn_pressed[4] = {false};

public:

	Button *btn[4];

	BankButtons(Button::ButtonPressedHandler onBtn0,
			Button::ButtonPressedHandler onBtn1,
			Button::ButtonPressedHandler onBtn2,
			Button::ButtonPressedHandler onBtn3);

	bool read(int btnIndex) {
		bool state = btn_pressed[btnIndex];
		btn_pressed[btnIndex] = false;
		return state;
	}

	void setPressed(int btnIndex) {
		btn_pressed[btnIndex] = true;
	}

	void update() {
		for (int btnIndex = 0; btnIndex < 4; ++btnIndex) {
			btn[btnIndex]->update();
		}
	}

};

#endif /* BANKBUTTONS_H_ */
