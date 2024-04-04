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

TEST(TerminalTest, ostream_operator_w_colors)
{
    Terminal::set_colors(true);
    std::ostringstream oss;
    oss << Terminal::Color::red;
    EXPECT_STREQ(oss.str().c_str(), Terminal::Color::red);
}

TEST(TerminalTest, ostream_operator_wo_colors)
{
    Terminal::set_colors(false);
    std::ostringstream oss;
    oss << Terminal::Color::red;
    EXPECT_STREQ(oss.str().c_str(), "");
}

TEST(TerminalTest, string_operator)
{
    const std::string & r = Terminal::Color::red;
    EXPECT_EQ(r, "\33[31m");
}

TEST(TerminalTest, fmt_formatter)
{
    std::string s = fmt::format("{}", Terminal::Color::red);
    EXPECT_STREQ(s.c_str(), Terminal::Color::red);
}

TEST(TerminalTest, color_codes)
{
    EXPECT_STREQ(Terminal::Color::black, "\33[30m");
    EXPECT_STREQ(Terminal::Color::red, "\33[31m");
    EXPECT_STREQ(Terminal::Color::green, "\33[32m");
    EXPECT_STREQ(Terminal::Color::yellow, "\33[33m");
    EXPECT_STREQ(Terminal::Color::blue, "\33[34m");
    EXPECT_STREQ(Terminal::Color::magenta, "\33[35m");
    EXPECT_STREQ(Terminal::Color::cyan, "\33[36m");
    EXPECT_STREQ(Terminal::Color::white, "\33[37m");
    EXPECT_STREQ(Terminal::Color::normal, "\33[39m");
}
