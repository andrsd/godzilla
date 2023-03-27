#include "gtest/gtest.h"
#include "Terminal.h"

using namespace godzilla;

TEST(TerminalTest, colors)
{
    unsigned int nc = Terminal::num_colors;

    Terminal::num_colors = 256;
    EXPECT_EQ(Terminal::has_colors(), true);

    Terminal::num_colors = 1;
    EXPECT_EQ(Terminal::has_colors(), false);

    Terminal::Color blk("\33[30m");
    EXPECT_EQ(blk.str, "");

    // restore
    Terminal::num_colors = nc;
}

TEST(TerminalTest, ostream_operator)
{
    std::ostringstream oss;
    oss << Terminal::Color::red;
    EXPECT_EQ(oss.str(), Terminal::Color::red.str);
}

TEST(TerminalTest, fmt_formatter)
{
    std::string s = fmt::format("{}", Terminal::Color::red);
    EXPECT_EQ(s, Terminal::Color::red.str);
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
