/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef EHSH_STDC_H
#define EHSH_STDC_H
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
void EhPlatformInit(EhPlatform_t** platform)
{
  (void)platform;
}

void EhPlatformDeInit(EhPlatform_t** platform)
{
  (void)platform;
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

#endif /* EHSH_STDC_H */
