#include <gtkmm/main.h>

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnPropNames.h>

#include "BandWindow.h"
#include "KinectInterface.h"

int main(int argc, char** argv)
{
	Ogre::Root *root = new Ogre::Root();
	if(!root->restoreConfig() && !root->showConfigDialog())
		return -1;

	root->initialise(false);

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./data", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	Gtk::Main kit(argc, argv);

	BandWindow window;
	window.show();

	KinectInterface::Initialise(window.GetOgreWidget(), window.GetOgreWidget()->GetSceneManager());

	while(!window.hasExited()) {
		root->renderOneFrame();
		kit.iteration();
	}

	delete root;
	return 0;
}
