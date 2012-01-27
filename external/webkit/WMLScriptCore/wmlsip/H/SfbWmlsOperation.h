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

#ifndef _OPERATION_H_
#define _OPERATION_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <SfbWmlsTypes.h>

PUBLIC
NCSState
ncs_executeOperation( NCSWMLSC* wmlsc, int opNum );

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif	/* _OPERATION_H_ */

