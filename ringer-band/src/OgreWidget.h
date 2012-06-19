/*
 * OgreWidget.h
 *
 *  Created on: 15 Jun 2012
 *      Author: tkcook
 */

#ifndef OGREWIDGET_H_
#define OGREWIDGET_H_

#include <gtkmm.h>
#include <glibmm/timer.h>
#include <Ogre.h>

class OgreWidget: public Gtk::Widget {
public:
	OgreWidget();
	virtual ~OgreWidget();

	void AddFigure(Ogre::Entity* figure, float* location);
	Ogre::SceneManager* GetSceneManager() { return mSceneMgr; }

protected:
      void createScene();

      virtual void on_size_request(Gtk::Requisition* requisition);
      virtual void on_size_allocate(Gtk::Allocation& allocation);
      virtual void on_map();
      virtual void on_unmap();
      virtual void on_realize();
      virtual void on_unrealize();
      virtual bool on_expose_event(GdkEventExpose* event);
      virtual bool on_idle();

      virtual bool on_motion_notify_event(GdkEventMotion *event);
      virtual bool on_button_press_event(GdkEventButton *event);
      virtual bool on_button_release_event(GdkEventButton *event);

      Glib::RefPtr<Gdk::Window> mRefGdkWindow;

      Ogre::RenderWindow* mRenderWindow;
      Ogre::SceneManager* mSceneMgr;
      Ogre::Viewport* mViewport;
      Ogre::Camera* mCamera;

};

#endif /* OGREWIDGET_H_ */
