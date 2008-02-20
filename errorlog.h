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

#define ERR_PREF __FILE__ << ':' << __LINE__ << ": "

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
	ErrorLog();
	int Size();
	int Count(ELType t);
	string PopFront(ELType t);
	string PopBack(ELType t);
	void Push(ELType t, string message, void *srcObj=NULL);
	void RegDisplayFunc(
		void (*newDispFunc)(ELType, const string&, void*));
private:
	list<string>* GetList(ELType t);
	/* This function lets us register gui-dependent external code
	   for displaying error messages. */
	void (*dispFunc)(ELType, const string&, void*);

	map<ELType, list<string> > logdata;
};

/* ErrorLog is not technically a singleton and an app could have separate error
   logs for different objects.  However, for our app I think we'll just do one,
   for which the global is below. */
extern ErrorLog el; 

#endif
