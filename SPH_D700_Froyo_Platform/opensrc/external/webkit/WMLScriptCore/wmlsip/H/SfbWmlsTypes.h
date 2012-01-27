/*
 * WAE Browser 1.0
 * Developed by Internet Team, Software Center, Samsung Electronics
 * Copyright (C) 1999 Samsung Electronics Co., Ltd.
 * ALL RIGHTS RESERVED
 */

/**
 * This file defines user-defined types and structures.
 *
 * @module	WMLScript UInt8code Interpreter
 * @author	Young-bae Oh (3416-0610, mong@swc.sec.samsung.co.kr)
 * @version	Created on 1999.5.1 by mong
 */

#ifndef _NCSTYPES_H_
#define _NCSTYPES_H_

#include <SfbWmlsUniCode.h>

#ifdef _NCTARGET
#ifdef _NC_TARGET_2410
#include "wap_sys.h"
#endif /* _NC_TARGET_2410 */
#define	NOKIA_COMPILER
#endif


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/// 2000.3.6
#define	NOKIA_COMPILER
//#define NOKIA_TOOLKIT_BUG
#define QFMALLOC

/* Compile Option */
#define INT_IS_32BIT	/* Machine dependency */
/* #define INTEGER_ONLY_DEVICE device doesn't supports floating-point operations */

//#include "wapTypes.h"

#define NCS_SUPPORTED_CHARACTER_SET		0x6a	/* UTF-8 */
#define PRIVATE		static
#define PUBLIC

#define NCS_MAJOR_VERSION		1
#define NCS_MINOR_VERSION		1
#define	NCS_MAX_INT		(int)0x7fffffff		/* 2147483647 */
#define	NCS_MIN_INT		(int)0x80000000		/* -2147483648 */
#define	NCS_MAX_FLOAT	(float)3.40282347E+38
#define	NCS_MIN_FLOAT	(float)1.17549435E-38
#define NCS_MAX_BUF_SIZE			50		 /* check by Mong */
#define NCS_DEFAULT_FLOAT_LENGTH	6

/* default typedef */
typedef enum _NCSBool
{
	NCS_FALSE = 0,
	NCS_TRUE = 1
} NCSBool;

typedef enum _NCSState 
{
	NCS_ERR = 0,
	NCS_OK,
	NCS_END_OF_CODE,
	NCS_EXIT_ENCOUNTERED,
	NCS_ABORT_ENCOUNTERED,
	NCS_ALERT_ENCOUNTERED,
	NCS_CONFIRM_ENCOUNTERED,
	NCS_PROMPT_ENCOUNTERED,
	NCS_LOAD_URL_ENCOUNTERED,
	NCS_LOAD_STRING_ENCOUNTERED,
	NCS_DEBUG_ENCOUNTERED
} NCSState;

typedef enum _NCSType 
{
	NCS_TYPE_INTEGER = 0,
	NCS_TYPE_FLOAT,
	NCS_TYPE_STRING,
	NCS_TYPE_BOOLEAN,
	NCS_TYPE_INVALID,
	NCS_TYPE_UNKNOWN
} NCSType;

typedef struct _Variable 
{
	NCSType type;		/* u_int8 */
	union 
	{
		NCSBool vBool;
		INT32 vInt;
		float vFloat;
		UniChar* vString;
	} v;		
} Variable;

typedef Variable Constant;
typedef Variable Operand;

typedef struct _NCSHeaderInfo 
{
	UINT8 versionMajorNumber;	/* Major version */
	UINT8 versionMinorNumber;	/* Minor version */
	int codeSize;		/* mb_u_int32 */
} NCSHeaderInfo;
	
typedef struct _NCSConstantPool 
{
	UINT16 numberOfConstants;	/* mb_u_int16 */
	Constant* constantList;	/* Constant list */
} NCSConstantPool;

typedef struct //_Pragma 
{
	UINT8 pragmaType;			/* u_int8 */
	int pragmaValue0;	
	int pragmaValue1;	
	int pragmaValue2;	
} Pragma;

typedef struct _NCSPragmaPool 
{
	UINT16 numberOfPragmas;	/* mb_u_int16 */
	Pragma* pragmaList;		/* Pragma list */
	int checkDomain;
	int checkPath;
} NCSPragmaPool;
	
typedef struct _FunctionName 
{
	UINT8 functionIndex;		/* u_int8 */
	UINT8 functionNameSize;	/* u_int8 */
	char* functionName;				/* UTF-8 */
} FunctionName;

typedef struct _Function 
{
	UINT8 numberOfArguments;	/* u_int8 */
	UINT8 numberOfLocalVariables;	/* u_int8 */
	UINT32 functionSize;	/* mb_u_int32 */
	unsigned char* codeArray;
} WMLSFunction; //Renamed Function to WMLSFunction

typedef struct _NCSFunctionPool 
{
	UINT8 numberOfFunctions;	/* u_int8	*/
	UINT8 numberOfFunctionNames;	/* u_int8 */
	FunctionName* functionNameList; /* Function name list */
	WMLSFunction* functionList;		/* Function list */ //Renamed Function to WMLSFunction
} NCSFunctionPool;

typedef struct _CSElement 
{
	UINT8 fIndex;
	UINT32 IP;
	int baseVarStack;
	int oprStackNum;
} CSElement;

typedef struct _NCSVMachine 
{
	UINT32 IP;
	unsigned char* code;
	UINT8 fIndex;
	WMLSFunction* runFunction; //Renamed Function to WMLSFunction
	
	Variable* varStack;
	int varStackSize;
	int varStackNum;

	Operand* oprStack;
	int oprStackSize;
	int oprStackNum;

	CSElement* callStack;
	int callStackSize;
	int callStackNum;

	int baseVarStack;
} NCSVMachine;

typedef struct _NCSWMLSC 
{
	NCSHeaderInfo* headerInfo;
	NCSConstantPool* constantPool;
	NCSPragmaPool* pragmaPool;
	NCSFunctionPool* functionPool;
	NCSVMachine* vMachine;
	int characterSet;
	char* url;
	int referer;
} NCSWMLSC;

/* NCSErr code enumeration */
typedef enum _NCSErr 
{
/* fatal errors */
	/* bytecode errors */
		/* bytecode verification err */
	NCS_ERR_VERIFICATION_FAILURE = 1,
	NCS_ERR_VERIFICATION_NULL_DOC,
	NCS_ERR_VERIFICATION_MAJOR_VERSION,
	NCS_ERR_VERIFICATION_MINOR_VERSION,
	NCS_ERR_VERIFICATION_CODE_SIZE,				/* 5 */
	NCS_ERR_VERIFICATION_INVALID_CONSTANT_TYPE,
	NCS_ERR_VERIFICATION_NEGATIVE_SIZE,
	NCS_ERR_VERIFICATION_DUPLICATE_DOMAIN_PRAGMA,
	NCS_ERR_VERIFICATION_DUPLICATE_PATH_PRAGMA,
	NCS_ERR_VERIFICATION_INVALID_PRAGMA_TYPE,	/* 10 */
	NCS_ERR_VERIFICATION_PRAGMA_VALUE,
	NCS_ERR_VERIFICATION_FUNCTION_INDEX,
	NCS_ERR_VERIFICATION_FUNCTION_NAME,
	NCS_ERR_VERIFICATION_FUNCTION_ARGUMENT_NUMBER,
	NCS_ERR_VERIFICATION_FUNCTION_SIZE,			/* 15 */
	NCS_ERR_VERIFICATION_STRING_FORMAT,
	NCS_ERR_VERIFICATION_INVALID_UTF_8,
	NCS_ERR_VERIFICATION_VARIABLE_INDEX,
	NCS_ERR_VERIFICATION_CONSTANT_INDEX,
	NCS_ERR_VERIFICATION_LIBRARY_INDEX,		/* 20 */
	NCS_ERR_VERIFICATION_INVALID_IP,
	
	NCS_ERR_FATAL_LIBRARY_FUNCTION,
	NCS_ERR_INVALID_FUNCTION_ARGUMENTS,
	NCS_ERR_EXTERNAL_FUNCTION_NOT_FOUND,
	NCS_ERR_UNABLE_TO_LOAD_COMPILATION_UNIT,	/* 25 */
	NCS_ERR_ACCESS_VIOLATION,
	NCS_ERR_STACK_UNDERFLOW,

	/* program specified abortion */
	NCS_ERR_PROGRAMMED_ABORT,

	/* memory exhaustion errors */
	NCS_ERR_STACK_OVERFLOW,
	NCS_ERR_OUT_OF_MEMORY,						/* 30 */

	/* external exceptions */
	NCS_ERR_USER_INITIATED,
	NCS_ERR_SYSTEM_INITIATED,

/* non-fatal errors */
	/* computational errors */
	NCS_ERR_DIVIDE_BY_ZERO,
	NCS_ERR_INTEGER_OVERFLOW,
	NCS_ERR_FLOATING_POINT_OVERFLOW,			/* 35 */
	NCS_ERR_FLOATING_POINT_UNDERFLOW,

	/* constant reference errors */
	NCS_ERR_NOT_NUMBER_CONSTANT,
	NCS_ERR_INFINITE_FLOATING_POINT_CONSTANT,
	NCS_ERR_ILLEGAL_FLOATING_POINT_REFERENCE,

	/* conversion errors */
	NCS_ERR_INTEGER_TOO_LARGE,					/* 40 */
	NCS_ERR_FLOATING_POINT_TOO_LARGE,
	NCS_ERR_FLOATING_POINT_TOO_SMALL,

	/* error for debugging */
	NCS_ERR_MONG_CHECK = 1024
} NCSErr;

/* global variable declaration */

extern char* nc_script_url;
extern char* nc_contentType;
extern NCSErr ncs_errCode;


/* imported function declaration */
int nc_isValidVarName( char* name );
char* nc_getVarFromBrowser( char* name);
int nc_setVarOfBrowser(char* a , char* b);
void nc_newContext(void);
int nc_prompt(char* a , char* b);
int nc_confirm(char* a,char* b,char* c);
int nc_alert(char* a);
char* nc_getCurrentCard(void);
int nc_loadURL( char* url );
char* nc_getCurrentURL(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _NCSTYPES_H_ */
