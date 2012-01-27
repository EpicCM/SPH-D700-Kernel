/*
 * Samsung Handset Platform
 * Copyright (c) 2000 Software Center, Samsung Electronics, Inc.
 * All rights reserved.
 */
/** 
 *
 * @author	venkat.nj@samsung.com
 */

#ifndef _WMLS_INTERPRET_H_
#define _WMLS_INTERPRET_H_

#include <Wmls.h>
#include <SfbWmlsTypes.h>
#include <SfbWmlsInterpret.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MID_BROWSER	0
#define SHP_VSWAP_ONLY

/* WMLScript engine Interpreter input params */
typedef struct _wmlsInterpreterInputparams_
{
	char			*InpBuff;	/* Input Buffer Containing the Script */
	unsigned int	InpBuffLen;	/* Buffer Length */
	const char		*pFragment ;
	const char		*pDocUrl ;
	//char			*pBaseUrl ;
	//char			*pRefree ;
	//char			*pReferrer ;
	int				iCharset ;	/* Defines the charset of the input buffer */

	void *pInterfacePtr ; /* interface pointer for use in alert/confirm/prompt */

} WMLS_IP_INPUTPARAMS ;
/* Global handle containing the Interpreter machine */
/*extern NCDoc *gpWmlsIntCtx;*/


#define WMLS_ERR_IP_ERROR					-20
#define WMLS_ERR_IP_EXIT_ENCOUNTERED		-21
#define WMLS_ERR_IP_ABORT_ENCOUNTERED		-22
#define WMLS_ERR_IP_ALERT_ENCOUNTERED		-23
#define WMLS_ERR_IP_CONFIRM_ENCOUNTERED		-24
#define WMLS_ERR_IP_PROMPT_ENCOUNTERED		-25
#define WMLS_ERR_IP_LOAD_URL_ENCOUNTERED	-26
#define WMLS_ERR_IP_LOAD_STRING_ENCOUNTERED	-27
#define WMLS_ERR_IP_DEBUG_ENCOUNTERED		-28
#define WMLS_ERR_IP_END_OF_CODE				-29

/* Exposed WMLSInterpreter functions */
/* WMLSInterpret Init */
WMLSRETCODE 
wmlsInterpreterInit( void * ctx) ;

/* WMLS Interpret */
//#define wmlsInterpret nc_parseScript

NCSState 
wmlsInterpret( NCDoc* doc, char* fragment, NCSErr* errCode ) ;

/* WMLSInterpret DeInit */
void 
wmlsInterpreterDeInit( void* ctx ) ;

/* Resets the interpreter params in the global ctx */
WMLSRETCODE 
wmlsInterpreterResetParams(void *pWmlseCtx,  WMLS_IP_INPUTPARAMS  *pInputParams) ;

/* Frees the interpreter params in the global ctx */
void
wmlsInterpreterFreeParams( void) ;

/* Maps the IP error code with the interpreter state */
WMLSRETCODE
wmlsIPStateErrMap( NCSState iState) ;

/* Error codes used internal to WMLS Interpreter */
#define	NC_OK			1	
#define	NC_MEM_ERR 		2	
#define	NC_LIST_ERR		4	
#define	NC_WARNING		8	
#define NC_NO_ACCESS	10     // not accessible error only used in this file
#define NC_STYLE_REQUEST 12
#define NC_CHANGED		14	// This value is for some change for ESMP
#define NC_UNSUPPORTED_CONTENTS	13
#define NC_TOO_LARGE_CONTENTS	11

#define NCCPRINTT(s)
#define NCCPRINT(s)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _WMLS_INTERPRET_H_ */
