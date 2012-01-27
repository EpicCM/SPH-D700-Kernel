#ifndef _UNICODE_H_
#define _UNICODE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//#include <stddef.h>

typedef unsigned short UniChar;

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif


int
UniIsSpace ( UniChar ch );

/* 
 * UniCode-String handling Functions
 * Implemented in UniStr.c
 */
size_t
UniStrLen( const UniChar* str );
UniChar*
UniStrCpy( UniChar* dest, const UniChar* src );
UniChar*
UniStrCat( UniChar* dest, const UniChar* src );
int
UniAcStrcmp( const UniChar* str1, const UniChar* str2 );
int
UniStrNCmp( const UniChar* str1, const UniChar* str2, size_t nChar );
double
UniStrToD( UniChar* str, UniChar** endPtr, BOOL* pSucceed );
long
UniStrToL( UniChar* str, UniChar** endPtr );
void
UniStrTrim( UniChar* src );
void
UniStrSqueeze( UniChar* src );
char*
UniStrToASCII( const UniChar* str );
UniChar*
UniFloatToStr( float fNum, int prec );

/* 
 * Element handling Functions
 * Implemented in UniElement.c
 */
int
UniGetElementNum( const UniChar* str, const UniChar seperator );
UniChar*
UniGetElementAt( const UniChar* str, const int index, const UniChar seperator );
void
UniRemoveElementAt( UniChar* str, const int index, const UniChar seperator );
UniChar*
UniInsertElementAt( const UniChar* str, const UniChar* element, const int index, const UniChar seperator );
UniChar*
UniReplaceElementAt( UniChar* str, const UniChar* element, const int index, const UniChar seperator );

UniChar*
ncs_newStrFromASCII( const char* src );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _UNICODE_H_ */
