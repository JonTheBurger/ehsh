/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef EHSH_CMD_H
#define EHSH_CMD_H
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <stdbool.h>  // true
#include <string.h>   // strnlen

// local
#include <ehsh/ehsh.h>

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
static inline void EhHelp(EhShell_t* shell)
{
  const char* arg = EhArgAt(shell, 0);
  if (arg == NULL)
  {
    arg = "";
  }
  size_t prefix = strnlen(arg, EHSH_CMDLINE_SIZE);

  for (size_t i = 0; i < shell->CmdCount; ++i)
  {
    if (strncmp(arg, shell->Cmds[i].Name, prefix) == 0)
    {
      EhPutStr(shell, shell->Cmds[i].Name);
      EhPutStr(shell, ": ");
      EhPutStr(shell, shell->Cmds[i].Help);
      EhPutNewline(shell);
    }
  }
}

static inline void EhExit(EhShell_t* shell)
{
  shell->Stop = true;
}

static inline void EhEcho(EhShell_t* shell)
{
  for (size_t i = 0; i < shell->ArgCount; ++i)
  {
    const char* arg = EhArgAt(shell, i);
    EhPutStr(shell, arg);
    EhPutNewline(shell);
  }
}

////////////////////////////////////////////////////////////////////////////////
// $Macros
////////////////////////////////////////////////////////////////////////////////
#define EHSH_COMMAND_HELP               \
  (EhCommand_t)                         \
  {                                     \
    "help", "Prints commands", &EhHelp, \
  }
#define EHSH_COMMAND_EXIT               \
  (EhCommand_t)                         \
  {                                     \
    "exit", "Quits the shell", &EhExit, \
  }
#define EHSH_COMMAND_ECHO                \
  (EhCommand_t)                          \
  {                                      \
    "echo", "Prints arguments", &EhEcho, \
  }

#endif /* EHSH_CMD_H */
