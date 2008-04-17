/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: widget_kanjihwpad.h

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

#ifndef widget_kanjihwpad_h
#define widget_kanjihwpad_h

/*#include <gtkmm/aspectframe.h>*/
#include <gtkmm/frame.h>
#include <gtkmm/drawingarea.h>
#include <vector>
#include <list>

/*class KanjiHWPad : public Gtk::AspectFrame {*/
class KanjiHWPad : public Gtk::Frame {
public:
	KanjiHWPad();
	const std::vector<wchar_t>& GetResults();
	void Clear();
	void Update();
private:
	bool OnExpose(GdkEventExpose* event);
	bool OnMotion(GdkEventMotion* event);
	bool OnPress(GdkEventButton* event);
	bool OnRelease(GdkEventButton* event);
	void LookupChars();
	Gtk::DrawingArea da;

	std::vector<wchar_t> results;
	std::list< std::list< std::pair<int,int> > > listLines;
	std::list< std::pair<int,int> >* pCurrentLine;
};

#endif
