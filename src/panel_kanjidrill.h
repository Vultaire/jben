#ifndef panel_kanjidrill_h
#define panel_kanjidrill_h

#include "widget_updatepanel.h"
#include <gtkmm/radiobutton.h>
#include <gtkmm/button.h>
#include <gtkmm/spinbutton.h>
#include "widget_coveredtextview.h"

class PanelKanjiDrill : public UpdatePanel {
public:
	PanelKanjiDrill();
	void Update();
private:
	void OnKanjiCountChange();
	void OnRdoRandom();
	void OnRdoStartIndex();
	void OnStart();
	void OnCorrect();
	void OnWrong();
	void OnStop();

	/* Pre-test GUI */
	Gtk::VBox vbPreTest;
	Gtk::RadioButton rdoRandom, rdoIndex, rdoReading, rdoWriting;
	Gtk::SpinButton spnCount, spnIndex;
	Gtk::Button btnStart;

	/* Test GUI */
	Gtk::VBox vbTest;
	CoveredTextView ctvKanji, ctvOnyomi, ctvKunyomi, ctvMeaning;
	Gtk::Button btnCorrect, btnWrong, btnStop;
	Gtk::Label lblScore, lblProgress;
};

#endif
