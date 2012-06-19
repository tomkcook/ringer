/*
 * SkeletonPositions.cpp
 *
 *  Created on: 17 Jun 2012
 *      Author: tkcook
 */

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnPropNames.h>
#include <Ogre.h>
#include "KinectInterface.h"
#include "SkeletonPositions.h"

SkeletonPositions::SkeletonPositions(unsigned int nId) {
	xn::SkeletonCapability skel = KinectInterface::sUserGenerator.GetSkeletonCap();
	skel.GetSkeletonJointPosition(nId, XN_SKEL_HEAD, Head);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_NECK, Neck);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_TORSO, Torso);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_WAIST, Waist);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_LEFT_COLLAR, Left.Collar);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_LEFT_SHOULDER, Left.Shoulder);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_LEFT_ELBOW, Left.Elbow);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_LEFT_WRIST, Left.Wrist);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_LEFT_HAND, Left.Hand);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_LEFT_FINGERTIP, Left.Fingertip);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_LEFT_HIP, Left.Hip);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_LEFT_KNEE, Left.Knee);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_LEFT_ANKLE, Left.Ankle);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_LEFT_FOOT, Left.Foot);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_RIGHT_COLLAR, Right.Collar);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_RIGHT_SHOULDER, Right.Shoulder);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_RIGHT_ELBOW, Right.Elbow);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_RIGHT_WRIST, Right.Wrist);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_RIGHT_HAND, Right.Hand);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_RIGHT_FINGERTIP, Right.Fingertip);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_RIGHT_HIP, Right.Hip);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_RIGHT_KNEE, Right.Knee);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_RIGHT_ANKLE, Right.Ankle);
	skel.GetSkeletonJointPosition(nId, XN_SKEL_RIGHT_FOOT, Right.Foot);
	KinectInterface::sUserGenerator.GetCoM(nId, CentreOfMass.position);
	CentreOfMass.fConfidence = 1;

	skel.GetSkeletonJointOrientation(nId, XN_SKEL_LEFT_SHOULDER, LeftShoulder);
}

SkeletonPositions::~SkeletonPositions() {
	// TODO Auto-generated destructor stub
}

