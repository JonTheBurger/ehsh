/** @file
 * SPDX-License-Identifier: BSL-1.0
 */
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <cstring>  // memset
#include <string>   // std::string

// 3rd
#include <gtest/gtest.h>

// local
#include <ehsh/ehsh.h>
#include <ehsh/extra/ehcmd.h>
#include <ehsh/platform/eh.fptr.h>

////////////////////////////////////////////////////////////////////////////////
// $Globals
////////////////////////////////////////////////////////////////////////////////
static constexpr size_t  TEST_BUF_SIZE      = 128;
const static EhCommand_t BUILTIN_COMMANDS[] = {
  EHSH_COMMAND_COMMENT,
  EHSH_COMMAND_HELP,
  EHSH_COMMAND_ECHO,
  EHSH_COMMAND_STTY,
  EHSH_COMMAND_EXIT,
};

////////////////////////////////////////////////////////////////////////////////
// $Functions
////////////////////////////////////////////////////////////////////////////////
class GivenShell : public testing::Test {
public:
  GivenShell() noexcept
  {
    EhGetCharFn = &GetCharHook;
    EhPutCharFn = &PutCharHook;

    const EhConfig_t cfg = {
      .Commands     = &BUILTIN_COMMANDS[0],
      .CommandCount = std::size(BUILTIN_COMMANDS),
    };
    EhInit(&Shell, &cfg);
    Shell.Context = this;
  }

  ~GivenShell() noexcept override
  {
    EhDeInit(&Shell);
  }

  static char GetCharHook(EhShell_t* shell)
  {
    auto& self = *static_cast<GivenShell*>(shell->Context);
    char  chr  = self.Input.front();

    self.Input.erase(0, 1);

    return chr;
  }

  static void PutCharHook(EhShell_t* shell, char chr)
  {
    auto& self = *static_cast<GivenShell*>(shell->Context);

    if ((chr == EHSH_ASCII_BS) || (chr == EHSH_ASCII_DEL))
    {
      self.Screen.pop_back();
    }
    else
    {
      self.Screen.push_back(chr);
    }

    self.Output.push_back(chr);
  }

protected:
  EhShell_t   Shell{};
  std::string Input{};   //< Simulated input stream, say typed from a keyboard
  std::string Screen{};  //< Simulated screen output; backspaces remove chars
  std::string Output{};  //< Raw simulated output stream; backspaces are appended as chars
};

class GivenTtyCrLfShell : public GivenShell {
public:
  GivenTtyCrLfShell() noexcept
  {
    Shell.Eol = EHSH_EOL_LF;
    Shell.Tty = true;
    Shell.Cr  = true;
    Shell.Lf  = true;
  }
};

class GivenLfShell : public GivenShell {
public:
  GivenLfShell() noexcept
  {
    Shell.Eol = EHSH_EOL_LF;
    Shell.Lf  = true;
  }
};

class GivenCrShell : public GivenShell {
public:
  GivenCrShell() noexcept
  {
    Shell.Eol = EHSH_EOL_CR;
    Shell.Cr  = true;
    Shell.Tty = false;
  }
};

class GivenTtyCrShell : public GivenShell {
public:
  GivenTtyCrShell() noexcept
  {
    Shell.Eol = EHSH_EOL_CR;
    Shell.Cr  = true;
    Shell.Tty = true;
  }
};

TEST(GivenNoShell, WhenInitedWithNullShell_ThenNullReturned)
{
  EhConfig_t config{};
  ASSERT_EQ(nullptr, EhInit(nullptr, &config));
}

TEST_F(GivenTtyCrLfShell, WhenArgAtBeyondArgCount_ThenNullReturned)
{
  ASSERT_EQ(nullptr, EhArgAt(&Shell, Shell.ArgCount));
}

TEST_F(GivenTtyCrLfShell, WhenInitedWithNullConfig_ThenNoChangesOccur)
{
  const EhShell_t old = Shell;
  ASSERT_EQ(&Shell, EhInit(&Shell, nullptr));
  ASSERT_EQ(0, memcmp(&old, &Shell, sizeof(Shell)));
}

TEST_F(GivenTtyCrLfShell, WhenInvalidCommandEntered_ThenErrorMessagePrinted)
{
  Input = "bogus\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    "> bogus\r\n"
    "No such command \"bogus\"\r\n"
    "> ");
}

TEST_F(GivenTtyCrLfShell, When3EchoArgsEntered_ThenPrintsCrLfPerLine)
{
  Input = "echo a b c\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    "> echo a b c\r\n"
    "a\r\n"
    "b\r\n"
    "c\r\n"
    "> ");
}

TEST_F(GivenCrShell, When3EchoArgsEntered_ThenPrintsOnlyOutput)
{
  Input = "echo a b c\r";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    "a\r"
    "b\r"
    "c\r");
}
TEST_F(GivenTtyCrLfShell, When1CommandMatchesInput_AndTabIsPressed_ThenTabCompletesMatch)
{
  Input = "ec\t";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    "> ec\r\n"
    "echo\r\n"
    "> echo");
}

TEST_F(GivenTtyCrLfShell, WhenMultipleCommandMatchesInput_AndTabIsPressed_ThenTabPrintsMatches)
{
  Input = "e\t";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    "> e\r\n"
    "echo\r\n"
    "exit\r\n"
    "> e");
}

TEST_F(GivenTtyCrLfShell, CommandHasNullName_AndTabIsPressed_ThenNothingPrinted)
{
  const EhCommand_t nullcmd[] = {
    { nullptr, nullptr, nullptr },
  };
  Shell.Cmds     = nullcmd;
  Shell.CmdCount = 1;

  Input = "\t";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(Output, "> ");
}

TEST_F(GivenCrShell, WhenTabIsPressed_ThenNothingPrinted)
{
  Input = "ech\t";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(Output, "");
}

TEST_F(GivenTtyCrLfShell, WhenCharsEnteredBeyondCommandLine_ThenScreenTruncatesUsingLastChar)
{
  Input.resize(EHSH_CMDLINE_SIZE, 'a');  // Fill the command line buffer
  Input += 'b';                          // and add one more character
  Input += static_cast<char>(EHSH_ASCII_EOT);

  std::string expected = "> ";
  expected.insert(expected.end(), EHSH_CMDLINE_SIZE, 'a');
  expected.pop_back();
  expected += 'b';  // The last char should be the most recently entered char

  EhExec(&Shell);

  ASSERT_EQ(expected, Screen);
}

TEST_F(GivenCrShell, WhenCharsEnteredBeyondCommandLine_BufferTruncatesToLastChar)
{
  Input = "echo abcdefghijk!";
  Input += '\r';
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  // Last printable character 'k' is overwritten
  ASSERT_EQ("abcdefghij!\r", Screen);
}

TEST_F(GivenTtyCrLfShell, WhenArgsEnteredBeyondArgMax_ThenTODO)
{
  ASSERT_EQ(EHSH_MAX_ARGS, 4);  // If this is > 4, this test needs updated; add more args
  Input += "echo 1 2 3 4 5\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    // Also need to add more args below:
    "> echo 1 2 3 4 5\r\n"
    "1\r\n"
    "2\r\n"
    "3\r\n"
    "4 5\r\n"
    "> ");
}

TEST_F(GivenTtyCrLfShell, WhenBackspaceSent_ThenSpaceOverwritesLastChar)
{
  Input = "ec" EHSH_BACKSPACE;
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    // Write "Backspace" to move cursor, "<space>" to write blank space, and "Backspace" to move cursor again.
    "> ec" EHSH_BACKSPACE " " EHSH_BACKSPACE);
}

TEST_F(GivenTtyCrLfShell, WhenBackspaceSent_AndCmdLineEmpty_ThenNothingHappens)
{
  Input = EHSH_BACKSPACE EHSH_DELETE EHSH_BACKSPACE;
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    "> ");
}

TEST_F(GivenShell, WhenBackspaceSent_ThenCursorMovesBackwards)
{
  Input = "exit";
  Input += EHSH_BACKSPACE EHSH_DELETE EHSH_BACKSPACE;
  Input += "cho hello\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(Output, "hello");
}

TEST_F(GivenTtyCrLfShell, WhenNullCommandCallbackFound_ThenCommandIgnored)
{
  const EhCommand_t nullcmd[] = {
    { "null", "null", nullptr },
  };
  Shell.Cmds     = nullcmd;
  Shell.CmdCount = 1;

  Input = "null\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    "> null\r\n"
    "> ");
}

TEST_F(GivenTtyCrLfShell, WhenNullCommandNameFound_ThenCommandIgnored)
{
  const EhCommand_t nullcmd[] = {
    { nullptr, nullptr, nullptr },
  };
  Shell.Cmds     = nullcmd;
  Shell.CmdCount = 1;

  Input = "null\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    "> null\r\n"
    "No such command \"null\"\r\n"
    "> ");
}

TEST_F(GivenTtyCrShell, WhenLfEntered_ThenCommandNotHandled)
{
  Input = "echo a\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(Output, "> echo a");
}

TEST_F(GivenTtyCrShell, WhenNegative1Read_ThenNothingHappens)
{
  Input = static_cast<char>(-1);
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(Output, "> ");
}

TEST_F(GivenTtyCrLfShell, WhenExitEntered_ThenShellStops)
{
  Input = "exit\r\n";
  // No EOT!

  EhExec(&Shell);

  ASSERT_EQ(
    Screen,
    "> exit\r\n"
    "> ");
}

TEST_F(GivenTtyCrLfShell, WhenCommentEntered_ThenCommentPrinted)
{
  Input = "# hello\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Screen,
    "> # hello\r\n"
    "> ");
}

TEST_F(GivenTtyCrLfShell, WhenHelpEntered_ThenCommandsPrinted)
{
  Input = "help\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Screen,
    "> help\r\n"
    "#: Comment\r\n"
    "help: Prints commands\r\n"
    "echo: Prints arguments\r\n"
    "stty: Configure shell EOL, TTY\r\n"
    "exit: Quits the shell\r\n"
    "> ");
}

TEST_F(GivenTtyCrLfShell, WhenHelpEnteredFor2MatchingCommands_ThenHelpsPrinted)
{
  Input = "help e\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Screen,
    "> help e\r\n"
    "echo: Prints arguments\r\n"
    "exit: Quits the shell\r\n"
    "> ");
}

TEST_F(GivenTtyCrLfShell, WhenSttyEntered_ThenCurrentConfigPrinted)
{
  Input = "stty\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Screen,
    "> stty\r\n"
    "+clt\r\n"
    "-r\r\n"
    "> ");
}

TEST_F(GivenCrShell, WhenSttyEntered_ThenCurrentConfigPrinted)
{
  Input = "stty\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Screen,
    "+cr\r"
    "-lt\r");
}

TEST_F(GivenLfShell, WhenSttyEntered_ThenCurrentConfigPrinted)
{
  Input = "stty\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Screen,
    "+l\n"
    "-crt\n");
}

TEST_F(GivenShell, WhenSttyEntered_AndAllFlagsEnabled_ThenConfigUpdates)
{
  Input = "stty clrt\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_TRUE(Shell.Cr);
  ASSERT_TRUE(Shell.Lf);
  ASSERT_TRUE(Shell.Eol);
  ASSERT_TRUE(Shell.Tty);
}

TEST_F(GivenShell, WhenSttyEnteredWithPlus_AndAllFlagsEnabled_ThenConfigUpdates)
{
  Input = "stty +clrt\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_TRUE(Shell.Cr);
  ASSERT_TRUE(Shell.Lf);
  ASSERT_TRUE(Shell.Eol);
  ASSERT_TRUE(Shell.Tty);
}

TEST_F(GivenTtyCrLfShell, WhenSttyEnteredWithMinus_AndAllFlagsDisabled_ThenConfigUpdates)
{
  Input = "stty -clrt\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_FALSE(Shell.Cr);
  ASSERT_FALSE(Shell.Lf);
  ASSERT_FALSE(Shell.Eol);
  ASSERT_FALSE(Shell.Tty);
}

TEST_F(GivenCrShell, WhenSttyEnteredWithMultipleCommands_ThenConfigUpdates)
{
  Input = "stty +lt -cr\r";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_FALSE(Shell.Cr);
  ASSERT_TRUE(Shell.Lf);
  ASSERT_FALSE(Shell.Eol);
  ASSERT_TRUE(Shell.Tty);
}
