/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: encoding_convert.h

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

#ifndef encoding_convert_h
#define encoding_convert_h

#include <iconv.h>
#include <string>
using namespace std;

extern string wcType;

#define utfconv_wm(data) ConvertString<wchar_t, char>(data, wcType.c_str(), "UTF-8")
#define utfconv_mw(data) ConvertString<char, wchar_t>(data, "UTF-8", wcType.c_str())

template <class tsrc, class tdest>
basic_string<tdest> ConvertString
(const basic_string<tsrc>& sourceData,
 const char* sourceEncoding,
 const char* targetEncoding)  {
	basic_string<tdest> result;
	int sLen = sourceData.length()+1;
	size_t inputBytesLeft  = sLen * sizeof(tsrc);
	size_t outputBytesLeft = sLen * 4;
	char *buffer = new char[outputBytesLeft];
	memset((void*)buffer, 0, outputBytesLeft);
	/* The libc iconv function takes a char* source, while the libiconv iconv
	   takes a const char* source. */
#ifdef __WXMSW__
	const char *srcData = (char *)sourceData.c_str();
#else
	char *srcData = (char *)sourceData.c_str();
#endif

	char *destData = buffer;

	/* libiconv stuff */
	size_t retcode;
	iconv_t conv = iconv_open(targetEncoding, sourceEncoding);
	if(conv == (iconv_t)-1) goto exit_now;
	retcode = iconv(conv,
						   &srcData, &inputBytesLeft,
						   &destData, &outputBytesLeft);
	if(retcode==(size_t)-1) {
		/* If we put in error reporting later, we'll want this to return
		   conversion errors.  For now though, we'll do nothing and simply
		   bail. */
		goto exit_now;
	}
	iconv_close(conv);

	/* Copy result data to the string or wstring */
	result = (tdest*) buffer;

exit_now:
	delete[] buffer;
	return result;
}

#endif
