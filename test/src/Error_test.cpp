#include "godzilla/Error.h"
#include "godzilla/CallStack.h"
#include "gmock/gmock.h"

using namespace godzilla;

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
    EXPECT_DEATH(expect_true(false, "error"), "\\[ERROR\\] error");

    EXPECT_NO_FATAL_FAILURE(expect_true(true, "error"));
}

TEST(ErrorTest, error)
{
    EXPECT_DEATH(error("error"), "\\[ERROR\\] error");
}

TEST(ErrorTest, warning)
{
    testing::internal::CaptureStdout();
    warning("warning");
    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, testing::HasSubstr("[WARNING] warning"));
}
