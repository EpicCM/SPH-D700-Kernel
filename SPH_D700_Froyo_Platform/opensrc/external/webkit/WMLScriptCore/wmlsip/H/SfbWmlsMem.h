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

#ifndef _MEM_H_
#define _MEM_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <SfbWmlsTypes.h>

/* global function declaration */
#ifndef _NCMEMCHK
PUBLIC
void*
ncs_malloc( int size );
#endif // _NCMEMECHK

PUBLIC
void*
ncs_calloc( int nmem, int size );

#ifndef _NCMEMCHK
PUBLIC
void*
ncs_realloc( void* oldP, int size );
PUBLIC
void
ncs_free( void* vp );
#endif /* _NCMEMCHK */
PUBLIC
void
ncs_incAllocCount( void* vp );
PUBLIC
void
ncs_decAllocCount(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _MEM_H_ */
