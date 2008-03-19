#ifndef widget_kanjihwpad_h
#define widget_kanjihwpad_h

#include <gtkmm/aspectframe.h>
#include <gtkmm/drawingarea.h>
#include <vector>
#include <list>

class KanjiHWPad : public Gtk::AspectFrame {
public:
	KanjiHWPad();
	const std::vector<wchar_t>& GetResults();
	void Clear();
	void Update();
private:
	bool OnExpose(GdkEventExpose* event);
	bool OnMotion(GdkEventMotion* event);
	bool OnPress(GdkEventButton* event);
	bool OnRelease(GdkEventButton* event);
	void LookupChars();
	Gtk::DrawingArea da;

	std::vector<wchar_t> results;
	std::list< std::list< std::pair<int,int> > > listLines;
	std::list< std::pair<int,int> >* pCurrentLine;
};

#endif
