#include "widget_storeddialog.h"
#include "preferences.h"
#include "string_utils.h"
#include <list>
#include <boost/format.hpp>
#include <iostream>

StoredDialog::StoredDialog
(const Glib::ustring& title, Gtk::Window& parent, const std::string& sizeKey)
	: Dialog(title, parent)
{
	this->prefsKey = sizeKey;

	Preferences* p = Preferences::Get();
	string& size = p->GetSetting(prefsKey);
	int x, y;
	if(size.length()>0) {
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
