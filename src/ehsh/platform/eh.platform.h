/** @file
 * SPDX-License-Identifier: BSL-1.0
 */
#ifndef EHSH_PLATFORM_H
#define EHSH_PLATFORM_H
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// local
#include <ehsh/ehsh.cfg.h>

////////////////////////////////////////////////////////////////////////////////
// $Macros
////////////////////////////////////////////////////////////////////////////////
#if EHSH_CFG_PLATFORM_AUTO
  #if defined(__CYGWIN__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__)
    #undef EHSH_CFG_PLATFORM_STDC
    #define EHSH_CFG_PLATFORM_STDC 1
  #elif defined(_MSC_VER)
    #undef EHSH_CFG_PLATFORM_WIN32
    #define EHSH_CFG_PLATFORM_WIN32 1
  #elif defined(__linux__)
    #undef EHSH_CFG_PLATFORM_LINUX
    #define EHSH_CFG_PLATFORM_LINUX 1
  #else
    #undef EHSH_CFG_PLATFORM_FPTR
    #define EHSH_CFG_PLATFORM_FPTR 1
  #endif
#endif

#if EHSH_CFG_PLATFORM_FPTR
#include <ehsh/platform/eh.fptr.h>
#elif EHSH_CFG_PLATFORM_STDC
#include <ehsh/platform/eh.stdc.h>
#elif EHSH_CFG_PLATFORM_LINUX
#include <ehsh/platform/eh.linux.h>
#elif EHSH_CFG_PLATFORM_WIN32
#include <ehsh/platform/eh.win32.h>
#endif

#endif /* EHSH_PLATFORM_H */
