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

#ifndef _LOADWMLSC_H_
#define _LOADWMLSC_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//#include "Smb.h"
#include <Wmls.h>
#include <SfbWmlsTypes.h>
#include <SfbWmlsInterpret.h>

PUBLIC
NCSWMLSC*
ncs_loadWMLSC( NCDoc* doc );
PUBLIC
void
ncs_freeWMLSC( NCSWMLSC* bc );
PUBLIC
NCSBool
ncs_isValidFunctionName( const char* fN );
PUBLIC
NCSBool
ncs_isStringConstant( NCSConstantPool* cp, int cIndex );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _LOADWMLSC_H_ */
