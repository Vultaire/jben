/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: widget_coveredtextview.cpp

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

#include "widget_coveredtextview.h"
#include "preferences.h"

#include <iostream>
using namespace std;

CoveredTextView::CoveredTextView
(const Glib::ustring& cover, const Glib::ustring& hidden,
 const bool& covered, const bool& japanese) : Frame() {
	/* Buffer initialization - create two buffers, but share one tag table */
	ptbCover = tv.get_buffer();
	ptbHidden = Gtk::TextBuffer::create(ptbCover->get_tag_table());

	/* Set up fonts */
	Preferences* p = Preferences::Get();
	Glib::RefPtr<Gtk::TextBuffer::TagTable> pTable;
	Glib::RefPtr<Gtk::TextTag> pTag;
	pTag = Gtk::TextTag::create("font");
	if(japanese) {
		pTag->property_font() = p->GetSetting("font.ja");
	} else {
		pTag->property_font() = p->GetSetting("font.en");
	}
	pTable = ptbCover->get_tag_table();
	pTable->add(pTag);
	pTable.clear();

	/* Set initial text */
	SetCoverText(cover);
	SetHiddenText(hidden);

	/* Set up colors */
	cBase = tv.get_style()->get_base(Gtk::STATE_NORMAL);
	/* cAlt.set("FFFF7F"); */
	/* set() seems broken, so set the color manually. */
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
	ptbCover->set_text("");
	ptbCover->insert_with_tag(ptbCover->get_iter_at_offset(0), str, "font");
}

void CoveredTextView::SetHiddenText(const Glib::ustring& str) {
	ptbHidden->set_text("");
	ptbHidden->insert_with_tag(ptbHidden->get_iter_at_offset(0), str, "font");
}

bool CoveredTextView::OnClick(GdkEventButton* event) {
	if(tv.get_buffer()==ptbCover) Show();
	return false;
}
