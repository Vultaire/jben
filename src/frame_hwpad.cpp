#include "frame_hwpad.h"
#ifndef __WIN32__
#	include "jben.xpm"
#	include "jben_48.xpm"
#	include "jben_32.xpm"
#	include "jben_16.xpm"
#endif
#include "version.h"
#include "preferences.h"
#include "string_utils.h"
#include "encoding_convert.h"
#include <glibmm/i18n.h>
#include <gtkmm/alignment.h>
#include <gtkmm/buttonbox.h>
#include <boost/format.hpp>

#define FHWPAD_SIZE_STR "wnd.kanjihwpad.size"

FrameHWPad::FrameHWPad() : btnClear(_("Clear")) {
	set_title(
		(boost::format(_("%s: Kanji Handwriting Pad")) % PROGRAM_NAME).str());

	Preferences* p = Preferences::Get();
	sOldFontStr = p->GetSetting("font.ja");

#ifndef __WIN32__
	/* Load icons */
	list< Glib::RefPtr<Gdk::Pixbuf> > lIcons;
	lIcons.push_back(Gdk::Pixbuf::create_from_xpm_data(iconJben_xpm));
	lIcons.push_back(Gdk::Pixbuf::create_from_xpm_data(iconJben_48_xpm));
	lIcons.push_back(Gdk::Pixbuf::create_from_xpm_data(iconJben_32_xpm));
	lIcons.push_back(Gdk::Pixbuf::create_from_xpm_data(iconJben_16_xpm));
	set_icon_list(lIcons);
#endif

	/* Logic copied from widget_storeddialog */
	string& size = p->GetSetting(FHWPAD_SIZE_STR);
	int x, y;
	if(!size.empty()) {
		std::list<string> ls = StrTokenize<char>(size, "x");
		if(ls.size()>=2) {
			x = atoi(ls.front().c_str());
			ls.pop_front();
			y = atoi(ls.front().c_str());
			set_default_size(x,y);
		}
	}

	Gtk::Alignment* paBox = manage(
		new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0, 0));
	paBox->add(hbResults);

	Pango::FontDescription *fd
		= new Pango::FontDescription(sOldFontStr);
	for(int i=0; i<5; i++) {
		btnLabels[i].set_text("　");
		btnLabels[i].modify_font(*fd);
		buttons[i].add(btnLabels[i]);
		buttons[i].get_child()->modify_font(*fd);
		buttons[i].signal_clicked().connect(
			sigc::bind<unsigned int>(
				sigc::mem_fun(*this, &FrameHWPad::OnKanjiClicked)
				, i));
		buttons[i].set_sensitive(false);
		hbResults.pack_start(buttons[i], Gtk::PACK_SHRINK);
	}
	delete fd;
	signal_delete_event()
		.connect(sigc::mem_fun(*this, &FrameHWPad::OnDeleteEvent), false);

	btnClear.signal_clicked()
		.connect(sigc::mem_fun(*this, &FrameHWPad::Clear));
	Gtk::HButtonBox* pboxClear = manage(new Gtk::HButtonBox(Gtk::BUTTONBOX_CENTER));
	pboxClear->pack_start(btnClear, Gtk::PACK_SHRINK);

	hwp.signal_button_release_event()
		.connect(sigc::mem_fun(*this, &FrameHWPad::OnRelease));

	Gtk::VBox* pvb = manage(new Gtk::VBox(false, 5));
	pvb->set_border_width(5);
	pvb->pack_start(hwp);
	pvb->pack_start(*paBox, Gtk::PACK_SHRINK);
	pvb->pack_start(*pboxClear, Gtk::PACK_SHRINK);
	add(*pvb);

	show_all_children();
}

FrameHWPad::~FrameHWPad() {
	/* Again, this is copied from widget_storeddialog. */
	Preferences* p = Preferences::Get();
	int x, y;
	get_size(x,y);
	string& size = p->GetSetting(FHWPAD_SIZE_STR);
	size = (boost::format("%dx%d") % x % y).str();
}

bool FrameHWPad::OnRelease(GdkEventButton* event) {
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
		Glib::RefPtr<Gtk::Clipboard> rcb = Gtk::Clipboard::get();
		rcb->set_text(s);
	}
}

bool FrameHWPad::OnDeleteEvent(GdkEventAny* event) {
	hide();
	Clear();
	return true;
}

void FrameHWPad::Clear() {
	hwp.Clear();
	Update();
}

void FrameHWPad::Update() {
	std::vector<wchar_t> vwc = hwp.GetResults();
	
	/* Check for a font update, and perform if necessary */
	std::string& newStr = Preferences::Get()->GetSetting("font.ja");
	if(sOldFontStr != newStr) {
		Pango::FontDescription fd(newStr);
		for(int i=0; i<5; i++) btnLabels[i].modify_font(fd);
		sOldFontStr = newStr;
	}

	std::wstring ws;
	for(std::vector<wchar_t>::iterator it = vwc.begin(); it!=vwc.end(); it++) {
		ws.append(1, *it);
	}

	std::string utf8char;
	size_t i;
	for(i=0; i<vwc.size(); i++) {
		utf8char = utfconv_wm(std::wstring().append(1, vwc[i]));
		btnLabels[i].set_text(utf8char);
		buttons[i].set_sensitive(true);
	}
	for(; i<5; i++) {
		btnLabels[i].set_text("　");
		buttons[i].set_sensitive(false);
	}
}
