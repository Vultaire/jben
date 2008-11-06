/*
Project: J-Ben
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File:         dialog_kanjitest.h
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

#ifndef dialog_kanjitest_h
#define dialog_kanjitest_h

#include "widget_storeddialog.h"
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include "dialog_kanjipretest.h"
#include "widget_coveredtextview.h"
#include <vector>

enum {
	DKT_TM_Reading = 1,
	DKT_TM_Writing
};

class DialogKanjiTest : public StoredDialog {
public:
	DialogKanjiTest(Gtk::Window& parent, DialogKanjiPreTest& settings);
	int testMode;
	unsigned int correct, totalTested, totalToTest;
	std::vector<wchar_t> testKanjiList, missedKanji;
protected :
        //[Alain]
        // Gtkmmm doesn't allow single key shortcuts so we must 
        // grab the key press event on the window level to get them
	virtual bool on_key_press_event (GdkEventKey* event);
private:
	void ShowNextKanji();
	void OnCorrect();
	void OnWrong();
	void OnStop();
	bool OnDeleteEvent(GdkEventAny* event);
        //[Alain]
        //Uncover the masked fields to get the answer
        void OnShowAnswer();

	CoveredTextView ctvKanji, ctvOnyomi, ctvKunyomi, ctvMeaning;
	Gtk::Button *btnCorrect, *btnWrong, *btnStop;
	//to show the sod
	Gtk::Image sodDisplay;
	std::string sodPath;
	//[Alain]
	// new button show answer
	Gtk::Button* btnShowAnswer;

	Gtk::Label lblScore, lblProgress;

	bool extraPractice; /* After all kanji are tested once through, this var gets set */
	bool lastWasCorrect;
	wchar_t currentKanji;
	int currentKanjiIndex;
	std::vector<wchar_t> testKanji; /* Internal list, originally identical to
									   testKanjiList */
};

#endif
