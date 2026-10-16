#include "godzilla/CallStack.h"
#include "godzilla/String.h"
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
fn4()
{
    godzilla::print_call_stack();
}

void
fn3()
{
    CALL_STACK_MSG();
    fn4();
}

void
fn2()
{
    CALL_STACK_MSG();
    fn3();
}

void
fn1(int, godzilla::String)
{
    CALL_STACK_MSG();
    fn2();
}

} // namespace unit_test

TEST(CallStackTest, dump)
{
#ifdef GODZILLA_WITH_STACKTRACE
    testing::internal::CaptureStderr();
    unit_test::fn1(1, "string");

    auto err = testing::internal::GetCapturedStderr();
    EXPECT_THAT(err, testing::HasSubstr("  at void unit_test::fn3()"));
    EXPECT_THAT(err, testing::HasSubstr("  at void unit_test::fn2()"));
    EXPECT_THAT(err, testing::HasSubstr("  at void unit_test::fn1("));
#else
    testing::internal::CaptureStderr();
    unit_test::fn1(1, "string");

    auto err = testing::internal::GetCapturedStderr();
    EXPECT_THAT(err, testing::StartsWith("No call stack available."));
#endif
}
