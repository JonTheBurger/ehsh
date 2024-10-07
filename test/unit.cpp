/** @file
 * SPDX-License-Identifier: BSL-1.0
 */
////////////////////////////////////////////////////////////////////////////////
// $Headers
////////////////////////////////////////////////////////////////////////////////
// std
#include <cstring>   // memset
#include <string>    // std::string

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
  EHSH_COMMAND_HELP,
  EHSH_COMMAND_ECHO,
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
    char chr = self.Input.front();

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
  std::string Input{};  //< Simulated input stream, say typed from a keyboard
  std::string Screen{}; //< Simulated screen output; backspaces remove chars
  std::string Output{}; //< Raw simulated output stream; backspaces are appended as chars
};

class GivenTtyCrLfShell : public GivenShell {
public:
  GivenTtyCrLfShell() noexcept
  {
    Shell.Eol = EHSH_EOL_LF;
    Shell.Tty = true;
    Shell.Cr = true;
    Shell.Lf = true;
  }
};

class GivenCrShell : public GivenShell {
public:
  GivenCrShell() noexcept
  {
    Shell.Eol = EHSH_EOL_CR;
    Shell.Cr = true;
  }
};

TEST_F(GivenTtyCrLfShell, WhenInvalidCommandEntered_ThenErrorMessagePrinted)
{
  Input = "bogus\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    "> bogus\r\n"
    "No such command \"bogus\"\r\n"
    "> "
  );
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
    "> "
  );
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
    "c\r"
  );
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
    "> echo"
  );
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
    "> e"
  );
}

TEST_F(GivenTtyCrLfShell, WhenCharsEnteredBeyondCommandLine_ThenScreenTruncatesUsingLastChar)
{
  Input.resize(EHSH_CMDLINE_SIZE, 'a'); // Fill the command line buffer
  Input += 'b'; // and add one more character
  Input += static_cast<char>(EHSH_ASCII_EOT);

  std::string expected = "> ";
  expected.insert(expected.end(), EHSH_CMDLINE_SIZE, 'a');
  expected.pop_back();
  expected += 'b'; // The last char should be the most recently entered char

  EhExec(&Shell);

  ASSERT_EQ(expected, Screen);
}

TEST_F(GivenTtyCrLfShell, WhenArgsEnteredBeyondArgMax_ThenTODO)
{
  ASSERT_EQ(EHSH_MAX_ARGS, 4); // If this is > 4, this test needs updated; add more args
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
    "> "
  );
}

TEST_F(GivenTtyCrLfShell, WhenBackspaceSent_ThenSpaceOverwritesLastChar)
{
  Input = "ec" EHSH_BACKSPACE;
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    // Write "Backspace" to move cursor, "<space>" to write blank space, and "Backspace" to move cursor again.
    "> ec" EHSH_BACKSPACE " " EHSH_BACKSPACE
  );
}

TEST_F(GivenTtyCrLfShell, WhenBackspaceSent_AndCmdLineEmpty_ThenNothingHappens)
{
  Input = EHSH_BACKSPACE EHSH_DELETE EHSH_BACKSPACE;
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    "> "
  );
}

TEST_F(GivenTtyCrLfShell, WhenNullCommandCallbackFound_ThenCommandIgnored)
{
  const EhCommand_t nullcmd[] = {
    { "null", "null", nullptr },
  };
  Shell.Cmds = nullcmd;
  Shell.CmdCount = 1;

  Input = "null\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    "> null\r\n"
    "> "
  );
}

TEST_F(GivenTtyCrLfShell, WhenNullCommandNameFound_ThenCommandIgnored)
{
  const EhCommand_t nullcmd[] = {
    { nullptr, nullptr, nullptr },
  };
  Shell.Cmds = nullcmd;
  Shell.CmdCount = 1;

  Input = "null\r\n";
  Input += static_cast<char>(EHSH_ASCII_EOT);

  EhExec(&Shell);

  ASSERT_EQ(
    Output,
    "> null\r\n"
    "No such command \"null\"\r\n"
    "> "
  );
}

