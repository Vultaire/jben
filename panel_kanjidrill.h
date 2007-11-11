/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: panel_kanjidrill.h

This file is part of J-Ben.

J-Ben is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2
as published by the Free Software Foundation.

J-Ben is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef panel_kanjidrill_h
#define panel_kanjidrill_h

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "redrawablepanel.h"
#include "wx/spinctrl.h"
#include "coveredtextbox.h"
#include <vector>
using namespace std;

class PanelKanjiDrill: public RedrawablePanel {
public:
	PanelKanjiDrill(wxWindow *owner);
	void Stop();
	void OnStart(wxCommandEvent& ev);
	void OnStop(wxCommandEvent& ev);
	void Redraw();
	bool TestInProgress();
	void OnKanjiCountChange(wxSpinEvent& ev);
	void OnRdoRandom(wxCommandEvent& ev);
	void OnRdoStartIndex(wxCommandEvent& ev);
	void OnCorrect(wxCommandEvent& ev);
	void OnWrong(wxCommandEvent& ev);
private:
	void UpdateKanjiCountSpinner();
	void UpdateStartIndexSpinner();
	void ShowNextKanji();

	/* General vars */
	bool testing;
	bool extraPractice; /* After all kanji are tested once through, this var gets set */
	bool lastWasCorrect;
	int testMode;
	unsigned int correct, totalTested, totalToTest;  /* "Final score: 56/76, 4 kanji not tested" */
	vector<wxChar> testKanji, missedKanji;
	wxChar currentKanji;
	int currentKanjiIndex;
	/* NOTE: Maybe add "list<int> correctKanji", and add option to save to a "mastered" kanji list? */
	/* Pre-test controls */
	wxPanel *pnlConfig;
	wxSpinCtrl *spnKanjiCount;
	wxRadioButton *rdoRandom, *rdoStartIndex;
	wxSpinCtrl *spnStartIndex;
	wxRadioButton *rdoKanjiReading, *rdoKanjiWriting;
	wxButton *btnStart;
	/* Test mode controls */
	wxPanel *pnlTest;
	CoveredTextBox *txtKanji, *txtOnyomi, *txtKunyomi, *txtEnglish;
	wxButton *btnCorrect, *btnWrong, *btnStop;
	wxStaticText *lblTestProgress;

	DECLARE_EVENT_TABLE()
};

#endif
