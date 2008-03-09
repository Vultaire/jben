#ifndef panel_kanjilisteditor_h
#define panel_kanjilisteditor_h

#include "widget_updatepanel.h"
#include <gtkmm/button.h>
#include <gtkmm/textview.h>

class PanelKanjiListEditor : public UpdatePanel {
public:
	PanelKanjiListEditor();
	bool ChangeDetected();
	void Update();
private:
	void OnCancel();
	void OnApply();
	void OnTextChanged();

	Gtk::TextView tvList;
	Gtk::Button btnApply, btnCancel;
	bool bChanged;
};

#endif
