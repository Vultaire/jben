#ifndef gtk_utils_h
#define gtk_utils_h

#include <string>
#include <gtkmm/textbuffer.h>

void SetTextBuf(Glib::RefPtr<Gtk::TextBuffer> rpBuf, std::string& text);

#endif
