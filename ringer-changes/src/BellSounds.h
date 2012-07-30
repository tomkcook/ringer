/*
 * BellSounds.h
 *
 *  Created on: 23 Jul 2012
 *      Author: tkcook
 */

#ifndef BELLSOUNDS_H_
#define BELLSOUNDS_H_
#include "Bell.h"
#include <memory>

namespace Ogre
{
	class Camera;
};

class BellSounds
{
public:
	BellSounds(unsigned nBells, Ogre::Camera*);
	virtual ~BellSounds();

	void BellSounded(BellStroke stroke, int bell);

private:
	struct BellSoundsImpl;
	std::auto_ptr<BellSoundsImpl> pimpl;
};

#endif /* BELLSOUNDS_H_ */
