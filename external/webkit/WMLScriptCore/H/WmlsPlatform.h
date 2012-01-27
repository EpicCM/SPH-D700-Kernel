/*
 * Samsung Handset Platform
 * Copyright (c) 2000 Software Center, Samsung Electronics, Inc.
 * All rights reserved.
 */
/** 
 *
 * @author	venkat.nj@samsung.com
 */

#ifndef _WMLSPLATFORM_H_
#define _WMLSPLATFORM_H_

#include <stdlib.h>
#include <math.h>
#include <string.h>

#if defined(WMLS_PLATFORM_ANDROID)
#include <utils/log.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef		int				BOOL;
typedef		int				INT32;
typedef		unsigned int	UINT32;
typedef		unsigned long	ULONG;
typedef		signed char		INT8;
typedef		unsigned short	UINT16;
typedef		unsigned char	UINT8;
typedef		unsigned char	UCHAR ;
typedef		unsigned		UINT;
typedef		unsigned short	MCHAR;
typedef		unsigned long	MCHAR32;
typedef		unsigned short	USHORT;


/* Math related functions */
#define wmls_pow		pow
#define wmls_sqrt		sqrt
#define wmls_rand		rand
#define wmls_srand		srand

/* String related functions */
#define wmls_strcat		strcat
#define wmls_strlen		strlen
#define wmls_strcmp		strcmp
#define wmls_strcpy		strcpy
#define wmls_strncpy	strncpy
#define wmls_strncmp	strncmp
#define wmls_strnicmp	strncasecmp

/* Utilities */
#define wmls_atoi		atoi
#define wmls_atof		atof
#define wmls_itoa		itoa
#define wmls_strstr		strstr
#define wmls_strrchr	strrchr
#define wmls_strchr		strchr

#define wmls_memset		memset
#define wmls_memcpy		memcpy

BOOL wmls_isdigit(INT32 c) ;
BOOL wmls_isalpha(INT32 c);
BOOL wmls_isalnum(INT32 c);

#if defined(WMLS_PLATFORM_ANDROID)
#define WMLS_ASSERT_WITH_MESSAGE(...)	LOG_ASSERT(0, "Assertion[NEVER GET HERE] %s:%s:%s", __FILE__, __LINE__, __FUNCTION__);
#define WMLS_ASSERT(assertion)	LOG_ASSERT(assertion, "Assertion in %s:%s:%s", __FILE__, __LINE__, __FUNCTION__);
#define WMLS_ERROR(...)	        LOGE(__VA_ARGS__)
#define WMLS_LOG(...)	        LOGD(__VA_ARGS__)
#else
#define WMLS_ASSERT_WITH_MESSAGE(...)	((void)0)
#define WMLS_ASSERT(...)	((void)0)
#define WMLS_ERROR(...)	    ((void)0)
#define WMLS_LOG(...)	    ((void)0)
#endif

INT32 wmls_tolower(INT32 c) ;


/* MCHAR related functions */
INT32 wmls_wcscmp(const MCHAR *s1, const MCHAR *s2);

INT32 wmls_wcscasecmp(const MCHAR *s1, const MCHAR *s2);

INT32 wmls_wcslen(const MCHAR *s);

MCHAR *wmls_wcscpy(MCHAR * s1, const MCHAR * s2);


/* Unicode conversion related functions */
BOOL WmlsConvertUTF8ToUTF16(MCHAR* dst, unsigned int dstLength,
							const char* src, unsigned int srcLength	);

BOOL WmlsConvertUTF16ToUTF8( char* dst, unsigned int dstLength,
							const MCHAR* src, unsigned int srcLength);


void* WmlsCalloc(int len, int aSize);

void* WmlsRealloc(void *ptr, int newSize);

void WmlsFree(void *ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // _WMLSPLATFORM_H_
