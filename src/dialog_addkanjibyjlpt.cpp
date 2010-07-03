/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dialog_addkanjibyjlpt.cpp

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

#include "dialog_addkanjibyjlpt.h"

#include <glibmm/i18n.h>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>
#include <cassert>

Glib::ustring jlptStrs[] = {
	_("Level 4 (Easy)"),
	_("Level 3"),
	_("Level 2"),
	_("Level 1 (Hard)")
};
int jlptStrCount = 4;

DialogAddKanjiByJLPT::DialogAddKanjiByJLPT(Gtk::Window& parent)
	: StoredDialog(_("Add Kanji By JLPT Level"), parent, "gui.dlg.addkanjibyfreq.size"),
	  btnOK(Gtk::Stock::OK),
	  btnCancel(Gtk::Stock::CANCEL)
{
	for(int i=0; i<jlptStrCount; i++) {
		comboLowLevel.append_text(jlptStrs[i]);
		comboHighLevel.append_text(jlptStrs[i]);
	}
	comboLowLevel.set_active_text(jlptStrs[0]);
	comboHighLevel.set_active_text(jlptStrs[0]);

	comboLowLevel.signal_changed()
		.connect(sigc::mem_fun(*this, &DialogAddKanjiByJLPT::OnLowValChange));
	comboHighLevel.signal_changed()
		.connect(sigc::mem_fun(*this, &DialogAddKanjiByJLPT::OnHighValChange));
	btnOK.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogAddKanjiByJLPT::OnOK));
	btnCancel.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogAddKanjiByJLPT::OnCancel));

	set_border_width(5);

	Gtk::VBox* pvb = get_vbox();
	pvb->set_spacing(5);
	pvb->pack_start(comboLowLevel);
	pvb->pack_start(comboHighLevel);

	Gtk::ButtonBox* phbb = get_action_area();
	phbb->pack_start(btnCancel);
	phbb->pack_start(btnOK);

	show_all_children();
}

int ComboBoxToJLPT(const Gtk::ComboBoxText& c) {
	int i;
	Glib::ustring uStr = c.get_active_text();
	for(i=0;i<jlptStrCount;i++)
		if(uStr==jlptStrs[i]) break;
	if(i<4) return 4-i;	/* 0-3 -> JLPT Level 4-1 */
	assert(0 && "Error: i>=4, should be less than 4.");
	return 0;           /* No jlpt listed in KANJIDIC */
}

int DialogAddKanjiByJLPT::GetLowLevel() {
	return ComboBoxToJLPT(comboLowLevel);
}

int DialogAddKanjiByJLPT::GetHighLevel() {
	return ComboBoxToJLPT(comboHighLevel);
}

void DialogAddKanjiByJLPT::OnOK() {
	OKProc();
}

void DialogAddKanjiByJLPT::OnCancel() {
	CancelProc();
}

void DialogAddKanjiByJLPT::OnLowValChange() {
	int low, high;
	low = GetLowLevel();
	high = GetHighLevel();
	/* Remember: with JLPT levels, a lower value is a higher level */
	if(low<high)
		comboHighLevel.set_active_text(comboLowLevel.get_active_text());
}

void DialogAddKanjiByJLPT::OnHighValChange() {
	int low, high;
	low = GetLowLevel();
	high = GetHighLevel();
	/* See note in above function. */
	if(high>low)
		comboLowLevel.set_active_text(comboHighLevel.get_active_text());
}

void DialogAddKanjiByJLPT::OKProc() {
	int l = GetLowLevel();
	int h = GetHighLevel();
	if(h>l) {
		Gtk::MessageDialog md
			(*this, _("Please specify your level range from highest to lowest "
					  "level number.  (Example: Level 4 to level 3)"));
		md.set_title(_("Bad JLPT level range"));
		md.run();
	} else
		response(Gtk::RESPONSE_OK);
}

void DialogAddKanjiByJLPT::CancelProc() {
	response(Gtk::RESPONSE_CANCEL);
}
