#include "gmock/gmock.h"
#include "godzilla/Types.h"
#include "godzilla/Formatters.h"
#include <fmt/format.h>

using namespace godzilla;

TEST(FormattersTest, insert_mode)
{
    EXPECT_EQ(fmt::format("{}", NOT_SET_VALUES), "NOT_SET_VALUES");
    EXPECT_EQ(fmt::format("{}", INSERT_VALUES), "INSERT_VALUES");
    EXPECT_EQ(fmt::format("{}", ADD_VALUES), "ADD_VALUES");
    EXPECT_EQ(fmt::format("{}", MAX_VALUES), "MAX_VALUES");
    EXPECT_EQ(fmt::format("{}", MIN_VALUES), "MIN_VALUES");
    EXPECT_EQ(fmt::format("{}", INSERT_ALL_VALUES), "INSERT_ALL_VALUES");
    EXPECT_EQ(fmt::format("{}", ADD_ALL_VALUES), "ADD_ALL_VALUES");
    EXPECT_EQ(fmt::format("{}", INSERT_BC_VALUES), "INSERT_BC_VALUES");
    EXPECT_EQ(fmt::format("{}", ADD_BC_VALUES), "ADD_BC_VALUES");
}

TEST(FormattersTest, field_id)
{
    FieldID fid(123);
    EXPECT_EQ(fmt::format("{}", fid), "123");
}
