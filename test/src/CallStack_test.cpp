#include "CallStack.h"
#include "gmock/gmock.h"
#include <unistd.h>
#include <signal.h>

using namespace godzilla;

TEST(CallStackTest, abort)
{
    _F_;

    pid_t my_pid = getpid();
    EXPECT_EXIT(kill(my_pid, SIGABRT),
                ::testing::ExitedWithCode(254),
                "Caught signal 6 \\(Abort\\)");
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
