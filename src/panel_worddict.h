#ifndef panel_worddict_h
#define panel_worddict_h

#include "widget_dictpanel.h"
#include <glibmm/ustring.h>

class PanelWordDict : public DictPanel {
public:
	PanelWordDict();
	void Update();
	void SetSearchString(const Glib::ustring& searchString);
private:
	void UpdateOutput();

	int currentIndex;
	Glib::ustring currentSearchString;

	void OnQueryEnter();
	void OnSearch();
	void OnPrevious();
	void OnNext();
	void OnRandom();
	void OnIndexUpdate();
};

#endif
