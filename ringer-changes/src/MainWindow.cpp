/*
 * MainWindow.cpp
 *
 *  Created on: 12 Jul 2012
 *      Author: tkcook
 */
#include "MainWindow.h"
#include "Display.h"
#include "Controller.h"
#include "BellSounds.h"

#include <string>

struct MainWindow::MainWindowImpl
{
	MainWindowImpl(int nBells) :
		mOgreWidget(nBells),
		m_Controller(nBells),
		mExited(false),
		m_CurrentFps(new Gtk::Label("0 fps"))
	{
	}

	BellDisplay mOgreWidget;
	Controller m_Controller;
	bool mExited;
	Gtk::Label *m_CurrentFps;
	std::auto_ptr<BellSounds> m_Sounds;
};

Gtk::Button* MainWindow::CreateCommand(const std::string& name, void(Controller::* cmd)())
{
	Gtk::Button* b = new Gtk::Button(name);
	b->signal_clicked().connect(sigc::mem_fun(&pimpl->m_Controller, cmd));
	return b;
}

MainWindow::MainWindow()
	: pimpl(new MainWindowImpl(6))
{
	set_border_width(10);

	Gtk::VBox *vb = new Gtk::VBox(false, 10);
	Gtk::HBox *hb = new Gtk::HBox(true, 10);

	hb->pack_start(*CreateCommand("Ring Up Peal", &Controller::RingUp));
	hb->pack_start(*CreateCommand("Ring", &Controller::Ring));
	hb->pack_start(*CreateCommand("Cheat", &Controller::GoToTop));
	hb->pack_start(*CreateCommand("Change", &Controller::DoChange));
	hb->pack_start(*CreateCommand("Plain Hunt", &Controller::GoPlainHuntDoubles));

	hb->pack_start(*pimpl->m_CurrentFps, false, false, 10);
	vb->pack_start(*hb, true, true, 10);
	vb->pack_start(pimpl->mOgreWidget, true, true, 10);

	add(*vb);
	show_all();

	Glib::signal_idle().connect(sigc::mem_fun(this, &MainWindow::UpdateBells));

}

MainWindow::~MainWindow()
{
	// TODO Auto-generated destructor stub
}

bool MainWindow::hasExited()
{
	return pimpl->mExited;
}

bool MainWindow::UpdateBells()
{
	if(!pimpl->mOgreWidget.IsInitialised())
	{
		pimpl->mOgreWidget.initialise_render_window();
		pimpl->m_Sounds.reset(new BellSounds(6, pimpl->mOgreWidget.Camera()));
		for(unsigned i = 0; i < 6; i++) {
			pimpl->m_Controller[i].Moved.connect(sigc::mem_fun(&(pimpl->mOgreWidget[i]), &BellEntity::BellMotion));
			pimpl->m_Controller[i].Indicate.connect(sigc::mem_fun(&pimpl->mOgreWidget[i], &BellEntity::Indicate));
			pimpl->m_Controller[i].Sounded.connect(sigc::bind(sigc::mem_fun(pimpl->m_Sounds.get(), &BellSounds::BellSounded), i));
		}
	}

	// Update the bell model.  The transfer from the model to the view is by signals.
	pimpl->m_Controller.Update();

	// Update the FPS display
	pimpl->m_CurrentFps->set_text(pimpl->mOgreWidget.GetFps());

	// Update the bell display
	pimpl->mOgreWidget.Update();

	return true;
}

bool MainWindow::on_delete_event(GdkEventAny* event)
{
	pimpl->mExited = true;
	return false;
}
