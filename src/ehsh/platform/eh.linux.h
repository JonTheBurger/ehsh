/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <string.h>

// 3rd
#include <termios.h>
#include <unistd.h>

// local
#include <ehsh/ehsh.h>

////////////////////////////////////////////////////////////////////////////////
// $Globals
////////////////////////////////////////////////////////////////////////////////
static struct termios gTermIO;

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
void eh_Init(eh_Shell* self, const eh_Command* commands, uint8_t count)
{
  self->Cmds     = commands;
  self->CmdCount = count;
  self->Cursor   = 0;

  // https://man7.org/linux/man-pages/man3/termios.3.html
  // "stty -a"
  tcgetattr(STDIN_FILENO, &gTermIO);
  struct termios termiosv = gTermIO;
  cfmakeraw(&termiosv);
  if (!self->Cr)
  {
    // This should almost certainly be true, but this prevents false from breaking everything
    termiosv.c_iflag |= ICRNL;
    // termiosv.c_iflag |= INLCR;
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &termiosv);
}

char eh_GetChar(eh_Shell* self)
{
  char c;
  if (read(STDIN_FILENO, &c, 1) == 0)
  {
    c = '\0';
  }
  return c;
}

void eh_PutChar(eh_Shell* self, char c)
{
  write(STDOUT_FILENO, &c, 1);
}

void eh_PutStr(eh_Shell* self, const char* str)
{
  write(STDOUT_FILENO, str, strnlen(str, 0xFF));
}

