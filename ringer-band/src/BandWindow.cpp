/*
 * BandWindow.cpp
 *
 *  Created on: 15 Jun 2012
 *      Author: tkcook
 */

#include "BandWindow.h"

BandWindow::BandWindow() :
	mOgreWidget(),
	mExited(false)
{
	set_border_width(10);

	Gtk::VBox *vb = new Gtk::VBox(false, 10);
	Gtk::Button *mb = new Gtk::Button("Some Button");

	vb->pack_start(*mb, true, true, 10);
	vb->pack_start(mOgreWidget, true, true, 10);

	add(*vb);
	show_all();

}

BandWindow::~BandWindow() {
}

bool BandWindow::on_delete_event(GdkEventAny* event) {
	mExited = true;
	return false;
}

bool BandWindow::on_key_press_event(GdkEventAny* event) {
	return true;
}

bool BandWindow::on_key_release_event(GdkEventAny* event) {
	return true;
}




