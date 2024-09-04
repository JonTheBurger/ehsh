/* Copyright (c) 2024 Jonathan Povirk (jontheburger at gmail dot com)
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef EHSH_H
#define EHSH_H
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <stdint.h> // uint8_t
#include <stdlib.h> // size_t

////////////////////////////////////////////////////////////////////////////////
// $Macros
////////////////////////////////////////////////////////////////////////////////
#define EHSH_HISTORY_SIZE 10
#define EHSH_CMDLINE_SIZE 16
#define EHSH_MAX_ARGS     4  // Up to 15
#define EHSH_PROPMT       "> "

#define EHSH_EMSG_NOCMD   "No such command"
#define EHSH_EMSG_SYNTAX  "Syntax error"
#define EHSH_EMSG_AUTH    "Bad permissions"
#define EHSH_EMSG_ARG     "Bad argument(s)"
#define EHSH_EMSG_BUSY    "Subsystem busy"

#define EHSH_EOL_LF       0
#define EHSH_EOL_CR       1

#define ESHS_TERMIOS       1

////////////////////////////////////////////////////////////////////////////////
// $Types
////////////////////////////////////////////////////////////////////////////////
typedef struct eh_Shell eh_Shell;
typedef struct eh_Command eh_Command;
typedef void (*eh_Callback)(eh_Shell* shell);

/// Command line command
struct eh_Command {
  /// Name of the command to execute
  const char* Name;
  /// Text description of command for the user
  const char* Help;
  /// Function to call when command is sent
  eh_Callback Callback;
};

/// State & Configuration associated with the current shell
struct eh_Shell {
  /// User context associated with this shell
  void* Context;

  /// Array of registered shell commands, alphabetized
  const eh_Command* Cmds;
  /// Number of registered shell commands
  uint8_t CmdCount;

  /// Position of cursor
  uint8_t Cursor;
  /// Command Line text input by user
  char CmdLine[EHSH_CMDLINE_SIZE]; // TODO: Should this be passed in?
  /// Always set to '\0' for safety
  char Nul;

  /// Indices of tokenized arguments
  uint8_t ArgIdx[EHSH_MAX_ARGS];
  /// Number of parsed argument tokens
  uint8_t ArgCount : 4;

  /// When to process commands for input line endings.
  /// Set to 0 to execute commands on LF (for CR+LF and LF-only line endings)
  /// Set to 1 to execute commands on CR only (for CR-only line endings)
  uint8_t Eol : 1;
  /// Act as a teletype terminal (echo chars as they are typed)
  uint8_t Tty : 1;
  /// Print carriage return upon new line
  uint8_t Cr : 1;
  /// Print line feed upon new line
  uint8_t Lf : 1;

  /// Set to 1 to stop running
  uint8_t Stop : 1;
  /// Set to 1 if this shell was heap allocated; always 0
  uint8_t Heap : 1;
  /// 
  uint8_t State : 3;
  /// Reserved for future versions of ehsh - if you're feeling feisty, use it until it is claimed in the future!
  uint8_t Reserved : 3;
};

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
/** @brief Creates an uninitialized shell.
 *
 * @param self Statically allocated shell.
 * @return Initialized shell, or `NULL` if you passed in `NULL` for some reason.
 */
eh_Shell* eh_Create(eh_Shell* self);

// TODO: This should be a user function
/** @brief Sets up a shell so it's ready for use.
 *
 * @param self The shell to set up; should have been created via eh_Create.
 * @param commands Array of commands that this shell will delegate to; must outlive `self`.
 * @param count The number of commands this shell will handle.
 */
void eh_Init(eh_Shell* self, const eh_Command* commands, uint8_t count);

/** @brief Runs the shell in a loop until eh_Shell.Stop is set to `true`.
 * The shell will always run for at least one iteration (do while).
 *
 * @param self The shell to execute.
 */
void eh_Exec(eh_Shell* self);

/** @brief Prints a newline based on the shell's CR+LF settings.
 *
 * @param self Shell to print to.
 */
void eh_PutNewline(eh_Shell* self);

/** @brief Destroys a previously created shell. */
void eh_Destroy(eh_Shell* self);

/** @brief User-defined function for a blocking character read.
 * See platform/ folder, for example eh.stdc.h.
 *
 * @param self Shell attempting to read a character.
 * @return character being read, or `(char)-1` on error.
 */
char eh_GetChar(eh_Shell* self);

/**
 * @brief User-defined function for character write.
 *
 * @param self Shell attempting to write a character.
 * @param c Character to write.
 */
void eh_PutChar(eh_Shell* self, char c);

/** @brief Writes a null-terminated string to the shell, excluding the null terminator.
 * This is just an optimization around eh_PutChar in a for loop.
 *
 * @param self Shell attempting to write a string.
 * @param str Null-terminated string to send.
 */
void eh_PutStr(eh_Shell* self, const char* str);

////////////////////////////////////////////////////////////////////////////////
// $Implementations
////////////////////////////////////////////////////////////////////////////////
/** @brief Gets the nth argument as a string.
 *
 * @param self Shell to get the current nth argument from.
 * @param index 0-indexed parameter to lookup.
 * @return Null-terminated parameter string to nth parameter, or `NULL` if out of range.
 */
static inline const char* eh_ArgAt(eh_Shell* self, uint8_t index)
{
  const char* arg = NULL;
  if (index < self->ArgCount)
  {
    arg = &self->CmdLine[self->ArgIdx[index]];
  }
  return arg;
}

#endif /* EHSH_H */
