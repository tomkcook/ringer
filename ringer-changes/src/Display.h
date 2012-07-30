/*
 * Display.h
 *
 *  Created on: 11 Jul 2012
 *      Author: tkcook
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <gtkmm.h>
#include <Ogre.h>
#include <string>
#include <vector>
#include "Bell.h"
#include "BellEntity.h"

using namespace std;

class BellDisplay: public Gtk::DrawingArea
{
private:
public:
	BellDisplay(int bellCount);
	virtual ~BellDisplay();

	void BellMoved(double bell, double clapper);

	std::string GetFps();

	BellEntity& operator[](int i) { return m_Bells[i]; }

	void Update();
	bool IsInitialised() { return mRenderWindow != 0; }
    void initialise_render_window();
    Ogre::Camera* Camera() { return mCamera; }

protected:
	virtual Gtk::SizeRequestMode get_request_mode_vfunc() const { return Gtk::SIZE_REQUEST_HEIGHT_FOR_WIDTH; }
	virtual void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const { minimum_width = natural_width = m_BellCount * 200; }
	virtual void get_preferred_height_vfunc(int& minimum_h, int& natural_h) const { minimum_h = natural_h = 200; }
	virtual void get_preferred_width_for_height_vfunc(int height, int& min_w, int& nat_w) const { min_w = nat_w = height * m_BellCount; }
	virtual void get_preferred_height_for_width_vfunc(int width, int& min_h, int& nat_h) const { min_h = nat_h = width / m_BellCount; }

    virtual void on_size_allocate(Gtk::Allocation& allocation);
    virtual void on_realize();
    virtual void on_unrealize();
    virtual void on_map();

    Ogre::RenderWindow* mRenderWindow;
    Ogre::SceneManager* mSceneMgr;
    Ogre::Viewport* mViewport;
    Ogre::Camera* mCamera;

public:
    std::vector<BellEntity> m_Bells;
    int m_BellCount;
};

#endif /* DISPLAY_H_ */
