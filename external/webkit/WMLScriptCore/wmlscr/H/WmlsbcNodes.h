/** 
* Samsung Handset Platform
* Copyright (c) 2007 Samsung Electronics, Inc.
* All rights reserved. 
**/
/*
*  <descr>
*  @author 
*/

#ifndef _WMLSBCNODES_H
#define _WMLSBCNODES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes */
#include <WmlsbcOpcodes.h>
#include <WmlsbcInterface.h>
/* Structures */

/* Defines the constants for 0,1 and -1 */
#define INT_CONST_0		0x0100
#define INT_CONST_1		0x0101
#define INT_CONST_M1	0x0102

typedef enum ebcnodetype
{
	BCNODETYPE_NONE = 0,	/* value is NULL */
	BCNODETYPE_EXPR,		/* value refers to a bytecode buffer */
	BCNODETYPE_STMT,		/* value refers to a bytecode buffer */
	BCNODETYPE_PREFIX,		/* value refers to BC_INST for prefix operation */
	BCNODETYPE_POSTFIX,		/* value refers to BC_INST for postfix operation */
	BCNODETYPE_NODELIST,	/* value refers to BC_INST for postfix operation */
	/* Identifier */
	BCNODETYPE_IDENTIFIER,	/* value is a pointer to a v-index (uint) in the variable list */
	/* Literals */
	BCNODETYPE_INTEGER,		/* value is a pointer to a c-index (uint) in the constant list */
	BCNODETYPE_FLOAT,		/* value is a pointer to a c-index (uint) in the constant list */
	BCNODETYPE_STRING,		/* value is a pointer to a c-index (uint) in the constant list */
	BCNODETYPE_BOOLEAN,		/* pointer to a boolean value */
	BCNODETYPE_INVALID,		/* value is NULL */
	/* Unary */
	EBCNODETYPE_RETURN,
	EBCNODETYPE_TYPEOF,
	EBCNODETYPE_ISVALID,
	EBCNODETYPE_PREINCR,
	EBCNODETYPE_PREDECR,
	EBCNODETYPE_POSTINCR,
	EBCNODETYPE_POSTDECR,
	EBCNODETYPE_BITWISENOT,
	EBCNODETYPE_LOGICALNOT,
	/* Binary */
	EBCNODETYPE_FOR_BODY,
	EBCNODETYPE_COMMA,
	EBCNODETYPE_EQ,
	EBCNODETYPE_MULEQ,
	EBCNODETYPE_DIVEQ,
	EBCNODETYPE_MODEQ,
	EBCNODETYPE_PLUSEQ,
	EBCNODETYPE_MINUSEQ,
	EBCNODETYPE_LSHIFTEQ,
	EBCNODETYPE_RSHIFTEQ,
	EBCNODETYPE_RZSHIFTEQ,
	EBCNODETYPE_IDIVEQ,
	EBCNODETYPE_BITWISEANDEQ,
	EBCNODETYPE_BITWISEXOREQ,
	EBCNODETYPE_BITWISEOREQ,
	EBCNODETYPE_LOGICALOR,
	EBCNODETYPE_LOGICALAND,
	EBCNODETYPE_BITWISEOR,
	EBCNODETYPE_BITWISEXOR,
	EBCNODETYPE_BITWISEAND,
	EBCNODETYPE_RELEQ,
	EBCNODETYPE_RELNOTEQ,
	EBCNODETYPE_LESSTHAN,
	EBCNODETYPE_MORETHAN,
	EBCNODETYPE_LESSTHANEQ,
	EBCNODETYPE_MORETHANEQ,
	EBCNODETYPE_LSHIFT,
	EBCNODETYPE_RSHIFT,
	EBCNODETYPE_RZSHIFT,
	EBCNODETYPE_PLUS,
	EBCNODETYPE_MINUS,
	EBCNODETYPE_MUL,
	EBCNODETYPE_DIV,
	EBCNODETYPE_IDIV,
	EBCNODETYPE_MOD,
	EBCNODETYPE_EXT_COMP_UNIT_PRAGMA,
	EBCNODETYPE_LOCAL_SCRIPT_FUNC_CALL,
	EBCNODETYPE_WHILE,
	/* Ternary */
	EBCNODETYPE_FOR,
	EBCNODETYPE_TERN_COND, /* ? : operator */
	EBCNODETYPE_EXT_SCRIPT_FUNC_CALL,
	EBCNODETYPE_LIBRARY_FUNC_CALL,
	EBCNODETYPE_IF_ELSE
} BCNODETYPE ;

/* Generic bytecode node structure */
typedef struct stbcNode
{
	BCNODETYPE ebcNodeType ; /* types none and invalid contain no value */
	void *pvBCNodeValue ;
} BC_NODE ;

/* Constant values for boolean and invalid */
typedef enum econst
{
	ECONST_TRUE =0,
	ECONST_FALSE,
	ECONST_INVALID
} ECONST ;

typedef struct argument
{
	BC_NODE	*pstNode ;
	BC_INST *pInst ;
	struct argument *pstNext ;
} ARGUMENT ;

typedef struct argumentList
{
	ARGUMENT	*pHeadNode ;
	ARGUMENT	*pTailNode ;
} ARGUMENT_LIST ;

typedef struct bcNodeList
{
	BC_NODE *pBCNode ;
	struct bcNodeList *pstNext ;
} BCNODELIST ;

#define YAL_IS_STMT(pBCNode)	(BCNODETYPE_STMT == ((BC_NODE*)pBCNode)->ebcNodeType)
#define YAL_IS_ID(pBCNode)		(BCNODETYPE_IDENTIFIER == ((BC_NODE*)pBCNode)->ebcNodeType)
#define YAL_IS_EXPR(pBCNode)	(BCNODETYPE_EXPR == ((BC_NODE*)pBCNode)->ebcNodeType)

#define YAL_IS_BOOL(pBCNode)	(BCNODETYPE_BOOLEAN == ((BC_NODE*)pBCNode)->ebcNodeType)
#define YAL_IS_INVALID(pBCNode)	(BCNODETYPE_INVALID == ((BC_NODE*)pBCNode)->ebcNodeType)
#define YAL_IS_INT(pBCNode)		(BCNODETYPE_INTEGER == ((BC_NODE*)pBCNode)->ebcNodeType)
#define YAL_IS_STRING(pBCNode)	(BCNODETYPE_STRING == ((BC_NODE*)pBCNode)->ebcNodeType)
#define YAL_IS_FLOAT(pBCNode)	(BCNODETYPE_FLOAT == ((BC_NODE*)pBCNode)->ebcNodeType)
#define YAL_IS_PREFIX(pBCNode)	(BCNODETYPE_PREFIX == ((BC_NODE*)pBCNode)->ebcNodeType)
#define YAL_IS_POSTFIX(pBCNode)	(BCNODETYPE_POSTFIX == ((BC_NODE*)pBCNode)->ebcNodeType)
#define YAL_IS_NODELIST(pBCNode)	(BCNODETYPE_NODELIST == ((BC_NODE*)pBCNode)->ebcNodeType)

#define WMLS_ALLOC_BCNODE(vBCNodehnd)	((BC_NODE *)WMLS_MEM_ALLOC(NULL, 1, sizeof(BC_NODE)))
#define WMLS_ALLOC_INT_NODE(vBCHnd)		((int *)WMLS_MEM_ALLOC(NULL, 1, sizeof(int)))
#define WMLS_FREE_INTNODE(pintHnd, Val)		if (Val) { WMLS_MEM_FREE(NULL, Val) ; }

#define WMLS_FREE_BCNODE(vBCHnd, ptr)	if (ptr) { wmlsDestroyBCNode(vBCHnd, ptr) ;}

#define WMLS_FREE_ID(pIdHnd, name)			if (name) { WMLS_MEM_FREE(NULL, name) ; }
#define WMLS_FREE_STRING(pStrHnd, name)		if (name) { WMLS_MEM_FREE(NULL, name) ; }

#define WMLS_FREE_ARGUMENT(vBCHnd, Arg)			if (Arg) { wmlsFreeArg(vBCHnd, Arg) ; }


WMLSRETCODE
wmlsCreateUnaryNode(void *vBCHnd, 
					BCNODETYPE eNodeType, 
					BC_NODE *pChild, 
					BC_NODE **pRes) ;

WMLSRETCODE
wmlsCreateBinaryNode(void *vBCHnd, 
					BCNODETYPE eNodeType, 
					BC_NODE *pLChild, 
					BC_NODE *pRChild, 
					BC_NODE **pRes) ;

WMLSRETCODE
wmlsCreateTernaryNode(void *vBCHnd, 
					  BCNODETYPE eNodeType, 
					  BC_NODE *pChild1, 
					  BC_NODE *pChild2, 
					  BC_NODE *pChild3, 
					  BC_NODE **pRes) ;

WMLSRETCODE
wmlsCreateIdentifierNode(void *vBCHnd, 
						 MCHAR *pmIdentifierVal,
						 BC_NODE **pRes) ;

WMLSRETCODE
wmlsCreateConstNode	(void *vBCHnd,
					 ECONST eConstType,
					 BC_NODE **pRes) ;

WMLSRETCODE
wmlsCreateIntegerNode(void *vBCHnd, 
					  unsigned short iIntIndex,
					  BC_NODE **pRes) ;

WMLSRETCODE
wmlsCreateFloatNode(void *vBCHnd, 
					  unsigned short iFloatIndex,
					  BC_NODE **pRes) ;

WMLSRETCODE
wmlsCreateStringNode(void *vBCHnd, 
					  MCHAR *StringVal,
					  BC_NODE **pRes) ;


WMLSRETCODE
wmlsFreeIdentifierNode(		void *vBCHnd,
							MCHAR *pmIdName) ;

void
wmlsInitFuncEnv (void *vBCHnd) ;

void
wmlsDestroyString (void *vBCHnd, MCHAR *pmStr) ;

void
wmlsDestroyInteger (void *vBCHnd, int *iIntVal) ;

void
wmlsDestroyFloat (void *vBCHnd, float *fVal) ;

void
wmlsDestroyId (void *vBCHnd, MCHAR *pmIdName) ;

void
wmlsDestroyBCNode (void *vBCHnd, BC_NODE *pBCNode) ;

WMLSRETCODE
wmlsCreateArgList(void *vBCHnd,
				  BC_NODE *Arg1,
				  BC_NODE *Arg2,
				  ARGUMENT_LIST **pArgList) ;

WMLSRETCODE
wmlsAddtoArgList(void *vBCHnd,
				  ARGUMENT_LIST *pArgList,
				  BC_NODE *Arg2,
				  ARGUMENT_LIST **ppArgList) ;

#ifdef __cplusplus
} //extern "C"
#endif /* __cplusplus */

#endif // _wmlsBCNODES_H
