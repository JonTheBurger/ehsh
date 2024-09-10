/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <assert.h>
#include <stdint.h>
#include <string.h>

// 3rd
#include <termios.h>
#include <unistd.h>

// local
#include <ehsh/ehsh.h>

////////////////////////////////////////////////////////////////////////////////
// $Types
////////////////////////////////////////////////////////////////////////////////
typedef struct EhPlatformContext {
  struct termios LastTermConfig; ///< Previous configuration of the terminal
} EhPlatformContext_t;

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
void EhPlatformInit(EhShell_t* self, void* context)
{
  assert(context != NULL);
  self->Context = context;
  EhPlatformContext_t* ctx = (EhPlatformContext_t*)(self->Context);
  // https://man7.org/linux/man-pages/man3/termios.3.html
  // "stty -a"
  tcgetattr(STDIN_FILENO, &ctx->LastTermConfig);
  struct termios termiosv = ctx->LastTermConfig;
  cfmakeraw(&termiosv);
  if (!self->Cr)
  {
    // This should almost certainly be true, but this prevents false from breaking everything
    termiosv.c_iflag |= ICRNL;
    // termiosv.c_iflag |= INLCR;
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &termiosv);
}

void EhPlatformDeInit(EhShell_t* self)
{
  assert(self->Context != NULL);
  const EhPlatformContext_t* context = (EhPlatformContext_t*)(self->Context);
  tcsetattr(STDIN_FILENO, TCSANOW, &context->LastTermConfig);
}

char EhGetChar(EhShell_t* self)
{
  (void)self;
  char c = 0;
  if (read(STDIN_FILENO, &c, 1) == 0)
  {
    c = '\0';
  }
  return c;
}

void EhPutChar(EhShell_t* self, char c)
{
  (void)self;
  write(STDOUT_FILENO, &c, 1);
}

void EhPutStr(EhShell_t* self, const char* str)
{
  (void)self;
  write(STDOUT_FILENO, str, strnlen(str, 0xFF));
}
