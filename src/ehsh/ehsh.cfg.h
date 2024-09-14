/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef EHSH_CFG_H
#define EHSH_CFG_H
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// local
#if (defined(__has_include) && (__has_include("ehsh.user.h")))
#include "ehsh.user.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// $Macros
////////////////////////////////////////////////////////////////////////////////
#ifndef EHSH_CFG_PLATFORM_FPTR
/** */
#define EHSH_CFG_PLATFORM_FPTR 0
#endif /* EHSH_CFG_PLATFORM_FPTR */

#ifndef EHSH_CFG_PLATFORM_STDC
/** */
#define EHSH_CFG_PLATFORM_STDC 0
#endif /* EHSH_CFG_PLATFORM_STDC */

#ifndef EHSH_CFG_PLATFORM_LINUX
/** */
#define EHSH_CFG_PLATFORM_LINUX 0
#endif /* EHSH_CFG_PLATFORM_LINUX */

#ifndef EHSH_CFG_PLATFORM_WIN32
/** */
#define EHSH_CFG_PLATFORM_WIN32 0
#endif /* EHSH_CFG_PLATFORM_WIN32 */

#ifndef EHSH_CFG_PLATFORM_AUTO
/** */
#define EHSH_CFG_PLATFORM_AUTO 1
#endif /* EHSH_CFG_PLATFORM_AUTO */

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

#endif /* EHSH_CFG_H */
