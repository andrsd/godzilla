#include "godzilla/Error.h"
#include "godzilla/Assert.h"
#include "godzilla/CallStack.h"
#include "gmock/gmock.h"

using namespace godzilla;

namespace {

#if 0
void
warn_fn()
{
    warning_once("Warning");
}
#endif

void
fn1_fail()
{
    CALL_STACK_MSG();
    expect_true(false, "error");
}

void
fn2(bool fail, const char *, int)
{
    CALL_STACK_MSG();
    if (fail)
        fn1_fail();
}

void
fn3(bool fail)
{
    CALL_STACK_MSG();
    fn2(fail, "asdf", 1234);
}

} // namespace

TEST(ErrorTest, mem_check)
{
    CALL_STACK_MSG();
    void * var = nullptr;
    EXPECT_DEATH(MEM_CHECK(var), "Out of memory");
}

TEST(ErrorTest, petsc_check_macro)
{
    EXPECT_DEATH(PETSC_CHECK(123), "\\[ERROR\\] PETSc error: 123");
}

TEST(ErrorTest, expect_true)
{
    EXPECT_DEATH(fn3(true), "\\[ERROR\\] error");

    EXPECT_NO_FATAL_FAILURE(expect_true(true, "error"));
}

TEST(ErrorTest, error)
{
    EXPECT_DEATH(internal::error("error"), "\\[ERROR\\] error");
}

TEST(ErrorTest, warning)
{
    testing::internal::CaptureStderr();
    internal::warning("warning");
    auto out = testing::internal::GetCapturedStderr();
    EXPECT_THAT(out, testing::HasSubstr("[WARNING] warning"));
}

#if 0
TEST(ErrorTest, warning_once)
{
    testing::internal::CaptureStdout();
    warn_fn();
    warn_fn();
    warn_fn();
    auto out = testing::internal::GetCapturedStdout();
    EXPECT_EQ(out, "\x1B[33m[WARNING] Warning\x1B[39m\n");
}
#endif
