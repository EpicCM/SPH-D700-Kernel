/*
 * Samsung Handset Platform
 * Copyright (c) 2000 Software Center, Samsung Electronics, Inc.
 * All rights reserved.
 */
/** 
 *
 * @author	venkat.nj@samsung.com
 */
#ifndef _WMLS_H_
#define _WMLS_H_

#include <WmlsPlatform.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



#define WMLS_DEBUG_MODE			1

#define SFB_SUPPORT_WMLSCRIPT		1

#define _SMB2_WAP_WTA			1

typedef int				WMLSRETCODE ;



#ifndef true
#define true	1
#endif
#ifndef false
#define false	0
#endif

#define RCODE_OK				(0 == rCode)
/* Different error codes */
#define WMLS_OK							0
#define	WMLS_ERR_MEM_ALLOC				-1
#define WMLS_ERR_INVALID_TYPE			-2
#define WMLS_ERR_VAL_NOT_FOUND			-3
#define WMLS_ERR_INVALID_PARAM			-4
#define WMLS_ERR_NAME_ALREADY_PRESENT	-5
#define WMLS_ERR_FUNCTION_FAILED		-6
#define WMLS_ERR_EXT_URL_ENCOUNTERED	-7

/* Memory Allocation and free Macros */
#if 0
/* This will be Removed once when Interfaces are stable */
#define WMLS_MEM_ALLOC(vBCHnd, len, size)	(calloc(len, size))
#define WMLS_MEM_REALLOC(vBCHnd, oldptr, memsize)	(realloc(oldptr, memsize))
#define WMLS_MEM_FREE(vBCHnd, ptr)			if (ptr) free(ptr)
# endif

#define WMLS_MEM_ALLOC(vBCHnd, len, size)	(WmlsCalloc(len, size))
#define WMLS_MEM_REALLOC(vBCHnd, oldptr, memsize)	(WmlsRealloc(oldptr, memsize))
#define WMLS_MEM_FREE(vBCHnd, ptr)			WmlsFree(ptr)


/* WMLS Interpreter Integration */
#if 0 
#ifndef MemAlloc
#define MemAlloc	malloc
#endif
#ifndef MemFree
#define MemFree		free
#endif
#endif


MCHAR *
wmlsDupMCharStr(MCHAR *pszInpStr) ;

#define WmlSwcsdup	wmlsDupMCharStr

/* math definitions */
#define SIGNED_8BIT_INT_MIN		-128
#define SIGNED_8BIT_INT_MAX		127
#define SIGNED_16BIT_INT_MIN	-32768
#define SIGNED_16BIT_INT_MAX	32767
#define SIGNED_32BIT_INT_MIN	-2147483648
#define SIGNED_32BIT_INT_MAX	2147483647
#define SIGNED_32BIT_FLOAT_MIN	-3.40282347E38
#define SIGNED_32BIT_FLOAT_MAX	3.40282347E38

#define WMLS_MAX_16BITUNSIGNED	0xffff

/*	These functions define the browser functions which perform 
	data management at the WMLSInterface
*/
/* Prototype for calling the interface handler for Browser.getVar */
typedef char*(*getVarFromBrw)	(void *pWMLIntfClass, 
								 char* name) ;
/* Not used anymore... Shall remove the same later  */
typedef BOOL(*isValidVarNameBrw) (void *pWMLIntfClass, 
								 char* name) ;
/* Prototype for calling the interface handler for Browser.setVar  */
typedef void(*setVarOfBrw)		(void *pWMLIntfClass, 
								char* VarName,
								char* VarValue) ;
/* Prototype for calling the interface handler for Browser.newContext */
typedef void(*newBrwContext)	(void *pWMLIntfClass) ;
/* Prototype for calling the interface handler for getting the current URL */
typedef char*(*getCurrentURL)	(void *pWMLIntfClass) ;
/* Prototype for calling the interface handler for Browser.getCurrentCard */
typedef char*(*getCurrentCard)	(void *pWMLIntfClass);
/* Prototype for calling the interface handler for Browser.refresh */
typedef void(*refreshDoc)		(void *pWMLIntfClass);
/* Prototype for calling the interface handler for Browser.go */
typedef void(*BrwGo)		(void *pWMLIntfClass, 
								 char *Value);
/* Prototype for calling the interface handler for Browser.prev */
typedef void(*BrwPrev)		(void *pWMLIntfClass, 
								 char *Value);
/* Prototype for calling the interface handler when wanting to resume executing */
typedef void(*BrwResumeScriptExec)		(void *pWMLIntfClass, 
										char *Value);
/* Prototype for calling the interface handler when downloading an external url */
typedef void(*BrwDownloadExtUrl)		(void *pWMLIntfClass, 
										char *Value);
/* Prototype for calling the interface handler when downloading an external url based on content type */
typedef void(*BrwDownloadUrl)		(void *pWMLIntfClass, 
										char *Value, char *contentType);
/* Prototype for calling the interface handler when running WmlScript Alert/Confirm/Prompt */
typedef BOOL(*RunConfirm)		(void *pWMLIntfClass, const char *Value1, const char *Value2, const char *Value3);
typedef BOOL(*RunAlert)			(void *pWMLIntfClass, const char *Value1);
typedef BOOL(*RunPrompt)		(void *pWMLIntfClass, const char *Value1,const char *Value2);

typedef struct wmlsBrwFuncHandler
{
	getVarFromBrw		BrwgetVar ;
	setVarOfBrw			BrwsetVar ;
	newBrwContext		BrwCtx ;
	getCurrentURL		BrwCurrUrl ;
	getCurrentCard		BrwCurrCard ;
	refreshDoc			BrwRefreshDoc ;
	BrwGo				BrwGoValue ;
	BrwPrev				BrwPrevValue ;

	BrwResumeScriptExec BrwResScriptExecValue ; /* Resumes the script Exec through browser (in case of alert/confirm/prompt */

	BrwDownloadExtUrl	BrwDownloadExtUrlValue ;
	BrwDownloadUrl		BrwDownloadUrlValue ;

	RunAlert			BrwRunAlert;
	RunConfirm			BrwRunConfirm;
	RunPrompt			BrwRunPrompt;

} WMLS_BRWFUNC_HANDLER ;

typedef struct _wmlHandler
{
	void *pWMLSIntfClass ;
	WMLS_BRWFUNC_HANDLER brwFunchandler ;

} WML_IP_HANDLER ;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // _WMLS_H_
