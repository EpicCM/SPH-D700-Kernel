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
 * @version	Created on 1999.5.18 by mong
 */

#ifndef _SFB_WMLS_STRINGS_H_
#define _SFB_WMLS_STRINGS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <SfbWmlsTypes.h>
#include <SfbWmlsUniCode.h>

PUBLIC
NCSBool
ncs_isEmptyString( Variable* var );
PUBLIC
void
ncs_initES( Variable* var );

PUBLIC
UniChar*
ncs_newStrFromUTF_8( const char* src );
PUBLIC
UniChar*
ncs_newStrFromUCS_2( const UniChar* src );
PUBLIC
UniChar*
ncs_newStrFromASCII( const char* src );
PUBLIC
char*
ncs_newASCIIFromUCS_2( const UniChar* src );
PUBLIC
UniChar*
ncs_newStrFromExtern( const char* src, int ctype );
PUBLIC
char*
ncs_newASCIIFromASCII( const char* src );
PUBLIC
NCSBool
IsValidCData( const UniChar* src );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _SFB_WMLS_STRINGS_H_ */
