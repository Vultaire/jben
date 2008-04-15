#ifndef frame_ksearch_helpers_h
#define frame_ksearch_helpers_h

#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>
#include <list>
#include "widget_kanjihwpad.h"

class KSearchBox : public Gtk::Frame {
public:
	KSearchBox(const Glib::ustring& label);
	virtual std::list<wchar_t> Filter(std::list<wchar_t> srcList) = 0;
	void pack_start(Gtk::Widget& child,
					Gtk::PackOptions options=Gtk::PACK_EXPAND_WIDGET,
					guint padding=0);
	void pack_end(Gtk::Widget& child,
				  Gtk::PackOptions options=Gtk::PACK_EXPAND_WIDGET,
				  guint padding=0);
protected:
	Gtk::HBox layout;
};

class KSearchHW : public KSearchBox {
public:
	KSearchHW();
	std::list<wchar_t> Filter(std::list<wchar_t> srcList);
private:
	void OnClear();

	Gtk::VBox leftLayout;
	KanjiHWPad khwp;
	Gtk::HButtonBox buttons;
	Gtk::Button btnClear;
};

class KSearchStroke : public KSearchBox {
public:
	KSearchStroke();
	std::list<wchar_t> Filter(std::list<wchar_t> srcList);
private:
	void OnMiscountToggled();

	Gtk::VBox leftLayout;
	Gtk::HBox strokeControls;
	Gtk::SpinButton strokeCount;
	Gtk::CheckButton allowCommonMiscounts;
	Gtk::HBox miscountControls;
	Gtk::CheckButton allowMiscounts;
	Gtk::Label plusMinus;
	Gtk::SpinButton miscountAdjust;
};

class KSearchSKIP : public KSearchBox {
public:
	KSearchSKIP();
	std::list<wchar_t> Filter(std::list<wchar_t> srcList);
private:
	void OnMiscountToggled();

	Gtk::VBox leftLayout;
	Gtk::HBox skipControls;
	Gtk::SpinButton index1, index2, index3;
	Gtk::Label sep1, sep2;
	Gtk::CheckButton allowMiscodes;
	Gtk::HBox miscountControls;
	Gtk::CheckButton allowMiscounts;
	Gtk::Label plusMinus;
	Gtk::SpinButton miscountAdjust;
};

#endif
