#include "Error.h"
#include "CallStack.h"
#include "gmock/gmock.h"
#include <unistd.h>
#include <signal.h>

using namespace godzilla;

TEST(ErrorTest, mem_check)
{
    _F_;
    void * var = nullptr;
    EXPECT_DEATH(MEM_CHECK(var), "Out of memory");
}

TEST(ErrorTest, check_petsc_error)
{
    EXPECT_DEATH(check_petsc_error(123), "\\[ERROR\\] PETSc error: 123");
}

// fprint tests

TEST(ErrorTest, fpf_plain)
{
    std::ostringstream oss;
    internal::fprintf(oss, "text");
    EXPECT_EQ(oss.str(), "text");
}

TEST(ErrorTest, fpf_str)
{
    std::ostringstream oss;
    internal::fprintf(oss, "str: %s", "text");
    EXPECT_EQ(oss.str(), "str: text");
}

TEST(ErrorTest, fpf_f)
{
    std::ostringstream oss;
    internal::fprintf(oss, "%f", 1.234);
    EXPECT_EQ(oss.str(), "1.234");
}

TEST(ErrorTest, fpf_e)
{
    std::ostringstream oss;
    internal::fprintf(oss, "%e", 1.234);
    EXPECT_EQ(oss.str(), "1.234000e+00");
}

TEST(ErrorTest, fpf_invalid_format)
{
    std::ostringstream oss;
    EXPECT_THROW(
        try { internal::fprintf(oss, "%e"); } catch (const std::runtime_error & e) {
            EXPECT_STREQ(e.what(), "invalid format: missing arguments");
            throw;
        },
        std::runtime_error);
}

TEST(ErrorTest, fpf_extra_arguments)
{
    std::ostringstream oss;
    EXPECT_THROW(
        try { internal::fprintf(oss, "%e", 1, 2); } catch (const std::runtime_error & e) {
            EXPECT_STREQ(e.what(), "extra arguments provided to fprintf");
            throw;
        },
        std::runtime_error);
}
