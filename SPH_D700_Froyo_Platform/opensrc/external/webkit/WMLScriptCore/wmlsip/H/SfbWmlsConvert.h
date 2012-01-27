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

#ifndef _CONVERT_H_
#define _CONVERT_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <SfbWmlsTypes.h>

typedef enum _Conversion 
{
	CONV_NO_CONVERSION = 0,
	CONV_BOOL = 1,
	CONV_INT,
	CONV_FLOAT,
	CONV_STRING,
	CONV_INT_OR_FLOAT_UNARY,
	CONV_INT_OR_FLOAT_BINARY,
	CONV_INT_OR_FLOAT_OR_STRING,
	CONV_NOT_SUPPORTED
} Conversion;

PUBLIC
NCSState
ncs_convertOperands( NCSVMachine* vM, const int oprNum, const int conv );
PUBLIC
void
ncs_convert2Float( Variable* var );
PUBLIC
void
ncs_convert2Numeric( Variable* var );
PUBLIC
void
ncs_convert2String( Variable* var );
PUBLIC
void
ncs_convert2NumericOrString( Variable* var1, Variable* var2 );
PUBLIC
void
ncs_convert2Invalid( Variable* var );
PUBLIC
void
ncs_convert2Bool( Variable* var );
PUBLIC
void
ncs_convert2Int( Variable* var );
PUBLIC
void
ncs_convertString2Integer( Variable* var ); 
PUBLIC
void
ncs_convertString2Float( Variable* var );

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif	/* _CONVERT_H_ */
