/*
 * KinectInterface.h
 *
 *  Created on: 16 Jun 2012
 *      Author: tkcook
 */

#ifndef KINECTINTERFACE_H_
#define KINECTINTERFACE_H_

class OgreWidget;

class KinectInterface {
public:
	static void Initialise(OgreWidget *display, Ogre::SceneManager* sceneManager);

	static xn::Context sContext;
	static xn::ScriptNode sScriptNode;
	static xn::DepthGenerator sDepthGenerator;
	static xn::UserGenerator sUserGenerator;
	static OgreWidget* sDisplay;
	static Ogre::SceneManager* sSceneMgr;
};

#endif /* KINECTINTERFACE_H_ */
