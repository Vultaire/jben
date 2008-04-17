/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dialog_kanjipretest.h

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

#ifndef dialog_kanjipretest_h
#define dialog_kanjipretest_h

#include "widget_storeddialog.h"
#include <gtkmm/radiobutton.h>
#include <gtkmm/spinbutton.h>

class DialogKanjiPreTest : public StoredDialog {
public:
   	DialogKanjiPreTest(Gtk::Window& parent);
	Gtk::RadioButton rdoRandom, rdoIndex, rdoReading, rdoWriting;
	Gtk::SpinButton spnCount, spnIndex;
private:
	void OnKanjiCountChange();
	void OnRdoRandom();
	void OnRdoStartIndex();
	void OnCancel();
	void OnStart();

	Gtk::Button btnCancel, btnStart;
};

#endif
