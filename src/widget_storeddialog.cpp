/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: widget_storeddialog.cpp

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

#include "widget_storeddialog.h"
#include "preferences.h"
#include "string_utils.h"
#include <list>
#include <boost/format.hpp>

StoredDialog::StoredDialog
(const Glib::ustring& title, Gtk::Window& parent, const std::string& sizeKey)
	: Dialog(title, parent)
{
	this->prefsKey = sizeKey;

	Preferences* p = Preferences::Get();
	string& size = p->GetSetting(prefsKey);
	int x, y;
	if(!size.empty()) {
		std::list<string> ls = StrTokenize<char>(size, "x");
		if(ls.size()>=2) {
			x = atoi(ls.front().c_str());
			ls.pop_front();
			y = atoi(ls.front().c_str());
			set_default_size(x,y);
		}
	}
}

StoredDialog::~StoredDialog() {
	Preferences* p = Preferences::Get();
	int x, y;
	get_size(x,y);
	string& size = p->GetSetting(prefsKey);
	size = (boost::format("%dx%d") % x % y).str();
}
