/** 
* Samsung Handset Platform
* Copyright (c) 2007 Samsung Electronics, Inc.
* All rights reserved. 
*/
/*
*  <descr>
*  @author 
*/

#ifndef _WMLSUTILS_H
#define _WMLSUTILS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes */
#include <WmlsbcInterface.h>
#include <WmlsbcNodes.h>
#include <WmlsPool.h>
/* Local function declarations */
/* Local Function definitions */
/* Global Function definitions */

int 
wmlsNumofArgs(void *pvArgListHnd) ;

int 
wmlsNumofArgsInFunc (void *pvFunctionPoolHnd, MCHAR *cInFuncName) ;

BC_NODE *
wmlsAddInstPop(void *pYalCtx, BC_NODE *Expr) ;

BC_NODE *
wmlsCreateExprStatement(void *pYalCtx, BC_NODE *Expr) ;

WMLSRETCODE
wmlsCreateBytecodeBuff(void *vBCHnd) ;

BC_INST*
wmlsFrameOperand(void *vBCHnd, BC_NODE *pNode) ;

BC_INST *
wmlsFrameJumpFWInst(void *pvBCHnd, unsigned int iOffset) ;

void
wmlsInitFuncEnv(void *vBCHnd) ;

WMLSRETCODE
wmlsCreateArgs(	void *vBCHnd,
				ARGUMENT_LIST *pArgList,
				ARGUMENT **pArgs) ;

void
wmlsAddArgToArgList(void *vBCHnd,
					ARGUMENT *Arg,
					ARGUMENT_LIST *pArgList) ;

BC_INST *
wmlsFrameLibFuncCall (	void *pvBCHnd,
						unsigned int uiLibIndex,
						unsigned int uiFuncIndex,
						ARGUMENT_LIST *pArgList) ;

BC_INST *
wmlsFrameESFuncCall (	void *pvBCHnd,
						int iURLIndex,
						int iFuncIndex,
						ARGUMENT_LIST *pArgList) ;

BC_INST *
wmlsFrameFuncCall (	void *pvBCHnd,
						unsigned int uiFuncIndex,
						ARGUMENT_LIST *pArgList) ;

unsigned int
wmlsCopyInst(void *pvBCHnd, BC_INST *pDesInst, BC_INST *pSrcInst) ;

BC_INST *
wmlsDuplicateInst(void *pvBCHnd, BC_INST *pSrcInst) ;

BC_INST *
wmlsFrameLoadVarInst(void *pvBCHnd, unsigned int uiVarIndex) ;

BC_INST *
wmlsFrameLoadInvalidInst (void *pvBCHnd) ;

BC_INST *
wmlsFrameLoadBoolInst (void *pvBCHnd, BOOL bValue) ;

BC_INST *
wmlsFrameLoadConstInst(void *pvBCHnd, unsigned int uiVarIndex) ;

BC_INST *
wmlsFrameStoreInst(void *pvBCHnd, unsigned int uiVarIndex) ;

BC_INST *
wmlsFrameIncrVarInst(void *pvBCHnd, unsigned int uiVarIndex) ;

BC_INST *
wmlsFrameDecrVarInst(void *pvBCHnd, unsigned int uiVarIndex) ;

BC_INST *
wmlsFrameOpInst(void *pvBCHnd, unsigned char ucOperator) ;

BC_INST*
wmlsFrameOperand(void *vBCHnd, BC_NODE *pNode) ;

BC_INST *
wmlsFrameJumpFWInst(void *pvBCHnd, unsigned int iOffset) ;

BC_INST *
wmlsFrameJumpBWInst(void *pvBCHnd, unsigned int iOffset) ;

BC_INST *
wmlsFrameTJumpBWInst(void *pvBCHnd, unsigned int iOffset) ;

BC_INST *
wmlsFrameTJumpFWInst(void *pvBCHnd, unsigned int iOffset) ;

BC_INST *
wmlsAppendInst(void *pvBCHnd, BC_INST *pstInst1, BC_INST *pstInst2) ;

WMLSRETCODE
wmlsCreateExtFuncName	(void *vBCHnd,
						MCHAR	*pmExtFuncName) ;


void
wmlsDestroyBCNode (void *vBCHnd, BC_NODE *pBCNode) ;

unsigned int
wmlsInsertConstPool(void *, MCHAR *) ;

BC_INST *
wmlsAddReturntoCodeArray(void *vBCHnd, BC_INST *pCodeArray) ;

BC_INST*
wmlsFramePrefixOperation(void *vBCHnd, BC_INST *pInst) ;

BC_INST*
wmlsFramePostfixOperation(void *vBCHnd, BC_INST *pInst) ;

/*	Given an inst buffer, this api finds out the variable 
	index in the inst buff */
int
wmlsGetVarIndexfromInstBuff(BC_INST *pcInstBuffer) ;

void
wmlsAddtoNameUrlMap(void *vBCHnd, int iNameIndex, int iUrlIndex) ;

WMLSRETCODE
wmlsLookupNameUrlMap(void *vBCHnd, int iNameIndex, int *iUrlIndex) ;

BC_NODE *
wmlsNormaliseExpr(void *vBCHnd, BC_NODE *pExprNode ) ;

BC_NODE *
wmlsMergeNodesinNodeList(void *vBCHnd, BCNODELIST *pNodeList) ;

BC_INST*
wmlsFrameASGInst(void *pvBCHnd, 
				 unsigned char ucOperator, 
				 unsigned int uiVarIndex) ;

BC_INST*
wmlsBreakInst(void *pvBCHnd) ;

BC_INST*
wmlsContInst(void *pvBCHnd) ;

void
wmlsUpdateOffsetWhile(void *vBCHnd, 
				 BC_INST *pInstbody,
				 int ValueBeg,
				 int ValueEnd) ;

void
wmlsUpdateOffsetFor(void *vBCHnd, 
				 BC_INST *pInstbody,
				 int ValueEnd,
				 int ValueJmp) ;

void 
wmlsFreeAllPools( void *vBCHnd ) ;

BC_NODE*
wmlsDuplicateNode(void *vBCHnd, BC_NODE *pNode) ;

void
wmlsFreeArg(void *vBCHnd, ARGUMENT_LIST *pArgList) ;

#ifdef __cplusplus
} //extern "C"
#endif /* __cplusplus */

#endif //_WMLSUTILS_H
