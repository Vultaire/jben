#include "dialog_kanjiposttest.h"
#include <boost/format.hpp>
#include <glibmm/i18n.h>
#include <gtkmm/stock.h>
#include <gtkmm/scrolledwindow.h>
#include <string>
#include <vector>
#include <sstream>
#include "encoding_convert.h"

DialogKanjiPostTest::DialogKanjiPostTest
(Gtk::Window& parent, DialogKanjiTest& test)
	: Dialog(_("Kanji Test: Results"), parent)
{
	/* Create result string */
	double score=0.0, progress=0.0;
	if(test.totalTested>0) {
		score = round(100000 * (double)test.correct /
					  (double)test.totalTested) / 1000;
		progress = round(100000 * (double)test.totalTested /
						 (double)test.totalToTest) / 1000;
	}

	std::wstring kanjiMissed;
	for(std::vector<wchar_t>::iterator vi=test.missedKanji.begin();
		vi!=test.missedKanji.end(); vi++)
		kanjiMissed.append(1, *vi);

	std::ostringstream oss;
	if(test.totalTested==test.totalToTest) {
		oss << _("- TEST COMPLETED -\n\n")
			<< (boost::format(_("Final score: %d/%d (%0.3f%%)\n"))
				% test.correct % test.totalTested % score);
		if(kanjiMissed.length()>0) {
			oss << _("List of missed kanji:\n\n") << utfconv_wm(kanjiMissed)
				<< '\n';
		}
	} else {
		oss << _("The test was NOT completed!\n\n")
			<< (boost::format(_("Test progress: %d/%d (%0.3f%%)\n"))
				% test.totalTested % test.totalToTest % progress)
			<< (boost::format(_("Partial score: %d/%d (%0.3f%%)\n"))
				% test.correct % test.totalTested % score);
		if(kanjiMissed.length()>0) {
			oss << _("List of missed kanji:\n\n") << utfconv_wm(kanjiMissed)
				<< '\n';
		}
	}

	tvResults.set_wrap_mode(Gtk::WRAP_WORD_CHAR);
	tvResults.set_editable(false);
	tvResults.get_buffer()->set_text(oss.str());

	Gtk::ScrolledWindow *pswTvResults = manage(new Gtk::ScrolledWindow);
	pswTvResults->add(tvResults);
	pswTvResults->set_shadow_type(Gtk::SHADOW_IN);
	pswTvResults->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	/* Display results */
	Gtk::Label* pLabel = manage(new Gtk::Label(_("Your test results are as follows:")));
	Gtk::VBox* pvb = get_vbox();
	pvb->pack_start(*pLabel, Gtk::PACK_SHRINK);
	pvb->pack_start(*pswTvResults);
	add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	show_all_children();
}
