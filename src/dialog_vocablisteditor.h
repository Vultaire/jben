#ifndef panel_vocablisteditor_h
#define panel_vocablisteditor_h

#include "widget_updatepanel.h"
#include <gtkmm/button.h>
#include <gtkmm/textview.h>

class PanelVocabListEditor : public UpdatePanel {
public:
	PanelVocabListEditor();
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
