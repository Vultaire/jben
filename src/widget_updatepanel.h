#ifndef widget_updatepanel_h
#define widget_updatepanel_h

#include <gtkmm/box.h>

class UpdatePanel : public Gtk::VBox {
public:
	UpdatePanel(bool homogeneous=false, int spacing=5);
	virtual void Update()=0;
};

#endif
