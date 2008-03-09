#include "dialog_kanjilisteditor.h"
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>
#include <glibmm/i18n.h>
#include <boost/format.hpp>
#include "listmanager.h"
#include "encoding_convert.h"

#include <iostream>
using namespace std;

DialogKanjiListEditor::DialogKanjiListEditor(Gtk::Window& parent)
	: Dialog(_("Kanji List Editor"), parent, true),
	  btnCancel(Gtk::Stock::CANCEL),
	  btnApply(Gtk::Stock::APPLY),
	  btnOK(Gtk::Stock::OK),
	  bChanged(false) {
	tvList.set_accepts_tab(false);
	tvList.get_buffer()->signal_changed()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnTextChanged));
	btnCancel.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnCancel));
	btnApply.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnApply));
	btnOK.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnOK));
	
	Gtk::ScrolledWindow *pswTvList = manage(new Gtk::ScrolledWindow);
	pswTvList->add(tvList);
	pswTvList->set_shadow_type(Gtk::SHADOW_IN);
	pswTvList->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	Gtk::VBox* pvb = get_vbox();
	pvb->set_spacing(5);
	pvb->pack_start(*pswTvList);

	Gtk::HButtonBox* phbb = get_action_area();
	phbb->pack_start(btnCancel);
	phbb->pack_start(btnApply);
	phbb->pack_start(btnOK);

	show_all_children();
}

void DialogKanjiListEditor::OnTextChanged() {
	bChanged = true;
}

void DialogKanjiListEditor::OnCancel() {
	cout << "Cancel" << endl;
	response(Gtk::RESPONSE_CANCEL);
}

void DialogKanjiListEditor::OnApply() {
	cout << "Apply" << endl;
	bChanged = false;
	ListManager* lm = ListManager::Get();
	lm->KList()->Clear();
	int result = lm->KList()->AddFromString(
		utfconv_mw(tvList.get_buffer()->get_text()).c_str());

	Gtk::MessageDialog md(
		(boost::format(_("Changes Saved.\nTotal kanji in list: %d"))
		 % result).str());
	md.set_title(_("Kanji List Editor"));
	md.run();
}

void DialogKanjiListEditor::OnOK() {
	cout << "OK" << endl;
	if(bChanged) OnApply();
	response(Gtk::RESPONSE_OK);
}

void DialogKanjiListEditor::Update() {
}
