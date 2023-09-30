#ifndef SDK_TRACE_H
#define SDK_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

void PFprintf( const char * format, ... );

#ifdef DEBUG

#define TRACE(format, ...)		PFprintf (format, ## __VA_ARGS__)
#define TRACE_STRING(format)	PFprintf (format)


#define dbg(format, ...) \
     TRACE("*line:%d:  " format "\n", __LINE__ , ## __VA_ARGS__)


#else // #ifndef DEBUG

#define TRACE(format, ...)
#define TRACE_STRING(format)
//#define data_TRACE(data,size)


#define dbg(format, ...)

#endif


#define TRACE2		UtilLogStringData

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // SDK_TRACE_H
