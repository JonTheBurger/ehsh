/** @file
 * SPDX-License-Identifier: BSL-1.0
 */
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <assert.h>   // assert
#include <stdbool.h>  // true
#include <stdlib.h>   // NULL
#include <string.h>   // memset
#include <stdio.h>

// local
#include <ehsh/ehsh.h>
#include <ehsh/extra/ehcmd.h>
#include <ehsh/platform/eh.fptr.h>

////////////////////////////////////////////////////////////////////////////////
// $Defines
////////////////////////////////////////////////////////////////////////////////
#define TEST_EOT       "\x04"
#define TEST_BACKSPACE "\x08"
#define TEST_BUF_SIZE  128

////////////////////////////////////////////////////////////////////////////////
// $Globals
////////////////////////////////////////////////////////////////////////////////
static uint8_t GetCursor = 0;
static uint8_t PutCursor = 0;
static char    GetChars[TEST_BUF_SIZE];
static char    PutChars[TEST_BUF_SIZE];

////////////////////////////////////////////////////////////////////////////////
// $Prototypes
////////////////////////////////////////////////////////////////////////////////
static char GetCharHook(EhShell_t* self);
static void PutCharHook(EhShell_t* self, char c);
static void Setup(void);

static void Test2(void);

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
/// This should maybe use a real test framework, but this makes it easy for
/// users to run tests.
int main()
{
  void (*const tests[])(void) = {
    Test2,
  };

  for (size_t i = 0; i < sizeof(tests) / sizeof(*tests); ++i)
  {
    Setup();
    tests[i]();
  }

  return 0;
}

static void Setup(void)
{
  EhGetCharFn = &GetCharHook;
  EhPutCharFn = &PutCharHook;
  GetCursor   = 0;
  PutCursor   = 0;
  memset(&GetChars[0], 0, sizeof(GetChars));
  memset(&PutChars[0], 0, sizeof(PutChars));
}

static char GetCharHook(EhShell_t* self)
{
  assert(self != NULL);
  assert(GetCursor < (sizeof(GetChars) / sizeof(*GetChars)));
  char c = GetChars[GetCursor];
  ++GetCursor;
  return c;
}

static void PutCharHook(EhShell_t* self, char c)
{
  assert(self != NULL);
  assert(PutCursor < (sizeof(PutChars) / sizeof(*PutChars)));
  PutChars[PutCursor] = c;
  ++PutCursor;
}

static void Test2(void)
{
  const EhCommand_t cmds[] = {
    EHSH_COMMAND_HELP,
    EHSH_COMMAND_ECHO,
    EHSH_COMMAND_EXIT,
  };
  EhShell_t shell;
  EhInit(
    &shell,
    &(EhConfig_t){
      .Commands     = cmds,
      .CommandCount = sizeof(cmds) / sizeof(*cmds),
      .Eol          = EHSH_EOL_LF,
      .Tty          = false,
      .Cr           = true,
      .Lf           = true,
    });

  strcpy(&GetChars[0], "echo a b c\r\n" TEST_EOT);

  EhExec(&shell);

  assert(strcmp(&PutChars[0], "a\r\nb\r\nc\r\n") == 0);

  EhDeInit(&shell);
}
