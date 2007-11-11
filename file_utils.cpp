/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: file_utils.cpp

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

#include "file_utils.h"
#include "wx/file.h"
#include "wx/ffile.h"
#include "wx/strconv.h"

/* Tries to open the specified file.  If successful, it'll read in its entire
   contents into a wxString, and apply the specified conversion. */
int ReadEncodedFile(const wxChar *filename, const wxChar *src_encoding, wxString& dest) {
	/* Read in the file.  NOTE: a "b" flag MIGHT be necessary for
	   Windows! */
	if(wxFile::Exists(filename)) {
		wxFFile *in = new wxFFile(filename, _T("r"));
		if(!in->IsOpened()) {  /* Bail if we couldn't read it */
			delete in;
			return REF_FILE_OPEN_ERROR;
		}

		/* Read in all the data - our files are "small", so we'll be lazy
	   	and "read all". */
		/* NOTE: Workaround for SLOW GTK performance
	         	provided by "doublemax" on wxforum.shadonet.com.
	   	What happens:
	   	1. Standard C functions are used to get the raw file size
	   	2. The file is read in using the wxFFile::Read() command instead of
	      	ReadAll().  Read does not perform local encoding to Unicode
	      	conversion.
	   	3. Use the wxString::GetWriteBuf function to get us a buffer to receive
	      	the wide character conversion.  File length * 2 is used, which isn't
	      	very memory efficient, but is CPU-efficient - far more important.
	   	4. wxCSConv::MB2WC is called to perform the conversion after the file
	      	is loaded.  This happens FAST. */

		size_t length = (size_t)in->Length();
		wxCharBuffer buf(length+1);
		length = in->Read(buf.data(), length);
		buf.data()[length]=_T('\0');
		in->Close();
		delete in;

		wxCSConv transcoder(src_encoding);

		length++;  /* Incremented so we will include the null character as part of
		              the copied string. */
		transcoder.MB2WC(dest.GetWriteBuf(length*2), buf.data(), length);
		dest.UngetWriteBuf();

		return REF_SUCCESS;
	} else return REF_FILE_NOT_FOUND;
}

/* Tries to open the specified file.  If successful, it'll read in its entire
   contents into a wxString, and apply the local-to-Unicode conversion.
   (Yes, this is -another- ugly workaround for just doing a wxFFile::ReadAll.)
   */
int ReadFile(const wxChar *filename, wxString& dest) {
	/* Read in the file.  NOTE: a "b" flag MIGHT be necessary for
	   Windows! */
	if(wxFile::Exists(filename)) {
		wxFFile *in = new wxFFile(filename, _T("r"));
		if(!in->IsOpened()) {  /* Bail if we couldn't read it */
			delete in;
			return REF_FILE_OPEN_ERROR;
		}

		size_t length = (size_t)in->Length();
		wxCharBuffer buf(length+1);
		length = in->Read(buf.data(), length);
		buf.data()[length]=_T('\0');
		in->Close();
		delete in;

		length++;  /* Incremented so we will include the null character as part of
	              	the copied string. */
		wxConvCurrent->MB2WC(dest.GetWriteBuf(length*2), buf.data(), length);
		dest.UngetWriteBuf();

		return REF_SUCCESS;
	} else {
#ifdef DEBUG
		printf("Could not find file \"%ls\".\n", filename);
#endif
		return REF_FILE_NOT_FOUND;
	}
}
