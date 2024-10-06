/** @file
 * SPDX-License-Identifier: BSL-1.0
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
#include <ehsh/platform/eh.win32.h>
#define MAIN_EOL EHSH_EOL_CR
#else
#include <ehsh/platform/eh.linux.h>
#define MAIN_EOL EHSH_EOL_LF
#endif

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
/// [Main]
int main(void)
{
  EhPlatform_t* platform = NULL;
  EhPlatformInit(&platform);

  const EhCommand_t cmds[] = {
    {
      "#",
      "Comment",
      &EhComment,
    },
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
    {
      "stty",
      "Configure shell EOL, TTY",
      &EhStty,
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
/// [Main]
