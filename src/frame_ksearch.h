/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: frame_ksearch.h

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef frame_ksearch
#define frame_ksearch

#include <gtkmm/window.h>
#include <gtkmm/paned.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/button.h>
#include <gtkmm/textview.h>
#include <string>
#include <list>

class FrameKSearch : public Gtk::Window {
public:
	FrameKSearch();
	~FrameKSearch();
	void Clear();
	void Update();
private:
	void OnAdd();
	void OnDrop(Gtk::Widget *pWidget);
	bool OnDeleteEvent(GdkEventAny* event);
	void OnResultClick(Gtk::Button *pButton);
	void UpdateResultLayout();

	Gtk::VPaned panes;
	Gtk::Statusbar statusBar;
	Gtk::Frame topFrame, bottomFrame;
	Gtk::VBox topLayout;
	Gtk::VBox bottomLayout;
	Gtk::HBox searchBar;
	Gtk::Label searchLabel;
	Gtk::ComboBoxText searchOpts;
	Gtk::HButtonBox searchButtonsUpper;
	Gtk::Button btnAdd, btnClear;
	Gtk::ScrolledWindow searchGUIWindow;
	Gtk::VBox searchGUIBox;
	Gtk::HButtonBox searchButtonsLower;
	Gtk::Button btnSearch;
	Gtk::ScrolledWindow resultWindow;
	/* Result box implementation: 2 ideas
	   1. -BUNCH- of buttons, like gjiten
	   2. GtkTextView with event tags
	   First: trying bunch of buttons together with GtkTextView.
	   (Studying gjiten, it appears this is how they implemented their results
	   area... since I can't get an object's width easily w/o realizing it,
	   let's just realize it within a word-wrappable container.) */
	Gtk::TextView resultLayout;
	std::list<Gtk::Button *> resultButtonList;
};

#endif
