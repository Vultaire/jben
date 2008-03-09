#include "panel_kanjidrill.h"
#include <glibmm/i18n.h>
#include <gtkmm/frame.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/scrolledwindow.h>
#include <iostream>
using namespace std;

PanelKanjiDrill::PanelKanjiDrill()
	: UpdatePanel(false, 0),
	  vbPreTest(false, 5),
	  rdoRandom(_("Choose randomly from list")),
	  rdoIndex(_("Start at index:")),
	  rdoReading(_("Reading kanji")),
	  rdoWriting(_("Writing kanji")),
	  btnStart(_("Start Drill")),
	  vbTest(false, 5),
	  ctvKanji(_("< Kanji >")),
	  ctvOnyomi(_("< Onyomi >")),
	  ctvKunyomi(_("< Kunyomi >")),
	  ctvMeaning(_("< Meaning >")),
	  btnCorrect(_("Correct")),
	  btnWrong(_("Wrong")),
	  btnStop(_("Stop Drill"))
{
	/* Connect signals */
	spnCount.signal_value_changed()
		.connect(sigc::mem_fun(*this, &PanelKanjiDrill::OnKanjiCountChange));
	rdoRandom.signal_toggled()
		.connect(sigc::mem_fun(*this, &PanelKanjiDrill::OnRdoRandom));
	rdoIndex.signal_toggled()
		.connect(sigc::mem_fun(*this, &PanelKanjiDrill::OnRdoStartIndex));
	btnStart.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelKanjiDrill::OnStart));
	btnCorrect.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelKanjiDrill::OnCorrect));
	btnWrong.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelKanjiDrill::OnWrong));
	btnStop.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelKanjiDrill::OnStop));

	/* Create pre-test GUI */
	Gtk::RadioButton::Group g = rdoRandom.get_group();
	rdoIndex.set_group(g);
	g = rdoReading.get_group();
	rdoWriting.set_group(g);

	spnCount.set_width_chars(5);
	spnIndex.set_width_chars(5);

	Gtk::Frame* pfKanjiSelect
		= manage(new Gtk::Frame(_("Choose Kanji to Test")));
	Gtk::Frame* pfTestSelect
		= manage(new Gtk::Frame(_("Choose Test Type")));
	Gtk::HBox* phbCount = manage(new Gtk::HBox(false, 5));
	Gtk::HBox* phbIndex = manage(new Gtk::HBox(false, 5));
	Gtk::VBox* pvbKanjiSelect  = manage(new Gtk::VBox(false, 5));
	Gtk::VBox* pvbTestSelect   = manage(new Gtk::VBox(false, 5));
	Gtk::HButtonBox* phbStartButtons = manage(new Gtk::HButtonBox);
	Gtk::Label* plblCount
		= manage(new Gtk::Label(_("Number of kanji to test:")));

	vbPreTest.set_border_width(5);
	vbPreTest.pack_start(*pfKanjiSelect,   Gtk::PACK_SHRINK);
	vbPreTest.pack_start(*pfTestSelect,    Gtk::PACK_SHRINK);
	vbPreTest.pack_start(*phbStartButtons, Gtk::PACK_SHRINK);

	pfKanjiSelect->add(*pvbKanjiSelect);
	pfTestSelect ->add(*pvbTestSelect);

	pvbKanjiSelect->set_border_width(5);
	pvbKanjiSelect->pack_start(*phbCount, Gtk::PACK_SHRINK);
	pvbKanjiSelect->pack_start(rdoRandom, Gtk::PACK_SHRINK);
	pvbKanjiSelect->pack_start(*phbIndex, Gtk::PACK_SHRINK);

	pvbTestSelect->set_border_width(5);
	pvbTestSelect->pack_start(rdoReading, Gtk::PACK_SHRINK);
	pvbTestSelect->pack_start(rdoWriting, Gtk::PACK_SHRINK);

	phbStartButtons->pack_start(btnStart, Gtk::PACK_SHRINK);

	phbCount->pack_start(*plblCount, Gtk::PACK_SHRINK);
	phbCount->pack_start(spnCount,   Gtk::PACK_SHRINK);

	phbIndex->pack_start(rdoIndex, Gtk::PACK_SHRINK);
	phbIndex->pack_start(spnIndex, Gtk::PACK_SHRINK);

	/* Create test GUI */
	Gtk::HBox* phbTestButtons = manage(new Gtk::HBox(false, 5));

	vbTest.set_border_width(5);
	vbTest.pack_start(ctvKanji,   Gtk::PACK_SHRINK);
	vbTest.pack_start(ctvOnyomi,  Gtk::PACK_SHRINK);
	vbTest.pack_start(ctvKunyomi, Gtk::PACK_SHRINK);
	vbTest.pack_start(ctvMeaning, Gtk::PACK_SHRINK);
	vbTest.pack_start(*phbTestButtons, Gtk::PACK_SHRINK);

	phbTestButtons->pack_start(btnCorrect, Gtk::PACK_SHRINK);
	phbTestButtons->pack_start(btnWrong,   Gtk::PACK_SHRINK);
	phbTestButtons->pack_end  (btnStop,    Gtk::PACK_SHRINK);	

	/* Add to current vbox and show */
	vbPreTest.show_all_children();
	vbTest.show_all_children();
	pack_start(vbPreTest);
	show_all_children();
}

void PanelKanjiDrill::OnKanjiCountChange() {
	cout << "KanjiCountChange" << endl;
}

void PanelKanjiDrill::OnRdoRandom() {
	cout << "Random" << endl;
	spnIndex.set_editable(false);
}

void PanelKanjiDrill::OnRdoStartIndex() {
	cout << "StartIndex" << endl;
	spnIndex.set_editable(true);
}

void PanelKanjiDrill::OnStart() {
	cout << "Start" << endl;
	remove(vbPreTest);
	pack_start(vbTest);
	show_all_children();
}

void PanelKanjiDrill::OnCorrect() {
	cout << "Correct" << endl;
}

void PanelKanjiDrill::OnWrong() {
	cout << "Wrong" << endl;
}

void PanelKanjiDrill::OnStop() {
	cout << "Stop" << endl;
	remove(vbTest);
	pack_start(vbPreTest);
	show_all_children();
}

void PanelKanjiDrill::Update() {}
