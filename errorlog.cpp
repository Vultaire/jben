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

ErrorLog el;

list<string>* ErrorLog::GetList(ELType t) {
	switch(t) {
	case EL_Error:
		return &errors;
	case EL_Warning:
		return &warnings;
	case EL_Info:
		return &info;
	}
	return NULL;
}

int ErrorLog::Size() {
	return errors.size() + warnings.size() + info.size();
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

void ErrorLog::Push(ELType t, string message) {
	list<string> *l = GetList(t);
	l->push_back(message);
}
