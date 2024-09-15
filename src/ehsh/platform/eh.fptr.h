/** @file
 * SPDX-License-Identifier: BSL-1.0
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
EHSH_WEAK char (*EhGetCharFn)(EhShell_t* self)                 = NULL;
EHSH_WEAK void (*EhPutCharFn)(EhShell_t* self, char chr)       = NULL;
EHSH_WEAK void (*EhPutStrFn)(EhShell_t* self, const char* chr) = NULL;

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
  char chr = EHSH_ASCII_EOT;

  if (EhGetCharFn != NULL)
  {
    chr = EhGetCharFn(self);
  }

  return chr;
}

EHSH_WEAK void EhPutChar(EhShell_t* self, char chr)
{
  if (EhPutCharFn != NULL)
  {
    EhPutCharFn(self, chr);
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
