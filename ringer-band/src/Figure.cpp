/*
 * Figure.cpp
 *
 *  Created on: 16 Jun 2012
 *      Author: tkcook
 */

#include "Figure.h"

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnPropNames.h>

#include "SkeletonPositions.h"

#include <iostream>

Figure::Figure(Ogre::Entity *figure, int userId)
	: mFigure(figure), mUserId(userId)
{
	std::cout << "Mesh bounded by distance " << mFigure->getBoundingRadius() << std::endl;
	initialRotation = figure->getSkeleton()->getBone("Humerus.L")->getOrientation();
}

Figure::~Figure() {
	// TODO Auto-generated destructor stub
}

inline Ogre::Vector3 XnToOgre(XnVector3D const & vector) {
	return Ogre::Vector3(-vector.X, vector.Y, vector.Z);
}

/*
 * The following bones are available in the model:
 *
Sheath.R

Hand.R
ThumbMed.R
ThumbProx.R
ThumbDist.R
IndexFingerMed.R
IndexFingerDist.R
IndexFingerProx.R
MiddleFingerMed.R
MiddleFingerProx.R
MiddleFingerDist.R
RingFingerProx.R
RingFingerDist.R
RingFingerMed.R
PinkyDist.R
PinkyMed.R
PinkyProx.R
Ulna.R
Humerus.R
Clavicle.R

Handle.R

Calf.R
Foot.R
Toe.R

Cheek.R
Eye.R
Brow.R

Sheath.L
Handle.L

ThumbDist.L
ThumbMed.L
ThumbProx.L
IndexFingerDist.L
IndexFingerMed.L
IndexFingerProx.L
MiddleFingerMed.L
MiddleFingerProx.L
MiddleFingerDist.L
RingFingerMed.L
RingFingerDist.L
RingFingerProx.L
PinkyDist.L
PinkyMed.L
PinkyProx.L
Hand.L
Humerus.L
Ulna.L
Clavicle.L

Thigh.L
Calf.L
Foot.L
Toe.L

Cheek.L
Eye.L
Brow.L

Brow.C
Jaw
TongueMid
TongueBase
TongueTip
UpperLip
LowerLip
Head
Neck
Chest
Stomach
Waist
Root
 *
 */
bool Figure::Update()
{
	SkeletonPositions pos(mUserId);
	//mFigure->getParentSceneNode()->setPosition(XnToOgre(pos.CentreOfMass.position));
	Ogre::SkeletonInstance* skel = mFigure->getSkeleton();
	Ogre::Bone *bone = skel->getBone("Humerus.L");
	bone->setManuallyControlled(true);
	//bone->setPosition(XnToOgre(pos.Left.Elbow.position) / 20);

	float *arr = pos.LeftShoulder.orientation.elements;
	Ogre::Quaternion ogreShoulderRotation(Ogre::Matrix3(arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8]));
	ogreShoulderRotation = ogreShoulderRotation * Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3(0, 1, 0));
	Ogre::Quaternion currentShoulderRotation = bone->getOrientation();
	Ogre::Quaternion orientationOffset = ogreShoulderRotation * currentShoulderRotation.Inverse();
	//Ogre::Vector3 rot = XnToOgre(pos.Left.Shoulder.position) - XnToOgre(pos.Left.Elbow.position);
	bone->rotate(orientationOffset);

	return true;
}
