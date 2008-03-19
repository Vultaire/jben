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

#include <iostream>
#include <sstream>
#include <map>

DialogKanjiTest::DialogKanjiTest
(Gtk::Window& parent, DialogKanjiPreTest& settings)
	: StoredDialog(_("J-Ben: Kanji Test"), parent, "dlg.kanjitest.size"),
	  ctvKanji(_("< Kanji >")),
	  ctvOnyomi(_("< Onyomi >")),
	  ctvKunyomi(_("< Kunyomi >")),
	  ctvMeaning(_("< Meaning >")),
	  btnCorrect(_("Correct")),
	  btnWrong(_("Wrong")),
	  btnStop(_("Stop Drill"))
{
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
	btnCorrect.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiTest::OnCorrect));
	btnWrong.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogKanjiTest::OnWrong));
	btnStop.signal_clicked()
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
	pvb->pack_start(ctvOnyomi,  Gtk::PACK_SHRINK);
	pvb->pack_start(ctvKunyomi, Gtk::PACK_SHRINK);
	pvb->pack_start(ctvMeaning, Gtk::PACK_SHRINK);

	Gtk::HButtonBox* phbbButtons = get_action_area();
	phbbButtons->pack_start(btnCorrect, Gtk::PACK_SHRINK);
	phbbButtons->pack_start(btnWrong,   Gtk::PACK_SHRINK);
	phbbButtons->pack_start(btnStop,    Gtk::PACK_SHRINK);	

	show_all_children();
}

void DialogKanjiTest::ShowNextKanji() {
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
	std::map<string, list<string> >::const_iterator
		mslsi = ki->meaning.find("en");
	if(mslsi != ki->meaning.end()) {
		ctvMeaning.SetHiddenText(ListToString<string>(mslsi->second, "、"));
	} else {
		ostringstream oss;
		oss << "Unable to find english meaning in character "
			<< ki->literal << "!";
		el.Push(EL_Warning, oss.str());
	}

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
		break;
	case DKT_TM_Writing:
		ctvOnyomi.Show();
		ctvKunyomi.Show();
		ctvMeaning.Show();
		break;
	}
}

void DialogKanjiTest::OnCorrect() {
	cout << "Correct" << endl;
	if(!extraPractice) {
		totalTested++;
		correct++;
	} else lastWasCorrect = true;
	ShowNextKanji();
}

void DialogKanjiTest::OnWrong() {
	cout << "Wrong" << endl;
	if(!extraPractice) {
		totalTested++;
		missedKanji.push_back(currentKanji);
	} else lastWasCorrect = false;
	ShowNextKanji();
}

void DialogKanjiTest::OnStop() {
	cout << "Stop" << endl;
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
	cout << "DeleteEvent" << endl;
	OnStop();
	return true;
}
