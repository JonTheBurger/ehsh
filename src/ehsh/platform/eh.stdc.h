/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <stdio.h>

// local
#include <ehsh/ehsh.h>

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
// TODO: this is stupid, fix this shit
void eh_Init(eh_Shell* self, const eh_Command* commands, uint8_t count)
{
  self->Cmds     = commands;
  self->CmdCount = count;
  self->Cursor   = 0;
}

char eh_GetChar(eh_Shell* self)
{
  (void)self;
  return getchar();
}

void eh_PutChar(eh_Shell* self, char c)
{
  (void)self;
  putchar(c);
}

void eh_PutStr(eh_Shell* self, const char* str)
{
  (void)self;
  printf("%s", str);
}

