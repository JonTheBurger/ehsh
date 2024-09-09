/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <stdint.h>
#include <stdio.h>

// local
#include <ehsh/ehsh.h>

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
// TODO: this is stupid, fix this shit
void EhInit(EhShell_t* self, const EhCommand_t* commands, uint8_t count)
{
  self->Cmds     = commands;
  self->CmdCount = count;
  self->Cursor   = 0;
}

char EhGetChar(EhShell_t* self)
{
  (void)self;
  return getchar();
}

void EhPutChar(EhShell_t* self, char c)
{
  (void)self;
  putchar(c);
}

void EhPutStr(EhShell_t* self, const char* str)
{
  (void)self;
  printf("%s", str);
}
