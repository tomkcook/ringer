/*
 * OgreWidget.cpp
 *
 *  Created on: 15 Jun 2012
 *      Author: tkcook
 */

#include "OgreWidget.h"

#include <gdkmm/drawable.h>
#include <gdkmm/general.h>

#include <gdk/gdkx.h>

#include <iostream>
#include <cstring>

#ifdef WIN32
#include <gdk/gdkwin32.h>
#endif

OgreWidget::OgreWidget() :
Glib::ObjectBase("ogrewidget"),
Gtk::Widget(),
mRenderWindow(0), mCamera(0), mSceneMgr(0), mViewport(0)
{
	set_flags(Gtk::NO_WINDOW);
	std::cout << "GType name: " << G_OBJECT_TYPE_NAME(gobj()) << std::endl;
}

OgreWidget::~OgreWidget()
{
}

void OgreWidget::AddFigure(Ogre::Entity* figure, float* location)
{
	Ogre::Quaternion quat = figure->getSkeleton()->getBone("Humerus.L")->getOrientation();
	Ogre::Vector3 axis;
	Ogre::Degree angle;
	quat.ToAngleAxis(angle, axis);
	std::cout << "Humerus initial rotation is " << angle << ", " << axis.x << ", " << axis.y << ", " << axis.z;
	Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("FigureNode");
	node->attachObject(figure);
	node->scale(20, 20, 20);
	node->translate(Ogre::Vector3(0, 0, 0));
	//node->translate(Ogre::Vector3(0, 0, 40));
	Ogre::ManualObject* myManualObject =  mSceneMgr->createManualObject("manual1");
	Ogre::SceneNode* myManualObjectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("manual1_node");

	Ogre::MaterialPtr myManualObjectMaterial = Ogre::MaterialManager::getSingleton().create("manual1Material","General");
	myManualObjectMaterial->setReceiveShadows(false);
	myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true);
	myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0);
	myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(0,0,1);
	myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1);

	Ogre::Bone* bone = figure->getSkeleton()->getBone("Humerus.L");
	myManualObject->begin("manual1Material", Ogre::RenderOperation::OT_LINE_LIST);
	std::cout << "Bone starts at " << bone->convertLocalToWorldPosition(bone->getPosition()) << std::endl;
	myManualObject->position(bone->convertLocalToWorldPosition(bone->getPosition()));
	myManualObject->position(bone->convertLocalToWorldPosition(bone->getPosition()) + Ogre::Vector3(10,0,0));
	// etc
	myManualObject->end();
	myManualObjectNode->scale(20, 20, 20);

	myManualObjectNode->attachObject(myManualObject);
}

void OgreWidget::on_size_request(Gtk::Requisition* requisition)
{
	*requisition = Gtk::Requisition();

	requisition->width = 800;
	requisition->height = 600;
}

void OgreWidget::on_size_allocate(Gtk::Allocation& allocation)
{
	set_allocation(allocation);

	if(mRefGdkWindow)
	{
		mRefGdkWindow->move_resize( allocation.get_x(), allocation.get_y(),
				allocation.get_width(), allocation.get_height() );
	}

	if (mRenderWindow)
	{
		mRenderWindow->windowMovedOrResized();
		mCamera->setAspectRatio(Ogre::Real(allocation.get_width()) / Ogre::Real(allocation.get_height()));
		on_expose_event(NULL);
	}
}

void OgreWidget::on_map()
{
	//Call base class:
	Gtk::Widget::on_map();
}

void OgreWidget::on_unmap()
{
	//Call base class:
	Gtk::Widget::on_unmap();
}

void OgreWidget::on_realize()
{
	//Call base class:
	Gtk::Widget::on_realize();

	Gtk::Allocation allocation = get_allocation();

	if(!mRefGdkWindow)
	{
		//Create the GdkWindow:
		GdkWindowAttr attributes;
		memset(&attributes, 0, sizeof(attributes));

		//Set initial position and size of the Gdk::Window:
		attributes.x = allocation.get_x();
		attributes.y = allocation.get_y();
		attributes.width = allocation.get_width();
		attributes.height = allocation.get_height();

		attributes.event_mask = get_events () | Gdk::EXPOSURE_MASK | Gdk::ALL_EVENTS_MASK ;
		attributes.window_type = GDK_WINDOW_CHILD;
		attributes.wclass = GDK_INPUT_OUTPUT;

		mRefGdkWindow = Gdk::Window::create(get_window(), &attributes, GDK_WA_X | GDK_WA_Y);

	}

	if (!mRenderWindow)
	{
		Ogre::NameValuePairList params;
#ifdef WIN32
		params["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned long)GDK_WINDOW_HWND(mRefGdkWindow->gobj()));
#else
		GdkWindow* parent = mRefGdkWindow->gobj();
		GdkDisplay* display = gdk_drawable_get_display(GDK_DRAWABLE(parent));
		GdkScreen* screen = gdk_drawable_get_screen(GDK_DRAWABLE(parent));

		Display* xdisplay = GDK_DISPLAY_XDISPLAY(display);
		Screen* xscreen = GDK_SCREEN_XSCREEN(screen);
		int screen_number = XScreenNumberOfScreen(xscreen);
		XID xid_parent = GDK_WINDOW_XWINDOW(parent);

		params["externalWindowHandle"] =
				Ogre::StringConverter::toString(reinterpret_cast<unsigned long>(xdisplay)) + ":" +
				Ogre::StringConverter::toString(static_cast<unsigned int>(screen_number)) + ":" +
				Ogre::StringConverter::toString(static_cast<unsigned long>(xid_parent));
#endif
		mRenderWindow = Ogre::Root::getSingleton().createRenderWindow("Gtk+Ogre Widget",
				allocation.get_width(), allocation.get_height(), false, &params);

		mRenderWindow->setAutoUpdated(false);

		unset_flags(Gtk::NO_WINDOW);

		set_window(mRefGdkWindow);

		set_double_buffered(true);
		//make the widget receive expose events
		mRefGdkWindow->set_user_data(gobj());
		mRefGdkWindow->set_back_pixmap(Glib::RefPtr<Gdk::Pixmap>(),false);

		createScene();

		// Start idle function for frame update/rendering
		Glib::signal_idle().connect( sigc::mem_fun(*this, &OgreWidget::on_idle) );

	}

}

void OgreWidget::on_unrealize()
{
	mRefGdkWindow.clear();
	//Call base class:
	Gtk::Widget::on_unrealize();
}

bool OgreWidget::on_expose_event(GdkEventExpose* event)
{

	if (mRenderWindow)
	{
		Ogre::Root::getSingletonPtr()->_fireFrameStarted();
		mRenderWindow->update();
		Ogre::Root::getSingletonPtr()->_fireFrameEnded();

	}
	return true;
}

bool OgreWidget::on_idle()
{
	on_expose_event(0);
	return true;
}

void OgreWidget::createScene()
{
	// Set default mipmap level & texture filtering
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);

	// Create scene manager
	mSceneMgr = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC, "SceneManager");

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5, 0.5));
	Ogre::Light* light = mSceneMgr->createLight("MainLight");
	light->setPosition(20, 80, 50);

	// Create the camera
	mCamera = mSceneMgr->createCamera("Camera");
	mCamera->setPosition(0, 0, 300);
	mCamera->lookAt(0, 0, 0);
	mCamera->setNearClipDistance(5);

	// Create one viewport, entire window
	mViewport = mRenderWindow->addViewport(mCamera);

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));
}

bool OgreWidget::on_motion_notify_event(GdkEventMotion *event) {
	return true;
}

bool OgreWidget::on_button_press_event(GdkEventButton *event) {
	return true;
}

bool OgreWidget::on_button_release_event(GdkEventButton *event) {
	return true;
}
