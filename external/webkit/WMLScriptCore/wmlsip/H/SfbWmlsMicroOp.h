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
 * @version	Created on 1999.7.6 by mong
 */

#ifndef _MICROOP_H_
#define _MICROOP_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* #includes */
#include <SfbWmlsTypes.h>

/* #defines */
/* Enumeration */
typedef enum _NCSMO
{
	NCS_MO_OK,
	NCS_MO_INTEGER_OVERFLOW,
	NCS_MO_FLOAT_OVERFLOW,
	NCS_MO_FLOAT_UNDERFLOW,
	NCS_MO_DIVIDE_BY_ZERO
} NCSMO;

/* typedefs */
/* Constant definitions */
/* Variable declarations */

/* Function declarations */
PUBLIC
NCSMO
ncs_moAddInteger( int* sum, const int a, const int b );
PUBLIC
NCSMO
ncs_moSubInteger( int* sub, const int a, const int b );
PUBLIC
NCSMO
ncs_moMulInteger( int* mul, const int a, const int b );
PUBLIC
NCSMO
ncs_moDivInteger( int* div, int* mod, const int a, const int b );

PUBLIC
NCSMO
ncs_moAddFloat( float* sum, const float a, const float b );
PUBLIC
NCSMO
ncs_moSubFloat( float* sub, const float a, const float b );
PUBLIC
NCSMO
ncs_moMulFloat( float* mul, const float a, const float b );
PUBLIC
NCSMO
ncs_moDivFloat( float* div, const float a, const float b );

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _MICROOP_H_ */
