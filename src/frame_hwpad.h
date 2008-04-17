/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: frame_hwpad.h

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

#ifndef frame_hwpad
#define frame_hwpad

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include "widget_kanjihwpad.h"
#include <string>

class FrameHWPad : public Gtk::Window {
public:
	FrameHWPad();
	~FrameHWPad();
	void Clear();
	void Update();
private:
	bool OnRelease(GdkEventButton* event);
	void OnKanjiClicked(unsigned int index);
	bool OnDeleteEvent(GdkEventAny* event);
	
	KanjiHWPad hwp;
	Gtk::HBox hbResults;
	Gtk::Button buttons[5];
	Gtk::Label btnLabels[5];
	Gtk::Button btnClear;
	std::string sOldFontStr;
};

#endif
