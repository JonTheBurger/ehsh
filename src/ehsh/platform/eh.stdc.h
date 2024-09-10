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
// $Types
////////////////////////////////////////////////////////////////////////////////
typedef struct EhPlatformContext {
  char Placeholder;
} EhPlatformContext_t;

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
void EhPlatformInit(EhShell_t* self, void* context)
{
  (void)self;
  (void)context;
}

void EhPlatformDeInit(EhShell_t* self)
{
  (void)self;
}

char EhGetChar(EhShell_t* self)
{
  (void)self;
  return (char)getchar();
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
