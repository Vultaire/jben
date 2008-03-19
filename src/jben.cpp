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

int main(int argc, char **argv) {
	Gtk::Main kit(argc, argv);

	KanjiList* kanjiList = NULL;
	VocabList* vocabList = NULL;

	/* the below -might- help on win32 systems, but for now is unused. */
#if 0
#ifndef __WIN32__
	setlocale(LC_ALL, "");
#endif
#endif
	
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

	/* Destruction */
	FrameMainGUI::Destroy();
	/* Careful about destruction order!
	   ~Preferences depends on ListManager! */
	Preferences::Destroy();
	ListManager::Destroy();
	KDict::Destroy();
	WDict::Destroy();
	DestroyUTFConv();
	return 0;
}
