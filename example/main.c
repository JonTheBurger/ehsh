/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// local
#include <ehsh/ehcmd.h>
#include <ehsh/ehsh.h>

////////////////////////////////////////////////////////////////////////////////
// $Macros
////////////////////////////////////////////////////////////////////////////////
#ifndef EHSH_CFG_PLATFORM_SPECIFIC
#define EHSH_CFG_PLATFORM_SPECIFIC 0
#endif /* EHSH_CFG_PLATFORM_SPECIFIC */

#if (EHSH_CFG_PLATFORM_SPECIFIC == 0)
#include "ehsh/platform/eh.stdc.h"
#elif defined(__linux__)
#include "ehsh/platform/eh.linux.h"
#elif defined(_MSC_VER)
#include "ehsh/platform/eh.stdc.h"
// #include "ehsh/platform/eh.win32.h"
#else
#include "ehsh/platform/eh.stdc.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
int main(void)
{
  const EhCommand_t cmds[] = {
    {
      "echo",
      "Prints arguments",
      &EhEcho,
    },
    {
      "exit",
      "Exits",
      &EhExit,
    },
    {
      "help",
      "Prints commands",
      &EhHelp,
    },
  };
  EhShell_t shell;
  EhInit(
    &shell,
    &(EhConfig_t){
      .Commands     = cmds,
      .CommandCount = sizeof(cmds) / sizeof(*cmds),
      .Eol          = EHSH_EOL_LF,
      .Tty          = true,
      .Cr           = true,
      .Lf           = true,
    });
  EhPlatformContext_t context;
  EhPlatformInit(&shell, &context);
  EhExec(&shell);
  EhPlatformDeInit(&shell);
  EhDeInit(&shell);

  return 0;
}
