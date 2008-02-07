/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: errorlog.h

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

#ifndef errorlog_h
#define errorlog_h

/* We include wxWidgets here simply for the ease of putting errors and
   warnings in immediate view via wxMessageBox messages.  This can easily be
   replaced with an alternative from another toolkit, or even just left
   silent and depend on the program polling the error log object.
   (Of course, errors at the very least really shouldn't be silent.) */
/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include <list>
#include <map>
#include <string>
using namespace std;

enum ELType {
	EL_Error=1,
	EL_Warning,
	EL_Info,
	EL_Silent
};

class ErrorLog {
public:
	int Size();
	int Count(ELType t);
	string PopFront(ELType t);
	string PopBack(ELType t);
	void Push(ELType t, string message, void *srcObj=NULL);
private:
	list<string>* GetList(ELType t);

	map<ELType, list<string> > logdata;
};

/* ErrorLog is not technically a singleton and an app could have separate error
   logs for different objects.  However, for our app I think we'll just do one,
   for which the global is below. */
extern ErrorLog el; 

#endif
