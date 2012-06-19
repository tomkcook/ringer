/*
 * BandWindow.h
 *
 *  Created on: 15 Jun 2012
 *      Author: tkcook
 */

#ifndef BANDWINDOW_H_
#define BANDWINDOW_H_

#include <gtkmm.h>

#include "OgreWidget.h"

class BandWindow : public Gtk::Window {
public:
	BandWindow();
	virtual ~BandWindow();

	virtual bool on_delete_event(GdkEventAny *event);
	bool hasExited() { return mExited; }

	virtual bool on_key_press_event(GdkEventAny *event);
	virtual bool on_key_release_event(GdkEventAny *event);

	OgreWidget *GetOgreWidget() { return &mOgreWidget; }

protected:
	OgreWidget mOgreWidget;
	bool mExited;
};

#endif /* BANDWINDOW_H_ */
