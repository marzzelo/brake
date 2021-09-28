/*
 * MenuTransitions.h
 *
 *  Created on: 28 sep. 2021
 *      Author: valdez
 */

#ifndef MENUTRANSITIONS_H_
#define MENUTRANSITIONS_H_



bool f1() {
	if (ev_key[1]) {
		ev_key[1] = false;
		return true;
	}
	return false;
}



#endif /* MENUTRANSITIONS_H_ */
