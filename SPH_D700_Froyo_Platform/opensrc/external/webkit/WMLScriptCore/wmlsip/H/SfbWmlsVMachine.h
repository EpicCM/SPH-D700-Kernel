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

#ifndef _VMACHINE_H_
#define _VMACHINE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <SfbWmlsTypes.h>

PUBLIC
NCSVMachine*
ncs_newVMachine(NCSWMLSC* wmlsc );
PUBLIC
void
ncs_freeVMachine(NCSVMachine* vM);
PUBLIC
NCSState
ncs_initRunFunction( NCSWMLSC* wmlsc, int fIndex );
PUBLIC
NCSState
ncs_pushCSElement( NCSVMachine* vM, CSElement cse );
PUBLIC
NCSState
ncs_popCSElement( NCSVMachine* vM, CSElement* cse );
PUBLIC
NCSState
ncs_isEmptyCallStack( NCSVMachine* vM );
PUBLIC
NCSState
ncs_pushOperand( NCSVMachine* vM, Operand* opr);
PUBLIC
NCSState
ncs_popOperand( NCSVMachine* vM, Operand* opr );
PUBLIC
NCSState
ncs_setVariable(  NCSVMachine* vM, Variable* var, int vIndex );
PUBLIC
NCSState
ncs_getVariable( NCSVMachine* vM, Variable* var, int vIndex );
PUBLIC
NCSState
ncs_getConstant( NCSConstantPool* cP, Constant* con, int cIndex );
PUBLIC
void 
ncs_freeObject( Operand* opr );
PUBLIC
NCSState
ncs_copyObject( Operand* dest, Operand* src );
PUBLIC
int
ncs_getByteCode( NCSVMachine* vM );
PUBLIC
int
ncs_getNextByteCode( NCSVMachine* vM );
PUBLIC
int
ncs_getNext2ByteCode( NCSVMachine* vM );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _VMACHINE_H_ */
