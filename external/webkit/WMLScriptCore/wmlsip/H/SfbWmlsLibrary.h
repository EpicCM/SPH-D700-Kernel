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

#ifndef _LIBRARY_H_
#define _LIBRARY_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <SfbWmlsTypes.h>

PUBLIC
NCSState 
ncs_callLibrary(NCSWMLSC* wmlsc , const int lIndex, int fIndex);

/* 
 * LANG LIBRARY FUNCTIONS
 */
PUBLIC
NCSState
ncs_libLangAbs( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangMin( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangMax( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangParseInt( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangParseFloat( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangIsInt( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangIsFloat( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangMaxInt( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangMinInt( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangFloat( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangExit( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangAbort( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangRandom( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangSeed( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libLangCharacterSet( NCSWMLSC* wmlsc, NCSVMachine* vM );

/* 
 * FLOAT LIBRARY FUNCTIONS
 */
PUBLIC
NCSState
ncs_libFloatInt( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libFloatFloor( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libFloatCeil( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libFloatPow( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libFloatRound( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libFloatSqrt( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libFloatMaxFloat( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libFloatMinFloat( NCSWMLSC* wmlsc, NCSVMachine* vM );

/*
 * STRING LIBRARY FUNCTIONS
 */
PUBLIC
NCSState
ncs_libStringLength( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringIsEmpty( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringCharAt( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringSubString( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringFind( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringReplace( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringElements( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringElementAt( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringRemoveAt( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringReplaceAt( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringInsertAt( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringSqueeze( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringTrim( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringCompare( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringToString( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libStringFormat( NCSWMLSC* wmlsc, NCSVMachine* vM );

/*
 * URL LIBRARY FUNCTIONS
 */
PUBLIC
NCSState
ncs_libURLIsValid( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libURLGetScheme( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libURLGetHost( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libURLGetPort( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libURLGetPath( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libURLGetParameters( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libURLGetQuery( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libURLGetFragment( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libURLGetBase( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libURLGetReferer( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libURLResolve( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libURLEscapeString( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libURLUnEscapeString( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libURLLoadString( NCSWMLSC* wmlsc, NCSVMachine* vM );

/*
 * WMLBROWSER LIBRARY FUNCTIONS
 */
PUBLIC
NCSState
ncs_libWMLBrowserGetVar( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libWMLBrowserSetVar( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libWMLBrowserGo( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libWMLBrowserPrev( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libWMLBrowserNewContext( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libWMLBrowserGetCurrentCard( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libWMLBrowserRefresh( NCSWMLSC* wmlsc, NCSVMachine* vM );

/*
 * DIALOG LIBRARY FUNCTIONS
 */
PUBLIC
NCSState
ncs_libDialogPrompt( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libDialogConfirm( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libDialogAlert( NCSWMLSC* wmlsc, NCSVMachine* vM );

#if 1 //defined(_SHP_CRYPTO_LIBRARY)
PUBLIC
NCSState
ncs_libCryptoSignText( NCSWMLSC* wmlsc, NCSVMachine* vM );
#endif // _SHP_CRYPTO_LIBRARY

#if defined(_SMB2_WAP_WTA)
PUBLIC
NCSState
ncs_libWTAPublicMakeCall( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libWTAPublicSendDTMF( NCSWMLSC* wmlsc, NCSVMachine* vM );
PUBLIC
NCSState
ncs_libWTAPublicAddPBEntry( NCSWMLSC* wmlsc, NCSVMachine* vM );
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif	/* _LIBRARY_H_ */
