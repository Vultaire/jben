#include "dialog_addkanjibyfreq.h"

#include <glibmm/i18n.h>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>

#include <iostream>
using namespace std;

DialogAddKanjiByFreq::DialogAddKanjiByFreq(Gtk::Window& parent)
	: Dialog(_("Add Kanji By Freq"), parent, true),
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

	Gtk::HButtonBox* phbb = get_action_area();
	phbb->pack_start(btnCancel);
	phbb->pack_start(btnOK);

	show_all_children();
}

int DialogAddKanjiByFreq::GetLowFreq() {
	cout << "GetLowFreq" << endl;
	return (int)(spinLowFreq.get_value());
}

int DialogAddKanjiByFreq::GetHighFreq() {
	cout << "GetHighFreq" << endl;
	return (int)(spinHighFreq.get_value());
}

void DialogAddKanjiByFreq::OnOK() {
	cout << "OnOK" << endl;
	OKProc();
}

void DialogAddKanjiByFreq::OnCancel() {
	cout << "OnCancel" << endl;
	CancelProc();
}

void DialogAddKanjiByFreq::OnLowValChange() {
	cout << "OnLowValChange" << endl;
	double low, high;
	low = spinLowFreq.get_value();
	high = spinHighFreq.get_value();
	if(low>high)
		spinHighFreq.set_value(low);
}

void DialogAddKanjiByFreq::OnHighValChange() {
	cout << "OnHighValChange" << endl;
	double low, high;
	low = spinLowFreq.get_value();
	high = spinHighFreq.get_value();
	if(high<low)
		spinLowFreq.set_value(high);
}

void DialogAddKanjiByFreq::OKProc() {
	cout << "OKProc" << endl;
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
	cout << "CancelProc" << endl;
	response(Gtk::RESPONSE_CANCEL);
}
