/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: widget_dictpanel.cpp

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

#include "widget_dictpanel.h"
#include "preferences.h"
#include <string>
#include <gtk/gtk.h>
#include <gtkmm/stock.h>
#include <glibmm/i18n.h>

DictPanel::DictPanel()
	: btnSearch(_("Search")),
	  btnPrev(Gtk::Stock::GO_BACK),
	  btnNext(Gtk::Stock::GO_FORWARD),
	  btnRand(_("Random")) {}

void DictPanel::Update() {
	/* Update will refresh the font tags in use by this control. */
	Glib::RefPtr<Gtk::TextBuffer::TagTable> pTable
		= tvResults.get_buffer()->get_tag_table();
	Glib::RefPtr<Gtk::TextTag> pTag;

	/* Remove old tags */
	pTag = pTable->lookup("en");
	if(pTag!=0) pTable->remove(pTag);
	pTag.clear();
	pTag = pTable->lookup("en.small");
	if(pTag!=0) pTable->remove(pTag);
	pTag.clear();
	pTag = pTable->lookup("ja");
	if(pTag!=0) pTable->remove(pTag);
	pTag.clear();
	pTag = pTable->lookup("ja.large");
	if(pTag!=0) pTable->remove(pTag);
	pTag.clear();

	/* Update tags */
	Preferences* p = Preferences::Get();
	pTag = Gtk::TextTag::create("en");
	pTag->property_font() = p->GetSetting("font.en");
	pTable->add(pTag);
	pTag->set_priority(0);
	pTag.clear();
	pTag = Gtk::TextTag::create("en.small");
	pTag->property_font() = p->GetSetting("font.en.small");
	pTable->add(pTag);
	pTag->set_priority(1);
	pTag.clear();
	pTag = Gtk::TextTag::create("ja");
	pTag->property_font() = p->GetSetting("font.ja");
	pTable->add(pTag);
	pTag->set_priority(2);
	pTag.clear();
	pTag = Gtk::TextTag::create("ja.large");
	pTag->property_font() = p->GetSetting("font.ja.large");
	pTable->add(pTag);
	pTag->set_priority(3);
	pTag.clear();

	/* Add default tags, if not already present. */
	/* This is done at the end so that these tags will get a higher default
	   priority than the other tags. */
	pTag = pTable->lookup("bold");
	if(pTag==0) {
		pTag = Gtk::TextTag::create("bold");
		pTag->property_weight_set() = true;
		pTag->property_weight() = Pango::WEIGHT_BOLD;
		pTable->add(pTag);
	}

	/* Also, let's set the entry field's font
	   to the normal-sized Japanese font. */
	Pango::FontDescription *fd
		= new Pango::FontDescription(p->GetSetting("font.ja"));
	entQuery.modify_font(*fd);
	delete fd;
}
