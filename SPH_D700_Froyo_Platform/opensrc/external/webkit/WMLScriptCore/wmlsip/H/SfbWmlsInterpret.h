/*
 * WAE Browser 1.0
 * Developed by Internet Team, Software Center, Samsung Electronics
 * Copyright (C) 1999 Samsung Electronics Co., Ltd.
 * ALL RIGHTS RESERVED
 */

/**
 *
 * @module	WMLScript Bytecode Interpreter
 * @author	Young-bae Oh (3416-0610, mong@swc.sec.samsung.co.kr)
 * @version	Created on 1999.5.1 by mong
 */

#ifndef _SFBWMLSINTERPRET_H_
#define _SFBWMLSINTERPRET_H_

//#include "Smb.h"
#include "Wmls.h"
#include "SfbWmlsTypes.h"
#include "SfbWmlsInterpreterUtils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum _OP
{
#define OPDEF(name,str,val,len,num,con,func) name = val,
#include <SfbWmlsOpCode.def>
#undef OPDEF
} OP;
typedef	struct _NCDoc	NCDoc;
struct _NCDoc {
	char			*url;			// document's URL 
	char			*base_url;		// base URL 

	char			*pRefree;		// base URL 
	char			*pReferrer;		// base URL 

	char			*raw_data;		// HTML document	
	
	int				c_length;
	int				bytes_read;		// current document length in bytes 
	int				charset;		// document's characterset

	void			*pInterfacePtr ; // interface class pointer
};
#define NCS_MODULE	4
PUBLIC
NCSState 
nc_parseScript(NCDoc* doc, char* fragment, NCSErr* errCode );
PUBLIC
OP 
ncs_getOpNum( unsigned int ch);
PUBLIC
NCSState
ncs_requestURL( char* urlBuf, char* fNameBuf, int args, Operand* oprList );
PUBLIC
NCSState
RunScript( NCDoc* doc, char* fragment, NCSState cState, NCSErr* errCode );

/* String Utilities */
extern int 
nc_isspace(char) ;
extern int 
nc_strcasecmp(char *, char *) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif	/* _SFBWMLSINTERPRET_H_ */
