#ifndef widget_coveredtextview_h
#define widget_coveredtextview_h

#include <gtkmm/frame.h>
#include <gtkmm/textview.h>

class CoveredTextView : public Gtk::Frame {
public:
	CoveredTextView(const Glib::ustring& cover="",
					const Glib::ustring& hidden="",
					const bool& covered=true);
	void Cover();
	void Show();
	void SetCoverText(const Glib::ustring& str);
	void SetHiddenText(const Glib::ustring& str);
private:
	bool OnClick(GdkEventButton* event);

	Gtk::TextView tv;
	Glib::RefPtr<Gtk::TextBuffer> ptbCover, ptbHidden;
	Gdk::Color cBase, cAlt;
};

#endif
