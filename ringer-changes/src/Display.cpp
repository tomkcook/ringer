/*
 * Display.cpp
 *
 *  Created on: 11 Jul 2012
 *      Author: tkcook
 */

#include "Display.h"
#include <sstream>
using namespace std;

//#include <gdkmm/drawable.h>
#include <gdkmm/general.h>

#include <gdk/gdkx.h>

BellDisplay::BellDisplay(int bellCount)
: mRenderWindow(0),
  m_BellCount(bellCount)
{
	set_has_window(true);
}

BellDisplay::~BellDisplay()
{
}

void BellDisplay::on_size_allocate(Gtk::Allocation& allocation)
{
	Gtk::DrawingArea::on_size_allocate(allocation);
	set_allocation(allocation);

	if (mRenderWindow)
	{
		mRenderWindow->windowMovedOrResized();
		mCamera->setAspectRatio(Ogre::Real(allocation.get_width()) / Ogre::Real(allocation.get_height()));
	}
}

void BellDisplay::on_realize()
{
	//Call base class:
	Gtk::DrawingArea::on_realize();
	set_realized(true);
}

void BellDisplay::on_map()
{
	Gtk::DrawingArea::on_map();
}

void BellDisplay::initialise_render_window()
{
	Gtk::Allocation allocation = get_allocation();
	set_app_paintable(true);
	set_child_visible(true);

	Ogre::NameValuePairList params;
#ifdef WIN32
	params["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned long)GDK_WINDOW_HWND(mRefGdkWindow->gobj()));
#else
//		GdkWindow* parent = mRefGdkWindow->gobj();
	GdkWindow* parent = get_window()->gobj();
	GdkDisplay* display = gdk_window_get_display(parent);
	GdkScreen* screen = gdk_window_get_screen(parent);

	Display* xdisplay = GDK_DISPLAY_XDISPLAY(display);
	Screen* xscreen = GDK_SCREEN_XSCREEN(screen);
	int screen_number = XScreenNumberOfScreen(xscreen);
	XID xid_parent = GDK_WINDOW_XID(parent);

	params["parentWindowHandle"] =
			Ogre::StringConverter::toString(reinterpret_cast<unsigned long>(xdisplay)) + ":" +
			Ogre::StringConverter::toString(static_cast<unsigned int>(screen_number)) + ":" +
			Ogre::StringConverter::toString(static_cast<unsigned long>(xid_parent));

#endif
	mRenderWindow = Ogre::Root::getSingleton().createRenderWindow("Gtk+Ogre Widget",
			allocation.get_width(), allocation.get_height(), false, &params);

	mRenderWindow->setAutoUpdated(false);
	mRenderWindow->setVisible(true);

	set_has_window(true);
	set_double_buffered(true);

	mRenderWindow->setActive(true);
	mRenderWindow->setVisible(true);

	// Set default mipmap level & texture filtering
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);

	// Create scene manager
	mSceneMgr = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC, "SceneManager");

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7, 0.7));
	Ogre::Light* light = mSceneMgr->createLight("MainLight");
	light->setPosition(10, 40, 50);

	// Create the camera
	mCamera = mSceneMgr->createCamera("Camera");
	mCamera->setPosition(0, 0, 50);
	mCamera->lookAt(0, 0, 0);
	mCamera->setNearClipDistance(5);

	// Create one viewport, entire window
	mViewport = mRenderWindow->addViewport(mCamera);

	// Alter the camera aspect ratio to match the viewport
	mCamera->setFOVy(Ogre::Degree(15));
	mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));

	mSceneMgr->getRootSceneNode()->rotate(Ogre::Vector3(1, 0, 0), Ogre::Degree(90));
	mSceneMgr->getRootSceneNode()->rotate(Ogre::Vector3(0, 0, 1), Ogre::Degree(90));

	double offset = -10.0 * (m_BellCount - 1) / 2.0;
	for(int i = 0; i < m_BellCount; i++) {
		Ogre::SceneNode* parent = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		m_Bells.push_back(BellEntity(mSceneMgr, parent));
		parent->rotate(Ogre::Quaternion(Ogre::Radian(-atan2(offset, 50)), Ogre::Vector3::UNIT_Z));
		m_Bells[i].Position(offset);
		offset += 10;
	}
}

void BellDisplay::Update()
{
	Ogre::Root::getSingletonPtr()->_fireFrameStarted();
	mRenderWindow->update();
	Ogre::Root::getSingletonPtr()->_fireFrameEnded();
}

void BellDisplay::on_unrealize()
{
	//Call base class:
	Gtk::DrawingArea::on_unrealize();
}

std::string BellDisplay::GetFps()
{
	std::stringstream ss;
	ss << mRenderWindow->getLastFPS() << " fps";
	return ss.str();
}
