/**
* Samsung Handset Platform
* Copyright (c) 2007 Samsung Electronics, Inc.
* All rights reserved.
*/
/*
*  <description>
*  @author
*/

#ifndef _WMLSINTUTILS_H
#define _WMLSINTUTILS_H

#include <Wmls.h>
#include <SfbWmlsTypes.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _WMLSIPURI
{
        char    *access;                /* Now known as "scheme" */
        char    *host;
        char    *absolute;
        char    *relative;
        char    *fragment;
} WMLSIPURI ;

int wmls_strdup(char **dest,char *src);
void wmls_scanUrl(char* szName, WMLSIPURI* pParts);
char* wmls_getUrl(const char* szAName, const char* szRelatedName);
char* wmls_canoUrl (char** pszFileName, char* szHost);
char* wmls_simplifyUrl (char** pszUrl);


#ifdef __cplusplus
} //extern "C"
#endif /* __cplusplus */

#endif	// _WMLSINTUTILS_H
