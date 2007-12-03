/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: global.h

This file is part of J-Ben.

J-Ben is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2
as published by the Free Software Foundation.

J-Ben is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef global_h
#define global_h

#define AUTHOR_NAME  "Paul Goins"
#define PROGRAM_NAME "J-Ben"
#define VERSION_STR  "1.1.1"
#define COPYRIGHT_DATE "2007"

#include "jben.h"
#include "preferences.h"

extern JBen *jben;
extern Preferences *prefs;

/* SKIP support should not be compiled in at this point until we can ensure that
   SKIP data can not be copied to the clipboard.
   I think this may already be the case, but I'd prefer to be careful in respect
   of the copyright holder's wishes. */
#if 0
#define USE_SKIP
#endif

#endif
