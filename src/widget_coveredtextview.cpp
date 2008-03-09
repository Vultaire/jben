#include "widget_coveredtextview.h"

#include <iostream>
using namespace std;

CoveredTextView::CoveredTextView
(const Glib::ustring& cover, const Glib::ustring& hidden,
 const bool& covered) : Frame() {
	/* Buffer initialization */
	ptbCover = tv.get_buffer();
	ptbHidden = Gtk::TextBuffer::create();
	SetCoverText(cover);
	SetHiddenText(hidden);

	/* Set up colors */
	/* set() seems broken, so set the color manually. */
	cBase = tv.get_style()->get_base(Gtk::STATE_NORMAL);
	//cAlt.set("FFFF7F");
	/* Color triplets for Gdk are 16 bits each, NOT 8! */
	cAlt.set_rgb(0xFFFF,0xFFFF,0x7FFF);
	Glib::RefPtr<Gdk::Colormap> rCM = tv.get_colormap();
	if(!rCM->alloc_color(cAlt)) {
		cerr << __FILE__ << ':' << __LINE__ << ": Could not allocate color!";
	}
	tv.set_colormap(rCM);
#ifdef DEBUG
	cout << "DEBUG: color=" << hex
		 << cAlt.get_red() << "r, "
		 << cAlt.get_green() << "g, "
		 << cAlt.get_blue() << "b, "
		 << cAlt.get_pixel() << " pixel value" << dec << endl;
#endif

	/* Set initial control status */
	if(covered)
		Cover();
	else
		Show();

	/* Set up textview stuff */
	tv.set_editable(false);
	tv.set_wrap_mode(Gtk::WRAP_WORD_CHAR);
	tv.signal_button_press_event()
		.connect(sigc::mem_fun(*this, &CoveredTextView::OnClick), false);

	add(tv);
}

void CoveredTextView::Cover() {
	tv.modify_base(Gtk::STATE_NORMAL, cAlt);
	tv.set_buffer(ptbCover);
}

void CoveredTextView::Show() {
	tv.modify_base(Gtk::STATE_NORMAL, cBase);
	tv.set_buffer(ptbHidden);
}

void CoveredTextView::SetCoverText(const Glib::ustring& str) {
	ptbCover->set_text(str);
}

void CoveredTextView::SetHiddenText(const Glib::ustring& str) {
	ptbHidden->set_text(str);
}

bool CoveredTextView::OnClick(GdkEventButton* event) {
	if(tv.get_buffer()==ptbCover) {
		cout << "OnClick (Uncovering control)" << endl;
		Show();
	} else {
		cout << "OnClick (Already uncovered)" << endl;
	}
	return false;
}
