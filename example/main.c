#include <ehsh/ehsh.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define RAWTERM 1

#if RAWTERM
#include "ehsh/platform/eh.linux.h"
#else
#include "ehsh/platform/eh.stdc.h"
#endif

void Echo(EhShell_t* shell)
{
  for (size_t i = 0; i < shell->ArgCount; ++i)
  {
    const char* arg = EhArgAt(shell, i);
    EhPutStr(shell, arg);
    EhPutNewline(shell);
  }
}

void Help(EhShell_t* shell)
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

void Quit(EhShell_t* shell)
{
  shell->Stop = true;
}

int main(void)
{
  const EhCommand_t cmds[] = {
    {
      "echo",
      "Prints arguments",
      &Echo,
    },
    {
      "ecco",
      "Prints arguments",
      &Echo,
    },
    {
      "help",
      "Prints this",
      &Help,
    },
    {
      "quit",
      "Exits",
      &Quit,
    },
  };
  EhShell_t shell;
  EhCreate(&shell);
  EhInit(&shell, cmds, sizeof(cmds) / sizeof(*cmds));
  shell.Cr  = true;
  shell.Lf  = true;
  shell.Eol = EHSH_EOL_LF;
  shell.Tty = true;

  EhExec(&shell);

  EhDestroy(&shell);

#if RAWTERM
  tcsetattr(STDIN_FILENO, TCSANOW, &GTermIo);
#endif
  return 0;
}
