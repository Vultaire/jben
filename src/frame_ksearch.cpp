#include "frame_ksearch.h"
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
#include "kdict.h"
#include <glibmm/i18n.h>
#include <gtkmm/alignment.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/viewport.h>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#include "widget_kanjihwpad.h"
#include "frame_ksearch_helpers.h"

#define FKSEARCH_SIZE_STR "wnd.kanjiksearch.size"
#define FKSEARCH_PANE_DIVPOS "wnd.kanjiksearch.pane.divider.pos"

FrameKSearch::FrameKSearch()
	: topLayout(false, 5), searchBar(false, 5),
	  searchLabel(_("Search Methods")),
	  searchButtonsUpper(Gtk::BUTTONBOX_START, 5),
	  btnAdd(_("Add")), btnClear(_("Clear")),
	  searchGUIBox(false, 5),
	  searchButtonsLower(Gtk::BUTTONBOX_CENTER, 5),
	  btnSearch(_("Search")) {
	set_title(
		(boost::format(_("%s: Kanji Search")) % PROGRAM_NAME).str());

	Preferences* p = Preferences::Get();

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
	string& size = p->GetSetting(FKSEARCH_SIZE_STR);
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
	string& divpos = p->GetSetting(FKSEARCH_PANE_DIVPOS);
	if(!divpos.empty()) {
		int pos = atoi(divpos.c_str());
		if(pos >= 0) panes.set_position(pos);
	}

	searchOpts.append_text(_("SKIP code"));
	/*searchOpts.append_text(_("Four Corner code"));*/
	/*searchOpts.append_text(_("Spahn/Hadamitzky code"));*/
	/*searchOpts.append_text(_("De Roo code"));*/
	/*searchOpts.append_text(_("Multi-radical lookup"));*/
	searchOpts.append_text(_("Stroke Count"));
	/*searchOpts.append_text(_("Bushu lookup"));*/
	searchOpts.append_text(_("Handwriting Recognition"));
	searchOpts.set_active_text(_("SKIP code"));

	/* Connect signals */
	signal_delete_event()
		.connect(sigc::mem_fun(*this, &FrameKSearch::OnDeleteEvent), false);
	btnAdd.signal_clicked()
		.connect(sigc::mem_fun(*this, &FrameKSearch::OnAdd));
	btnClear.signal_clicked()
		.connect(sigc::mem_fun(*this, &FrameKSearch::Clear));
	btnSearch.signal_clicked()
		.connect(sigc::mem_fun(*this, &FrameKSearch::Update));

	/* Set up GUI */
	add(panes);
	panes.pack1(topFrame);
	panes.pack2(bottomFrame);
	topFrame.set_shadow_type(Gtk::SHADOW_IN);
	topFrame.add(topLayout);
	bottomFrame.set_shadow_type(Gtk::SHADOW_IN);
	bottomFrame.add(bottomLayout);

	topLayout.set_border_width(5);
	topLayout.pack_start(searchBar, Gtk::PACK_SHRINK);
	topLayout.pack_start(searchGUIWindow, Gtk::PACK_EXPAND_WIDGET);
	topLayout.pack_end(searchButtonsLower, Gtk::PACK_SHRINK);
	searchBar.pack_start(searchLabel, Gtk::PACK_SHRINK);
	searchBar.pack_start(searchOpts, Gtk::PACK_SHRINK);
	searchBar.pack_end(searchButtonsUpper, Gtk::PACK_SHRINK);
	searchGUIWindow.add(searchGUIBox);
	searchGUIWindow.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
	/* The ScrolledWindow, for VBox objects, auto-adds a Viewport with a
	   shadowed border.  We don't want this. */
	((Gtk::Viewport*)searchGUIWindow.get_child())
		->set_shadow_type(Gtk::SHADOW_NONE);
	searchButtonsLower.pack_start(btnSearch, Gtk::PACK_SHRINK);
	searchButtonsUpper.pack_start(btnAdd, Gtk::PACK_SHRINK);
	searchButtonsUpper.pack_start(btnClear, Gtk::PACK_SHRINK);

	bottomLayout.pack_start(resultWindow, Gtk::PACK_EXPAND_WIDGET);
	bottomLayout.pack_start(statusBar, Gtk::PACK_SHRINK);
	resultWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	resultWindow.add(resultLayout);
	resultLayout.set_wrap_mode(Gtk::WRAP_CHAR);
	resultLayout.set_editable(false);

	show_all_children();
	Update();
}

FrameKSearch::~FrameKSearch() {
	/* Again, this is copied from widget_storeddialog. */
	Preferences* p = Preferences::Get();
	int x, y;
	get_size(x,y);
	string& size = p->GetSetting(FKSEARCH_SIZE_STR);
	size = (boost::format("%dx%d") % x % y).str();
	string& divpos = p->GetSetting(FKSEARCH_PANE_DIVPOS);
	divpos = (boost::format("%d") % panes.get_position()).str();
}

bool FrameKSearch::OnDeleteEvent(GdkEventAny* event) {
	hide();
	Clear();
	return true;
}

void FrameKSearch::Clear() {
	Gtk::Box::BoxList& bl = searchGUIBox.children();
	while(!bl.empty()) {
		Widget* w = bl.front().get_widget();
		searchGUIBox.remove(*w);
		delete w;
	}

	Update();
}

#include <iostream>
typedef std::pair<wchar_t, KInfo> kdict_pair;
void FrameKSearch::Update() {
	Gtk::Box::BoxList& bl = searchGUIBox.children();
	list<wchar_t> results;
	if(!bl.empty()) {
		/* Create list of all kanji from KDict */
		const unordered_map<wchar_t, KInfo>* m = KDict::Get()->GetHashTable();
		foreach(const kdict_pair& p, *m) {
			results.push_back(p.first);
		}
		/* Run through all filters */
		foreach(Gtk::Box_Helpers::Child& c, bl) {
			KSearchBox* box = (KSearchBox*)c.get_widget();
			results = box->Filter(results);
		}
	}

	/* Clear existing results GUI */
	Gtk::Button* pb;
	while(!resultButtonList.empty()) {
		pb = resultButtonList.front();
		resultLayout.remove(*pb);
		delete pb;
		resultButtonList.pop_front();
	}

	/* Display results */
	/* Copying JWPce's 1500 character limit - still overkill, but I don't want
	   J-Ben to be seen as "lesser" because of trimming lower. */
	size_t size = results.size();
	if(size <= 1500) {
		statusBar.pop();
		statusBar.push((boost::format(_("Total results: %d (Loading buttons...)")) % size).str());
		resultLayout.get_buffer()->set_text("");

		/* Create buttons for new results */
		std::string& fontStr = Preferences::Get()->GetSetting("font.ja");
		Pango::FontDescription fd(fontStr);
		foreach(wchar_t& wc, results) {
			pb = new Gtk::Button;
			pb->set_label(utfconv_wm(wstring()+=wc));
			pb->get_child()->modify_font(fd);
			pb->signal_clicked().connect(
				sigc::bind<Gtk::Button*>(
					sigc::mem_fun(*this, &FrameKSearch::OnResultClick), pb));
			resultButtonList.push_back(pb);
		}

		UpdateResultLayout();
	} else {
		resultLayout.get_buffer()->set_text(
			_("Over 1500 results.  Please narrow down your search."));
	}
	statusBar.pop();
	statusBar.push((boost::format(_("Total results: %d")) % size).str());
}

void FrameKSearch::UpdateResultLayout() {
	Glib::RefPtr<Gtk::TextBuffer> rp = resultLayout.get_buffer();

	/* Create buffer of child anchors */
	Glib::RefPtr<Gtk::TextChildAnchor> rpAnchor;
	for(int i=resultButtonList.size(); i>0; i--) {
		rpAnchor = rp->create_child_anchor(rp->end());
	}

	/* Insert a control at each child tag found. */
	Gtk::TextBuffer::iterator it = rp->begin();
	foreach(Gtk::Button* pb, resultButtonList) {
		rpAnchor = it.get_child_anchor();
		if(it == rp->end())
			assert(0 && "shouldn't happen - at least, I don't think so");
		it++;

		while(!rpAnchor) {
			rpAnchor = it.get_child_anchor();
			if(it == rp->end())
				assert(0 && "shouldn't happen - at least, I don't think so");
			it++;			
		}

		resultLayout.add_child_at_anchor(*pb, rpAnchor);
	}
	resultLayout.show_all_children();
}

/**
 * Add a search filter to the end of the filter list.
 */
void FrameKSearch::OnAdd() {
	string selected = searchOpts.get_active_text();
	KSearchBox* ksb = NULL;
	if(selected==_("Handwriting Recognition")) {
		ksb = manage(new KSearchHW);
	} else if(selected==_("Stroke Count")) {
		ksb = manage(new KSearchStroke);
	} else if(selected==_("SKIP code")) {
		ksb = manage(new KSearchSKIP);
	}

	if(ksb) {
		Gtk::HButtonBox* hbb = manage(new Gtk::HButtonBox);
		Gtk::Button* btn = manage(new Gtk::Button(_("Drop")));
		btn->signal_clicked().connect(
			sigc::bind<Gtk::Widget*>(
				sigc::mem_fun(*this, &FrameKSearch::OnDrop), ksb));

		ksb->pack_end(*hbb, Gtk::PACK_SHRINK);
		hbb->pack_start(*btn);

		if(selected==_("Handwriting Recognition")) {
			searchGUIBox.pack_start(*ksb, Gtk::PACK_EXPAND_WIDGET);
		} else {
			searchGUIBox.pack_start(*ksb, Gtk::PACK_SHRINK);
		}
		searchGUIBox.show_all_children();
	}
}

/**
 * Drop a search filter, based on index.
 */
void FrameKSearch::OnDrop(Gtk::Widget* pWidget) {
	Gtk::Box::BoxList& bl = searchGUIBox.children();
	Gtk::Box::BoxList::iterator it = bl.find(*pWidget);
	if(it!=bl.end()) {
		searchGUIBox.remove(*pWidget);
		delete pWidget;
	}

	Update();
}

void FrameKSearch::OnResultClick(Gtk::Button *pButton) {
	Gtk::Clipboard::get()->set_text(pButton->get_label());
}
