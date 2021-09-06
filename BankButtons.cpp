/*
 * BankButtons.cpp
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#include "Arduino.h"
#include "BankButtons.h"
#include "Button.h"

BankButtons::BankButtons(
		Button::ButtonPressedHandler onBtn0,
		Button::ButtonPressedHandler onBtn1,
		Button::ButtonPressedHandler onBtn2,
		Button::ButtonPressedHandler onBtn3
	) {


	btn[0] = new Button(BTN0, onBtn0, NULL);
	btn[1] = new Button(BTN1, onBtn1, NULL);
	btn[2] = new Button(BTN2, onBtn2, NULL);
	btn[3] = new Button(BTN3, onBtn3, NULL);

}

