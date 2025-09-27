#include <gmock/gmock.h>
#include "godzilla/Ref.h"

using namespace godzilla;

TEST(RefTest, test_ref)
{
    int a = 1, b = 2;
    std::vector<Ref<int>> int_refs;

    int_refs.push_back(ref(a));
    int_refs.push_back(ref(b));

    for (auto & r : int_refs)
        r.get() += 10;

    EXPECT_EQ(int_refs[0], 11);
    EXPECT_EQ(int_refs[1], 12);

    std::vector<Ref<int>> copy;
    for (auto & a : int_refs)
        copy.push_back(a);
    EXPECT_EQ(copy[0], 11);
    EXPECT_EQ(copy[1], 12);
}

TEST(RefTest, test_cref)
{
    int a = 1, b = 2;
    std::vector<Ref<const int>> int_refs;

    int_refs.push_back(cref(a));
    int_refs.push_back(cref(b));

    std::vector<int> numbers;
    for (auto & r : int_refs)
        numbers.push_back(r.get());

    EXPECT_EQ(numbers[0], 1);
    EXPECT_EQ(numbers[1], 2);

    std::vector<Ref<const int>> copy;
    for (auto & a : int_refs)
        copy.push_back(a);
    EXPECT_EQ(copy[0], 1);
    EXPECT_EQ(copy[1], 2);
}
