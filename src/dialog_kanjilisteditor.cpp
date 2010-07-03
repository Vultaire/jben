/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dialog_kanjilisteditor.cpp

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

DialogKanjiListEditor::DialogKanjiListEditor(Gtk::Window& parent)
	: StoredDialog(_("Kanji List Editor"), parent,
				   "gui.dlg.kanjilisteditor.size"),
	  btnAddFile(  _("From File")),
	  btnAddGrade( _("By Jouyou Grade")),
	  btnAddJLPT(  _("By JLPT Level")),
	  btnAddFreq(  _("By Frequency")),
	  btnSortGrade(_("By Jouyou Grade")),
	  btnSortJLPT( _("By JLPT Level")),
	  btnSortFreq( _("By Frequency")),
	  /*btnSortBoth( _("By Both")),*/
	  btnCancel(Gtk::Stock::CANCEL),
	  btnApply(Gtk::Stock::APPLY),
	  btnOK(Gtk::Stock::OK),
	  bChanged(false),
	  bChangesMade(false)
{
	pdAddByGrade = NULL;
	pdAddByJLPT  = NULL;
	pdAddByFreq  = NULL;

	tvList.set_accepts_tab(false);
	tvList.set_wrap_mode(Gtk::WRAP_CHAR);
	tvList.get_buffer()->signal_changed()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnTextChanged));
	btnAddFile.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnAddFile));
	btnAddGrade.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnAddGrade));
	btnAddJLPT.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnAddJLPT));
	btnAddFreq.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnAddFreq));
	btnSortGrade.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnSortGrade));
	btnSortJLPT.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnSortJLPT));
	btnSortFreq.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnSortFreq));
#if 0
	btnSortBoth.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnSortBoth));
#endif
	btnCancel.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnCancel));
	btnApply.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnApply));
	btnOK.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnOK));
	signal_delete_event()
		.connect(sigc::mem_fun(*this, &DialogKanjiListEditor::OnDeleteEvent),
			false);
	
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
	pvbAdd->pack_start(btnAddFreq,  Gtk::PACK_SHRINK);
	pvbAdd->pack_start(btnAddJLPT, Gtk::PACK_SHRINK);
	pvbAdd->pack_start(btnAddGrade, Gtk::PACK_SHRINK);

	pvbSort->pack_start(btnSortFreq,  Gtk::PACK_SHRINK);
	pvbSort->pack_start(btnSortJLPT, Gtk::PACK_SHRINK);
	pvbSort->pack_start(btnSortGrade, Gtk::PACK_SHRINK);
#if 0
	pvbSort->pack_start(btnSortBoth,  Gtk::PACK_SHRINK);
#endif

	Gtk::ButtonBox* phbb = get_action_area();
	phbb->pack_start(btnCancel);
	phbb->pack_start(btnApply);
	phbb->pack_start(btnOK);

	Update();
	show_all_children();
}

DialogKanjiListEditor::~DialogKanjiListEditor() {
	if(pdAddByGrade) delete pdAddByGrade;
	if(pdAddByJLPT) delete pdAddByJLPT;
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
			md.set_title(_("Add Kanji by Jouyou Grade"));
			md.run();
		}
	}
}

void DialogKanjiListEditor::OnAddJLPT() {
	if(ApplyIfNeeded()) {
		ListManager* lm = ListManager::Get();
		if(!pdAddByJLPT)
			pdAddByJLPT = new DialogAddKanjiByJLPT(*this);
		int result = pdAddByJLPT->run();
		pdAddByJLPT->hide();
		if(result==Gtk::RESPONSE_OK) {
			result = lm->KList()->AddByJLPT(
				pdAddByJLPT->GetLowLevel(),
				pdAddByJLPT->GetHighLevel());

			Update();

			Gtk::MessageDialog md
				(*this, (boost::format(_("Added %d kanji to the list."))
						 % result).str());
			md.set_title(_("Add Kanji by JLPT Level"));
			md.run();
		}
	}
}

void DialogKanjiListEditor::OnAddFreq() {
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
	}
}

void DialogKanjiListEditor::OnSortGrade() {
	if(ApplyIfNeeded()) {
		ListManager* lm = ListManager::Get();
		lm->KList()->Sort(ST_GRADE);
		Update();
	}
}

void DialogKanjiListEditor::OnSortJLPT() {
	if(ApplyIfNeeded()) {
		ListManager* lm = ListManager::Get();
		lm->KList()->Sort(ST_JLPT);
		Update();
	}
}

void DialogKanjiListEditor::OnSortFreq() {
	if(ApplyIfNeeded()) {
		ListManager* lm = ListManager::Get();
		lm->KList()->Sort(ST_FREQUENCY);
		Update();
	}
}

#if 0
void DialogKanjiListEditor::OnSortBoth() {
	if(ApplyIfNeeded()) {
		ListManager* lm = ListManager::Get();
		lm->KList()->Sort(ST_FREQUENCY);
		lm->KList()->Sort(ST_GRADE);
		Update();
	}
}
#endif

void DialogKanjiListEditor::OnCancel() {
	Update(); /* Since the dialog is not destroyed, prepare it
				 for next time. */
	if(bChangesMade) {
		bChangesMade = false;
		response(Gtk::RESPONSE_OK);
	} else response(Gtk::RESPONSE_CANCEL);
}

void DialogKanjiListEditor::OnApply() {
	bChanged = false;
	bChangesMade = true;
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
	if(bChanged) OnApply();
	bChangesMade = false;
	response(Gtk::RESPONSE_OK);
}

void DialogKanjiListEditor::Update() {
	bool bOldState = bChanged; /* Changing the text buffer will touch this flag,
								  so we'll save the state. */
	ListManager* lm = ListManager::Get();
	wstring ws = lm->KList()->ToString(20);
	string s;
	if(!ws.empty()) s = utfconv_wm(ws);
	tvList.get_buffer()->set_text(s);
	bChanged = bOldState;
}

bool DialogKanjiListEditor::OnDeleteEvent(GdkEventAny* event) {
	OnCancel();
	return true;
}
