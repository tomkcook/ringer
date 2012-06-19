/*
 * KinectInterface.cpp
 *
 *  Created on: 16 Jun 2012
 *      Author: tkcook
 */

#include "OgreWidget.h"
#include "Figure.h"

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnPropNames.h>

#include "KinectInterface.h"

#include <map>
#include <iostream>
xn::Context KinectInterface::sContext;
xn::ScriptNode KinectInterface::sScriptNode;
xn::DepthGenerator KinectInterface::sDepthGenerator;
xn::UserGenerator KinectInterface::sUserGenerator;
Ogre::SceneManager *KinectInterface::sSceneMgr;
OgreWidget* KinectInterface::sDisplay;

typedef std::map<int, Figure*> FigureMap;

static FigureMap figures;

void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& /*generator*/, XnUserID nId, void* /*pCookie*/)
{
	KinectInterface::sUserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
	std::cout << "Got a new user." << std::endl;
}

void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& /*generator*/, XnUserID nId, void* /*pCookie*/)
{
	KinectInterface::sSceneMgr->destroySceneNode(figures[nId]->GetEntity()->getParentSceneNode());
	KinectInterface::sSceneMgr->destroyEntity(figures[nId]->GetEntity());
	figures[nId] = NULL;
}

void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& /*capability*/, const XnChar* strPose, XnUserID nId, void* /*pCookie*/)
{
	KinectInterface::sUserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	KinectInterface::sUserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}

void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& /*capability*/, XnUserID nId, void* /*pCookie*/)
{
}

void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& /*capability*/, XnUserID nId, XnCalibrationStatus eStatus, void* /*pCookie*/)
{
	if (eStatus == XN_CALIBRATION_STATUS_OK)
	{
		std::cout << "Adding a new figure to the scene";
		KinectInterface::sUserGenerator.GetSkeletonCap().StartTracking(nId);
		Ogre::Entity *entity = KinectInterface::sSceneMgr->createEntity("Sinbad.mesh");
		Figure *f = new Figure(entity, nId);
		XnPoint3D com;
		KinectInterface::sUserGenerator.GetCoM(nId, com);
		std::cout << " at " << com.X << ", " << com.Y << ", " << com.Z << std::endl;
		KinectInterface::sDisplay->AddFigure(entity, (float*)(&com));
		Glib::signal_idle().connect(sigc::mem_fun(f, &Figure::Update));
		figures[nId] = f;
	}
	else
	{
		KinectInterface::sUserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
	}
}

bool OpenNIUpdate() {
	KinectInterface::sContext.WaitAnyUpdateAll();
	return true;
}

void KinectInterface::Initialise(OgreWidget *display, Ogre::SceneManager* sceneManager)
{
	sDisplay = display;
	sSceneMgr = sceneManager;

	xn::EnumerationErrors errors;
	if(sContext.InitFromXmlFile("config.xml", sScriptNode, &errors) != XN_STATUS_OK)
	{
		std::cout << "Error initialising OpenNI:" << std::endl;
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		std::cout << strError << std::endl;
	}
	sContext.FindExistingNode(XN_NODE_TYPE_DEPTH, sDepthGenerator);
	if(sContext.FindExistingNode(XN_NODE_TYPE_USER, sUserGenerator) != XN_STATUS_OK)
	{
		std::cout << "Error finding user generator." << std::endl;
	}
	if(!sUserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON)) {
		std::cout << "User generator doesn't support skelingtons." << std::endl;
	}

	if(sUserGenerator.GetSkeletonCap().NeedPoseForCalibration()) {
		std::cout << "User generator is hard to calibrate." << std::endl;
	}

	XnCallbackHandle userCB, userCalCB, userCalFCB;
	sUserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, 0, userCB);
	sUserGenerator.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, 0, userCalCB);
	sUserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, 0, userCalFCB);
	sUserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
	sContext.StartGeneratingAll();
	Glib::signal_idle().connect(sigc::ptr_fun(OpenNIUpdate));
}

