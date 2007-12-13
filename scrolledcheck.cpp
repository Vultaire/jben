/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: scrolledcheck.cpp

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

#include "scrolledcheck.h"

BEGIN_EVENT_TABLE(ScrolledCheck, wxCheckBox)
	EVT_SET_FOCUS(ScrolledCheck::OnSetFocus)
END_EVENT_TABLE()

ScrolledCheck::ScrolledCheck(wxScrolledWindow *owner, int id, int localIndex, const wxString& label)
  : wxCheckBox(owner, id, label) {
	index = localIndex; /* This is our index of which checkbox this is within its parent wxScrolledWindow. */
}

void ScrolledCheck::OnSetFocus(wxFocusEvent& event) {
	int viewX, viewY, sizeY, stepY, sizeInYSteps, newY;
	wxScrolledWindow *scrl = (wxScrolledWindow *)this->GetParent();

	/* Get our current step index and the total number of y steps in our dialog */
	scrl->GetViewStart(&viewX, &viewY);
	scrl->GetClientSize(NULL, &sizeY);
	scrl->GetScrollPixelsPerUnit(NULL, &stepY);
	sizeInYSteps = sizeY / stepY;

	/* Calculate whether we need to scroll the window, or whether we just leave it alone. */
	newY = viewY;
	if(index < viewY)
		newY = index;
	else if(index > viewY + sizeInYSteps-1)
		newY = index - (sizeInYSteps-1);

	if(newY!=viewY)	scrl->Scroll(viewX, newY);
}
