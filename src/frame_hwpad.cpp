#include "frame_hwpad.h"
#include "preferences.h"
#include "jben.xpm"
#include <glibmm/i18n.h>
#include <gtkmm/alignment.h>
#include "string_utils.h"
#include <boost/format.hpp>
#include "encoding_convert.h"

#include <iostream>

#define FHWPAD_SIZE_STR "wnd.kanjihwpad.size"

FrameHWPad::FrameHWPad() {
	set_title(_("Kanji Handwriting Pad"));
	Glib::RefPtr<Gdk::Pixbuf> rpIcon
		= Gdk::Pixbuf::create_from_xpm_data(iconJben_xpm);
	set_icon(rpIcon);

	/* Logic copied from widget_storeddialog */
	Preferences* p = Preferences::Get();
	string& size = p->GetSetting(FHWPAD_SIZE_STR);
	int x, y;
	if(size.length()>0) {
		std::list<string> ls = StrTokenize<char>(size, "x");
		if(ls.size()>=2) {
			x = atoi(ls.front().c_str());
			ls.pop_front();
			y = atoi(ls.front().c_str());
			set_default_size(x,y);
		}
	} else
		set_default_size(200, 230);

	Gtk::Alignment* paBox = manage(
		new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0, 0));
	paBox->add(hbResults);
	for(int i=0; i<5; i++) {
		buttons[i].set_label("");
		buttons[i].set_relief(Gtk::RELIEF_NONE);
		buttons[i].signal_clicked().connect(
			sigc::bind<unsigned int>(
				sigc::mem_fun(*this, &FrameHWPad::OnKanjiClicked)
				, i));
		buttons[i].set_sensitive(false);
		hbResults.pack_start(buttons[i], Gtk::PACK_SHRINK);
	}
	signal_delete_event()
		.connect(sigc::mem_fun(*this, &FrameHWPad::OnDeleteEvent), false);

	hwp.signal_button_release_event()
		.connect(sigc::mem_fun(*this, &FrameHWPad::OnRelease));

	Gtk::VBox* pvb = manage(new Gtk::VBox(false, 5));
	pvb->set_border_width(5);
	pvb->pack_start(hwp);
	pvb->pack_start(*paBox, Gtk::PACK_SHRINK);
	add(*pvb);

	show_all_children();
}

FrameHWPad::~FrameHWPad() {
	/* Again, this is copied from widget_storeddialog. */
	std::cout << "FrameHWPad destructor called." << std::endl;
	Preferences* p = Preferences::Get();
	int x, y;
	get_size(x,y);
	string& size = p->GetSetting(FHWPAD_SIZE_STR);
	size = (boost::format("%dx%d") % x % y).str();
}

bool FrameHWPad::OnRelease(GdkEventButton* event) {
	std::cout << "Signal CAUGHT by FrameHWPad::OnRelease!" << std::endl;

	Update();
	return true;
}

void FrameHWPad::OnKanjiClicked(unsigned int index) {
	const std::vector<wchar_t>* pv = &hwp.GetResults();
	if(index < pv->size()) {
		std::wstring ws;
		ws.append(1, (*pv)[index]);
		/* Convert to utf */
		std::string s = utfconv_wm(ws);
		/* Copy to clipboard */
		std::cout << "Copying \"" << s << "\" to clipboard." << std::endl;
		Glib::RefPtr<Gtk::Clipboard> rcb = Gtk::Clipboard::get();
		rcb->set_text(s);
	}
}

bool FrameHWPad::OnDeleteEvent(GdkEventAny* event) {
	hide();
	Clear();
	std::cout << "OnDeleteEvent caught!" << std::endl;
	return true;
}

void FrameHWPad::Clear() {
	hwp.Clear();
	Update();
}

void FrameHWPad::Update() {
	std::vector<wchar_t> vwc = hwp.GetResults();

	std::wstring ws;
	for(std::vector<wchar_t>::iterator it = vwc.begin(); it!=vwc.end(); it++) {
		ws.append(1, *it);
	}
	std::cout << "Results: [" << utfconv_wm(ws) << ']' << std::endl;

	std::string utf8char;
	size_t i;
	for(i=0; i<vwc.size(); i++) {
		utf8char = utfconv_wm(std::wstring().append(1, vwc[i]));
		buttons[i].set_label(utf8char);
		buttons[i].set_sensitive(true);
	}
	for(; i<5; i++) {
		buttons[i].set_label("");
		buttons[i].set_sensitive(false);
	}
}
