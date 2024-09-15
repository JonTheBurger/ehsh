/** @file
 * SPDX-License-Identifier: BSL-1.0
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
#define EHSH_CFG_PLATFORM_AUTO 0
#endif /* EHSH_CFG_PLATFORM_AUTO */

#endif /* EHSH_CFG_H */
