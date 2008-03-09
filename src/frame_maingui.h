#ifndef frame_maingui_h
#define frame_maingui_h

#include <gtkmm/window.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/menu.h>
#include <gtkmm/notebook.h>
#include <gtkmm/box.h>

#include "panel_worddict.h"
#include "panel_vocablisteditor.h"
#include "panel_kanjidict.h"
#include "panel_kanjidrill.h"
#include "panel_kanjilisteditor.h"
#include "panel_config.h"
#include "dialog_addkanjibygrade.h"
#include "dialog_addkanjibyfreq.h"

class FrameMainGUI : public Gtk::Window {
public:
	static FrameMainGUI& Get();
	static void Destroy();
	void Update();
	void LockTabs();
	void UnlockTabs();
private:
	void OnSwitchPage(GtkNotebookPage* page, guint page_num);
	void OnMenuFileQuit();
	void OnMenuEditVocab();
	void OnMenuEditKanji();
	void OnMenuEditPrefs();
	void OnMenuHelpAbout();
	void OnMenuHelpLicense();

	Glib::RefPtr<Gtk::UIManager> refUIManager;
	Glib::RefPtr<Gtk::ActionGroup> refActionGroup;
	Gtk::Menu kanjiMenu;
	Gtk::VBox layoutBox;
	Gtk::Notebook tabs;
	PanelWordDict panelWordDict;
	PanelKanjiDict panelKanjiDict;

	FrameMainGUI();
	~FrameMainGUI();
	static FrameMainGUI* singleton;
};

#endif
