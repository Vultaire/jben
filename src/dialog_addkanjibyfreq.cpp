/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dialog_addkanjibyfreq.cpp

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

#include "dialog_addkanjibyfreq.h"

#include <glibmm/i18n.h>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>

DialogAddKanjiByFreq::DialogAddKanjiByFreq(Gtk::Window& parent)
	: StoredDialog(_("Add Kanji By Freq"), parent, "gui.dlg.addkanjibyfreq.size"),
	  btnOK(Gtk::Stock::OK),
	  btnCancel(Gtk::Stock::CANCEL)
{
	spinLowFreq .set_numeric();
	spinHighFreq.set_numeric();
	spinLowFreq .set_digits(0);
	spinHighFreq.set_digits(0);
	spinLowFreq .set_range(1,2501);
	spinHighFreq.set_range(1,2501);
	spinLowFreq .set_value(1);
	spinHighFreq.set_value(2501);
	spinLowFreq .set_width_chars(4);
	spinHighFreq.set_width_chars(4);
	spinLowFreq .set_increments(1,10);
	spinHighFreq.set_increments(1,10);

	spinLowFreq.signal_value_changed()
		.connect(sigc::mem_fun(*this, &DialogAddKanjiByFreq::OnLowValChange));
	spinHighFreq.signal_value_changed()
		.connect(sigc::mem_fun(*this, &DialogAddKanjiByFreq::OnHighValChange));
	btnOK.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogAddKanjiByFreq::OnOK));
	btnCancel.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogAddKanjiByFreq::OnCancel));

	set_border_width(5);

	Gtk::VBox* pvb = get_vbox();
	pvb->set_spacing(5);
	pvb->pack_start(spinLowFreq);
	pvb->pack_start(spinHighFreq);

	Gtk::ButtonBox* phbb = get_action_area();
	phbb->pack_start(btnCancel);
	phbb->pack_start(btnOK);

	show_all_children();
}

int DialogAddKanjiByFreq::GetLowFreq() {
	return (int)(spinLowFreq.get_value());
}

int DialogAddKanjiByFreq::GetHighFreq() {
	return (int)(spinHighFreq.get_value());
}

void DialogAddKanjiByFreq::OnOK() {
	OKProc();
}

void DialogAddKanjiByFreq::OnCancel() {
	CancelProc();
}

void DialogAddKanjiByFreq::OnLowValChange() {
	double low, high;
	low = spinLowFreq.get_value();
	high = spinHighFreq.get_value();
	if(low>high)
		spinHighFreq.set_value(low);
}

void DialogAddKanjiByFreq::OnHighValChange() {
	double low, high;
	low = spinLowFreq.get_value();
	high = spinHighFreq.get_value();
	if(high<low)
		spinLowFreq.set_value(high);
}

void DialogAddKanjiByFreq::OKProc() {
	int l = GetLowFreq();
	int h = GetHighFreq();
	if(h<l) {
		Gtk::MessageDialog md
			(*this, _("Your high frequency rank cannot be less "
					  "than your low frequency rank."));
		md.set_title(_("Bad frequency range"));
		md.run();
	} else
		response(Gtk::RESPONSE_OK);
}

void DialogAddKanjiByFreq::CancelProc() {
	response(Gtk::RESPONSE_CANCEL);
}
