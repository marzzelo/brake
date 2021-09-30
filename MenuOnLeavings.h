/*
 * MenuOnLeavings.h
 *
 *  Created on: 29 sep. 2021
 *      Author: valdez
 */

#ifndef MENUONLEAVINGS_H_
#define MENUONLEAVINGS_H_




/**
 * Array of on-leaving functions.
 * NOTE: ORDER MUST FOLLOW THE ENUM  MenuFSM::OnLeavings
 */
void (*menuOnLeavings[])(void) = {

	// IDLE ->
	[] () {
		// Serial << "\n------\nLEAVING ST_MENU_IDLE\n\n";
	},

	// MENU MAIN ->
	[ ] ( ) {
		checkCommands = false;
		keyPadRx->start( );
	},

};


#endif /* MENUONLEAVINGS_H_ */
