/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dialog_setmobile.cpp

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

#include "dialog_setmobile.h"

#include <glibmm/i18n.h>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>
#include "preferences.h"

DialogSetMobile::DialogSetMobile()
	: Gtk::MessageDialog(_("How do you want to use J-Ben?"), false,
						 Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE),
	  btnStandard(_("Standard")),
	  btnMobile  (_("Mobile"))
{
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
	Preferences::Get()->GetSetting("config_save_target") = "home";
	response(DSM_STANDARD);
	hide();
}

void DialogSetMobile::OnMobile() {
	Preferences::Get()->GetSetting("config_save_target") = "mobile";
	response(DSM_MOBILE);
	hide();
}
