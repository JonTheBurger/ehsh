/** @file
 * SPDX-License-Identifier: BSL-1.0
 *
 * Provides a set of basic example commands that projects can optionally choose
 * to incorporate.
 *
 * @addtogroup commands
 * Example commands commonly implemented by projects.
 * @{
 */
#ifndef EHSH_CMD_H
#define EHSH_CMD_H
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <stdbool.h>  // true
#include <string.h>   // strnlen

// local
#include <ehsh/ehsh.h>

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
/** Prints all available commands with their help strings.
 *
 * @param shell Shell containing commands to display.
 *
 * @code{.sh}
 * > help
 * echo: Prints arguments
 * exit: Exits
 * help: Prints commands
 * >
 * @endcode
 */
static inline void EhHelp(EhShell_t* shell)
{
  const char* arg = EhArgAt(shell, 0);
  if (arg == NULL)
  {
    arg = "";
  }
  size_t prefix = strnlen(arg, EHSH_CMDLINE_SIZE);

  for (size_t i = 0; i < shell->CmdCount; ++i)
  {
    if (strncmp(arg, shell->Cmds[i].Name, prefix) == 0)
    {
      EhPutStr(shell, shell->Cmds[i].Name);
      EhPutStr(shell, ": ");
      EhPutStr(shell, shell->Cmds[i].Help);
      EhPutNewline(shell);
    }
  }
}

/** Stops the shell loop from executing.
 *
 * @param shell Shell to stop.
 *
 * @code{.sh}
 * # Shell loop will now end:
 * > exit
 * @endcode
 */
static inline void EhExit(EhShell_t* shell)
{
  shell->Stop = true;
}

/** Prints the arguments passed in.
 *
 * @param shell Shell used to parse & print arguments.
 *
 * @code{.sh}
 * # Each argument is echoed on a new line:
 * > echo a b c
 * a
 * b
 * c
 * >
 * @endcode
 */
static inline void EhEcho(EhShell_t* shell)
{
  for (size_t i = 0; i < shell->ArgCount; ++i)
  {
    const char* arg = EhArgAt(shell, i);
    EhPutStr(shell, arg);
    EhPutNewline(shell);
  }
}

/** Does nothing.
 *
 * @param shell Ignored.
 *
 * @code{.sh}
 * # Comments start with a hash and a space. They do nothing:
 * > # This is a comment
 * # Comments only have an effect at the start of a line
 * > echo #
 * #
 * @endcode
 */
static inline void EhComment(EhShell_t* shell)
{
  (void)shell;
}

/** Controls shell options.
 *
 * @param shell Shell whose options shall be get or set.
 *
 * @code{.sh}
 * # Shell options are a single letter each:
 * #   c: Print CR line endings.
 * #   l: Print LF line endings (enable with c for CR/LF).
 * #   r: When set, input newlines end with CR, else ends with LF.
 * #   t: Tty mode (echo typed characters & support tab completion).
 * # Pass no arguments to print the current shell options:
 * > stty
 * +clt
 * -r
 * # Pass arguments starting with + or - to set shell options.
 * #   Multiple characters can be given:
 * > stty +l
 * > stty -lt
 * > stty +cr +l -t
 * # The + can be omitted. The following line also enables:
 * > stty clrt
 * @endcode
 */
static inline void EhStty(EhShell_t* shell)
{
  if (shell->ArgCount == 0)
  {
    // Get options
    EhPutChar(shell, '+');
    if (shell->Cr) { EhPutChar(shell, 'c'); }
    if (shell->Lf) { EhPutChar(shell, 'l'); }
    if (shell->Eol) { EhPutChar(shell, 'r'); }
    if (shell->Tty) { EhPutChar(shell, 't'); }
    EhPutNewline(shell);

    EhPutChar(shell, '-');
    if (!shell->Cr) { EhPutChar(shell, 'c'); }
    if (!shell->Lf) { EhPutChar(shell, 'l'); }
    if (!shell->Eol) { EhPutChar(shell, 'r'); }
    if (!shell->Tty) { EhPutChar(shell, 't'); }
    EhPutNewline(shell);
  }
  else
  {
    // Set options
    for (size_t i = 0; i < shell->ArgCount; ++i)
    {
      const char*  arg    = EhArgAt(shell, i);
      const size_t length = strnlen(arg, EHSH_CMDLINE_SIZE);
      const bool   value  = arg[0] != '-';

      for (size_t j = 0; j < length; ++j)
      {
        switch (arg[j])
        {
          case 'c':
            shell->Cr = value;
            break;
          case 'l':
            shell->Lf = value;
            break;
          case 'r':
            shell->Eol = value;
            break;
          case 't':
            shell->Tty = value;
            break;
          default:
            break;
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// $Macros
////////////////////////////////////////////////////////////////////////////////
#define EHSH_HELP_HELP "Prints commands"
#define EHSH_HELP_ECHO "Prints arguments"
#define EHSH_HELP_COMMENT "Comment"
#define EHSH_HELP_STTY "Configure shell EOL, TTY"
#define EHSH_HELP_EXIT "Quits the shell"

#define EHSH_COMMAND_HELP            \
  {                                  \
    "help", EHSH_HELP_HELP, &EhHelp, \
  }
#define EHSH_COMMAND_ECHO            \
  {                                  \
    "echo", EHSH_HELP_ECHO, &EhEcho, \
  }
#define EHSH_COMMAND_COMMENT            \
  {                                     \
    "#", EHSH_HELP_COMMENT, &EhComment, \
  }
#define EHSH_COMMAND_STTY            \
  {                                  \
    "stty", EHSH_HELP_STTY, &EhStty, \
  }
#define EHSH_COMMAND_EXIT            \
  {                                  \
    "exit", EHSH_HELP_EXIT, &EhExit, \
  }

/** @} */
#endif /* EHSH_CMD_H */
