#include "gmock/gmock.h"
#include "godzilla/HashMap.h"

using namespace godzilla;
using namespace testing;

TEST(HashMap, reset)
{
    HashMap<Int, Int> h;
    h.create();
    h.set(1) = 1234;
    h.reset();
    EXPECT_EQ(h.get_size(), 0);
    h.destroy();
}

TEST(HashMap, clear)
{
    HashMap<Int, Int> h;
    h.create();
    for (Int i = 0; i < 13; i++)
        h.set(i + 1) = 100 + i;
    h.clear();
    EXPECT_EQ(h.get_size(), 0);
    h.destroy();
}

TEST(HashMap, resize)
{
    HashMap<Int, Int> h;
    h.create();
    h.resize(10);
    EXPECT_EQ(h.get_capacity(), 16);
    EXPECT_EQ(h.get_size(), 0);
    h.destroy();
}

TEST(HashMap, get_size)
{
    HashMap<Int, Int> h;
    h.create();
    for (Int i = 0; i < 13; i++)
        h.set(i + 1) = 100 + i;
    EXPECT_EQ(h.get_size(), 13);
    h.destroy();
}

TEST(HashMap, get_capacity)
{
    HashMap<Int, Int> h;
    h.create();
    EXPECT_EQ(h.get_capacity(), 0);
    h.destroy();
}

TEST(HashMap, has)
{
    HashMap<Int, Int> h;
    h.create();
    for (Int i = 0; i < 5; i++)
        h.set(i + 1) = 100 + i;
    for (Int i = 0; i < 5; i++)
        EXPECT_TRUE(h.has(i + 1));
    h.destroy();
}

TEST(HashMap, get_set)
{
    HashMap<Int, Int> h;
    h.create();
    for (Int i = 0; i < 5; i++)
        h.set(i + 1) = 100 + i;
    for (Int i = 0; i < 5; i++)
        EXPECT_EQ(h.get(i + 1), 100 + i);
    h.destroy();
}

TEST(HashMap, del)
{
    HashMap<Int, Int> h;
    h.create();
    for (Int i = 0; i < 5; i++)
        h.set(i + 1) = 100 + i;
    h.del(3);
    EXPECT_FALSE(h.has(3));
    h.destroy();
}

TEST(HashMap, query_set)
{
    HashMap<Int, Int> h;
    h.create();
    for (Int i = 0; i < 5; i++)
        EXPECT_TRUE(h.query_set(i + 1, 100 + i));
    for (Int i = 0; i < 5; i++)
        EXPECT_FALSE(h.query_set(i + 1, 100 + i));
    h.destroy();
}

TEST(HashMap, query_del)
{
    HashMap<Int, Int> h;
    h.create();
    for (Int i = 0; i < 5; i++)
        h.set(i + 1) = 100 + i;
    for (Int i = 0; i < 5; i++)
        EXPECT_TRUE(h.query_del(i + 1));
    for (Int i = 0; i < 5; i++)
        EXPECT_FALSE(h.query_del(i + 1));
    h.destroy();
}

TEST(HashMap, find)
{
    HashMap<Int, Int> h;
    h.create();
    for (Int i = 0; i < 5; i++)
        h.set(i + 1) = 100 + i;
    EXPECT_FALSE(h.find(1) == h.end());
    EXPECT_FALSE(h.find(2) == h.end());
    EXPECT_FALSE(h.find(3) == h.end());
    EXPECT_FALSE(h.find(4) == h.end());
    EXPECT_FALSE(h.find(5) == h.end());
    EXPECT_TRUE(h.find(123) == h.end());
    h.destroy();
}

TEST(HashMap, put)
{
    HashMap<Int, Int> h;
    h.create();
    for (Int i = 0; i < 5; i++)
        h.put(i + 1);

    EXPECT_TRUE(h.has(1));
    EXPECT_TRUE(h.has(2));
    EXPECT_TRUE(h.has(3));
    EXPECT_TRUE(h.has(4));
    EXPECT_TRUE(h.has(5));
}

TEST(HashMap, get_keys)
{
    HashMap<Int, Int> h;
    h.create();
    for (Int i = 0; i < 5; i++)
        h.set(i + 1) = 100 + i;

    std::vector<Int> keys = h.get_keys();
    EXPECT_THAT(keys, UnorderedElementsAre(1, 2, 3, 4, 5));
}

TEST(HashMap, get_vals)
{
    HashMap<Int, Int> h;
    h.create();
    for (Int i = 0; i < 5; i++)
        h.set(i + 1) = 100 + i;

    std::vector<Int> keys = h.get_vals();
    EXPECT_THAT(keys, UnorderedElementsAre(100, 101, 102, 103, 104));
}

TEST(HashMap, uint64_64)
{
    Hash64_t key(10);
    EXPECT_EQ(Hash_UInt64_64(key), 10802492104133333621UL);
}

TEST(HashMap, uint32_v1)
{
    Hash64_t key(10);
    EXPECT_EQ(Hash_UInt32_v1(key), 3429890023);
}
