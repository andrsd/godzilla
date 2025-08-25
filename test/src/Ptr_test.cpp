#include <gmock/gmock.h>
#include "godzilla/CallStack.h"
#include "godzilla/Ptr.h"
#include "godzilla/Exception.h"

using namespace godzilla;

namespace {

class MyBigObj {
public:
    MyBigObj(int size) : sz(size) { this->a = new int[size]; }

    ~MyBigObj() { delete[] this->a; }

    int
    size() const
    {
        return this->sz;
    }

private:
    int sz;
    int * a;
};

} // namespace

TEST(PtrTest, allocate)
{
    auto obj = Ptr<MyBigObj>::alloc(10000);
    EXPECT_EQ(obj.ref_count(), 1);
}

TEST(PtrTest, ptr_access)
{
    auto obj = Ptr<MyBigObj>::alloc(1359);
    EXPECT_EQ(obj->size(), 1359);
    EXPECT_EQ((*obj).size(), 1359);
}

TEST(PtrTest, circular_dependency)
{
    bool a_deleted = false;
    bool b_deleted = false;

    class B;

    class A {
    public:
        A(bool * del) : deleted(del) {}
        ~A() { *this->deleted = true; }

        Ptr<B> b_ptr;

    private:
        bool * deleted;
    };

    class B {
    public:
        B(bool * del) : deleted(del) {}
        ~B() { *this->deleted = true; }

        Ptr<A> a_ptr;

    private:
        bool * deleted;
    };

    {
        auto a = Ptr<A>::alloc(&a_deleted);
        auto b = Ptr<B>::alloc(&b_deleted);

        a->b_ptr = b;
        b->a_ptr = a;

        EXPECT_EQ(a.ref_count(), 2);
        EXPECT_EQ(b.ref_count(), 2);
    }

    EXPECT_FALSE(a_deleted);
    EXPECT_FALSE(b_deleted);
}

TEST(PtrTest, copy)
{
    auto obj1 = Ptr<MyBigObj>::alloc(10000);
    Ptr<MyBigObj> obj2 = obj1;
    EXPECT_EQ(obj1.ref_count(), 2);
    EXPECT_EQ(obj2.ref_count(), 2);
}

TEST(PtrTest, null_ptr_access)
{
    CALL_STACK_MSG();
    Ptr<MyBigObj> ptr;
    try {
        *ptr;
        FAIL() << "Expected exception";
    }
    catch (const Exception & e) {
        EXPECT_STREQ(e.what(), "Access into a null pointer");
    }
}

//

TEST(PtrArrTest, allocate)
{
    auto obj = Ptr<int[]>::alloc(10);
    EXPECT_EQ(obj.ref_count(), 1);
}
