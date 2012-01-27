/* A Bison parser, made by GNU Bison 1.875b.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     tINVALID = 258,
     tTRUE = 259,
     tFALSE = 260,
     tINTEGER = 261,
     tFLOAT = 262,
     tIDENTIFIER = 263,
     tSTRING = 264,
     tACCESS = 265,
     tAGENT = 266,
     tBREAK = 267,
     tCONTINUE = 268,
     tIDIV = 269,
     tDOMAIN = 270,
     tEQUIV = 271,
     tEXTERN = 272,
     tFOR = 273,
     tFUNCTION = 274,
     tHEADER = 275,
     tHTTP = 276,
     tIF = 277,
     tISVALID = 278,
     tMETA = 279,
     tNAME = 280,
     tPATH = 281,
     tRETURN = 282,
     tTYPEOF = 283,
     tUSE = 284,
     tUSER = 285,
     tVAR = 286,
     tWHILE = 287,
     tURL = 288,
     tLBRACE = 289,
     tRBRACE = 290,
     tSEMI = 291,
     tHASH = 292,
     LOWERTHANELSE = 293,
     tELSE = 294,
     tCOMMA = 295,
     tBITWISEOREQ = 296,
     tBITWISEXOREQ = 297,
     tBITWISEANDEQ = 298,
     tRZSHIFTEQ = 299,
     tRSHIFTEQ = 300,
     tLSHIFTEQ = 301,
     tMINUSEQ = 302,
     tPLUSEQ = 303,
     tMODEQ = 304,
     tDIVEQ = 305,
     tMULEQ = 306,
     tEQ = 307,
     tCOLON = 308,
     tHOOK = 309,
     tLOGICALOR = 310,
     tLOGICALAND = 311,
     tBITWISEOR = 312,
     tBITWISEXOR = 313,
     tBITWISEAND = 314,
     tRELTYPENOTEQ = 315,
     tRELTYPEEQ = 316,
     tRELNOTEQ = 317,
     tRELEQ = 318,
     tMORETHANEQ = 319,
     tMORETHAN = 320,
     tLESSTHANEQ = 321,
     tLESSTHAN = 322,
     tRZSHIFT = 323,
     tRSHIFT = 324,
     tLSHIFT = 325,
     tMINUS = 326,
     tPLUS = 327,
     tIDIVEQ = 328,
     tMOD = 329,
     tDIV = 330,
     tMUL = 331,
     tVOID = 332,
     tNEW = 333,
     tDELETE = 334,
     tLOGICALNOT = 335,
     tBITWISENOT = 336,
     tDECR = 337,
     tINCR = 338,
     tUMINUS = 339,
     tRPAREN = 340,
     tLPAREN = 341,
     tRSQBR = 342,
     tLSQBR = 343,
     tDOT = 344
   };
#endif
#define tINVALID 258
#define tTRUE 259
#define tFALSE 260
#define tINTEGER 261
#define tFLOAT 262
#define tIDENTIFIER 263
#define tSTRING 264
#define tACCESS 265
#define tAGENT 266
#define tBREAK 267
#define tCONTINUE 268
#define tIDIV 269
#define tDOMAIN 270
#define tEQUIV 271
#define tEXTERN 272
#define tFOR 273
#define tFUNCTION 274
#define tHEADER 275
#define tHTTP 276
#define tIF 277
#define tISVALID 278
#define tMETA 279
#define tNAME 280
#define tPATH 281
#define tRETURN 282
#define tTYPEOF 283
#define tUSE 284
#define tUSER 285
#define tVAR 286
#define tWHILE 287
#define tURL 288
#define tLBRACE 289
#define tRBRACE 290
#define tSEMI 291
#define tHASH 292
#define LOWERTHANELSE 293
#define tELSE 294
#define tCOMMA 295
#define tBITWISEOREQ 296
#define tBITWISEXOREQ 297
#define tBITWISEANDEQ 298
#define tRZSHIFTEQ 299
#define tRSHIFTEQ 300
#define tLSHIFTEQ 301
#define tMINUSEQ 302
#define tPLUSEQ 303
#define tMODEQ 304
#define tDIVEQ 305
#define tMULEQ 306
#define tEQ 307
#define tCOLON 308
#define tHOOK 309
#define tLOGICALOR 310
#define tLOGICALAND 311
#define tBITWISEOR 312
#define tBITWISEXOR 313
#define tBITWISEAND 314
#define tRELTYPENOTEQ 315
#define tRELTYPEEQ 316
#define tRELNOTEQ 317
#define tRELEQ 318
#define tMORETHANEQ 319
#define tMORETHAN 320
#define tLESSTHANEQ 321
#define tLESSTHAN 322
#define tRZSHIFT 323
#define tRSHIFT 324
#define tLSHIFT 325
#define tMINUS 326
#define tPLUS 327
#define tIDIVEQ 328
#define tMOD 329
#define tDIV 330
#define tMUL 331
#define tVOID 332
#define tNEW 333
#define tDELETE 334
#define tLOGICALNOT 335
#define tBITWISENOT 336
#define tDECR 337
#define tINCR 338
#define tUMINUS 339
#define tRPAREN 340
#define tLPAREN 341
#define tRSQBR 342
#define tLSQBR 343
#define tDOT 344




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 489 "../wmlscr/grammar/wmlsgrammar.y"
typedef union YYSTYPE {
		BC_NODE			*pstBCNode ;
		unsigned short	uzValue ; /* all literals */
		ECONST			ebcConstVal ; /* Boolean and invalid values */
		BOOL			bBoolVal ;
		FUNCTION_LIST	*pstFuncDecl ;
		VAR_LIST		*pstVarList ;
		MCHAR			*pmStrVal ;/* identifier */
		ARGUMENT_LIST	*pstArglist ;
		ARGUMENT		*pstArgs ;
	} YYSTYPE;
/* Line 1252 of yacc.c.  */
#line 227 "WmlsParser.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE wmlsyylval;



