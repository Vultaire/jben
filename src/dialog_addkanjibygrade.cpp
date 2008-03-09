#include "dialog_addkanjibygrade.h"

#include <glibmm/i18n.h>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>

#include <iostream>
using namespace std;

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
	: Dialog(_("Add Kanji By Grade"), parent, true),
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

	Gtk::HButtonBox* phbb = get_action_area();
	phbb->pack_start(btnOK);
	phbb->pack_start(btnCancel);

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
	cout << "GetLowGrade" << endl;
	return ComboBoxToKanjidicGrade(comboLowGrade);
}

int DialogAddKanjiByGrade::GetHighGrade() {
	cout << "GetHighGrade" << endl;
	return ComboBoxToKanjidicGrade(comboHighGrade);
}

void DialogAddKanjiByGrade::OnOK() {
	cout << "OnOK" << endl;
	OKProc();
}

void DialogAddKanjiByGrade::OnCancel() {
	cout << "OnCancel" << endl;
	CancelProc();
}

void DialogAddKanjiByGrade::OnLowValChange() {
	cout << "OnLowValChange" << endl;
	int low, high;
	low = GetLowGrade();
	high = GetHighGrade();
	/* 0 is a special code for non-graded kanji and is treated
	   as the highest grade level here. */
	if(high!=0 && (low>high || low==0))
		comboHighGrade.set_active_text(comboLowGrade.get_active_text());
}

void DialogAddKanjiByGrade::OnHighValChange() {
	cout << "OnHighValChange" << endl;
	int low, high;
	low = GetLowGrade();
	high = GetHighGrade();
	/* 0 is a special code for non-graded kanji and is treated
	   as the highest grade level here. */
	if(high!=0 && (high<low || low==0))
		comboLowGrade.set_active_text(comboHighGrade.get_active_text());
}

void DialogAddKanjiByGrade::OKProc() {
	cout << "OKProc" << endl;
	int l = GetLowGrade();
	int h = GetHighGrade();
	/* 0 is a special code for non-graded kanji and is treated
	   as the highest grade level here. */
	if((h<l && h!=0) || (l==0 && h!=0)) {
		Gtk::MessageDialog md
			(_("Your high grade level cannot be less than your low grade level."));
		md.set_title(_("Bad grade range"));
		md.run();
		//md.hide();
	} else
		response(Gtk::RESPONSE_OK);
}

void DialogAddKanjiByGrade::CancelProc() {
	cout << "CancelProc" << endl;
	response(Gtk::RESPONSE_CANCEL);
}
