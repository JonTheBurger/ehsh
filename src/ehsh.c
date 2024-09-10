/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <stdbool.h>  // bool
#include <string.h>   // memset

// local
#include <ehsh/ehsh.h>

////////////////////////////////////////////////////////////////////////////////
// $Macros
////////////////////////////////////////////////////////////////////////////////
#define EHSH_BACKSPACE "\x08"

#if defined(__GNUC__) || defined(__clang__)
#define EHSH_WEAK __attribute__((weak))
#elif defined(__ICCARM__)
#define EHSH_WEAK __weak
#else
#define EHSH_WEAK
#endif

////////////////////////////////////////////////////////////////////////////////
// $Types
////////////////////////////////////////////////////////////////////////////////
/// ASCII control characters
typedef enum EhAscii {
  EHSH_ASCII_EOT = 4,    ///< End of transmission
  EHSH_ASCII_BS  = 8,    ///< Backspace
  EHSH_ASCII_DEL = 127,  ///< Delete
} EhAscii_t;

////////////////////////////////////////////////////////////////////////////////
// $Globals
////////////////////////////////////////////////////////////////////////////////
EHSH_WEAK char (*EhGetCharFn)(EhShell_t* self)         = NULL;
EHSH_WEAK void (*EhPutCharFn)(EhShell_t* self, char c) = NULL;

////////////////////////////////////////////////////////////////////////////////
// $Prototypes
////////////////////////////////////////////////////////////////////////////////
/** @brief Handles the currently loaded EhShell.CmdLine,
 * calling its handler with tokenized args.
 *
 * @param self Shell whose current command line will be handled.
 */
static bool EhHandleCmdLine(EhShell_t* self);

/** @brief Saves the indices of the arguments,
 * replacing spaces in EhShell.CmdLine with '\0'.
 *
 * @param self Shell whose arguments shall be tokenized.
 */
static void EhTokenize(EhShell_t* self);

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
static void EhOnNewline(EhShell_t* self)
{
  if (self->Tty)
  {
    EhPutNewline(self);
  }
  if (!EhHandleCmdLine(self))
  {
    EhPutStr(self, EHSH_EMSG_NOCMD " \"");
    EhPutStr(self, self->CmdLine);
    EhPutChar(self, '"');
    EhPutNewline(self);
  }
  if (self->Tty)
  {
    EhPutStr(self, EHSH_PROMPT);
  }

  // Reset
  memset(&self->CmdLine[0], 0, sizeof(self->CmdLine));
  self->Cursor = 0;
}

static void EhOnBackspace(EhShell_t* self)
{
  if (self->Cursor > 0)
  {
    self->CmdLine[self->Cursor - 1] = '\0';
    --self->Cursor;
    if (self->Tty)
    {
      // Move cursor left, print space, move cursor left
      EhPutStr(self, EHSH_BACKSPACE " " EHSH_BACKSPACE);
    }
  }
}

static void EhOnTab(EhShell_t* self)
{
  uint8_t matches   = 0;
  uint8_t lastMatch = 0;

  for (size_t i = 0; (i < self->CmdCount); ++i)
  {
    if ((self->Cmds[i].Name != NULL) && (strncmp(self->CmdLine, self->Cmds[i].Name, self->Cursor) == 0))
    {
      ++matches;
      lastMatch = i;
      EhPutNewline(self);
      EhPutStr(self, self->Cmds[i].Name);
    }
  }

  if (matches)
  {
    if (matches == 1)
    {
      strncpy(&self->CmdLine[0], self->Cmds[lastMatch].Name, sizeof(self->CmdLine));
      self->Cursor = strnlen(&self->CmdLine[0], EHSH_CMDLINE_SIZE);
    }
    EhPutNewline(self);
    EhPutStr(self, EHSH_PROMPT);
    EhPutStr(self, self->CmdLine);
  }
}

static void EhOnChar(EhShell_t* self, char c)
{
  // If the cursor is at the end of the buffer
  if (self->Cursor >= EHSH_CMDLINE_SIZE)
  {
    // Move the cursor to the last valid position
    self->Cursor = EHSH_CMDLINE_SIZE - 1;
    // and Send a backspace to the TTY
    if (self->Tty)
    {
      EhPutChar(self, EHSH_ASCII_BS);
    }
  }

  self->CmdLine[self->Cursor] = c;
  if (self->Tty)
  {
    EhPutChar(self, c);
  }

  ++self->Cursor;
}

EhShell_t* EhInit(EhShell_t* self, const EhConfig_t* config)
{
  if ((self != NULL) && (config != NULL))
  {
    memset(self, 0, sizeof(*self));
    self->Cmds     = config->Commands;
    self->CmdCount = config->CommandCount;
    self->Eol      = config->Eol;
    self->Tty      = config->Tty;
    self->Cr       = config->Cr;
    self->Lf       = config->Lf;
  }
  return self;
}

void EhDeInit(EhShell_t* self)
{
  (void)self;
}

void EhExec(EhShell_t* self)
{
  if (self->Tty)
  {
    EhPutStr(self, EHSH_PROMPT);
  }

  do
  {
    char c = EhGetChar(self);

    if (c == '\n')
    {
      if (self->Eol == EHSH_EOL_LF)
      {
        EhOnNewline(self);
      }
    }
    else if (c == '\r')
    {
      if (self->Eol == EHSH_EOL_CR)
      {
        EhOnNewline(self);
      }
    }
    else if ((c < 0) || (c == EHSH_ASCII_EOT))
    {
      self->Stop = 1;
    }
    else if (c == EHSH_ASCII_DEL)
    {
      EhOnBackspace(self);
    }
    else if (c == '\t')
    {
      if (self->Tty)
      {
        EhOnTab(self);
      }
    }
    else
    {
      EhOnChar(self, c);
    }
  } while (!self->Stop);
}

void EhPutNewline(EhShell_t* self)
{
  if (self->Cr)
  {
    EhPutChar(self, '\r');
  }
  if (self->Lf)
  {
    EhPutChar(self, '\n');
  }
}

static bool EhHandleCmdLine(EhShell_t* self)
{
  bool found = false;
  EhTokenize(self);

  for (size_t i = 0; ((!found) && (i < self->CmdCount)); ++i)
  {
    if ((self->Cmds[i].Callback != NULL) && (strncmp(self->Cmds[i].Name, self->CmdLine, EHSH_CMDLINE_SIZE) == 0))
    {
      found = true;
      self->Cmds[i].Callback(self);
    }
  }

  return found;
}

// TODO: Multiple spaces
static void EhTokenize(EhShell_t* self)
{
  self->Nul      = '\0';
  self->ArgCount = 0;

  char* delim = &self->CmdLine[0];
  while ((delim != NULL) && (self->ArgCount < EHSH_MAX_ARGS))
  {
    delim = strchr(delim, ' ');
    if (delim != NULL)
    {
      size_t index                 = delim - &self->CmdLine[0];
      self->CmdLine[index]         = '\0';
      self->ArgIdx[self->ArgCount] = index + 1;
      ++self->ArgCount;
      ++delim;
    }
  }
}

EHSH_WEAK char EhGetChar(EhShell_t* self)
{
  char c = EHSH_ASCII_EOT;

  if (EhGetCharFn != NULL)
  {
    c = EhGetCharFn(self);
  }

  return c;
}

EHSH_WEAK void EhPutChar(EhShell_t* self, char c)
{
  if (EhPutCharFn != NULL)
  {
    EhPutCharFn(self, c);
  }
}

EHSH_WEAK void EhPutStr(EhShell_t* self, const char* str)
{
  for (size_t i = 0; str[i] != '\0'; ++i)
  {
    EhPutChar(self, str[i]);
  }
}
