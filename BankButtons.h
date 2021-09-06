/*
 * BankButtons.h
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#ifndef BANKBUTTONS_H_
#define BANKBUTTONS_H_

#include "Button.h"

#define BTN0 	31
#define BTN1 	33
#define BTN2 	35
#define BTN3 	37

class BankButtons {

private:

public:

	Button *btn[4];

	BankButtons(Button::ButtonPressedHandler onBtn0,
			Button::ButtonPressedHandler onBtn1,
			Button::ButtonPressedHandler onBtn2,
			Button::ButtonPressedHandler onBtn3);

	void update() {
		for (int btnIndex = 0; btnIndex < 4; ++btnIndex) {
			btn[btnIndex]->update();
		}
	}

};

#endif /* BANKBUTTONS_H_ */
