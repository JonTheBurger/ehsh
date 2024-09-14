/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef EHSH_LINUX_H
#define EHSH_LINUX_H
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <assert.h>
#include <stdlib.h>
#include <string.h>

// 3rd
#include <termios.h>
#include <unistd.h>

// local
#include <ehsh/ehsh.h>

////////////////////////////////////////////////////////////////////////////////
// $Types
////////////////////////////////////////////////////////////////////////////////
struct EhPlatform {
  struct termios LastTermConfig; ///< Previous configuration of the terminal
};

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
void EhPlatformInit(EhPlatform_t** platform)
{
  assert(platform != NULL);

  *platform = malloc(sizeof(EhPlatform_t));
  assert(*platform != NULL);
  // https://man7.org/linux/man-pages/man3/termios.3.html
  // "stty -a"
  tcgetattr(STDIN_FILENO, &(*platform)->LastTermConfig);
  struct termios termiosv = (*platform)->LastTermConfig;
  cfmakeraw(&termiosv);
  if (!self->Cr)
  {
    // This should almost certainly be true, but this prevents false from breaking everything
    termiosv.c_iflag |= ICRNL;
    // termiosv.c_iflag |= INLCR;
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &termiosv);
}

void EhPlatformDeInit(EhPlatform_t** platform)
{
  assert(platform != NULL);
  tcsetattr(STDIN_FILENO, TCSANOW, &(*platform)->LastTermConfig);
  free(*platform);
}

char EhGetChar(EhShell_t* self)
{
  (void)self;
  char c = EHSH_ASCII_EOT;
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

#endif /* EHSH_LINUX_H */
