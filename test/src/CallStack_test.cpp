#include "godzilla/CallStack.h"
#include "gmock/gmock.h"

void
segfault()
{
    volatile int * x = nullptr;
    [[maybe_unused]] int a = *x; // NOLINT
}

TEST(CallStackTest, abort)
{
    CALL_STACK_MSG();
    EXPECT_EXIT(segfault(),
                testing::ExitedWithCode(254),
                "Caught signal 11 \\(Segmentation violation\\)");
}

TEST(CallStackTest, new_callstack_is_empty)
{
    CALL_STACK_MSG();
    godzilla::CallStack callstack;
    EXPECT_EQ(callstack.get_size(), 0);
}

namespace unit_test {

void
fn3()
{
    CALL_STACK_MSG();
    godzilla::get_callstack().dump();
}

void
fn2()
{
    CALL_STACK_MSG();
    fn3();
}

void
fn1(int i, godzilla::String s)
{
    CALL_STACK_MSG();
    fn2();
}

} // namespace unit_test

TEST(CallStackTest, dump)
{
#ifndef NDEBUG
    testing::internal::CaptureStderr();
    unit_test::fn1(1, "string");

    auto err = testing::internal::GetCapturedStderr();
    EXPECT_THAT(err, testing::StartsWith("Call stack:"));
    EXPECT_THAT(err, testing::HasSubstr("  #0: void unit_test::fn3()"));
    EXPECT_THAT(err, testing::HasSubstr("  #1: void unit_test::fn2()"));
    EXPECT_THAT(err, testing::HasSubstr("  #2: void unit_test::fn1("));
#endif
}
