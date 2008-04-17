/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: widgeet_coveredtextview.h

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

#ifndef widget_coveredtextview_h
#define widget_coveredtextview_h

#include <gtkmm/frame.h>
#include <gtkmm/textview.h>

class CoveredTextView : public Gtk::Frame {
public:
	CoveredTextView(const Glib::ustring& cover="",
					const Glib::ustring& hidden="",
					const bool& covered=true,
					const bool& japanese=true);
	void Cover();
	void Show();
	void SetCoverText(const Glib::ustring& str);
	void SetHiddenText(const Glib::ustring& str);
private:
	bool OnClick(GdkEventButton* event);

	Gtk::TextView tv;
	Glib::RefPtr<Gtk::TextBuffer> ptbCover, ptbHidden;
	Gdk::Color cBase, cAlt;
};

#endif
