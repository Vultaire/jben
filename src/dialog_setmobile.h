/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dialog_setmobile.h

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
