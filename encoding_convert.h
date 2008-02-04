#ifndef encoding_convert_h
#define encoding_convert_h

#include <iconv.h>
#include <string>
using namespace std;

extern string wcType;

template <class tsrc, class tdest>
basic_string<tdest> ConvertString
(const basic_string<tsrc>& sourceData,
 const char* sourceEncoding,
 const char* targetEncoding)  {
	basic_string<tdest> result;
	size_t inputBytesLeft = (sourceData.length()+1) * sizeof(tsrc);

	/* Set sizing vars, and create a byte buffer sufficiently large for any
	   conversion. */
	size_t outputBytesLeft =
		(inputBytesLeft) * (sizeof(wchar_t) / sizeof(tsrc));
	char *buffer = new char[outputBytesLeft + sizeof(wchar_t)];
	memset((void*)buffer, 0, outputBytesLeft + sizeof(wchar_t));
	char *srcData = (char *)sourceData.c_str();
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
