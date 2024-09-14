/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef EHSH_WIN32_H
#define EHSH_WIN32_H
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 3rd
#include <Windows.h>

// local
#include <ehsh/ehsh.h>

////////////////////////////////////////////////////////////////////////////////
// $Types
////////////////////////////////////////////////////////////////////////////////
struct EhPlatform {
  DWORD LastConsoleMode; ///< Previous configuration of the console
};

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
void EhPlatformInit(EhPlatform_t** platform)
{
  assert(platform != NULL);

  *platform = malloc(sizeof(EhPlatform_t));
  assert(*platform != NULL);

  HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
  assert(hstdin != INVALID_HANDLE_VALUE);

  BOOL ok = GetConsoleMode(hstdin, &(*platform)->LastConsoleMode);
  assert(ok != 0);

  ok = SetConsoleMode(hstdin, ENABLE_PROCESSED_INPUT | ENABLE_EXTENDED_FLAGS);
  assert(ok != 0);
}

void EhPlatformDeInit(EhPlatform_t** platform)
{
  assert(platform != NULL);
  assert(*platform != NULL);

  HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
  assert(hstdin != INVALID_HANDLE_VALUE);

  BOOL ok = SetConsoleMode(hstdin, (*platform)->LastConsoleMode);
  assert(ok != 0);
}

char EhGetChar(EhShell_t* self)
{
  (void)self;
  char c = '\0';

  HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
  assert(hstdin != INVALID_HANDLE_VALUE);

  INPUT_RECORD record;
  memset(&record, 0, sizeof(record));
  DWORD count = 0;

  while (c == '\0')
  {
    BOOL ok = ReadConsoleInput(hstdin, &record, 1, &count);
    if ((ok) && (count == 1) && (record.EventType == KEY_EVENT) && (record.Event.KeyEvent.bKeyDown))
    {
      c = record.Event.KeyEvent.uChar.AsciiChar;
    }
  }

  return c;
}

void EhPutChar(EhShell_t* self, char c)
{
  if ((self->Cursor == 0) && (c == EHSH_ASCII_BS))
  {
    printf("lmao");
  }
  else
  {
    putchar(c);
  }
}

void EhPutStr(EhShell_t* self, const char* str)
{
  (void)self;
  printf("%s", str);
}

#endif /* EHSH_WIN32_H */
