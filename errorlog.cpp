/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: errorlog.cpp

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

#include "errorlog.h"
#include "encoding_convert.h" /* Required for wxMessageBox output */

ErrorLog el;

list<string>* ErrorLog::GetList(ELType t) {
	return &logdata[t];
}

int ErrorLog::Size() {
	size_t s=0;
	for(map<ELType, list<string> >::iterator mi = logdata.begin();
		mi != logdata.end(); mi++) {
		s += mi->second.size();
	}
	return s;
}

int ErrorLog::Count(ELType t) {
	list<string> *l = GetList(t);
	if(!l) return -1;
	return l->size();
}

string ErrorLog::PopFront(ELType t) {
	string s;
	list<string> *l = GetList(t);
	if(l) {
		s = l->front();
		l->pop_front();
	}
	return s;
}

string ErrorLog::PopBack(ELType t) {
	string s;
	list<string> *l = GetList(t);
	if(l) {
		s = l->back();
		l->pop_back();
	}
	return s;
}

void ErrorLog::Push(ELType t, string message, void *srcObj) {
	list<string> *l = GetList(t);
	l->push_back(message);
	if(t==EL_Error) {
		wxMessageBox(utfconv_mw(message), _T("Error"),
					 wxOK | wxICON_ERROR, (wxWindow *)srcObj);
	} else if(t==EL_Warning) {
		wxMessageBox(utfconv_mw(message), _T("Warning"),
					 wxOK | wxICON_WARNING, (wxWindow *)srcObj);
	} else if(t==EL_Info) {
		wxMessageBox(utfconv_mw(message), _T("Info"),
					 wxOK | wxICON_INFORMATION, (wxWindow *)srcObj);
	}
}
