/** @file
 * SPDX-License-Identifier: BSL-1.0
 */
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <stdbool.h>  // bool
#include <string.h>   // memset

// local
#include <ehsh/ehsh.h>
#include <ehsh/platform/eh.platform.h>

////////////////////////////////////////////////////////////////////////////////
// $Macros
////////////////////////////////////////////////////////////////////////////////
#if __STDC_VERSION__ > 201112L  // for static_assert
static_assert(EHSH_MAX_ARGS <= 15, "ehsh currently only supports up to a maximum of 15 arguments");
#endif /* __STDC_VERSION__ > 201112L */

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
    EhPutStr(self, "No such command \"");
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

static void EhOnChar(EhShell_t* self, char chr)
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

  self->CmdLine[self->Cursor] = chr;
  if (self->Tty)
  {
    EhPutChar(self, chr);
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
    char chr = EhGetChar(self);

    if (chr == '\n')
    {
      if (self->Eol == EHSH_EOL_LF)
      {
        EhOnNewline(self);
      }
    }
    else if (chr == '\r')
    {
      if (self->Eol == EHSH_EOL_CR)
      {
        EhOnNewline(self);
      }
    }
    else if (chr == EHSH_ASCII_EOT)
    {
      self->Stop = 1;
    }
    else if ((chr == EHSH_ASCII_DEL) || (chr == EHSH_ASCII_BS))
    {
      EhOnBackspace(self);
    }
    else if (chr == '\t')
    {
      if (self->Tty)
      {
        EhOnTab(self);
      }
    }
    else if (chr != (char)-1)
    {
      EhOnChar(self, chr);
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
    if ((self->Cmds[i].Name != NULL) && (strncmp(self->Cmds[i].Name, self->CmdLine, EHSH_CMDLINE_SIZE) == 0))
    {
      found = true;
      if (self->Cmds[i].Callback != NULL)
      {
        self->Cmds[i].Callback(self);
      }
    }
  }

  return found;
}

// TODO: Multiple spaces
// TODO: Comment
// TODO: Empty command just prints a newline
// TODO: Password mode
// TODO: History? (shell hook?)
// TODO: RunOne (so ehsh doesn't need its own thread)
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
