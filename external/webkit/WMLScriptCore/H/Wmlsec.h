/*
 * Samsung Handset Platform
 * Copyright (c) 2000 Software Center, Samsung Electronics, Inc.
 * All rights reserved.
 */
/** 
 * @description	Defines the interface for the WMLS Cont
 * @author	venkat.nj@samsung.com
 */#ifndef _WMLSEC_H_
#define _WMLSEC_H_

//#include <Wmls.h>
#include <WmlsInterpret.h>


#define DllEXPORT  __attribute__((visibility("default")))

/* The states of the script engine */
#define WMLSE_STATE_IDLE				0x00
#define WMLSE_STATE_EXECUTING			0x01
#define WMLSE_STATE_SUSPENDED			0x02
#define WMLSE_STATE_ONEVENTEXECUTING 	0x03

#define IS_WMLSE_IDLE(pWmlseHnd)							(WMLSE_STATE_IDLE == ((WMLSEC *)pWmlseHnd)->cSEState)
#define IS_WMLSE_SUSPEND(pWmlseHnd)							(WMLSE_STATE_SUSPENDED == ((WMLSEC *)pWmlseHnd)->cSEState)
#define IS_WMLSE_EXECUTING(pWmlseHnd)						(WMLSE_STATE_EXECUTING == ((WMLSEC *)pWmlseHnd)->cSEState)
#define IS_WMLSE_ONEVENTEXECUTING(pWmlseHnd)				(WMLSE_STATE_ONEVENTEXECUTING == ((WMLSEC *)pWmlseHnd)->cSEState)

#define SET_WMLSE_STATE_ONEVENT_ENCOUNTERED(pWmlseHnd)		(((WMLSEC *)pWmlseHnd)->cSEState = WMLSE_STATE_ONEVENTEXECUTING)

typedef struct _wmlsec_
{
	unsigned char *pCompiledBuff ;
	unsigned int uiBuffLen ;
	WMLSRETCODE rIPExitCode ;
	void *pWMLSIntfClass ; /* Class pointer of WMLSInterface */
	/*unsigned char ucWMLSEState ;*/
	WMLS_BRWFUNC_HANDLER brwFunchandler ; /* Callback functions for browser dependedent calls*/

	char cSEState ; /* Contains the state of the script engine */
	char *cUrlContenttype ;
	NCDoc * pWmlsIntCtx ;
	WML_IP_HANDLER* pWmlIpHdlr;
} WMLSEC ;

#define WMLSEC_SET_CONTENT_TYPE(pSechnd, Value)	(((WMLSEC *)pSechnd)->cUrlContenttype = Value)

typedef struct _wmlsecInputParams_
{
	void *pWMLSIntfClass ;
	WMLS_BRWFUNC_HANDLER brwFunchandler ;

} WMLSECINPUTPARAMS ;

#define WMLS_GET_COMPILEDBUFF(vWmlshnd)	(((WMLSEC *)vWmlshnd)->pCompiledBuff)
#define WMLS_GET_COMPILEDBUFFLEN(vWmlshnd)	(((WMLSEC *)vWmlshnd)->uiBuffLen)

#define WMLS_SET_COMPILEDBUFF(vWmlshnd,buffer)	(((WMLSEC *)vWmlshnd)->pCompiledBuff = buffer)
#define WMLS_SET_COMPILEDBUFFLEN(vWmlshnd,buff_len)	(((WMLSEC *)vWmlshnd)->uiBuffLen = buff_len)

/* Different states of WMLSE */

/* Script engine input params */
typedef struct _wmlsinputparams_
{
	char			*InpBuff;	/* Input Buffer Containing the Script */
	int	InpBuffLen;	/* Buffer Length */
	const char			*pFragment ;
	const char			*pDocUrl ;
	char			*pBaseUrl ;
	char			*pRefree ;
	char			*pReferrer ;
	int				iCharset ;	/* Defines the charset of the input buffer */
	char			*urlContenttype ; /* Determines the content type of the downloaded content */
	
	void *pInterfacePtr ; /* interface pointer for use in alert/confirm/prompt */

} WMLSINPUTPARAMS ;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Init the script engine context */
DllEXPORT WMLSRETCODE 
WmlsecInit(WMLSECINPUTPARAMS *pSecIPParams, void **pwmlseCtx);

/* DeInit the script engine context */
DllEXPORT void
WmlsecDeInit(void *pwmlseCtx);

/*	Compiles the given script input. the compiled buffer
	is stored in the pWmlseCtx handle.
	The return code is one of the following:
	WMLS_OK : Success
	WMLS_ERR_SYNTAX_ERROR
*/
DllEXPORT WMLSRETCODE
WmlsecCompileScript(void *pWmlseCtx, 
					char *InpBuff, 
					unsigned int InpBuffLen,
					int charset);
/*	Executes the given input buffer set in the WMLSECPARAMS 
	The return code is one of the following:
	WMLS_OK : Success
	WMLS_ERR_EXT_URL_ENCOUNTERED : Encountered an external script
	WMLS_ERR_RUNTIME_ERROR
*/
DllEXPORT WMLSRETCODE
WmlsecExecuteScript(void *pWmlseCtx, 
					WMLSINPUTPARAMS *pInputParams);
/*	Compiles the given script input and executes the same.
	The result of the script content is stored in the pWmlseCtx handle.
	The return code is one of the following:
	WMLS_OK : Success
	WMLS_ERR_EXT_URL_ENCOUNTERED : Encountered an external script
	WMLS_ERR_SYNTAX_ERROR
	WMLS_ERR_RUNTIME_ERROR
*/
DllEXPORT WMLSRETCODE 
WmlsecEvaluateScript(void *pWmlseCtx, 
					 WMLSINPUTPARAMS *pInputParams);

/*	Compiles the given script input and executes the same.
	The result of the script content is stored in the pWmlseCtx handle.
	The return code is one of the following:
	WMLS_OK : Success
	WMLS_ERR_EXT_URL_ENCOUNTERED : Encountered an external script
	WMLS_ERR_RUNTIME_ERROR
*/
DllEXPORT WMLSRETCODE 
WmlsecResumeExecution(void *pWmlseCtx,
					  WMLSINPUTPARAMS *pInputParams) ;

/*	Terminates the Script engine execution.
	The return code is one of the following:
	WMLS_OK : Success
*/
DllEXPORT void 
WmlsecTerminateExecution(void *pWmlseCtx);

//#include "WMLScriptECOM.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // _WMLSEC_H_
