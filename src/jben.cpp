/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: jben.cpp

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

#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>
#include <glibmm/i18n.h>
#include "frame_maingui.h"
#include "errorlog.h"
#include "preferences.h"
#include "kdict.h"
#include "wdict.h"
#include "listmanager.h"
#include "encoding_convert.h"
#include "dialog_setmobile.h"

#include <iostream>
using namespace std;

#define DEFAULT_LIST_NAME "master"

void ErrorLogDisplayFunc(ELType t, const string& message, void *srcObj) {
	Gtk::MessageDialog* pmd = NULL;
	switch(t) {
	case EL_Error:
#ifdef DEBUG
		cout << "[Error] " << message << endl;
#endif
		if(srcObj)
			pmd = new Gtk::MessageDialog(*((Gtk::Window*)srcObj),
										 message, false, Gtk::MESSAGE_ERROR);
		else
			pmd = new Gtk::MessageDialog(message, false, Gtk::MESSAGE_ERROR);
		pmd->set_title(_("Error"));
		pmd->run();
		break;
	case EL_Warning:
#ifdef DEBUG
		cout << "[Warning] " << message << endl;
#endif
		if(srcObj)
			pmd = new Gtk::MessageDialog(*((Gtk::Window*)srcObj),
										 message, false, Gtk::MESSAGE_WARNING);
		else
			pmd = new Gtk::MessageDialog(message, false, Gtk::MESSAGE_WARNING);
		pmd->set_title(_("Warning"));
		pmd->run();
		break;
	case EL_Info:
#ifdef DEBUG
		cout << "[Info] " << message << endl;
#endif
		if(srcObj)
			pmd = new Gtk::MessageDialog(*((Gtk::Window*)srcObj),
										 message, false, Gtk::MESSAGE_INFO);
		else
			pmd = new Gtk::MessageDialog(message, false, Gtk::MESSAGE_INFO);
		pmd->set_title(_("Information"));
		pmd->run();
		break;
	case EL_Silent:
#ifdef DEBUG
		cout << "[Silent] " << message << endl;
#endif
		/* do nothing */
		break;
	}
	if(pmd) {
		pmd->hide();
		delete pmd;
	}
}

void jben_cleanup() {
	FrameMainGUI::Destroy();
	/* Careful about destruction order!
	   Proper Preferences::Destroy() call depends on ListManager! */
	Preferences::Destroy();
	ListManager::Destroy();
	KDict::Destroy();
	WDict::Destroy();
	DestroyUTFConv();
}

int main(int argc, char **argv) {
	Gtk::Main kit(argc, argv);

	KanjiList* kanjiList = NULL;
	VocabList* vocabList = NULL;

	/* Various initialization */
	srand(time(NULL));
	InitUTFConv();
	el.RegDisplayFunc(ErrorLogDisplayFunc);

	/* Dictionary loading, etc., depends on our config file. */
	Preferences *prefs = Preferences::Get();
	if(!prefs) {
		/* This -should- never occur now. */
		cerr << "Could not create preferences object.  FATAL ERROR!\n" << endl;
		return false;
	}

	string& cfgSaveTarget = prefs->GetSetting("config_save_target");
	if(cfgSaveTarget=="unset") {
#ifdef __WIN32__
		/* If config save target is unset, prompt the user. */
		DialogSetMobile dsm;

		Gtk::Main::run(dsm);
		if(cfgSaveTarget != "home" && cfgSaveTarget != "mobile") {
			jben_cleanup();
			return 0;
		}

#else
		/* Mobile configurations are currently unsupported on Linux;
		   default to home-based config. */
		cfgSaveTarget = "home";
#endif
	}

	const KDict* kd = KDict::Get();
	const WDict* wd = WDict::Get();
	ListManager* lm = ListManager::Get();

	if(wd->MainDataLoaded()) {
		lm->AddVocabList(DEFAULT_LIST_NAME);
		lm->ChooseVocabList(DEFAULT_LIST_NAME);
		vocabList = lm->VList();
		vocabList->AddList(
			utfconv_mw(prefs->GetSetting("vocablist")));
	}
	else vocabList = NULL;
	if(kd->MainDataLoaded()) {
		lm->AddKanjiList(DEFAULT_LIST_NAME);
		lm->ChooseKanjiList(DEFAULT_LIST_NAME);
		kanjiList = lm->KList();
		kanjiList->AddFromString(
			utfconv_mw(prefs->GetSetting("kanjilist")));
	}
	else kanjiList = NULL;

	FrameMainGUI& gui = FrameMainGUI::Get();
	Gtk::Main::run(gui);

	jben_cleanup();
	return 0;
}
