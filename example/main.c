/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// local
#include <ehsh/ehsh.h>
#include <ehsh/extra/ehcmd.h>

////////////////////////////////////////////////////////////////////////////////
// $Macros
////////////////////////////////////////////////////////////////////////////////
#if WIN32
#define MAIN_EOL EHSH_EOL_CR
#else
#define MAIN_EOL EHSH_EOL_LR
#endif

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
int main(void)
{
  EhPlatform_t* platform;
  EhPlatformInit(&platform);

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
      .Eol          = MAIN_EOL,
      .Tty          = true,
      .Cr           = true,
      .Lf           = true,
    });
  EhExec(&shell);
  EhDeInit(&shell);

  EhPlatformDeInit(&platform);
  return 0;
}
