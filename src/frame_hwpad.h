#ifndef frame_hwpad
#define frame_hwpad

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include "widget_kanjihwpad.h"
#include <string>

class FrameHWPad : public Gtk::Window {
public:
	FrameHWPad();
	~FrameHWPad();
	void Clear();
	void Update();
private:
	bool OnRelease(GdkEventButton* event);
	void OnKanjiClicked(unsigned int index);
	bool OnDeleteEvent(GdkEventAny* event);
	
	KanjiHWPad hwp;
	Gtk::HBox hbResults;
	Gtk::Button buttons[5];
	Gtk::Label btnLabels[5];
	Gtk::Button btnClear;
	std::string sOldFontStr;
};

#endif
