#ifndef dialog_SetMobile_h
#define dialog_SetMobile_h

#include <gtkmm/messagedialog.h>
#include <gtkmm/button.h>

#define DSM_STANDARD 1
#define DSM_MOBILE   2

class DialogSetMobile : public Gtk::MessageDialog {
public:
	DialogSetMobile();
private:
	void OnStandard();
	void OnMobile();

	Gtk::Button btnStandard, btnMobile;
};

#endif
