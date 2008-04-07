#ifndef jben_defines_h
#define jben_defines_h

#ifdef __WIN32__
#	define CFGFILE "jben.cfg"
#	define HOMEENV "APPDATA"
#else
#	define CFGFILE ".jben"
#	define HOMEENV "HOME"
#endif

#ifndef JB_DATADIR
#	define JB_DATADIR ".."
#endif

#endif
