#include "dialog_vocablisteditor.h"
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>
#include <glibmm/i18n.h>
#include <boost/format.hpp>
#include "listmanager.h"
#include "encoding_convert.h"

DialogVocabListEditor::DialogVocabListEditor(Gtk::Window& parent)
	: StoredDialog(_("Vocab List Editor"), parent, "gui.dlg.vocablisteditor.size"),
	  btnCancel(Gtk::Stock::CANCEL),
	  btnApply(Gtk::Stock::APPLY),
	  btnOK(Gtk::Stock::OK),
	  bChanged(false),
	  bChangesMade(false)
{
	tvList.set_accepts_tab(false);
	tvList.set_wrap_mode(Gtk::WRAP_WORD_CHAR);
	tvList.get_buffer()->signal_changed()
		.connect(sigc::mem_fun(*this, &DialogVocabListEditor::OnTextChanged));
	btnCancel.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogVocabListEditor::OnCancel));
	btnApply.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogVocabListEditor::OnApply));
	btnOK.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogVocabListEditor::OnOK));
	signal_delete_event()
		.connect(sigc::mem_fun(*this, &DialogVocabListEditor::OnDeleteEvent),
			false);

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

	Update();
	show_all_children();
}

void DialogVocabListEditor::OnTextChanged() {
	bChanged = true;
}

void DialogVocabListEditor::OnCancel() {
	Update(); /* Since the dialog is not destroyed, prepare it
				 for next time. */
	if(bChangesMade) {
		bChangesMade = false;
		response(Gtk::RESPONSE_OK);
	} else response(Gtk::RESPONSE_CANCEL);
}

void DialogVocabListEditor::OnApply() {
	bChanged = false;
	bChangesMade = true;
	ListManager* lm = ListManager::Get();
	lm->VList()->Clear();
	int result = lm->VList()->AddList
		(utfconv_mw(tvList.get_buffer()->get_text()).c_str(),
		 (void*)this);

	Update();

	Gtk::MessageDialog md
		(*this, (boost::format(_("Changes Saved.\nTotal vocab in list: %d"))
				 % result).str());
	md.set_title(_("Vocab List Editor"));
	md.run();
}

void DialogVocabListEditor::OnOK() {
	if(bChanged) OnApply();
	bChangesMade = false;
	response(Gtk::RESPONSE_OK);
}

void DialogVocabListEditor::Update() {
	bool bOldState = bChanged; /* Changing the text buffer will touch this flag,
								  so we'll save the state. */
	ListManager* lm = ListManager::Get();
	wstring ws = lm->VList()->ToString();
	string s;
	if(!ws.empty()) s = utfconv_wm(ws);
	tvList.get_buffer()->set_text(s);
	bChanged = bOldState;
}

bool DialogVocabListEditor::OnDeleteEvent(GdkEventAny* event) {
	OnCancel();
	return true;
}
