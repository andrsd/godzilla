#include "godzilla/CallStack.h"
#include "gmock/gmock.h"

using namespace godzilla;
using namespace testing;

void
segfault()
{
    volatile int * x = nullptr;
    [[maybe_unused]] int a = *x; // NOLINT
}

TEST(CallStackTest, abort)
{
    CALL_STACK_MSG();
    EXPECT_EXIT(segfault(), ::ExitedWithCode(254), "Caught signal 11 \\(Segmentation violation\\)");
}

TEST(CallStackTest, new_callstack_is_empty)
{
    CALL_STACK_MSG();
    godzilla::internal::CallStack callstack;
    EXPECT_EQ(callstack.get_size(), 0);
}

namespace unit_test {

void
fn3()
{
    CALL_STACK_MSG();
    godzilla::internal::get_callstack().dump();
}

void
fn2()
{
    CALL_STACK_MSG("fn2()");
    fn3();
}

void
fn1(int i, const std::string & s)
{
    CALL_STACK_MSG("fn1(i={}, s='{}')", i, s);
    fn2();
}

} // namespace unit_test

TEST(CallStackTest, dump)
{
    testing::internal::CaptureStderr();
    unit_test::fn1(1, "string");

    auto err = testing::internal::GetCapturedStderr();
    EXPECT_THAT(err, StartsWith("Call stack:"));
    EXPECT_THAT(err, HasSubstr("  #0: void unit_test::fn3()"));
    EXPECT_THAT(err, HasSubstr("  #1: fn2()"));
    EXPECT_THAT(err, HasSubstr("  #2: fn1(i=1, s='string')"));
}

TEST(CallStackTest, stack)
{
    godzilla::internal::CallStack::Msg msg1(__FILE__, __LINE__, "fn1");
    godzilla::internal::CallStack::Msg msg2(__FILE__, __LINE__, "fn2");

    auto & cs = godzilla::internal::get_callstack();
    ASSERT_EQ(cs.get_size(), 2);
    EXPECT_EQ(cs.at(0)->msg, "fn1");
    EXPECT_EQ(cs.at(1)->msg, "fn2");
}
