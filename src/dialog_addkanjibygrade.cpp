/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dialog_addkanjibygrade.cpp

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

#include "dialog_addkanjibygrade.h"

#include <glibmm/i18n.h>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>

Glib::ustring strs[] = {
	_("Jouyou Kanji Grade 1"),
	_("Jouyou Kanji Grade 2"),
	_("Jouyou Kanji Grade 3"),
	_("Jouyou Kanji Grade 4"),
	_("Jouyou Kanji Grade 5"),
	_("Jouyou Kanji Grade 6"),
	_("Jouyou Kanji, General Usage"),
	_("Jinmeiyou Kanji (for names)"),
	_("Non-Jouyou/Non-Jinmeiyou Kanji")
};
int strCount = 9;

DialogAddKanjiByGrade::DialogAddKanjiByGrade(Gtk::Window& parent)
	: StoredDialog(_("Add Kanji By Jouyou Grade"), parent, "gui.dlg.addkanjibyfreq.size"),
	  btnOK(Gtk::Stock::OK),
	  btnCancel(Gtk::Stock::CANCEL)
{
	for(int i=0; i<9; i++) {
		comboLowGrade.append_text(strs[i]);
		comboHighGrade.append_text(strs[i]);
	}
	comboLowGrade.set_active_text(strs[0]);
	comboHighGrade.set_active_text(strs[0]);

	comboLowGrade.signal_changed()
		.connect(sigc::mem_fun(*this, &DialogAddKanjiByGrade::OnLowValChange));
	comboHighGrade.signal_changed()
		.connect(sigc::mem_fun(*this, &DialogAddKanjiByGrade::OnHighValChange));
	btnOK.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogAddKanjiByGrade::OnOK));
	btnCancel.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogAddKanjiByGrade::OnCancel));

	set_border_width(5);

	Gtk::VBox* pvb = get_vbox();
	pvb->set_spacing(5);
	pvb->pack_start(comboLowGrade);
	pvb->pack_start(comboHighGrade);

	Gtk::ButtonBox* phbb = get_action_area();
	phbb->pack_start(btnCancel);
	phbb->pack_start(btnOK);

	show_all_children();
}

int ComboBoxToKanjidicGrade(const Gtk::ComboBoxText& c) {
	int i;
	Glib::ustring uStr = c.get_active_text();
	for(i=0;i<strCount;i++)
		if(uStr==strs[i]) break;
	if(i<6) return i+1;	/* G1-G6 in KANJIDIC */
	if(i==6) return 8;  /* G8 (Jouyou Jr. High) in KANJIDIC */
	if(i==7) return 9;  /* G9 (Jinmeiyou) in KANJIDIC */
	return 0;           /* No grade listed in KANJIDIC */
}

int DialogAddKanjiByGrade::GetLowGrade() {
	return ComboBoxToKanjidicGrade(comboLowGrade);
}

int DialogAddKanjiByGrade::GetHighGrade() {
	return ComboBoxToKanjidicGrade(comboHighGrade);
}

void DialogAddKanjiByGrade::OnOK() {
	OKProc();
}

void DialogAddKanjiByGrade::OnCancel() {
	CancelProc();
}

void DialogAddKanjiByGrade::OnLowValChange() {
	int low, high;
	low = GetLowGrade();
	high = GetHighGrade();
	/* 0 is a special code for non-graded kanji and is treated
	   as the highest grade level here. */
	if(high!=0 && (low>high || low==0))
		comboHighGrade.set_active_text(comboLowGrade.get_active_text());
}

void DialogAddKanjiByGrade::OnHighValChange() {
	int low, high;
	low = GetLowGrade();
	high = GetHighGrade();
	/* 0 is a special code for non-graded kanji and is treated
	   as the highest grade level here. */
	if(high!=0 && (high<low || low==0))
		comboLowGrade.set_active_text(comboHighGrade.get_active_text());
}

void DialogAddKanjiByGrade::OKProc() {
	int l = GetLowGrade();
	int h = GetHighGrade();
	/* 0 is a special code for non-graded kanji and is treated
	   as the highest grade level here. */
	if((h<l && h!=0) || (l==0 && h!=0)) {
		Gtk::MessageDialog md
			(*this, _("Your high grade level cannot be less than your low "
					  "grade level."));
		md.set_title(_("Bad jouyou grade range"));
		md.run();
	} else
		response(Gtk::RESPONSE_OK);
}

void DialogAddKanjiByGrade::CancelProc() {
	response(Gtk::RESPONSE_CANCEL);
}
