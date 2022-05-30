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
