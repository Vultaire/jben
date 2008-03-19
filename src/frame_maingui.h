#ifndef frame_maingui_h
#define frame_maingui_h

#include <gtkmm/window.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/menu.h>
#include <gtkmm/notebook.h>
#include <gtkmm/box.h>

#include "panel_worddict.h"
#include "panel_kanjidict.h"
#include "dialog_vocablisteditor.h"
#include "dialog_kanjilisteditor.h"
#include "dialog_config.h"
#include "dialog_kanjipretest.h"
#include "dialog_kanjitest.h"
#include "dialog_kanjiposttest.h"
#include "frame_hwpad.h"

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
	void OnMenuPracticeKanji();
	void OnMenuToolsHand();
	void OnMenuHelpAbout();
	void OnMenuHelpLicense();

	Glib::RefPtr<Gtk::UIManager> refUIManager;
	Glib::RefPtr<Gtk::ActionGroup> refActionGroup;
	Gtk::Menu kanjiMenu;
	Gtk::VBox layoutBox;
	Gtk::Notebook tabs;
	PanelWordDict panelWordDict;
	PanelKanjiDict panelKanjiDict;

	DialogKanjiListEditor* pdKanjiListEditor;
	DialogVocabListEditor* pdVocabListEditor;
	DialogConfig* pdConfig;
	DialogKanjiPreTest* pdKanjiPreTest;
	FrameHWPad* pfHWPad;

	FrameMainGUI();
	~FrameMainGUI();
	static FrameMainGUI* singleton;
};

#endif
