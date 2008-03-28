#include "dialog_setmobile.h"

#include <glibmm/i18n.h>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>

DialogSetMobile::DialogSetMobile()
	: Gtk::MessageDialog(_("How do you want to use J-Ben?"), false,
						 Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE),
	  btnStandard(_("Standard")),
	  btnMobile  (_("Mobile"))
{
	set_deletable(false);
	set_title(_("Standard or Mobile?"));
	set_secondary_text(
		_("If this is a normal installation, please click \"Standard\".\n"
		  "If J-Ben was installed onto a USB or other removable disk, please "
		  "click \"Mobile\".\n"
		  "(This setting can be changed later via the Edit->Preferences "
		  "menu.)"));

	btnStandard.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogSetMobile::OnStandard));
	btnMobile.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogSetMobile::OnMobile));

	Gtk::HButtonBox* phbb = get_action_area();
	phbb->pack_start(btnStandard);
	phbb->pack_start(btnMobile);

	show_all_children();
}

void DialogSetMobile::OnStandard() {
	response(DSM_STANDARD);
}

void DialogSetMobile::OnMobile() {
	response(DSM_MOBILE);
}
