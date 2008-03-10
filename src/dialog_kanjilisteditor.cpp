#include "dialog_kanjilisteditor.h"
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/stock.h>
#include <glibmm/i18n.h>
#include <boost/format.hpp>
#include "listmanager.h"
#include "encoding_convert.h"
#include "file_utils.h"
#include <list>

#include <iostream>
using namespace std;

DialogKanjiListEditor::DialogKanjiListEditor(Gtk::Window& parent)
	: Dialog(_("Kanji List Editor"), parent, true),
	  btnAddFile(  _("From File")),
	  btnAddGrade( _("By Grade")),
	  btnAddFreq(  _("By Frequency")),
	  btnSortGrade(_("By Grade")),
	  btnSortFreq( _("By Frequency")),
	  btnSortBoth( _("By Both")),
	  btnCancel(Gtk::Stock::CANCEL),
	  btnApply(Gtk::Stock::APPLY),
	  btnOK(Gtk::Stock::OK),
	  bChanged(false) {
	set_default_size(450, -1);

	pdAddByGrade = NULL;
	pdAddByFreq  = NULL;

	tvList.set_accepts_tab(false);
	tvList.set_wrap_mode(Gtk::WRAP_CHAR);
	tvList.get_buffer()->signal_changed()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnTextChanged));
	btnAddFile.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnAddFile));
	btnAddGrade.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnAddGrade));
	btnAddFreq.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnAddFreq));
	btnSortGrade.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnSortGrade));
	btnSortFreq.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnSortFreq));
	btnSortBoth.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnSortBoth));
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

	/* Create layout */
	Gtk::HBox* phbMain = manage(new Gtk::HBox(false, 5));
	Gtk::VBox* pvbSide = manage(new Gtk::VBox(false, 5)); /* Side buttons */

	get_vbox()->pack_start(*phbMain);
	phbMain->set_spacing(5);
	phbMain->pack_start(*pvbSide, Gtk::PACK_SHRINK);
	phbMain->pack_start(*pswTvList);

	Gtk::Frame* pfAdd  = manage(new Gtk::Frame(_("Add Kanji")));
	Gtk::Frame* pfSort = manage(new Gtk::Frame(_("Sort Kanji")));
	pfAdd ->set_shadow_type(Gtk::SHADOW_IN);
	pfSort->set_shadow_type(Gtk::SHADOW_IN);
	pvbSide->pack_start(*pfAdd,  Gtk::PACK_SHRINK);
	pvbSide->pack_start(*pfSort, Gtk::PACK_SHRINK);

	Gtk::VBox* pvbAdd  = manage(new Gtk::VBox);
	Gtk::VBox* pvbSort = manage(new Gtk::VBox);
	pfAdd ->add(*pvbAdd);
	pfSort->add(*pvbSort);

	pvbAdd->pack_start(btnAddFile,  Gtk::PACK_SHRINK);
	pvbAdd->pack_start(btnAddGrade, Gtk::PACK_SHRINK);
	pvbAdd->pack_start(btnAddFreq,  Gtk::PACK_SHRINK);

	pvbSort->pack_start(btnSortGrade, Gtk::PACK_SHRINK);
	pvbSort->pack_start(btnSortFreq,  Gtk::PACK_SHRINK);
	pvbSort->pack_start(btnSortBoth,  Gtk::PACK_SHRINK);

	Gtk::HButtonBox* phbb = get_action_area();
	phbb->pack_start(btnCancel);
	phbb->pack_start(btnApply);
	phbb->pack_start(btnOK);

	Update();
	show_all_children();
}

DialogKanjiListEditor::~DialogKanjiListEditor() {
	if(pdAddByGrade) delete pdAddByGrade;
	if(pdAddByFreq) delete pdAddByFreq;
}

void DialogKanjiListEditor::OnTextChanged() {
	bChanged = true;
}

bool DialogKanjiListEditor::ApplyIfNeeded() {
	if(!bChanged) return true;
	Gtk::MessageDialog md(*this, _("Unsaved changes are present.  These must be"
								   " saved before continuing.  Continue?"),
						  false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
	md.set_title(_("Unsaved changes detected"));
	int result = md.run();
	md.hide();
	if(result==Gtk::RESPONSE_YES) {
		OnApply();
		return true;
	}
	return false;
}

void DialogKanjiListEditor::OnAddFile() {
	cout << "OnAddFile" << endl;
	if(ApplyIfNeeded()) {
		ListManager* lm = ListManager::Get();
		Gtk::FileChooserDialog fcd(_("Import from file"));
		fcd.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		fcd.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
		fcd.set_select_multiple();
		int result = fcd.run();
		if(result==Gtk::RESPONSE_OK) {
			std::vector<Glib::ustring> filenames
				= fcd.get_filenames();
			wstring file, allFiles;
			for(size_t i=0; i<filenames.size(); i++) {
				if(ReadEncodedFile(filenames[i].c_str(), file)==REF_SUCCESS)
					allFiles.append(file);
			}
			int result = lm->KList()->AddFromString(allFiles);

			Update();

			Gtk::MessageDialog md
				(*this, (boost::format(_("Added %d kanji to the list."))
						 % result).str());
			md.set_title(_("Add Kanji From File"));
			md.run();
		}
	}
}

void DialogKanjiListEditor::OnAddGrade() {
	cout << "OnAddGrade" << endl;
	if(ApplyIfNeeded()) {
		ListManager* lm = ListManager::Get();
		if(!pdAddByGrade)
			pdAddByGrade = new DialogAddKanjiByGrade(*this);
		int result = pdAddByGrade->run();
		pdAddByGrade->hide();
		if(result==Gtk::RESPONSE_OK) {
			result = lm->KList()->AddByGrade(
				pdAddByGrade->GetLowGrade(),
				pdAddByGrade->GetHighGrade());

			Update();

			Gtk::MessageDialog md
				(*this, (boost::format(_("Added %d kanji to the list."))
						 % result).str());
			md.set_title(_("Add Kanji by Grade"));
			md.run();
		}
		cout << "Result: " << result << endl;
	}
}

void DialogKanjiListEditor::OnAddFreq() {
	cout << "OnAddFreq" << endl;
	if(ApplyIfNeeded()) {
		ListManager* lm = ListManager::Get();
		if(!pdAddByFreq)
			pdAddByFreq = new DialogAddKanjiByFreq(*this);
		int result = pdAddByFreq->run();
		pdAddByFreq->hide();
		if(result==Gtk::RESPONSE_OK) {
			result = lm->KList()->AddByFrequency(
				pdAddByFreq->GetLowFreq(),
				pdAddByFreq->GetHighFreq());

			Update();

			Gtk::MessageDialog md
				(*this, (boost::format(_("Added %d kanji to the list."))
						 % result).str());
			md.set_title(_("Add Kanji by Frequency"));
			md.run();
		}
		cout << "Result: " << result << endl;
	}
}

void DialogKanjiListEditor::OnSortGrade() {
	cout << "OnSortGrade" << endl;
	if(ApplyIfNeeded()) {
		ListManager* lm = ListManager::Get();
		lm->KList()->Sort(ST_GRADE);
		Update();
	}
}

void DialogKanjiListEditor::OnSortFreq() {
	cout << "OnSortFreq" << endl;
	if(ApplyIfNeeded()) {
		ListManager* lm = ListManager::Get();
		lm->KList()->Sort(ST_FREQUENCY);
		Update();
	}
}

void DialogKanjiListEditor::OnSortBoth() {
	cout << "OnSortBoth" << endl;
	if(ApplyIfNeeded()) {
		ListManager* lm = ListManager::Get();
		lm->KList()->Sort(ST_FREQUENCY);
		lm->KList()->Sort(ST_GRADE);
		Update();
	}
}

void DialogKanjiListEditor::OnCancel() {
	cout << "Cancel" << endl;
	Update(); /* Since the dialog is not destroyed, prepare it
				 for next time. */
	response(Gtk::RESPONSE_CANCEL);
}

void DialogKanjiListEditor::OnApply() {
	cout << "Apply" << endl;
	bChanged = false;
	ListManager* lm = ListManager::Get();
	lm->KList()->Clear();
	int result = lm->KList()->AddFromString(
		utfconv_mw(tvList.get_buffer()->get_text()).c_str());

	Update();

	Gtk::MessageDialog md
		(*this, (boost::format(_("Changes Saved.\nTotal kanji in list: %d"))
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
	cout << "Update" << endl;
	bool bOldState = bChanged; /* Changing the text buffer will touch this flag,
								  so we'll save the state. */
	ListManager* lm = ListManager::Get();
	wstring ws = lm->KList()->ToString(20);
	string s;
	if(ws.length()>0) s = utfconv_wm(ws);
	tvList.get_buffer()->set_text(s);
	bChanged = bOldState;
}
