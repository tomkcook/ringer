/*
 * BellEntity.cpp
 *
 *  Created on: 17 Jul 2012
 *      Author: tkcook
 */

#include "BellEntity.h"
#include <sstream>

static int materialNumber = 0;
static int MaterialNumber() { return materialNumber++; }

BellEntity::BellEntity(Ogre::SceneManager* mgr, Ogre::SceneNode* parent)
{
	std::stringstream materialName;
	materialName << "indicator_" << MaterialNumber();

	m_BellNode = parent->createChildSceneNode(Ogre::Vector3(0,0,0), Ogre::Quaternion::IDENTITY);
	m_BellEntity = mgr->createEntity("Bell.mesh");
	m_BellNode->attachObject(m_BellEntity);
	m_BellNode->translate(0, 0, 1.4);
	m_BellNode->scale(0.116, 0.116, 0.116);
	m_BellEntity->getSubEntity(0)->setMaterial(m_BellEntity->getSubEntity(0)->getMaterial()->clone(materialName.str()));
	m_ClapperNode = m_BellNode->createChildSceneNode(Ogre::Vector3(0,0,0), Ogre::Quaternion::IDENTITY);
	m_ClapperEntity = mgr->createEntity("Clapper.mesh");
	m_ClapperNode->attachObject(m_ClapperEntity);
	m_ClapperNode->translate(0, 0, -1.8);
	m_ClapperNode->scale(1.0/0.116, 1.0/0.116, 1.0/0.116);
	m_ClapperNode->setInheritOrientation(true);
}

BellEntity::~BellEntity()
{
}

void BellEntity::Position(double offset)
{
	m_BellNode->setPosition(0, offset, 0);
}

void BellEntity::Rotate(double angle)
{
	m_BellNode->setOrientation(Ogre::Quaternion(Ogre::Radian(angle), Ogre::Vector3(1, 0, 0)));
}

void BellEntity::SetClapper(double angle)
{
	m_ClapperNode->setOrientation(Ogre::Quaternion(Ogre::Radian(angle), Ogre::Vector3(1, 0, 0)));
}

void BellEntity::BellMotion(double bell, double clapper)
{
	Rotate(bell);
	SetClapper(clapper);
}

void BellEntity::Indicate(double value)
{
	m_BellEntity->getSubEntity(0)->getMaterial()->setSelfIllumination(value, 0, 0);
}


