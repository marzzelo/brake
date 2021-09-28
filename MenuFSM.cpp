/*
 * MenuFSM.cpp
 *
 *  Created on: 28 sep. 2021
 *      Author: valdez
 */

#include "MenuFSM.h"

MenuFSM::MenuFSM(bool (*f)(void)) {
	_menu = new StateMachine(MenuState::MENU_COUNT, N_TRANSITIONS);

	_st_menu = MenuState::ST_MENU_IDLE;

	_menu->AddTransition(ST_MENU_IDLE, ST_MENU_MAIN, f);

}


