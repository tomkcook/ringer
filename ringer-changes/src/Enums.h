/*
 * Enums.h
 *
 *  Created on: 24 Jul 2012
 *      Author: tkcook
 */

#ifndef ENUMS_H_
#define ENUMS_H_

enum BellCommand : unsigned char {
	LEAD_UP,
	RING_UP,
	RING_DOWN,
	LEAD,
	RING,
	HOLD_UP,
	CUT_IN,
	STAND,
	IDLE
};

enum BellStroke : unsigned char{
	NO_STROKE,
	HAND_STROKE,
	BACK_STROKE
};



#endif /* ENUMS_H_ */
