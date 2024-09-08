#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <ehsh/ehsh.h>

#define RAWTERM 0

#if RAWTERM
  #include "ehsh/platform/eh.linux.h"
#else
  #include "ehsh/platform/eh.stdc.h"
#endif

void Echo(eh_Shell* shell)
{
  for (size_t i = 0; i < shell->ArgCount; ++i)
  {
    const char* arg = eh_ArgAt(shell, i);
    eh_PutStr(shell, arg);
    eh_PutNewline(shell);
  }
}

void Help(eh_Shell* shell)
{
  const char* arg = eh_ArgAt(shell, 0);
  if (arg == NULL)
  {
    arg = "";
  }
  size_t prefix = strnlen(arg, EHSH_CMDLINE_SIZE);

  for (size_t i = 0; i < shell->CmdCount; ++i)
  {
    if (strncmp(arg, shell->Cmds[i].Name, prefix) == 0)
    {
      eh_PutStr(shell, shell->Cmds[i].Name);
      eh_PutStr(shell, ": ");
      eh_PutStr(shell, shell->Cmds[i].Help);
      eh_PutNewline(shell);
    }
  }
}

void Quit(eh_Shell* shell)
{
  shell->Stop = true;
}

int main(void)
{
  const eh_Command cmds[] = {
    {"echo", "Prints arguments", &Echo, },
    {"help", "Prints this", &Help, },
    {"quit", "Exits", &Quit, },
  };
  eh_Shell shell;
  eh_Create(&shell);
  eh_Init(&shell, cmds, sizeof(cmds) / sizeof(*cmds));
  shell.Cr = true;
  shell.Lf = true;
  shell.Eol = EHSH_EOL_LF;
  shell.Tty = true;

  eh_Exec(&shell);

  eh_Destroy(&shell);

#if RAWTERM
  tcsetattr(STDIN_FILENO, TCSANOW, &gTermIO);
#endif
  return 0;
}
