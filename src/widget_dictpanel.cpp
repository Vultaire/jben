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

#if 0
#include <iostream>
using namespace std;

static void ShowTag(GtkTextTag *tag, gpointer data) {
	cout << "Tag!" << endl;
	cout << "\tPriority: " << gtk_text_tag_get_priority(tag) << endl;
	gchararray buf;
	g_object_get(tag, "font", &buf, NULL);
	if(buf!=NULL) {
		cout << "\tFont name: " << buf << endl;
		g_free(buf);
	}
}
#endif

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
#if 0
	gtk_text_tag_table_foreach(pTable->gobj(), ShowTag, NULL);
#endif

#if 0
	/* Also, let's set the entry field's font
	   to the normal-sized Japanese font. */
	/* - DOES NOT YET WORK, so disabled! - */
	Pango::FontDescription *fd
		= new Pango::FontDescription(p->GetSetting("font.en"));
	entQuery.unset_font();
	entQuery.modify_font(*fd);
	entQuery.queue_resize();
	delete fd;
#endif
}
