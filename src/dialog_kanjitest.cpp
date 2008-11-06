/*
Project: J-Ben
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File:         dialog_kanjitest.cpp
Author:       Paul Goins
Contributors: Alain Bertrand

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

#include "dialog_kanjitest.h"
#include <glibmm/i18n.h>
#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/dialog.h>
#include <gtkmm/messagedialog.h>
#include "kdict.h"
#include "errorlog.h"
#include "listmanager.h"
#include "encoding_convert.h"
#include "string_utils.h"
#include <boost/format.hpp>
#include "preferences.h"
#include "file_utils.h"

#include <sstream>
#include <map>
#include <gdkmm/pixbuf.h>

DialogKanjiTest::DialogKanjiTest
(Gtk::Window& parent, DialogKanjiPreTest& settings)
: StoredDialog(_("J-Ben: Kanji Test"), parent, "gui.dlg.kanjitest.size"),
	ctvKanji(_("< Kanji >")),
	ctvOnyomi(_("< Onyomi >")),
	ctvKunyomi(_("< Kunyomi >")),
	ctvMeaning(_("< Meaning >"),"",true,false)
{

	/* Create buttons dynamically to reflect keyboard shortcuts. */
	Preferences *prefs=Preferences::Get();
	string s = (boost::format(_("Show Answer (_%s)"))
		    % prefs->GetSetting("kanjitest.showanswer")).str();
	btnShowAnswer= manage (new Gtk::Button(s,true));
	s = (boost::format(_("Correct (_%s)"))
	     % prefs->GetSetting("kanjitest.correctanswer")).str();
	btnCorrect = manage(new Gtk::Button(s,true));
	s = (boost::format(_("Wrong (_%s)"))
	     % prefs->GetSetting("kanjitest.wronganswer")).str();
	btnWrong =  manage(new Gtk::Button(s,true));
	s = (boost::format(_("Stop Drill (_%s)"))
	     % prefs->GetSetting("kanjitest.stopdrill")).str();
	btnStop =  manage(new Gtk::Button(s,true));

	/* Init our required legal disclaimer */
	sodDisclaim.set_text(_("Kanji stroke order graphics used under license from KanjiCafe.com."));
	Pango::FontDescription fd(prefs->GetSetting("font.en.small"));
	sodDisclaim.modify_font(fd);

	/* Test setup */
	ListManager* lm = ListManager::Get();
	unsigned int i, startIndex;

	/* Reset test variables */
	currentKanjiIndex = -1;
	extraPractice = false;
	lastWasCorrect = false; /* used for Extra Practice mode only */
	totalToTest = settings.spnCount.get_value_as_int();
	testKanji.clear();
	missedKanji.clear();
	correct = totalTested = 0;
	if(settings.rdoReading.get_active())
		testMode = DKT_TM_Reading;
	else if(settings.rdoWriting.get_active())
		testMode = DKT_TM_Writing;
	else {
		/* Log an error; this shouldn't happen. */
		ostringstream oss;
		oss << ERR_PREF << "Unknown test type; defaulting to a reading test."
			"(This -is- a bug, please report it.)";
		el.Push(EL_Error, oss.str());
		/* Default to a reading test. */
		testMode = DKT_TM_Reading;
	}

	/* Create kanji test list based upon selected options */
	if(settings.rdoIndex.get_active()) {
		/* Copy from study list at specified index. */
		/* Remember, the GUI is 1-based. */
		startIndex = settings.spnIndex.get_value_as_int() - 1;
		for(i=startIndex; i < startIndex + totalToTest; i++)
			testKanji.push_back((*lm->KList())[i]);
	} else {
		/* Random character selection */
		vector<wchar_t> localVector = lm->KList()->GetVector();
		wchar_t c;
		while(testKanji.size()<totalToTest) {
			i = rand() % localVector.size();
			c = localVector[i];
			testKanji.push_back(c);
			localVector.erase(localVector.begin() + i);
		}
	}

	/* Load up the first kanji */
	ShowNextKanji();

	/* GUI setup */
	btnShowAnswer->signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiTest::OnShowAnswer));
	btnCorrect->signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiTest::OnCorrect));
	btnWrong->signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiTest::OnWrong));
	btnStop->signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiTest::OnStop));
	this->signal_delete_event()
		.connect(sigc::mem_fun(*this, &DialogKanjiTest::OnDeleteEvent));

	Gtk::HBox* phbScore = manage(new Gtk::HBox(false, 15));
	phbScore->pack_start(lblProgress, Gtk::PACK_SHRINK);
	phbScore->pack_end  (lblScore,    Gtk::PACK_SHRINK);

	Gtk::VBox* pvb = get_vbox();
	pvb->set_spacing(5);
	pvb->set_border_width(5);
	pvb->pack_start(*phbScore,  Gtk::PACK_SHRINK);
	pvb->pack_start(ctvKanji,   Gtk::PACK_SHRINK);
	if (testMode==DKT_TM_Writing){
		pvb->pack_start(sodDisplay, Gtk::PACK_SHRINK);
		pvb->pack_start(sodDisclaim, Gtk::PACK_SHRINK);
	}
	pvb->pack_start(ctvOnyomi,  Gtk::PACK_SHRINK);
	pvb->pack_start(ctvKunyomi, Gtk::PACK_SHRINK);
	pvb->pack_start(ctvMeaning, Gtk::PACK_SHRINK);

	Gtk::HButtonBox* phbbButtons = get_action_area();
	phbbButtons->pack_start(*btnShowAnswer, Gtk::PACK_SHRINK);
	phbbButtons->pack_start(*btnCorrect, Gtk::PACK_SHRINK);
	phbbButtons->pack_start(*btnWrong,   Gtk::PACK_SHRINK);
	phbbButtons->pack_start(*btnStop,    Gtk::PACK_SHRINK);

	show_all_children();
	sodDisplay.hide(); /* -might- cause a brief rendering blip when this
			      object is quicly shown and hidden? */
	sodDisclaim.hide();
}

void DialogKanjiTest::ShowNextKanji() {
	Preferences* prefs = Preferences::Get();
	long options= prefs->kanjidicOptions;
	const KDict* kd = KDict::Get();
	const KInfo* ki;
	/* Remove the current kanji, and get the new one */
	if(!extraPractice) {
		if(currentKanjiIndex!=-1)
			testKanji.erase(testKanji.begin()+currentKanjiIndex);
		if(testKanji.size()>0) {
			currentKanjiIndex = rand() % testKanji.size();
			currentKanji = testKanji[currentKanjiIndex];
		} else { /* If there's no more kanji to get... */
			if(missedKanji.size()>0) {
				extraPractice = true;
				testKanji = missedKanji;
			} else {
				response(Gtk::RESPONSE_OK);
				return;
			}
		}
	}
	if(extraPractice) {
		if(lastWasCorrect)
			testKanji.erase(testKanji.begin()+currentKanjiIndex);
		if(testKanji.size()>0) {
			currentKanjiIndex = rand() % testKanji.size();
			currentKanji = testKanji[currentKanjiIndex];
		} else { /* If there's no more kanji to get... */
			response(Gtk::RESPONSE_OK);
			return;
		}
	}

	/* Update CoveredTextBoxes with new info from KANJIDIC. */
	ki = kd->GetEntry(currentKanji);
	ctvKanji.Cover();
	ctvOnyomi.Cover();
	ctvKunyomi.Cover();
	ctvMeaning.Cover();
	ctvKanji.SetHiddenText(utfconv_wm(wstring(L"").append(1,currentKanji)));
	ctvOnyomi.SetHiddenText(ListToString<string>(ki->onyomi, "、"));
	ctvKunyomi.SetHiddenText(ListToString<string>(ki->kunyomi, "、"));
	ctvMeaning.SetHiddenText(ListToString<string>(ki->meaning, "、"));
	sodDisplay.hide();
	sodDisclaim.hide();
	sodDisplay.set("");
	sodPath = KDict::GetSodFileName(*ki, options);

	/* Update the test status label */
	double score=0.0, progress=0.0;
	if(totalTested>0) {
		score = round(100000 * (double)correct /
					  (double)totalTested) / 1000;
		progress = round(100000 * (double)totalTested /
						 (double)totalToTest) / 1000;
	}
	if(!extraPractice) {
		lblScore.set_text(
			(boost::format(_("Current score: %d/%d (%0.3f%%)"))
			 % correct % totalTested % score).str());
		lblProgress.set_text(
			(boost::format(_("Test Progress: %d/%d (%0.3f%% done)"))
			 % totalTested % totalToTest % progress).str());
	} else {
		lblScore.set_text(
			(boost::format(_("Final score: %d/%d (%0.3f%%)"))
			 % correct % totalTested % score).str());
		lblProgress.set_text(
			(boost::format(_("Extra practice: %d remaining"))
			 % testKanji.size()).str());
	}

	/* Uncover the field(s) appropriate for the current test mode. */
	switch(testMode) {
	case DKT_TM_Reading:
		ctvKanji.Show();
		if (prefs->GetSetting("kanjitest.kanjiread.showenglish")=="true")
			ctvMeaning.Show();
		if (prefs->GetSetting("kanjitest.kanjiread.showkunyomi")=="true")
			ctvKunyomi.Show();
		if (prefs->GetSetting("kanjitest.kanjiread.showonyomi")=="true")
			ctvOnyomi.Show();
		break;
	case DKT_TM_Writing:
		if (prefs->GetSetting("kanjitest.kanjiwrite.showenglish")=="true")
			ctvMeaning.Show();
		if (prefs->GetSetting("kanjitest.kanjiwrite.showkunyomi")=="true")
			ctvKunyomi.Show();
		if (prefs->GetSetting("kanjitest.kanjiwrite.showonyomi")=="true")
			ctvOnyomi.Show();
		break;
	}
}

void DialogKanjiTest::OnCorrect() {
	if(!extraPractice) {
		totalTested++;
		correct++;
	} else lastWasCorrect = true;
	ShowNextKanji();
}

void DialogKanjiTest::OnWrong() {
	if(!extraPractice) {
		totalTested++;
		missedKanji.push_back(currentKanji);
	} else lastWasCorrect = false;
	ShowNextKanji();
}

void DialogKanjiTest::OnStop() {
	/* If testing is not complete, prompt to make sure we want to quit. */
	int result = Gtk::RESPONSE_YES;
	if(testKanji.size()!=0) {
		Gtk::MessageDialog md(_("A test is still in progress!  Are you sure?"),
							  false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO);
		md.set_title(_("Test in progress!"));
		result = md.run();
	}
	if(result==Gtk::RESPONSE_YES) response(Gtk::RESPONSE_CANCEL);
}

bool DialogKanjiTest::OnDeleteEvent(GdkEventAny* event) {
	OnStop();
	return true;
}

void DialogKanjiTest::OnShowAnswer(){
	ctvKanji.Show();
	ctvMeaning.Show();
	ctvKunyomi.Show();
	ctvOnyomi.Show();
	if(FileExists(sodPath.c_str())) {
		sodDisplay.set(sodPath);
		sodDisplay.show();
		sodDisclaim.show();
	}
}

bool DialogKanjiTest::on_key_press_event (GdkEventKey* event){
	/* get prefs */
	bool result = false;
	Preferences *prefs=Preferences::Get();
	Glib::ustring sh,co,wr,st;
	/* get the shortcuts */
	sh=prefs->GetSetting("kanjitest.showanswer");
	co=prefs->GetSetting("kanjitest.correctanswer");
	wr=prefs->GetSetting("kanjitest.wronganswer");
	st=prefs->GetSetting("kanjitest.stopdrill");
	/* get the character */
	guint32 car=gdk_keyval_to_unicode(event->keyval);
	/* check if the character is one the shortcuts */
	if (sh[0]==car){
		btnShowAnswer->clicked();
		result = true;
	} else if (co[0]==car){
		btnCorrect->clicked();
		result = true;
	} else if  (wr[0]==car){
		btnWrong->clicked();
		result=true;
	} else if (st[0]==car){
		btnStop->clicked();
		result=true;
	}
	return result;
}
