#ifndef dialog_kanjitest_h
#define dialog_kanjitest_h

#include "widget_storeddialog.h"
#include <gtkmm/button.h>
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
private:
	void ShowNextKanji();
	void OnCorrect();
	void OnWrong();
	void OnStop();
	bool OnDeleteEvent(GdkEventAny* event);

	CoveredTextView ctvKanji, ctvOnyomi, ctvKunyomi, ctvMeaning;
	Gtk::Button btnCorrect, btnWrong, btnStop;
	Gtk::Label lblScore, lblProgress;

	bool extraPractice; /* After all kanji are tested once through, this var gets set */
	bool lastWasCorrect;
	wchar_t currentKanji;
	int currentKanjiIndex;
	std::vector<wchar_t> testKanji; /* Internal list, originally identical to
									   testKanjiList */
};

#endif
