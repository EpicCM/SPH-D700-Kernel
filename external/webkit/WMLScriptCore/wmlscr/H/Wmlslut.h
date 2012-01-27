/** 
* Samsung Handset Platform
* Copyright (c) 2007 Samsung Electronics, Inc.
* All rights reserved. 
*/
/*
*  <descr>
*  @author 
*/
#ifndef _WMLSLUT_H
#define _WMLSLUT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <WmlsParser.h> /* Contains the WML Script tokens */

typedef struct KeywordEntry{
	const char *s;	// Keyword
	int token;		// Token Value
}KeywordEntry;


static const struct KeywordEntry KeywordMainTable[] = {

	/* Keywords used by WMLS */
	{ "access" , tACCESS },
	{ "agent" , tAGENT },
	{ "break" , tBREAK },
	{ "continue" , tCONTINUE },
	{ "div" , tIDIV },
	{ "div=" , tIDIVEQ },
	{ "domain" , tDOMAIN },
	{ "else" , tELSE },
	{ "equiv" , tEQUIV },
	{ "extern" , tEXTERN },
	{ "for" , tFOR },
	{ "function" , tFUNCTION },
	{ "header" , tHEADER },
	{ "http" , tHTTP },
	{ "if" , tIF },
	{ "isvalid" , tISVALID },
	{ "meta" , tMETA },
	{ "name" , tNAME },
	{ "path" , tPATH },
	{ "return" , tRETURN },
	{ "typeof" , tTYPEOF },
	{ "use" , tUSE },
	{ "user" , tUSER },
	{ "var" , tVAR },
	{ "while" , tWHILE },
	{ "url" , tURL },

	/* Keywords not used by WMLS */
	{ "delete" , 0 },
	{ "in" , 0 },
	{ "lib" , 0 },
	{ "new" , 0 },
	{ "null" , 0 },
	{ "this" , 0 },
	{ "void" , 0 },
	{ "with" , 0 },

	/* Future reserved words */
	{ "case" , 0 },
	{ "catch" , 0 },
	{ "class" , 0 },
	{ "const" , 0 },
	{ "debugger" , 0 },
	{ "default" , 0 },
	{ "do" , 0 },
	{ "enum" , 0 },
	{ "export" , 0 },
	{ "extends" , 0 },
	{ "finally" , 0 },
	{ "import" , 0 },
	{ "private" , 0 },
	{ "public" , 0 },
	{ "sizeof" , 0 },
	{ "struct" , 0 },
	{ "super" , 0 },
	{ "throw" , 0 },
	{ "try" , 0 }
};

#ifdef __cplusplus
} //extern "C"
#endif /* __cplusplus */

#endif //_WMLSLUT_H
