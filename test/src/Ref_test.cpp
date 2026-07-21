#include <gmock/gmock.h>
#include "godzilla/Ref.h"
#include "godzilla/Qtr.h"

using namespace godzilla;

TEST(RefTest, test_ref)
{
    int a = 1, b = 2;
    std::vector<Ref<int>> int_refs;

    int_refs.push_back(ref(a));
    int_refs.push_back(ref(b));

    for (auto & r : int_refs)
        r.get() += 10;

    EXPECT_EQ(*int_refs[0], 11);
    EXPECT_EQ(*int_refs[1], 12);

    std::vector<Ref<int>> copy;
    for (auto & a : int_refs)
        copy.push_back(a);
    EXPECT_EQ(*copy[0], 11);
    EXPECT_EQ(*copy[1], 12);
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
    EXPECT_EQ(*copy[0], 1);
    EXPECT_EQ(*copy[1], 2);
}

TEST(RefTest, compare)
{
    class Base {
    public:
        int i;
    };

    class Derived : public Base {
    public:
        double d;
    };

    auto ptr = Qtr<Derived>::alloc();
    auto d = Ref<Derived>(*ptr);
    auto b = Ref<Base>(*ptr);
    EXPECT_TRUE(b == d);
}

TEST(RefTest, late_ref)
{
    LateRef<int> lref;
    EXPECT_TRUE(lref.is_null());
    EXPECT_FALSE(lref);

    int i;
    Ref<int> ref(i);
    lref.set(ref);
    EXPECT_TRUE(lref);
    EXPECT_FALSE(lref.is_null());
}

TEST(RefTest, access)
{
    class Base {
    public:
        int i;

        int
        get_i() const
        {
            return i;
        }
    };

    LateRef<Base> lref;
    Base i = { 10 };
    Ref<Base> ref(i);
    lref.set(ref);

    EXPECT_EQ(lref->i, 10);

    const LateRef<Base> cref = lref;
    EXPECT_EQ(cref->get_i(), 10);
}

TEST(RefTest, access_null_stops)
{
    class Base {
    public:
        int i;
    };

    LateRef<Base> lref;
    EXPECT_DEATH(lref->i = 10, "Accessing null reference");
}

TEST(RefTest, op_less)
{
    // NOTE: operator< is tested via `std::map` which is ordered (cpt. Obvious)

    struct A {
        int i;
        double d;
    };

    A a1 { 10, 123. };
    A a2 { 11, 201. };

    std::vector<Ref<const A>> as;
    as.emplace_back(ref(a1));
    as.emplace_back(ref(a2));

    std::map<Ref<const A>, int> m;
    m.emplace(as[0], 1001);
    m.emplace(as[1], 2001);

    auto v0 = m.at(as[0]);
    EXPECT_EQ(v0, 1001);
}
