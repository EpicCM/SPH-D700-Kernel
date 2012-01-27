/** 
* Samsung Handset Platform
* Copyright (c) 2007 Samsung Electronics, Inc.
* All rights reserved. 
*/
/*
*  <descr>
*  @author 
*/
#ifndef _WMLSLEXER_H
#define _WMLSLEXER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <Wmls.h>
#include <WmlsbcInterface.h>
#include <WmlsbcNodes.h>
#include <WmlsPool.h>

/*
 * Depicts lexer's various states
*/
typedef enum state {Start,
			
			InSingleLineComment,
			InMultiLineComment,

			InIdentifierOrKeyword,
			Identifier,

			InPunctuator,
			Punctuator,

			InDecIntOrDecFloat,
			DecInt,
			DecFloat,

			InExponentIndicator,
			InExponent,

			InNum0,

			InHex,
			Hex,

			InOctal,
			Octal,

			InBool,
			BoolTrue,
			BoolFalse,

			InString,
			String,
			InEscapeSequence,
			InCharacterEscape,
			InOctalEscape,
			InHexEscape,
			InUnicodeEscape,

			InInvalid,
			Invalid,

			Eof,
			Other,
			Bad
}State;


typedef struct lexCtx
{
     BOOL done;						// flag used to check if a valid token is formed

     MCHAR *buffer;					// Input buffer
     unsigned int bufSize;			// Size of input buffer
     unsigned int bufPos;			// Current position of the lex scanner in the buffer

     unsigned char *buffer8;		// temp buffer
     MCHAR *buffer16;				// temp buffer to hold token value
     MCHAR tBuf[256];				// temp buffer to hold token value
     unsigned int tBufPos;

     State state;					// Current State of the lexer

     int curr,next1,next2,next3;
	 int prevToken;

     void *pvConstantPoolHnd ;		/* Handle to the constant pool list */

}WMLS_LEX_HND ;

/* Local declarations */
void shift(unsigned int p);
void setDone(State s);
void writeToBuffer16();
void writeToBuffer8();
void freeMem();

int lookUpKeyword();

BOOL isWhiteSpace();
BOOL isLineTerminator();

BOOL isIdentStart();
BOOL isPunctuatorStart();
BOOL isDecIntOrDecFloatStart();
BOOL isConstOrIdentPrevToken();


BOOL isValidIdentChar();
int ValidPunct();
BOOL isValidDecIntChar();
BOOL isValidHexChar(int);
BOOL isValidOctalChar(int);
BOOL isHexDigit(MCHAR);

char ConvertToDecFloat();						/* Convert buffer8 to float. Add float to constant pool and return CP index */
unsigned short ConvertToDecInt();				/* Convert buffer8 containing integer to int. Add int to constant pool and return CP index */

int BoolOrInvalidLiteral();

void record(int c);

void ConvertUnicodeToUTF8(unsigned char *des,unsigned short *src,int nChar);

/* Global declarations */
/*
 * Init values in the Lexer Context. Internally called by the lexer
*/
void initLexerContext();


/*
 * Initializes the Lexer Context depending on the input recieved from the Parser
 * @param ctx[in] : param required for initialization 
*/
WMLSRETCODE
wmlsLexerInit(void *ctx); // called by the parser

/**
  * DeInitializes the Lexer Context
**/
void wmlsLexerDeInit();

/*
 * Lexing Function
*/
int wmlsyylex();

#ifdef __cplusplus
} //extern "C"
#endif /* __cplusplus */

#endif //_WMLSLEXER_H

