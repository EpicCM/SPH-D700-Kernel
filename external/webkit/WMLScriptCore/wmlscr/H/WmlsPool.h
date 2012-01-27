/** 
* Samsung Handset Platform
* Copyright (c) 2007 Samsung Electronics, Inc.
* All rights reserved. 
*/
/*
*  <descr>
*  @author 
*/

#ifndef _WMLSPOOL_H
#define _WMLSPOOL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <WmlsbcInterface.h>

/* Version Number */
#define WMLS_VERSION_NO_1_1		0x01
#define WMLS_CHARSET_UTF8		0x6A		// Charset : UTF-8

char *
wmlsMCharToChar(MCHAR *pzMcharStrn) ; //returns the char eq of the MCHAR string

/* Constant types (7-255 RESERVED) */
#define CONST_TYPE_INT8		0x00
#define CONST_TYPE_INT16	0x01
#define CONST_TYPE_INT32	0x02
#define CONST_TYPE_FLOAT	0x03
#define CONST_TYPE_UTF8STR	0x04
#define CONST_TYPE_EMPTYSTR	0x05
#define CONST_TYPE_ECESTR	0x06
#define CONST_TYPE_PREDEFINED_VAL	0x07

/* Structure of the constant pool list */
typedef struct constantPoolList
{
	unsigned char	ucConstantIndex ; /* Represents the constant index for the corresponding name */
	unsigned char	ucConstType ; /* Once of the above constant types */
	union _uvCP_
	{
		unsigned int uiValue ; /* General value */
		char	cInteger8 ; /* 8-bit signed integer */
		short	kInteger16 ; /* 16-bit signed integer */
		int		iInteger32 ; /* 32-bit signed integer */
		float	fFloatValue ; /* 32-bit signed float value */
		MCHAR	*pmStr ; /* UTF-8 string. Empty string rep by NULL */
	}uv ;
	struct constantPoolList	*pstNextConstant ;
} CONSTANT_POOL_LIST ;

/* Handle to the constant pool */
typedef struct _constPoolHnd
{
	int iNumConst ;
	CONSTANT_POOL_LIST *pHead ;
} CONSTPOOLHND ;

/* Pragma types (4-255 RESERVED) */
#define PRAGMA_TYPE_ACCESS_DOMAIN			0x00
#define PRAGMA_TYPE_ACCESS_PATH				0x01
#define PRAGMA_TYPE_USER_AGENT_PROP			0x02
#define PRAGMA_TYPE_USER_AGENT_PROP_SCHEME	0x03

typedef struct pragmaUserAgentProp
{
	unsigned short ukPropertyNameIndex ;
	unsigned short ukContentIndex ;
} USER_AGENT_PROP ;

typedef struct pragmaUserAgentPropScheme
{
	unsigned short ukPropertyNameIndex ;
	unsigned short ukContentIndex ;
	unsigned short ukSchemeIndex ;
} USER_AGENT_PROP_SCHEME ;

/* Structure of the pragma pool list */
typedef struct pragmaPoolList
{
	unsigned char ucPragmaType ; /* One of the above constant types */
	union _uvP_
	{
		unsigned short	ukAccessDomainPathIndex ; /* Access Domain / Access Path */
		USER_AGENT_PROP *pstUserAgentProp ; /* User Agent property */
		USER_AGENT_PROP_SCHEME *pstUserAgentPropScheme ; /* User Agent property and scheme */
	}uv ;
	struct pragmaPoolList *pstNextPragma ;
} PRAGMA_POOL_LIST ;

/* Handle to the pragma pool */
typedef struct _pragmaPoolHnd
{
	int iNumPrag ;
	PRAGMA_POOL_LIST *pHead ;
} PRAGMAPOOLHND ;

/* Structure of the variables pool list */
typedef struct varPool
{
	unsigned char	ucSymbolIndex ; /* Represents the symbol index for the corresponding name */
	MCHAR			*pmSymbolName ; /* To do: determine the datattype. Represents the name of the symbol in */
	struct varPool	*pstNextSymbol ;
} VAR_LIST ;

/* Structure of the variables pool list */
typedef struct varListHnd
{
	VAR_LIST *pstHead ;
	VAR_LIST *pstTail ;
} VAR_LIST_HEAD ;


/* Structure of the function name table which contains ext functions */
typedef struct functionNameTable
{
	unsigned char	cFuncIndex ;
	MCHAR			*pmFuncName ; /* UTF8 function name */
	struct functionNameTable *pstNextFuncName ;
} FUNCTION_NAME_TABLE ;

/* Structure of the function */
typedef struct functionList
{
	MCHAR			*pmFuncName ; // NULL = Extern Function
	unsigned char	ucNumArgs ;
	unsigned char	ucNumLocalVars ;
	BC_INST			*pstCodeArray ;
	struct functionList *pstNextFunc ;
} FUNCTION_LIST ;

typedef struct functPoolHnd
{
	void *pvFuncNameTableListHnd ; /* Function name table list handle */
	void *pvFuncListHnd ; /* Function list handle */
} FUNCPOOLHND ;

typedef struct BCGen_CP
{
	unsigned char	*pConstPool ; 
	int			iCodeSize ;
} BCGEN_CPList ;

typedef struct BCGen_PP
{
	unsigned char		*pPragPool ; 
	int			iCodeSize ;
} BCGEN_PPList ;

typedef struct BCGen_FP
{
	unsigned char		*pFuncPool ;
	int			iCodeSize ;
} BCGEN_FPList ;

typedef struct BCGen_Header
{
	BCGEN_CPList	*pConstPoolList;
	BCGEN_PPList	*pPragPoolList;
	BCGEN_FPList	*pFuncPoolList;
} BCGEN_Header ;

// Constant Pool
WMLSRETCODE 
wmlsInitConstantPool(void **pvConstPoolHnd) ;

WMLSRETCODE 
wmlsLookupConstPool(void *pvConstPoolHnd, 
					unsigned char ucType, 
					void *pvValue, 
					int *iConstIndex) ;

/* returns the constant pool index of the int const if already present .. inCpList*/
unsigned char 
wmlsInsertIntToConstPool ( CONSTPOOLHND * , double , int );

/* returns constant pool index of string const if already present -- inCpListString*/
unsigned char 
wmlsConstPoolValIsString( CONSTPOOLHND * , MCHAR * ,unsigned char );

/* adds to constant pool and returns constant pool index -- addToCpList */
unsigned char 
wmlsAddtoConstPool( CONSTPOOLHND * , double , int );

/* adds string to constant pool and returns CP index -- addToCpListString */
unsigned char 
wmlsAddStrConstPool( CONSTPOOLHND * , MCHAR * , unsigned char );

void 
wmlsDeInitConstantPool(void *pvConstPoolHnd) ;

//Variable Pool

WMLSRETCODE 
wmlsInitVarPool(void **pvVarPoolHnd) ;

WMLSRETCODE 
wmlsInsertVarPool(void *pvVarPoolHnd, 
				  MCHAR *pmInVarName,
				  int *iVarIndex) ;

WMLSRETCODE 
wmlsLookupVarPool(void *pvVarPoolHnd, 
				  MCHAR *pmVarName,
				  int *iVarIndex) ;

void
wmlsResetVarPool(void *pvVarPoolHnd) ;

void 
wmlsDeInitVarPool(void *pvVarPoolHnd);


//Function Pool

WMLSRETCODE
wmlsInitFunctionPool(void **pvFunctionPoolHnd) ;

//Lookup is used in both the insert functions 
WMLSRETCODE 
wmlsLookupFuncName(void *pvFunctionPoolHnd, 
				   MCHAR *cInFuncName,
				   int *iFuncIndex) ;

WMLSRETCODE 
wmlsAddtoFuncTable(void *pvFunctionPoolHnd, 
				   MCHAR *cInFuncName, 
				   int iFLIndex,
				   int *iFuncIndex) ;

WMLSRETCODE 
wmlsCreateFunction(void *pvFunctionPoolHnd, 
				   MCHAR *cInFuncName, 
				   unsigned char ucInArgs, 
				   unsigned char ucLocalVars, 
				   BC_INST *psInCodeArr,
				   int *iFuncIndex) ;

int 
wmlsUpdateFunction(void *pvFunctionPoolHnd, 
				   MCHAR *cInFuncName, 
				   unsigned char ucInArgs,
				   int *iFuncIndex) ;

void 
wmlsDeInitFunctionPool(void *pvFunctionPoolHnd) ;

/* PRAGMA POOL */
/* To Initialize the Pragma Pool */
WMLSRETCODE 
wmlsInitPragmaPool(void **pvPragmaPoolHnd) ;

/* To Insert a pragma in the Pragma Pool */
WMLSRETCODE 
wmlsInsertPragmaPool(void *pvPragmaPoolHnd, 
					unsigned char ucPragmaType, 
					int ukInPragma1, 
					int ukInPragma2, 
					int ukInPragma3) ;
/* To Lookup a function in the Pragma Pool */
WMLSRETCODE 
wmlsLookupPragmaPool(	void *pvPragmaPoolHnd, 
						unsigned char ucPragmaType,
						unsigned char ucConstIndex) ;

/* To De-Initialize the Pragma Pool */
void
wmlsDeInitPragmaPool(void *pvPragmaPoolHnd) ;


// BYTECODE GENERATOR

WMLSRETCODE
wmlsCreateBCFnPool(void *pvFunctionPoolHnd,  BCGEN_FPList *pFPList) ;

WMLSRETCODE 
wmlsCreateBCConstPool(void *pvConstPoolHnd, BCGEN_CPList *pCPList) ;

WMLSRETCODE 
wmlsCreateBCHeader(BCGEN_Header *pBCHeader, 
				   unsigned char **pByteCode, 
				   unsigned int *piByteCodeLen) ;

WMLSRETCODE 
wmlsCreateBCPragPool(void *pvPragmaPoolHnd, BCGEN_PPList *pPPList) ;

/* This converts int16 or int32 to it's mb equivalent */
unsigned char *
wmlsEncodembUInt(unsigned int value, 
				   unsigned int *puiMbLenOut) ;

BOOL 
wmlsIsConstES(void *pvConstPoolHnd, unsigned int iConstindex) ;

void
wmlsFreeNameUrlMap(NAME_URL_MAP *pNameUrlMap) ;

#ifdef __cplusplus
} //extern "C"
#endif /* __cplusplus */

#endif //_WMLSPOOL_H
