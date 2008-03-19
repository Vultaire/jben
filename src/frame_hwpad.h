#ifndef frame_hwpad
#define frame_hwpad

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include "widget_kanjihwpad.h"

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
	
	void Hide();
	KanjiHWPad hwp;
	Gtk::HBox hbResults;
	Gtk::Button buttons[5];
};

#endif
