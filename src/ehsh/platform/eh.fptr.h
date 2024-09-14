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
// $Macros
////////////////////////////////////////////////////////////////////////////////
#if defined(__GNUC__) || defined(__clang__)
#define EHSH_WEAK __attribute__((weak))
#elif defined(__ICCARM__)
#define EHSH_WEAK __weak
#else
#define EHSH_WEAK
#endif

////////////////////////////////////////////////////////////////////////////////
// $Globals
////////////////////////////////////////////////////////////////////////////////
EHSH_WEAK char (*EhGetCharFn)(EhShell_t* self)               = NULL;
EHSH_WEAK void (*EhPutCharFn)(EhShell_t* self, char c)       = NULL;
EHSH_WEAK void (*EhPutStrFn)(EhShell_t* self, const char* c) = NULL;

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
  if (EhPutStrFn != NULL)
  {
    EhPutStrFn(self, str);
  }
  else if (EhPutCharFn != NULL)
  {
    for (size_t i = 0; str[i] != '\0'; ++i)
    {
      EhPutCharFn(self, str[i]);
    }
  }
}
