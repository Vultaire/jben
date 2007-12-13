/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dictload.h

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

#ifndef dictload_h
#define dictload_h

#define DL_SUCCESS            0x0
#define DL_KANJIDIC_NOT_FOUND 0x1
#define DL_EDICT_NOT_FOUND    0x2
#define DL_FILE_IO_ERROR      0x4

/* This class used to be intended as a splash window,
   but loading KANJIDIC takes almost no time. Maybe after
   adding EDICT we might change it back, but for now it's
   a non-GUI class. */
/*class DictionaryLoader: public wxFrame {*/
class DictionaryLoader {
public:
	DictionaryLoader();
	int LoadDictionaries();
};

#endif
