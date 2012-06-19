/*
 * Figure.h
 *
 *  Created on: 16 Jun 2012
 *      Author: tkcook
 */

#ifndef FIGURE_H_
#define FIGURE_H_

#include <Ogre.h>

class Figure {
public:
	Figure(Ogre::Entity* figure, int userId);
	virtual ~Figure();

	bool Update();
	Ogre::Entity* GetEntity() { return mFigure; }
protected:
	Ogre::Entity* mFigure;
	Ogre::Quaternion initialRotation;
	int mUserId;
};

#endif /* FIGURE_H_ */
