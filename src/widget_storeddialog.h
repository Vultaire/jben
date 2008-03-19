#ifndef widget_storeddialog_h
#define widget_storeddialog_h

#include <gtkmm/dialog.h>

class StoredDialog : public Gtk::Dialog {
public:
	StoredDialog(const Glib::ustring& title, Gtk::Window& parent,
				 const std::string& sizeKey);
	virtual ~StoredDialog();
private:
	std::string prefsKey;
};

#endif
