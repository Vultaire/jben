/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: panel_kanjidrill.cpp

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

#include "panel_kanjidrill.h"
#include "global.h"
#include "maingui.h"
#include <stdlib.h>
#include <algorithm>
using namespace std;

enum {
	/* Controls to configure the test */
	ID_pnlConfig = 1,
	ID_spnKanjiCount,
	ID_sbKanjiSelect,
	ID_rdoRandom,
	ID_rdoStartIndex,
	ID_spnStartIndex,
	ID_rdoKanjiReading,
	ID_rdoKanjiWriting,
	ID_btnStart,
	/* Controls for use during the test */
	ID_pnlTest,
	ID_txtKanji,
	ID_txtOnyomi,
	ID_txtKunyomi,
	ID_txtEnglish,
	ID_btnCorrect,
	ID_btnWrong,
	ID_btnStop,
	ID_lblTestProgress
};

enum {
	PKD_TM_Reading = 1,
	PKD_TM_Writing
};

BEGIN_EVENT_TABLE(PanelKanjiDrill, wxPanel)
	EVT_SPINCTRL(ID_spnKanjiCount, PanelKanjiDrill::OnKanjiCountChange)
	EVT_RADIOBUTTON(ID_rdoRandom, PanelKanjiDrill::OnRdoRandom)
	EVT_RADIOBUTTON(ID_rdoStartIndex, PanelKanjiDrill::OnRdoStartIndex)
	EVT_BUTTON(ID_btnStart, PanelKanjiDrill::OnStart)

	EVT_BUTTON(ID_btnCorrect, PanelKanjiDrill::OnCorrect)
	EVT_BUTTON(ID_btnWrong, PanelKanjiDrill::OnWrong)
	EVT_BUTTON(ID_btnStop, PanelKanjiDrill::OnStop)
END_EVENT_TABLE()

#if 0
wxMenuBar *storedBar;
#endif

PanelKanjiDrill::PanelKanjiDrill(wxWindow *owner) : RedrawablePanel(owner) {
	/* Create test config controls */
	pnlConfig = new wxPanel(this, ID_pnlConfig);

	wxPanel *pnlKanjiSelect = new wxPanel(pnlConfig, wxID_ANY);
	wxStaticText *lblKanjiCount = new wxStaticText(pnlKanjiSelect, wxID_ANY, _T("Number of kanji to test: "));
	spnKanjiCount = new wxSpinCtrl(pnlKanjiSelect, ID_spnKanjiCount);
	spnKanjiCount->SetValue(20);
	rdoRandom = new wxRadioButton(pnlKanjiSelect, ID_rdoRandom, _T("Choose randomly from list"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	rdoStartIndex = new wxRadioButton(pnlKanjiSelect, ID_rdoStartIndex, _T("Start at index: "));
	spnStartIndex = new wxSpinCtrl(pnlKanjiSelect, ID_spnStartIndex);
	spnStartIndex->Enable(false);

	wxPanel *pnlTestSelect = new wxPanel(pnlConfig, wxID_ANY);
	rdoKanjiReading = new wxRadioButton(pnlTestSelect, ID_rdoKanjiReading, _T("Reading Kanji"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	rdoKanjiWriting = new wxRadioButton(pnlTestSelect, ID_rdoKanjiWriting, _T("Writing Kanji"));

	btnStart = new wxButton(pnlConfig, ID_btnStart, _T("Start Drill"));

	/* Create test controls */
	pnlTest = new wxPanel(this, ID_pnlTest);
	txtKanji = new CoveredTextBox(pnlTest, ID_txtKanji, _T("< Kanji >"));
	wxFont fnt = txtKanji->GetFont();
	fnt.SetPointSize(fnt.GetPointSize()+10);
	txtKanji->SetFont(fnt);
	txtOnyomi = new CoveredTextBox(pnlTest, ID_txtOnyomi, _T("< Onyomi >"));
	txtKunyomi = new CoveredTextBox(pnlTest, ID_txtKunyomi, _T("< Kunyomi >"));
	txtEnglish = new CoveredTextBox(pnlTest, ID_txtEnglish, _T("< English >"), _T("Some overly ridiculously long English string to see how resizing will work when uncovering a string which is too big for the current shape/size of the CoveredTextBox."));
	btnCorrect = new wxButton(pnlTest, ID_btnCorrect, _T("Correct"));
	btnWrong = new wxButton(pnlTest, ID_btnWrong, _T("Wrong"));
	btnStop = new wxButton(pnlTest, ID_btnStop, _T("Stop Drill"));
	lblTestProgress = new wxStaticText(pnlTest, ID_lblTestProgress, _T(""));

	/* Create sizers */
	/* Our custom radio box for selecting the kanji to test */
	wxStaticBoxSizer *kanjiConfigSizer = new wxStaticBoxSizer(wxVERTICAL, pnlKanjiSelect, _T("Choose Kanji to Test"));
	/* Row 1: Number of kanji to test */
	wxBoxSizer *kanjiCountSizer = new wxBoxSizer(wxHORIZONTAL);
	kanjiCountSizer->Add(lblKanjiCount, 0, wxALIGN_CENTER_VERTICAL);
	kanjiCountSizer->Add(spnKanjiCount, 0, wxALIGN_CENTER_VERTICAL);
	/* Row 2: Random kanji, or do range by start index */
	wxBoxSizer *kanjiSelectSizer = new wxBoxSizer(wxVERTICAL);
	kanjiSelectSizer->Add(rdoRandom, 1, wxALIGN_CENTER_VERTICAL);
	wxBoxSizer *kanjiStartIndexSizer = new wxBoxSizer(wxHORIZONTAL);
	kanjiStartIndexSizer->Add(rdoStartIndex, 0, wxALIGN_CENTER_VERTICAL);
	kanjiStartIndexSizer->Add(spnStartIndex, 0, wxALIGN_CENTER_VERTICAL);
	kanjiStartIndexSizer->AddStretchSpacer(1);
	kanjiSelectSizer->Add(kanjiStartIndexSizer, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL);
	/* Finally: Add both rows to the static box sizer */
	kanjiConfigSizer->Add(kanjiCountSizer, 0);
	kanjiConfigSizer->Add(kanjiSelectSizer, 0, wxEXPAND);
	pnlKanjiSelect->SetSizer(kanjiConfigSizer);

	/* Our custom radio box for selecting the test mode */
	wxStaticBoxSizer *testModeSizer = new wxStaticBoxSizer(wxVERTICAL, pnlTestSelect, _T("Choose Test Type"));
	testModeSizer->Add(rdoKanjiReading, 1);
	testModeSizer->Add(rdoKanjiWriting, 1);
	pnlTestSelect->SetSizer(testModeSizer);

	/* Create any other horizontal sizers */
	wxBoxSizer *correctWrongSizer = new wxBoxSizer(wxHORIZONTAL);
	correctWrongSizer->Add(btnCorrect, 0, wxRIGHT, 10);
	correctWrongSizer->Add(btnWrong);
	correctWrongSizer->AddStretchSpacer(1);
	correctWrongSizer->Add(btnStop);
	/* The config sizer */
	wxBoxSizer *configSizer = new wxBoxSizer(wxVERTICAL);
	configSizer->Add(pnlKanjiSelect, 0, wxEXPAND | wxBOTTOM, 10);
	configSizer->Add(pnlTestSelect, 0, wxEXPAND | wxBOTTOM, 10);
	configSizer->Add(btnStart, 0, wxALIGN_CENTER_HORIZONTAL);
	pnlConfig->SetSizer(configSizer);
	/* The test mode sizer */
	wxBoxSizer *testSizer = new wxBoxSizer(wxVERTICAL);
	testSizer->Add(txtKanji,   0, wxEXPAND | wxBOTTOM, 10);
	testSizer->Add(txtOnyomi,  0, wxEXPAND | wxBOTTOM, 10);
	testSizer->Add(txtKunyomi, 0, wxEXPAND | wxBOTTOM, 10);
	testSizer->Add(txtEnglish, 0, wxEXPAND | wxBOTTOM, 10);
	testSizer->Add(correctWrongSizer, 0, wxEXPAND);
	testSizer->AddStretchSpacer(1);
	testSizer->Add(lblTestProgress, 0, wxEXPAND);
	pnlTest->SetSizer(testSizer);
	/* The panel-wide sizer */
	wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
	panelSizer->Add(pnlConfig, 1, wxEXPAND | wxALL, 10);
	panelSizer->Add(pnlTest, 1, wxEXPAND | wxALL, 10);

	testing=false;
	pnlTest->Show(false);               /* Set the test panel as invisible at the beginning */
	this->SetSizerAndFit(panelSizer);   /* Now apply the sizer afterwards so layout occurs properly. */
}

void PanelKanjiDrill::OnStart(wxCommandEvent& ev) {
	/* Placeholder code */
	/* We need to do the following:
	   0. (Possibly) Verify that test mode is NOT enabled when this event is triggered
	      (In case the button can somehow be pushed when not visible.)  (Done)
	   0.5. (Possibly) Validate the input of the user.  Might not be necessary if all controls limit input properly.
	      (They do now.)
	   1. Reset any test variables
	   2. Create kanji test list based upon selected options
	   3. Load up the first kanji to be tested
	   4. Switch to the test GUI */

	unsigned int i, startIndex;
	if(!testing) {
		/* Reset test variables */
		currentKanjiIndex = -1;
		extraPractice = false;
		lastWasCorrect = false; /* used for Extra Practice mode only */
		totalToTest = spnKanjiCount->GetValue();
		testKanji.clear();
		missedKanji.clear();
		correct = totalTested = 0;
		if(rdoKanjiReading->GetValue()) testMode = PKD_TM_Reading;
		else if(rdoKanjiWriting->GetValue()) testMode = PKD_TM_Writing;
		else {
			wxMessageBox(_T("Unknown test mode selected!"), _T("Error!"), wxOK | wxICON_ERROR, this);
			return;
		}

		/* Create kanji test list based upon selected options */
		if(rdoStartIndex->GetValue()) {
			/* Copy from study list at specified index */
			startIndex = spnStartIndex->GetValue() - 1;  /* Remember, the GUI uses a 1-base. */
			for(i=startIndex; i < startIndex + totalToTest; i++)
				testKanji.push_back((*jben->kanjiList)[i]);
		} else {
			/* Random character selection */
			vector<wxChar> localVector = jben->kanjiList->GetVector();
			wxChar c;
			while(testKanji.size()<totalToTest) {
				i = rand() % localVector.size();
				c = localVector[i];
				testKanji.push_back(c);
				localVector.erase(localVector.begin() + i);
			}
		}

		/* Debug: Show our list of kanji to test */
#ifdef DEBUG
		printf("DEBUG: Kanji to test: ");
		for(vector<wxChar>::iterator vi=testKanji.begin();vi!=testKanji.end();vi++)
			printf("%lc", *vi);
		printf("\n");
#endif

		/* Load up the first kanji */
		ShowNextKanji();

		/* Switch the displayed panel */
		jben->gui->GetMenuBar()->EnableTop(GUI_menuKanji, false);
		pnlConfig->Show(false);
		pnlTest->Show(true);
		pnlTest->SetFocus();
		this->GetSizer()->Layout();
		testing=true;
	}
}

void PanelKanjiDrill::Stop() {
	double score=0.0;
	if(totalTested>0) score = round(100000*(double)correct/(double)totalTested)/1000;
	wxString kanjiMissed;
	for(vector<wxChar>::iterator vi=missedKanji.begin(); vi!=missedKanji.end(); vi++)
		kanjiMissed.append(*vi);

	/* Display test results */
	if(totalTested==totalToTest) {
		wxMessageBox(
			wxString::Format(_T("TEST COMPLETED\n\nFinal Score: %d/%d (%0.3f%%)\nKanji Missed: %s"),
				correct, totalTested, score, kanjiMissed.c_str()),
			_T("Test Results"), wxOK | wxICON_INFORMATION, this);
	} else {
		wxMessageBox(
			wxString::Format(_T("TEST INCOMPLETE\n\nScore: %d/%d (%0.3f%%)\nTest Progress: %d/%d (%0.3f%%)\nKanji Missed: %s"),
				correct, totalTested, score, totalTested, totalToTest,
				round(100000*(double)totalTested/(double)totalToTest)/1000,
				kanjiMissed.c_str()),
			_T("Test Results"), wxOK | wxICON_INFORMATION, this);
	}

	/* Switch the displayed panel */
	pnlTest->Show(false);
	pnlConfig->Show(true);
	pnlConfig->SetFocus();
	this->GetSizer()->Layout();
	jben->gui->GetMenuBar()->EnableTop(GUI_menuKanji, true);
	testing=false;
}

void PanelKanjiDrill::OnStop(wxCommandEvent& ev) {
	if(testing) {
		/* If testing is not complete, prompt to make sure we want to quit. */
		int result = wxYES;
		if(testKanji.size()!=0) {
			result = wxMessageBox(_T("A test is still in progress!  Are you sure?"), _T("Test in progress!"), wxYES_NO | wxICON_EXCLAMATION, this);
		}
		if(result==wxYES) {
			Stop();
		}
	}
}

bool PanelKanjiDrill::TestInProgress() {return testing;}

void PanelKanjiDrill::UpdateKanjiCountSpinner() {
	int max = jben->kanjiList->Size();
	if(max>0) {
		spnKanjiCount->SetRange(1, max);
		int i = spnKanjiCount->GetValue();
		if(i<1) {i=1; spnKanjiCount->SetValue(1);}
		if(i>max) {i=max; spnKanjiCount->SetValue(max);}
	}
}

void PanelKanjiDrill::UpdateStartIndexSpinner() {
	int max = jben->kanjiList->Size();
	int i = spnKanjiCount->GetValue();
	int indexMax = max - (i-1);
	spnStartIndex->SetRange(1, indexMax);
	i = spnStartIndex->GetValue();
	if(i<1) spnStartIndex->SetValue(1);
	if(i>indexMax) spnStartIndex->SetValue(indexMax);
}

void PanelKanjiDrill::OnKanjiCountChange(wxSpinEvent& ev) {
	UpdateStartIndexSpinner();
}

void PanelKanjiDrill::Redraw() {
	if(!testing) {
		int max = jben->kanjiList->Size();
		if(max>0) {
			UpdateKanjiCountSpinner();
			UpdateStartIndexSpinner();
			this->Enable(true);
		} else {
			this->Enable(false);
		}
	}
}

void PanelKanjiDrill::OnRdoRandom(wxCommandEvent& ev) {
	spnStartIndex->Enable(false);
}

void PanelKanjiDrill::OnRdoStartIndex(wxCommandEvent& ev) {
	spnStartIndex->Enable(true);
}

void PanelKanjiDrill::ShowNextKanji() {
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
			}
			else Stop();
		}
	}
	if(extraPractice) {
		if(lastWasCorrect)
			testKanji.erase(testKanji.begin()+currentKanjiIndex);
		if(testKanji.size()>0) {
			currentKanjiIndex = rand() % testKanji.size();
			currentKanji = testKanji[currentKanjiIndex];
		} else { /* If there's no more kanji to get... */
			Stop();
		}
	}

	/* Update CoveredTextBoxes with new info from KANJIDIC. */
	txtKanji->Cover(); txtOnyomi->Cover(); txtKunyomi->Cover(); txtEnglish->Cover();
	txtKanji->SetHiddenStr(wxString(currentKanji));
	txtOnyomi->SetHiddenStr(jben->kdict->GetOnyomiStr(currentKanji));
	txtKunyomi->SetHiddenStr(jben->kdict->GetKunyomiStr(currentKanji));
	txtEnglish->SetHiddenStr(jben->kdict->GetEnglishStr(currentKanji));

	/* Update the test status label */
	double score=0.0;
	if(totalTested>0) score = round(100000*(double)correct/(double)totalTested)/1000;
	if(!extraPractice) {
		lblTestProgress->SetLabel(wxString::Format(
			_T("Current score: %d/%d (%0.3f%%)     Test Progress: %d/%d (%0.3f%% done)"),
			correct, totalTested, score,
			totalTested, totalToTest, round(100000*double(totalTested)/double(totalToTest))/1000));
	} else {
		lblTestProgress->SetLabel(wxString::Format(
			_T("Extra practice: %d remaining     Final score: %d/%d (%0.3f%%)"),
			testKanji.size(), correct, totalTested, score));
	}

	/* Refresh layout */
	this->GetSizer()->Layout();

	/* Uncover the field(s) appropriate for the current test mode. */
	switch(testMode) {
	case PKD_TM_Reading:
		txtKanji->Uncover();
		break;
	case PKD_TM_Writing:
		txtOnyomi->Uncover();
		txtKunyomi->Uncover();
		txtEnglish->Uncover();
		break;
	}
}

void PanelKanjiDrill::OnCorrect(wxCommandEvent& ev) {
	if(!extraPractice) {
		totalTested++;
		correct++;
	} else lastWasCorrect = true;
	ShowNextKanji();
}

void PanelKanjiDrill::OnWrong(wxCommandEvent& ev) {
	if(!extraPractice) {
		totalTested++;
		missedKanji.push_back(currentKanji);
	} else lastWasCorrect = false;
	ShowNextKanji();
}
