#ifndef _URL_H_
#define _URL_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

typedef struct _WMLSURI
{
	char* scheme;
	char* host;
	char* port;
	char* absolute;
	char* relative;
	char* param;
	char* query;
	char* fragment;
} WMLSURI;

typedef enum _HTURIEncoding
{
	URL_XALPHAS = 0x1,		/* Escape all unsafe characters */
	URL_XPALPHAS = 0x2,		/* As URL_XALPHAS but allows '+' */
	URL_PATH = 0x4,			/* As URL_XPALPHAS but allows '/' */
	URL_DOSFILE = 0x8		/* As URL_URLPATH but allows ':' */
} HTURIEncoding;

int
URLIsValid( const char* url );
char*
URLResolve( const char* base, const char* url );
int
URLIsAbsolute (const char* url);
char*
URLGetScheme( const char* url );
char*
URLGetHost( const char* url );
char*
URLGetPort( const char* url );
char*
URLGetPath( const char* url );
char*
URLGetParam( const char* url );
char*
URLGetQuery( const char* url );
char*
URLGetFragment( const char* url );

char*
URLEscape (const char* str, HTURIEncoding mask);
void
URLUnEscape (char const* str);

char*
ncs_newASCIIFromASCII( const char* src );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _URL_H_ */

