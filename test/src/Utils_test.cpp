#include "gmock/gmock.h"
#include "godzilla/Range.h"
#include "godzilla/Utils.h"

using namespace godzilla;
using namespace testing;

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
    EXPECT_EQ(utils::human_time(0), "0.00s");
    EXPECT_EQ(utils::human_time(0.5), "0.50s");
    EXPECT_EQ(utils::human_time(10), "10.00s");
    EXPECT_EQ(utils::human_time(60), "1m");
    EXPECT_EQ(utils::human_time(70), "1m 10.00s");
    EXPECT_EQ(utils::human_time(70.5), "1m 10.50s");
    EXPECT_EQ(utils::human_time(3600), "1h");
    EXPECT_EQ(utils::human_time(3720), "1h 2m");
    EXPECT_EQ(utils::human_time(3725), "1h 2m 5.00s");
    EXPECT_EQ(utils::human_time(3725.2), "1h 2m 5.20s");
}

TEST(UtilsTest, index_of)
{
    std::vector<int> vals = { 2, 5, 8 };
    EXPECT_EQ(utils::index_of(vals, 2), 0);
    EXPECT_EQ(utils::index_of(vals, 5), 1);
    EXPECT_EQ(utils::index_of(vals, 8), 2);

    EXPECT_THROW({ utils::index_of(vals, 1); }, std::runtime_error);
}

TEST(UtilsTest, enumerate_stdvec)
{
    std::vector<int> v = { 10, 11, 12, 14, 15 };

    std::vector<int> indices;
    std::vector<int> values;
    for (const auto & [i, value] : enumerate(v)) {
        indices.push_back(i);
        values.push_back(value);
    }

    EXPECT_THAT(indices, ElementsAre(0, 1, 2, 3, 4));
    EXPECT_THAT(values, ElementsAre(10, 11, 12, 14, 15));
}

TEST(UtilsTest, enumerate_range)
{
    godzilla::Range r(10, 16);
    std::vector<int> indices;
    std::vector<int> values;
    for (const auto & [i, value] : enumerate(r)) {
        indices.push_back(i);
        values.push_back(value);
    }

    EXPECT_THAT(indices, ElementsAre(0, 1, 2, 3, 4, 5));
    EXPECT_THAT(values, ElementsAre(10, 11, 12, 13, 14, 15));
}

TEST(UtilsTest, enumerate_const_stdvec)
{
    std::vector<int> v = { 10, 11, 12, 14, 15 };
    const auto & cv = v;

    std::vector<int> indices;
    std::vector<int> values;
    for (const auto & [i, value] : enumerate(cv)) {
        indices.push_back(i);
        values.push_back(value);
    }

    EXPECT_THAT(indices, ElementsAre(0, 1, 2, 3, 4));
    EXPECT_THAT(values, ElementsAre(10, 11, 12, 14, 15));
}

TEST(UtilsTest, human_number)
{
    EXPECT_EQ(utils::human_number(1), "1");
    EXPECT_EQ(utils::human_number(999), "999");
    EXPECT_EQ(utils::human_number(1000), "1,000");
    EXPECT_EQ(utils::human_number(999999), "999,999");
    EXPECT_EQ(utils::human_number(1000000), "1,000,000");
    EXPECT_EQ(utils::human_number(1000000000ll), "1,000,000,000");
}

TEST(UtilsTest, join_std_vec_empty)
{
    std::vector<int> vals;
    String s = join(", ", vals);
    EXPECT_EQ(s, "");
}

TEST(UtilsTest, join_std_vec_int)
{
    std::vector<Int> vals = { 1, 3, 5, 7, 9 };
    String s = join(", ", vals);
    EXPECT_EQ(s, "1, 3, 5, 7, 9");
}

TEST(UtilsTest, split_empty)
{
    auto parts = split(" ", "");
    EXPECT_EQ(parts.size(), 0);
}

TEST(UtilsTest, split_one)
{
    auto parts = split(" ", "hello");
    ASSERT_EQ(parts.size(), 1);
    EXPECT_EQ(parts[0], "hello");
}

TEST(UtilsTest, split_multi)
{
    auto parts = split(" ", "apple orange strawberry");
    ASSERT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "apple");
    EXPECT_EQ(parts[1], "orange");
    EXPECT_EQ(parts[2], "strawberry");
}
