/*
 * SkeletonPositions.h
 *
 *  Created on: 17 Jun 2012
 *      Author: tkcook
 */

#ifndef SKELETONPOSITIONS_H_
#define SKELETONPOSITIONS_H_

class SkeletonPositions {
public:
	SkeletonPositions(unsigned int nId);
	virtual ~SkeletonPositions();

	XnSkeletonJointPosition CentreOfMass;
	XnSkeletonJointPosition Head;
	XnSkeletonJointPosition Neck;
	XnSkeletonJointPosition Torso;
	XnSkeletonJointPosition Waist;
	struct {
		XnSkeletonJointPosition Collar;
		XnSkeletonJointPosition Shoulder;
		XnSkeletonJointPosition Elbow;
		XnSkeletonJointPosition Wrist;
		XnSkeletonJointPosition Hand;
		XnSkeletonJointPosition Fingertip;

		XnSkeletonJointPosition Hip;
		XnSkeletonJointPosition Knee;
		XnSkeletonJointPosition Ankle;
		XnSkeletonJointPosition Foot;
	} Left, Right;

	XnSkeletonJointOrientation LeftShoulder;
};

#endif /* SKELETONPOSITIONS_H_ */
