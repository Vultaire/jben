/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: frame_maingui.h

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

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
#include "frame_ksearch.h"

class FrameMainGUI : public Gtk::Window {
public:
	static FrameMainGUI& Get();
	static void Destroy();
	void Update();
	void LockTabs();
	void UnlockTabs();
private:
	void OnMenuFileQuit();
	void OnMenuEditVocab();
	void OnMenuEditKanji();
	void OnMenuEditPrefs();
	void OnMenuPracticeKanji();
	void OnMenuToolsHand();
	void OnMenuToolsKanjiSearch();
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
	FrameKSearch* pfKSearch;

	FrameMainGUI();
	~FrameMainGUI();
	static FrameMainGUI* singleton;
};

#endif
