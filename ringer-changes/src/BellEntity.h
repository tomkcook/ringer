/*
 * BellEntity.h
 *
 *  Created on: 17 Jul 2012
 *      Author: tkcook
 */

#ifndef BELLENTITY_H_
#define BELLENTITY_H_
#include <Ogre.h>
#include <AL/al.h>
#include <AL/alut.h>

class BellEntity
{
public:
	BellEntity(Ogre::SceneManager* mgr, Ogre::SceneNode* parent);
	virtual ~BellEntity();

	void Position(double offset);
	void Rotate(double angle);
	void SetClapper(double angle);
	void BellMotion(double bell, double clapper);

	void Indicate(double value);

protected:
	Ogre::Entity* m_BellEntity;
	Ogre::SceneNode* m_BellNode;
	Ogre::Entity *m_ClapperEntity;
	Ogre::SceneNode* m_ClapperNode;
};

#endif /* BELLENTITY_H_ */
