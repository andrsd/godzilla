#include "CallStack.h"
#include "gmock/gmock.h"

using namespace godzilla;

void
segfault()
{
    volatile int * x = nullptr;
    int a = *x;
}

TEST(CallStackTest, abort)
{
    _F_;
    EXPECT_EXIT(segfault(),
                ::testing::ExitedWithCode(254),
                "Caught signal 11 \\(Segmentation violation\\)");
}

TEST(CallStackTest, dump)
{
    _F_;

    testing::internal::CaptureStderr();
    godzilla::internal::get_callstack().dump();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                ::testing::StartsWith("Call stack:\n  #0:"));
}

TEST(CallStackTest, alloc)
{
    _F_;

    auto * callstack = new godzilla::internal::CallStack();
    delete callstack;
}
