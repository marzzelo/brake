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
		Button::ButtonPressedHandler onLongBtn0,
		Button::ButtonPressedHandler onBtn1,
		Button::ButtonPressedHandler onLongBtn1
		) {

	btn[0] = new Button(BTN0, onBtn0, onLongBtn0);
	btn[1] = new Button(BTN1, onBtn1, onLongBtn1);

}

uint8_t BankButtons::read(int btnIndex) {

	int state = RELEASED;

	if (btn_pressed[btnIndex])
		state = PRESSED;

	if (btn_longpressed[btnIndex])
		state = LONG_PRESSED;

	btn_pressed[btnIndex] = false;
	btn_longpressed[btnIndex] = false;

	return state;
}

void BankButtons::setPressed(int btnIndex) {
	btn_pressed[btnIndex] = true;
}

void BankButtons::setLongPressed(int btnIndex) {
	btn_longpressed[btnIndex] = true;
}

void BankButtons::update() {
	btn[0]->update();
	btn[1]->update();
}

void BankButtons::reset() {
	btn_pressed[0] = false;
	btn_longpressed[0] = false;
	btn_pressed[1] = false;
	btn_longpressed[1] = false;
}
