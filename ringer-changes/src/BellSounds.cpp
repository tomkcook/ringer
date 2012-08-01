/*
 * BellSounds.cpp
 *
 *  Created on: 23 Jul 2012
 *      Author: tkcook
 */

#include "BellSounds.h"
#include <AL/al.h>
#include <AL/alut.h>
#include <Ogre.h>
#include <sstream>
#include <iostream>
// Support rings of up to 22 bells
unsigned semitoneOffsets[] = {
		0,  2,  4,  5,  7,  9,  11,
		12, 14, 16, 17, 19, 21, 23,
		24, 26, 28, 29, 31, 33, 35,
		36
};
struct BellSounds::BellSoundsImpl
{
	ALuint buffer[8];
	std::vector<ALuint> sources;
	unsigned m_BellCount;
};

BellSounds::BellSounds(unsigned nBells, Ogre::Camera* camera) :
		pimpl(new BellSoundsImpl())
{
	pimpl->m_BellCount = nBells;
	pimpl->sources = std::vector<ALuint>(nBells, 0);

	alutInit(0, 0);
	const ALfloat *listenerPos = camera->getPosition().ptr();
	ALfloat listenerVelocity[3] = { 0, 0, 0};
	Ogre::Vector3 axes[3];
	camera->getOrientation().ToAxes(axes[0], axes[1], axes[2]);
	ALfloat listenerOri[6];
	for(int ii = 0; ii < 3; ii++)
	{
		listenerOri[ii] = axes[0][ii];
		listenerOri[ii+3] = axes[1][ii];
	}

	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVelocity);
	alListenerfv(AL_ORIENTATION, listenerOri);

	for(unsigned i = 1; i < 9; i++) {
		std::stringstream ss;
		ss << "data/bell-" << i << ".wav";
		pimpl->buffer[i-1] = alutCreateBufferFromFile(ss.str().c_str());
		if(pimpl->buffer[i-1] == AL_NONE) {
			std::cout << "Error loading " << ss.str().c_str() << ": " << alutGetError() << std::endl;
			throw alutGetError();
		}
	}

	alGenSources(nBells, pimpl->sources.data());

	ALfloat sourcePos[] = { 0, 0, 0 };
	ALfloat sourceVel[] = { 0, 0, 0 };

	for(unsigned ii = 0; ii < nBells; ii++)
	{
		// Scale the pitch to the correct note for the bell
		alSourcef(pimpl->sources[ii], AL_PITCH, 1.0f);
		alSourcef(pimpl->sources[ii], AL_GAIN, 1.0f);
		alSourcefv(pimpl->sources[ii], AL_POSITION, sourcePos);
		alSourcefv(pimpl->sources[ii], AL_VELOCITY, sourceVel);
		alSourcei(pimpl->sources[ii], AL_BUFFER, pimpl->buffer[ii]);
		alSourcei(pimpl->sources[ii], AL_LOOPING, AL_FALSE);
	}
}

BellSounds::~BellSounds()
{
	// TODO Auto-generated destructor stub
}

#include <iostream>

void BellSounds::BellSounded(BellStroke stroke, int bell)
{
	alSourcePlay(pimpl->sources[bell]);
}


