/*
 * MainWindow.h
 *
 *  Created on: 12 Jul 2012
 *      Author: tkcook
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <gtkmm.h>

#include <memory>
#include <string>

class Controller;

class MainWindow: public Gtk::Window
{
public:
	MainWindow();
	virtual ~MainWindow();

	virtual bool on_delete_event(GdkEventAny *event);
	bool hasExited();

protected:
	struct MainWindowImpl;
	std::auto_ptr<MainWindowImpl> pimpl;

	Gtk::Button* CreateCommand(const std::string& name, void(Controller::* cmd)());
	bool UpdateBells();
};

#endif /* MAINWINDOW_H_ */
