/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: preferences.h

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

#ifndef preferences_h
#define preferences_h

#define PREFS_SUCCESS      0x0
#define PREFS_FILENOTFOUND 0x01u
#define PREFS_UNHANDLED    0x80000000u

#include <map>
#include <string>
#include <boost/regex.hpp>
using namespace std;

class Preferences {
public:
	static Preferences *Get();
	static void Destroy();
	~Preferences();
	void SetDefaultPrefs();
	string& GetSetting(string key, string defaultValue = "");
	list<string> GetKeyList(string searchKey);
	list<string> GetKeyList(boost::regex exp);

	/* Options contained within config file - these may eventually go away. */
	unsigned long kanjidicOptions;
	unsigned long kanjidicDictionaries;
private:
	int LoadFile(const char *filename);
	string GetPrefsStr();
	void UpgradeConfigFile();

	/* Use a map for storing most other options we may add. */
	map<string, string> stringOpts;
	string originalSaveTarget;

	/* Singleton-related stuff */
	Preferences();
	static Preferences *prefsSingleton;	
};

#endif
