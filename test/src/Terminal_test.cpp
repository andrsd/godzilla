#include "gtest/gtest.h"
#include "godzilla/Terminal.h"

using namespace godzilla;

TEST(TerminalTest, colors)
{
    Terminal::set_colors(true);
    EXPECT_TRUE(Terminal::has_colors());

    Terminal::set_colors(false);
    EXPECT_FALSE(Terminal::has_colors());
}

TEST(TerminalTest, ostream_operator_w_colors_color)
{
    Terminal::set_colors(true);
    std::ostringstream oss;
    oss << Terminal::red;
    EXPECT_STREQ(oss.str().c_str(), Terminal::red);
}

TEST(TerminalTest, ostream_operator_wo_colors_color)
{
    Terminal::set_colors(false);
    std::ostringstream oss;
    oss << Terminal::red;
    EXPECT_STREQ(oss.str().c_str(), "");
}

TEST(TerminalTest, ostream_operator_w_colors_code)
{
    Terminal::set_colors(true);
    std::ostringstream oss;
    oss << Terminal::erase_line;
    EXPECT_STREQ(oss.str().c_str(), Terminal::erase_line);
}

TEST(TerminalTest, ostream_operator_wo_colors_code)
{
    Terminal::set_colors(false);
    std::ostringstream oss;
    oss << Terminal::erase_line;
    EXPECT_STREQ(oss.str().c_str(), Terminal::erase_line);
}

TEST(TerminalTest, string_operator)
{
    String r = Terminal::red;
    EXPECT_EQ(r, "\33[31m");
}

TEST(TerminalTest, fmt_formatter)
{
    String s = fmt::format("{}", Terminal::red);
    EXPECT_STREQ(s.c_str(), Terminal::red);
}

TEST(TerminalTest, color_codes)
{
    EXPECT_STREQ(Terminal::black, "\33[30m");
    EXPECT_STREQ(Terminal::red, "\33[31m");
    EXPECT_STREQ(Terminal::green, "\33[32m");
    EXPECT_STREQ(Terminal::yellow, "\33[33m");
    EXPECT_STREQ(Terminal::blue, "\33[34m");
    EXPECT_STREQ(Terminal::magenta, "\33[35m");
    EXPECT_STREQ(Terminal::cyan, "\33[36m");
    EXPECT_STREQ(Terminal::white, "\33[37m");
    EXPECT_STREQ(Terminal::normal, "\33[39m");
}

TEST(TerminalTest, codes)
{
    EXPECT_STREQ(Terminal::erase_screen, "\033[2J");
    EXPECT_STREQ(Terminal::erase_line, "\033[2K");
    EXPECT_STREQ(Terminal::erase_ln_to_cursor, "\033[1K");
    EXPECT_STREQ(Terminal::erase_ln_from_cursor, "\033[0K");
}
