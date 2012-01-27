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
 * @version	Created on 1999.5.20 by mong
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <SfbWmlsTypes.h>
#include <WmlsPlatform.h>

/*
 * DEBUG 
 */
#if defined(NCS_DEBUG)
#	define NCSDEBUG(code) { code; }
#else
#	define NCSDEBUG(code)
#endif /* NCS_DEBUG */

#define NEVER_GET_HERE()	WMLS_ASSERT_WITH_MESSAGE(0, "NEVER_GET_HERE")

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

PUBLIC
void
ncs_printOperandStack( NCSVMachine* vM );
PUBLIC
void
ncs_printHeaderInfo( NCSHeaderInfo* hi );
PUBLIC
void
ncs_printConstantPool( NCSConstantPool* cp );
PUBLIC
void
ncs_printConstant( Constant* con );
PUBLIC
void
ncs_printPragmaPool( NCSPragmaPool* pp );
PUBLIC
void
ncs_printFunctionPool( NCSFunctionPool* f );

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _DEBUG_H */
