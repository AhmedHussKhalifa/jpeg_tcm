// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// MS windows or Mac OSx
#define IS_BUILD_MS_WINDOWS                 1

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(_MSC_VER)
#define OS_WIN
#endif


#ifdef OS_WIN

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#endif

// TODO: reference additional headers your program requires here
