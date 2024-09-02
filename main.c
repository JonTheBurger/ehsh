#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Shell.h
#define RAWTERM 1
// TODO: History
#define SHELL_HISTORY_SIZE 10
#define SHELL_CMDLINE_SIZE 16
#define SHELL_MAX_ARGS     4  // Up to 15
#define SHELL_PROPMT       "> "
// TODO: Errors
#define SHELL_EMSG_NOCMD   "No such command"
#define SHELL_EMSG_SYNTAX  "Syntax error"
#define SHELL_EMSG_AUTH    "Bad permissions"
#define SHELL_EMSG_ARG     "Bad argument(s)"
#define SHELL_EMSG_BUSY    "Subsystem busy"

struct Shell;

typedef void (*ShellFunction_t)(struct Shell* shell);

typedef struct ShellCmd {
  const char*     Name;
  const char*     Help;
  ShellFunction_t Callback;
} ShellCmd_t;

typedef struct Shell {
  const ShellCmd_t* Commands;     ///< Array of registered shell commands, alphabetized
  uint8_t           CommandCount; ///< Number of registered shell commands
  uint8_t           Cursor;       ///< Position of cursor
  // TODO: Should this be passed in?
  char              CommandLine[SHELL_CMDLINE_SIZE];  ///< Text input by user
  char              Nul;                              ///< Always set to '\0' for safety
  uint8_t           Arguments[SHELL_MAX_ARGS];  ///< Indices of tokenized arguments
  uint8_t           ArgumentCount : 4;          ///< Number of parsed argument tokens
  uint8_t           Stop : 1;          ///< Set to 1 to stop running
  uint8_t           Crlf : 1;          ///< Use carriage return
  uint8_t           Tty : 1;           ///< Act as a teletype terminal (echo chars as they are typed)
  uint8_t           User : 1;          ///< User state, e.g. privilege escalation level
  void*             Context;           ///< User context associated with this shell
} Shell_t;

int ShellInit(Shell_t* self, const ShellCmd_t* commands, size_t count);
void ShellDeInit(Shell_t* self);
void ShellExec(Shell_t* self);
void ShellProcess(Shell_t* self, char c);
const char* ShellGetArgument(Shell_t* self, size_t index);
void ShellHandleCommand(Shell_t* self);
char ShellGetChar();
void ShellPutChar(char c);
void ShellPutStr(const char* str);

void Echo(Shell_t* shell)
{
  for (size_t i = 0; i < shell->ArgumentCount; ++i)
  {
    ShellPutStr(&shell->CommandLine[shell->Arguments[i]]);
    if (shell->Crlf)
    {
      ShellPutChar('\r');
    }
    ShellPutChar('\n');
  }
}

void Help(Shell_t* shell)
{
  const char* arg = ShellGetArgument(shell, 0);
  if (arg == NULL)
  {
    arg = "";
  }
  size_t prefix = strnlen(arg, SHELL_CMDLINE_SIZE);

  for (size_t i = 0; i < shell->CommandCount; ++i)
  {
    if (shell->Commands[i].Name != NULL)
    {
      if (strncmp(arg, shell->Commands[i].Name, prefix) == 0)
      {
        ShellPutStr(shell->Commands[i].Name);
        ShellPutChar(':');
        ShellPutChar(' ');
        ShellPutStr(shell->Commands[i].Help);
        if (shell->Crlf)
        {
          ShellPutChar('\r');
        }
        ShellPutChar('\n');
      }
    }
  }
}

void Quit(Shell_t* shell)
{
  shell->Stop = true;
}

int main(void)
{
  const ShellCmd_t cmds[] = {
    {"echo", "Prints arguments", &Echo, },
    {"help", "Prints this", &Help, },
    {"quit", "Exits", &Quit, },
  };
  Shell_t shell;
  ShellInit(&shell, cmds, sizeof(cmds) / sizeof(*cmds));

  ShellExec(&shell);

  ShellDeInit(&shell);
  return 0;
}

// Shell.c
#if RAWTERM
#include <termios.h>
#include <unistd.h>
static struct termios PreviousTermIO;
#endif

static void ShellTokenize(Shell_t* self);

const char* ShellGetArgument(Shell_t* self, size_t index)
{
  const char* arg = NULL;
  if (index < self->ArgumentCount)
  {
    arg = &self->CommandLine[self->Arguments[index]];
  }
  return arg;
}

void ShellExec(Shell_t* self)
{
  ShellPutStr(SHELL_PROPMT);
  do
  {
    char c = ShellGetChar();

    if (c > 0)
    {
      ShellProcess(self, c);
    }
    else
    {
      self->Stop = true;
    }

  } while (!self->Stop);
}

enum AsciiCtrl {
  ASCII_CTRL_BS = 8,    ///< Backspace
  ASCII_CTRL_DEL = 127, ///< Delete
} AsciiCtrl_t;

// Handles printing
void ShellProcess(Shell_t* self, char c)
{
  if (c == ASCII_CTRL_DEL)
  {
    if (self->Cursor > 0)
    {
      self->CommandLine[self->Cursor - 1] = '\0';
      --self->Cursor;
      if (self->Tty)
      {
        ShellPutChar(ASCII_CTRL_BS);
        ShellPutChar(' ');
        ShellPutChar(ASCII_CTRL_BS);
      }
    }
  }
  else if (c == '\t')
  {
    ShellDeInit(self);
    // TODO: Help
  }
  else if ((c == '\r') && (self->Crlf))
  {
    if (self->Tty)
    {
      ShellPutChar('\r');
      ShellPutChar('\n');
      // Wait for '\n'
    }

    ShellHandleCommand(self);
  }
  else if (c == '\n')
  {
    if (self->Tty)
    {
      ShellPutChar('\r');
      ShellPutChar('\n');
    }

    ShellHandleCommand(self);
  }
  else if (self->Cursor < SHELL_CMDLINE_SIZE)
  {
    self->CommandLine[self->Cursor] = c;
    ++self->Cursor;
    if (self->Tty)
    {
      ShellPutChar(c);
    }
  }
  else
  {
    self->CommandLine[SHELL_CMDLINE_SIZE - 1] = c;
    if (self->Tty)
    {
      ShellPutChar(ASCII_CTRL_BS);
      ShellPutChar(c);
    }
  }
}

// TODO: multiple spaces
static void ShellTokenize(Shell_t* self)
{
  self->Nul           = '\0';
  self->ArgumentCount = 0;

  char* delim = &self->CommandLine[0];
  while ((delim != NULL) && (self->ArgumentCount < SHELL_MAX_ARGS))
  {
    delim = strchr(delim, ' ');
    if (delim != NULL)
    {
      size_t index = delim - &self->CommandLine[0];
      self->CommandLine[index] = '\0';
      self->Arguments[self->ArgumentCount] = index + 1;
      ++self->ArgumentCount;
      ++delim;
    }
  }
}

void ShellHandleCommand(Shell_t* self)
{
  ShellTokenize(self);
  bool found = false;

  for (size_t i = 0; ((!found) && (i < self->CommandCount)); ++i)
  {
    if (strncmp(self->CommandLine, self->Commands[i].Name, SHELL_CMDLINE_SIZE) == 0)
    {
      if (self->Commands[i].Callback != NULL)
      {
        found = true;
        self->Commands[i].Callback(self);
      }
    }
  }

  if (!found)
  {
    ShellPutStr(SHELL_EMSG_NOCMD);
    ShellPutChar(' ');
    ShellPutStr(self->CommandLine);
    if (self->Crlf)
    {
      ShellPutChar('\r');
    }
    ShellPutChar('\n');
  }

  // Reset
  memset(&self->CommandLine[0], 0, sizeof(self->CommandLine));
  self->Cursor = 0;
  ShellPutStr(SHELL_PROPMT);
}

int ShellInit(Shell_t* self, const ShellCmd_t* commands, size_t count)
{
  // TODO: Memberwise init? How should init work really?
  self->Commands     = commands;
  self->CommandCount = count;
  self->Cursor = 0;
  memset(&self->CommandLine[0], 0, sizeof(self->CommandLine));
  self->Nul = '\0';
  memset(&self->Arguments[0], 0, sizeof(self->Arguments));
  self->ArgumentCount = 0;

  self->Stop         = false;
  self->Crlf         = RAWTERM;
  self->Tty          = RAWTERM;

#if RAWTERM
  // https://man7.org/linux/man-pages/man3/termios.3.html
  // "stty -a"
  tcgetattr(STDIN_FILENO, &PreviousTermIO);
  struct termios termiosv = PreviousTermIO;
  cfmakeraw(&termiosv);
  if (!self->Crlf)
  {
    // This should almost certainly be true, but this prevents false from breaking everything
    termiosv.c_iflag |= ICRNL;
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &termiosv);
#endif

  return 0;
}

void ShellDeInit(Shell_t* self)
{
#if RAWTERM
  tcsetattr(STDIN_FILENO, TCSANOW, &PreviousTermIO);
#endif
}

char ShellGetChar()
{
#if RAWTERM
  char c;
  if (read(STDIN_FILENO, &c, 1) == 0)
  {
    c = '\0';
  }
  return c;
#else
  return getchar();
#endif
}

void ShellPutChar(char c)
{
#if RAWTERM
  write(STDOUT_FILENO, &c, 1);
#else
  putchar(c);
#endif
}

void ShellPutStr(const char* str)
{
  if (str != NULL)
  {
#if RAWTERM
    write(STDOUT_FILENO, str, strnlen(str, 0xFF));
#else
    printf("%s", str);
#endif
  }
}
