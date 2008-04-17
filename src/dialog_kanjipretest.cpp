/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dialog_kanjipretest.cpp

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

#include "dialog_kanjipretest.h"
#include <gtkmm/stock.h>
#include <gtkmm/frame.h>
#include <glibmm/i18n.h>
#include "listmanager.h"

DialogKanjiPreTest::DialogKanjiPreTest(Gtk::Window& parent)
	: StoredDialog(_("Kanji Test: Settings"), parent, "gui.dlg.kanjipretest.size"),
	  rdoRandom(_("Choose randomly from list")),
	  rdoIndex(_("Start at index:")),
	  rdoReading(_("Reading kanji")),
	  rdoWriting(_("Writing kanji")),
	  btnCancel(Gtk::Stock::CANCEL),
	  btnStart(_("Start Test"))
{
	Gtk::RadioButton::Group g = rdoRandom.get_group();
	rdoIndex.set_group(g);
	g = rdoReading.get_group();
	rdoWriting.set_group(g);

	spnCount.signal_value_changed()
		.connect(sigc::mem_fun(*this, &DialogKanjiPreTest::OnKanjiCountChange));
	rdoRandom.signal_toggled()
		.connect(sigc::mem_fun(*this, &DialogKanjiPreTest::OnRdoRandom));
	rdoIndex.signal_toggled()
		.connect(sigc::mem_fun(*this, &DialogKanjiPreTest::OnRdoStartIndex));
	btnCancel.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiPreTest::OnCancel));
	btnStart.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiPreTest::OnStart));

	int iListSize = ListManager::Get()->KList()->Size();
	spnIndex.set_sensitive(false);
	spnCount.set_range(0.0, (double)iListSize);
	spnCount.set_numeric();
	spnIndex.set_numeric();
	spnCount.set_digits(0);
	spnIndex.set_digits(0);
	spnCount.set_range(0.0, (double)iListSize);
	spnCount.set_value(min(20, iListSize));
	if(iListSize>0) {
		spnIndex.set_range(1.0, (double)iListSize - spnCount.get_value() + 1.0);
		spnIndex.set_value(1.0);
	} else {
		spnIndex.set_range(0.0, 0.0);
		spnIndex.set_value(0.0);
	}
	spnCount.set_width_chars(5);
	spnIndex.set_width_chars(5);
	spnCount.set_increments(1,10);
	spnIndex.set_increments(1,10);

	Gtk::Frame* pfKanjiSelect
		= manage(new Gtk::Frame(_("Choose Kanji to Test")));
	Gtk::Frame* pfTestSelect
		= manage(new Gtk::Frame(_("Choose Test Type")));
	Gtk::HBox* phbCount = manage(new Gtk::HBox(false, 5));
	Gtk::HBox* phbIndex = manage(new Gtk::HBox(false, 5));
	Gtk::VBox* pvbKanjiSelect  = manage(new Gtk::VBox(false, 5));
	Gtk::VBox* pvbTestSelect   = manage(new Gtk::VBox(false, 5));
	Gtk::Label* plblCount
		= manage(new Gtk::Label(_("Number of kanji to test:")));

	Gtk::VBox* pvb = get_vbox();
	pvb->set_spacing(5);
	pvb->set_border_width(5);
	pvb->pack_start(*pfKanjiSelect,   Gtk::PACK_SHRINK);
	pvb->pack_start(*pfTestSelect,    Gtk::PACK_SHRINK);

	pfKanjiSelect->add(*pvbKanjiSelect);
	pfTestSelect ->add(*pvbTestSelect);

	pvbKanjiSelect->set_border_width(5);
	pvbKanjiSelect->pack_start(*phbCount, Gtk::PACK_SHRINK);
	pvbKanjiSelect->pack_start(rdoRandom, Gtk::PACK_SHRINK);
	pvbKanjiSelect->pack_start(*phbIndex, Gtk::PACK_SHRINK);

	pvbTestSelect->set_border_width(5);
	pvbTestSelect->pack_start(rdoReading, Gtk::PACK_SHRINK);
	pvbTestSelect->pack_start(rdoWriting, Gtk::PACK_SHRINK);

	phbCount->pack_start(*plblCount, Gtk::PACK_SHRINK);
	phbCount->pack_start(spnCount,   Gtk::PACK_SHRINK);

	phbIndex->pack_start(rdoIndex, Gtk::PACK_SHRINK);
	phbIndex->pack_start(spnIndex, Gtk::PACK_SHRINK);

	Gtk::HButtonBox* phbb = get_action_area();
	phbb->pack_start(btnCancel);
	phbb->pack_start(btnStart);

	OnKanjiCountChange(); /* Force a button update */
	show_all_children();
}

void DialogKanjiPreTest::OnKanjiCountChange() {
	int iListSize  = ListManager::Get()->KList()->Size();
	int iCount     = spnCount.get_value_as_int();
	bool bCanStart = btnStart.is_sensitive();
	if(iListSize > 0) {
		double dVal = spnIndex.get_value();
		double dMax = (double)iListSize - (double)iCount + 1.0;
		spnIndex.set_range(1.0, dMax);
		dVal = max(dVal, 1.0);
		dVal = min(dVal, dMax);
		spnIndex.set_value(dVal);		
	} else {
		spnIndex.set_range(0.0, 0.0);
		spnIndex.set_value(0.0);
	}
	if(iCount > 0) {
		if(!bCanStart) btnStart.set_sensitive(true);
	} else {
		if(bCanStart)  btnStart.set_sensitive(false);
	}
}

void DialogKanjiPreTest::OnRdoRandom() {
	spnIndex.set_sensitive(false);
}

void DialogKanjiPreTest::OnRdoStartIndex() {
	spnIndex.set_sensitive(true);
}

void DialogKanjiPreTest::OnCancel() {
	response(Gtk::RESPONSE_CANCEL);
}

void DialogKanjiPreTest::OnStart() {
	response(Gtk::RESPONSE_OK);
}
