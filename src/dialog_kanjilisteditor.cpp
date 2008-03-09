#include "panel_kanjilisteditor.h"
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

PanelKanjiListEditor::PanelKanjiListEditor()
	: btnApply(Gtk::Stock::APPLY),
	  btnCancel(Gtk::Stock::CANCEL),
	  bChanged(false) {
	btnApply.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelKanjiListEditor::OnApply));
	btnCancel.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelKanjiListEditor::OnCancel));
	tvList.get_buffer()->signal_changed()
		.connect(sigc::mem_fun(*this, &PanelKanjiListEditor::OnTextChanged));
	tvList.set_accepts_tab(false);

	Gtk::ScrolledWindow *pswTxtList = manage(new Gtk::ScrolledWindow);
	Gtk::HButtonBox *phbbButtons
		= manage(new Gtk::HButtonBox(Gtk::BUTTONBOX_SPREAD));

	pswTxtList->add(tvList);
	pswTxtList->set_shadow_type(Gtk::SHADOW_IN);
	pswTxtList->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	phbbButtons->pack_start(btnApply, Gtk::PACK_EXPAND_PADDING);
	phbbButtons->pack_start(btnCancel, Gtk::PACK_EXPAND_PADDING);

	set_border_width(5);
	pack_start(*pswTxtList, Gtk::PACK_EXPAND_WIDGET);
	pack_start(*phbbButtons, Gtk::PACK_SHRINK);	
}

void PanelKanjiListEditor::OnCancel() {
	cout << "Cancel" << endl;
	bChanged = false;
	//Redraw();
}

void PanelKanjiListEditor::OnApply() {
	cout << "Apply" << endl;
	bChanged = false;
	ListManager* lm = ListManager::Get();
	lm->KList()->Clear();
	int result = lm->KList()->AddFromString(
		utfconv_mw(tvList.get_buffer()->get_text()).c_str());

	//FrameMainGUI::Get()->Redraw();
	//Redraw();	/* Might be redundant now with the above Redraw() call... */

	Gtk::MessageDialog md(
		(boost::format(_("Changes Saved.\nTotal kanji in list: %d"))
		 % result).str());
	md.set_title(_("Kanji List Editor"));
	md.run();
}

void PanelKanjiListEditor::OnTextChanged() {
	if(!bChanged) {
		cout << "TextChanged" << endl;
		bChanged = true;
	}
}

bool PanelKanjiListEditor::ChangeDetected() {return bChanged;}

void PanelKanjiListEditor::Update() {}
