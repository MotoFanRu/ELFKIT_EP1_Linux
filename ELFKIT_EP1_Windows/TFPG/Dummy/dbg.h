#ifndef ADBG__H
#define ADBG__H

#include <utilities.h>


void Log( const char * format, ... );

//#define Log		UtilLogStringData


#define dbg( format ) PFprintf( format "\n" )

#ifndef WIN32
	#define dbgf( format, ... ) PFprintf( format "\n", ## __VA_ARGS__ )
#else
	#define dbgf PFprintf
#endif

#endif
