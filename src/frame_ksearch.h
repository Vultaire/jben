#ifndef frame_ksearch
#define frame_ksearch

#include <gtkmm/window.h>
#include <gtkmm/paned.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/button.h>
#include <gtkmm/textview.h>
#include <string>
#include <list>

class FrameKSearch : public Gtk::Window {
public:
	FrameKSearch();
	~FrameKSearch();
	void Clear();
	void Update();
private:
	void OnAdd();
	void OnDrop(Gtk::Widget *pWidget);
	bool OnDeleteEvent(GdkEventAny* event);
	void OnResultClick(Gtk::Button *pButton);
	void UpdateResultLayout();

	Gtk::VPaned panes;
	Gtk::Statusbar statusBar;
	Gtk::Frame topFrame, bottomFrame;
	Gtk::VBox topLayout;
	Gtk::VBox bottomLayout;
	Gtk::HBox searchBar;
	Gtk::Label searchLabel;
	Gtk::ComboBoxText searchOpts;
	Gtk::HButtonBox searchButtonsUpper;
	Gtk::Button btnAdd, btnClear;
	Gtk::ScrolledWindow searchGUIWindow;
	Gtk::VBox searchGUIBox;
	Gtk::HButtonBox searchButtonsLower;
	Gtk::Button btnSearch;
	Gtk::ScrolledWindow resultWindow;
	/* Result box implementation: 2 ideas
	   1. -BUNCH- of buttons, like gjiten
	   2. GtkTextView with event tags
	   First: trying bunch of buttons together with GtkTextView.
	   (Studying gjiten, it appears this is how they implemented their results
	   area... since I can't get an object's width easily w/o realizing it,
	   let's just realize it within a word-wrappable container.) */
	Gtk::TextView resultLayout;
	std::list<Gtk::Button *> resultButtonList;
};

#endif
