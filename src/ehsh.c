/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <stdbool.h> // bool
#include <string.h>  // memset

// local
#include <ehsh/ehsh.h>

////////////////////////////////////////////////////////////////////////////////
// $Macros
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// $Types
////////////////////////////////////////////////////////////////////////////////
/// ASCII control characters
typedef enum eh_Ascii {
  EHSH_ASCII_EOT = 4,    ///< End of transmission
  EHSH_ASCII_BS  = 8,    ///< Backspace
  EHSH_ASCII_DEL = 127,  ///< Delete
} eh_Ascii;

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
/** @brief Handles the currently loaded eh_Shell.CmdLine,
 * calling its handler with tokenized args.
 *
 * @param self Shell whose current command line will be handled.
 */
static bool eh_HandleCmdLine(eh_Shell* self);

/** @brief Saves the indices of the arguments,
 * replacing spaces in eh_Shell.CmdLine with '\0'.
 *
 * @param self Shell whose arguments shall be tokenized.
 */
static void eh_Tokenize(eh_Shell* self);

////////////////////////////////////////////////////////////////////////////////
// $Implementations
////////////////////////////////////////////////////////////////////////////////
static void eh_OnNewline(eh_Shell* self)
{
  if (self->Tty)
  {
    eh_PutNewline(self);
  }
  if (!eh_HandleCmdLine(self))
  {
    eh_PutStr(self, EHSH_EMSG_NOCMD " \"");
    eh_PutStr(self, self->CmdLine);
    eh_PutChar(self, '"');
    eh_PutNewline(self);
  }
  eh_PutStr(self, EHSH_PROPMT);

  // Reset
  memset(&self->CmdLine[0], 0, sizeof(self->CmdLine));
  self->Cursor = 0;
}

static void eh_OnBackspace(eh_Shell* self)
{
  if (self->Cursor > 0)
  {
    self->CmdLine[self->Cursor - 1] = '\0';
    --self->Cursor;
    if (self->Tty)
    {
      // Move cursor left, print space, move cursor left
      eh_PutStr(self, "\08 \08");
    }
  }
}

static void eh_OnTab(eh_Shell* self)
{
  // TODO: Help
}

static void eh_OnChar(eh_Shell* self, char c)
{
  // If the cursor is at the end of the buffer
  if (self->Cursor >= EHSH_CMDLINE_SIZE)
  {
    // Move the cursor to the last valid position
    self->Cursor = EHSH_CMDLINE_SIZE - 1;
    // and Send a backspace to the TTY
    if (self->Tty)
    {
      eh_PutChar(self, EHSH_ASCII_BS);
    }
  }

  self->CmdLine[self->Cursor] = c;
  if (self->Tty)
  {
    eh_PutChar(self, c);
  }

  ++self->Cursor;
}

eh_Shell* eh_Create(eh_Shell* self)
{
  if (self != NULL)
  {
    memset(self, 0, sizeof(*self));
  }
  return self;
}

void eh_Exec(eh_Shell* self)
{
  eh_PutStr(self, EHSH_PROPMT);

  do
  {
    char c = eh_GetChar(self);

    if (c == '\n')
    {
      if (self->Eol == EHSH_EOL_LF)
      {
        eh_OnNewline(self);
      }
    }
    else if (c == '\r')
    {
      if (self->Eol == EHSH_EOL_CR)
      {
        eh_OnNewline(self);
      }
    }
    else if ((c < 0) || (c == EHSH_ASCII_EOT))
    {
      self->Stop = 1;
    }
    else if (c == EHSH_ASCII_DEL)
    {
      eh_OnBackspace(self);
    }
    else if (c == '\t')
    {
      eh_OnTab(self);
    }
    else
    {
      eh_OnChar(self, c);
    }
  } while (!self->Stop);
}

void eh_PutNewline(eh_Shell* self)
{
  if (self->Cr)
  {
    eh_PutChar(self, '\r');
  }
  if (self->Lf)
  {
    eh_PutChar(self, '\n');
  }
}

void eh_Destroy(eh_Shell* self)
{
  (void)self;
}

static bool eh_HandleCmdLine(eh_Shell* self)
{
  bool found = false;
  eh_Tokenize(self);

  for (size_t i = 0; ((!found) && (i < self->CmdCount)); ++i)
  {
    if ((self->Cmds[i].Callback != NULL) && (strncmp(self->CmdLine, self->Cmds[i].Name, EHSH_CMDLINE_SIZE) == 0))
    {
      found = true;
      self->Cmds[i].Callback(self);
    }
  }

  return found;
}

// TODO: Multiple spaces
static void eh_Tokenize(eh_Shell* self)
{
  self->Nul      = '\0';
  self->ArgCount = 0;

  char* delim = &self->CmdLine[0];
  while ((delim != NULL) && (self->ArgCount < EHSH_MAX_ARGS))
  {
    delim = strchr(delim, ' ');
    if (delim != NULL)
    {
      size_t index = delim - &self->CmdLine[0];
      self->CmdLine[index] = '\0';
      self->ArgIdx[self->ArgCount] = index + 1;
      ++self->ArgCount;
      ++delim;
    }
  }
}
