/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dialog_kanjilisteditor.h

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

#ifndef dialog_kanjilisteditor_h
#define dialog_kanjilisteditor_h

#include "widget_storeddialog.h"
#include <gtkmm/button.h>
#include <gtkmm/textview.h>
#include "dialog_addkanjibygrade.h"
#include "dialog_addkanjibyjlpt.h"
#include "dialog_addkanjibyfreq.h"

class DialogKanjiListEditor : public StoredDialog {
public:
	DialogKanjiListEditor(Gtk::Window& parent);
	~DialogKanjiListEditor();
	void Update();
private:
	bool OnDeleteEvent(GdkEventAny* event);
	void OnTextChanged();
	bool ApplyIfNeeded();
	void OnAddFile();
	void OnAddGrade();
	void OnAddJLPT();
	void OnAddFreq();
	void OnSortGrade();
	void OnSortJLPT();
	void OnSortFreq();
	/*void OnSortBoth();*/
	void OnCancel();
	void OnApply();
	void OnOK();

	Gtk::TextView tvList;
	Gtk::Button btnAddFile, btnAddGrade, btnAddJLPT, btnAddFreq;
	Gtk::Button btnSortGrade, btnSortJLPT, btnSortFreq/*, btnSortBoth*/;
	Gtk::Button btnCancel, btnApply, btnOK;
	/* bChanged tracks for whether Apply is needed,
	   bChangesMade determines whether Cancel/Close return RESPONSE_OK. */
	bool bChanged, bChangesMade;
	DialogAddKanjiByGrade* pdAddByGrade;
	DialogAddKanjiByJLPT* pdAddByJLPT;
	DialogAddKanjiByFreq*  pdAddByFreq;
};

#endif
