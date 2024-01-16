#include "gmock/gmock.h"
#include "godzilla/Utils.h"

using namespace godzilla;

TEST(UtilsTest, to_lower)
{
    EXPECT_EQ(utils::to_lower("ASDF"), "asdf");
}

TEST(UtilsTest, to_upper)
{
    EXPECT_EQ(utils::to_upper("asdf"), "ASDF");
}

TEST(UtilsTest, has_suffix)
{
    EXPECT_TRUE(utils::has_suffix("asdf", "df"));
    EXPECT_FALSE(utils::has_suffix("asdf", "long_string"));
    EXPECT_FALSE(utils::has_suffix("asdf", "as"));
}

TEST(UtilsTest, ends_with)
{
    EXPECT_TRUE(utils::ends_with("asdf", "df"));
    EXPECT_FALSE(utils::ends_with("asdf", "long_string"));
    EXPECT_FALSE(utils::ends_with("asdf", "as"));
}

TEST(UtilsTest, has_prefix)
{
    EXPECT_TRUE(utils::has_prefix("asdf", "as"));
    EXPECT_FALSE(utils::has_prefix("asdf", "long_string"));
    EXPECT_FALSE(utils::has_prefix("asdf", "df"));
}

TEST(UtilsTest, starts_with)
{
    EXPECT_TRUE(utils::starts_with("asdf", "as"));
    EXPECT_FALSE(utils::starts_with("asdf", "long_string"));
    EXPECT_FALSE(utils::starts_with("asdf", "df"));
}

TEST(UtilsTest, map_keys)
{
    std::map<int, int> m = { { 1, 200 }, { 2, 201 }, { 5, 203 }, { 9, 204 } };

    std::vector<int> keys = utils::map_keys(m);
    EXPECT_THAT(keys, testing::UnorderedElementsAre(1, 2, 5, 9));
}

TEST(UtilsTest, map_values)
{
    std::map<int, int> m = { { 1, 200 }, { 2, 201 }, { 5, 203 }, { 9, 204 } };

    std::vector<int> keys = utils::map_values(m);
    EXPECT_THAT(keys, testing::UnorderedElementsAre(200, 201, 203, 204));
}

TEST(UtilsTest, human_time)
{
    EXPECT_EQ(utils::human_time(0), "0s");
    EXPECT_EQ(utils::human_time(0.5), "0.500s");
    EXPECT_EQ(utils::human_time(10), "10s");
    EXPECT_EQ(utils::human_time(60), "1m");
    EXPECT_EQ(utils::human_time(70), "1m 10s");
    EXPECT_EQ(utils::human_time(70.5), "1m 10.500s");
    EXPECT_EQ(utils::human_time(3600), "1h");
    EXPECT_EQ(utils::human_time(3720), "1h 2m");
    EXPECT_EQ(utils::human_time(3725), "1h 2m 5s");
    EXPECT_EQ(utils::human_time(3725.2), "1h 2m 5.200s");
}

TEST(UtilsTest, index_of)
{
    std::vector<int> vals = { 2, 5, 8 };
    EXPECT_EQ(utils::index_of(vals, 2), 0);
    EXPECT_EQ(utils::index_of(vals, 5), 1);
    EXPECT_EQ(utils::index_of(vals, 8), 2);

    EXPECT_THROW({ utils::index_of(vals, 1); }, std::runtime_error);
}
