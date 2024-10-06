/**
 * @addtogroup ehsh
 * @{
 *
 * @file
 * SPDX-License-Identifier: BSL-1.0
 *
 * Default configuration for ehsh. Users can pre-define these macros on the
 * command line when compiling ehsh & its users to override the settings, or
 * you can supply an "ehsh.user.h" file to the include path of ehsh.
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
#ifndef EHSH_CMDLINE_SIZE
/** Number of characters in the command line, excluding null terminator. */
#define EHSH_CMDLINE_SIZE 16
#endif /* EHSH_CMDLINE_SIZE */

#ifndef EHSH_MAX_ARGS
/** Maximum number of arguments that ehsh can tokenize.
 *
 * @note ehsh currently only supports a maximum of 15 arguments. This is checked
 * with a C11 static_assert, if available.
 *
 * Arguments are space-delimited:
 *
 * @code{.sh}
 * # echo has been passed 3 arguments:
 * > echo a b c
 * a
 * b
 * c
 * @endcode
 *
 * If the command line contains more spaces than EHSH_MAX_ARGS supports, then
 * the last argument contains the remainder of the command line, including
 * spaces:
 *
 * @code{.sh}
 * # Say we set EHSH_MAX_ARGS to 4, the 4th arg is "d e f":
 * > echo a b c d e f
 * a
 * b
 * c
 * d e f
 * @endcode
 */
#define EHSH_MAX_ARGS 4  // Up to 15
#endif /* EHSH_MAX_ARGS */

#ifndef EHSH_PROMPT
/** String printed in tty mode to prompt the user for input.
 *
 * @code{.sh}
 * # Assuming EHSH_PROMPT "> ":
 * > echo hi
 * hi
 * # Assuming EHSH_PROMPT "$ ":
 * $ echo hi
 * hi
 * # Assuming EHSH_PROMPT ">>>":
 * >>>echo hi
 * hi
 * # Assuming EHSH_PROMPT "(ehsh): ":
 * (ehsh): echo hi
 * hi
 * @endcode
 */
#define EHSH_PROMPT "> "
#endif /* EHSH_PROMPT */

#ifndef EHSH_CFG_PLATFORM_FPTR
/** Defines all platform hook symbols as weak when supported. Default
 * implementations use weak function pointers (which can be overridden):
 *
 * @see EhGetCharFn
 * @see EhPutCharFn
 * @see EhPutStrFn
 *
 * For more information, @see ehsh.fptr.h
 */
#define EHSH_CFG_PLATFORM_FPTR 0
#endif /* EHSH_CFG_PLATFORM_FPTR */

#ifndef EHSH_CFG_PLATFORM_STDC
/** Uses standard C functions `getchar()`, `putchar()`, and `printf` to
 * implement the shell. This mode does not support tab completion.
 * This is used on Mac, BSD, Cygwin when EHSH_CFG_PLATFORM_AUTO is set.
 * @see ehsh.stdc.h
 */
#define EHSH_CFG_PLATFORM_STDC 0
#endif /* EHSH_CFG_PLATFORM_STDC */

#ifndef EHSH_CFG_PLATFORM_LINUX
/** Uses `termios` to configure an interactive shell on Linux. @see ehsh.linux.h */
#define EHSH_CFG_PLATFORM_LINUX 0
#endif /* EHSH_CFG_PLATFORM_LINUX */

#ifndef EHSH_CFG_PLATFORM_WIN32
/** Uses `SetConsoleMode` to configure an interactive shell on Windows. @see ehsh.win32.h */
#define EHSH_CFG_PLATFORM_WIN32 0
#endif /* EHSH_CFG_PLATFORM_WIN32 */

#ifndef EHSH_CFG_PLATFORM_AUTO
/** Automatically detect the target platform; disregarding any previously set macros. */
#define EHSH_CFG_PLATFORM_AUTO 0
#endif /* EHSH_CFG_PLATFORM_AUTO */

#endif /* EHSH_CFG_H */
