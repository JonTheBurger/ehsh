/**
 * @addtogroup platform
 * Example code for implementing platform-specific ehsh hooks.
 * @{
 *
 * @file
 * SPDX-License-Identifier: BSL-1.0
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

void EhPutChar(EhShell_t* self, char chr)
{
  (void)self;
  putchar(chr);
}

void EhPutStr(EhShell_t* self, const char* str)
{
  (void)self;
  printf("%s", str);
}

#endif /* EHSH_STDC_H */
