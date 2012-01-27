/** 
* Samsung Handset Platform
* Copyright (c) 2007 Samsung Electronics, Inc.
* All rights reserved. 
*/
/*
*  <descr>
*  @author 
*/
#ifndef _WMLSLIBLOOKUP_H
#define _WMLSLIBLOOKUP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <Wmls.h>

/* Mapping of Library Name to Library Index */
typedef struct _LibNameMap
{
	int lIndex;
	MCHAR *lName;	// Library Name
}LibNameMap;

/* Library lookup table */
typedef struct _LibElementMap
{
	int lIndex;
	int fIndex;
	MCHAR *fName;	// Function Name
	int paraNum;
}LibElementMap;

int wmlsLookupLibFuncName(int iLibIndex, MCHAR *pmFuncName) ;

int wmlsLookupLibName(MCHAR *pmLibName) ;

int wmlsNumofArgsInLibFunc(int iLibIndex, int iFuncIndex) ;

#ifdef __cplusplus
} //extern "C"
#endif /* __cplusplus */

#endif //_WMLSLIBLOOKUP_H
