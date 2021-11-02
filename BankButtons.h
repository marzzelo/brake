/*

 * BankButtons.h
 *
 *  Created on: 6 sep. 2021
 *      Author: valdez
 */

#ifndef BANKBUTTONS_H_
#define BANKBUTTONS_H_

#include "Button.h"

#define BTN0 	31		//<! Start / Reset
#define BTN1 	33		//<! Select / Tare

class BankButtons {

private:
	volatile bool btn_pressed[2] = {false};
	volatile bool btn_longpressed[2] = {false};

public:

	enum ButtonState {
		RELEASED = 0,
		PRESSED,
		LONG_PRESSED,
	};

	enum ButtonNames {
		BTN_START_RESET = 0,
		BTN_SELECT_TARE
	};

	Button *btn[2];

	BankButtons(
		Button::ButtonPressedHandler onBtn0,
		Button::ButtonPressedHandler onLongBtn0,
		Button::ButtonPressedHandler onBtn1,
		Button::ButtonPressedHandler onLongBtn1
	);

	uint8_t read(int btnIndex);

	void setPressed(int btnIndex);

	void setLongPressed(int btnIndex);

	void update();

	void reset();

};

#endif /* BANKBUTTONS_H_ */
