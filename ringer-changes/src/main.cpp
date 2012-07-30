#include <Display.h>
#include <Ogre.h>
#include <gtkmm/main.h>
#include <MainWindow.h>

int main(int argc, char** argv)
{
	try {
	Ogre::Root *root = new Ogre::Root();
	if(!root->restoreConfig() && !root->showConfigDialog())
		return -1;

	root->initialise(false);

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./data", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	Gtk::Main kit(argc, argv);

	MainWindow window;
	window.show();

	while(!window.hasExited()) {
		//root->renderOneFrame();
		kit.iteration();
	}
	} catch( Ogre::Exception& e) {
		std::cerr << "Caught unhandled Ogre exception: " << e.getFullDescription() << std::endl;
	}
//	delete root;
	return 0;
}
