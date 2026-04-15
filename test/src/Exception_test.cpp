#include "gmock/gmock.h"
#include "godzilla/Exception.h"
#include "godzilla/CallStack.h"
#include "fmt/core.h"

using namespace godzilla;

TEST(Exception, ctor)
{
    Exception e(fmt::format("number={}, str={}", 1, "str"));
    EXPECT_EQ(e.rank(), -1);
    EXPECT_STREQ(e.what(), "number=1, str=str");
}

TEST(Exception, ctor_w_rank)
{
    Exception exc(2, "exception");
    EXPECT_EQ(exc.rank(), 2);
    EXPECT_STREQ(exc.what(), "exception");
}

namespace {

void
fn1()
{
    CALL_STACK_MSG();
    throw Exception("Something bad happend");
}

void
fn2()
{
    CALL_STACK_MSG();
    fn1();
}

void
fn3()
{
    CALL_STACK_MSG();
    fn2();
}

void
fn4(int)
{
    CALL_STACK_MSG();
    fn3();
}

void
fn5()
{
    CALL_STACK_MSG();
    fn4(2);
}

} // namespace

TEST(Exception, print)
{
    testing::internal::CaptureStderr();

    try {
        fn5();
    }
    catch (Exception & e) {
        print(e);
    }

    auto output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("Something bad happend"));
#ifndef NDEBUG
    EXPECT_THAT(output, testing::HasSubstr("fn5"));
    EXPECT_THAT(output, testing::HasSubstr("fn4"));
    EXPECT_THAT(output, testing::HasSubstr("fn3"));
    EXPECT_THAT(output, testing::HasSubstr("fn2"));
#endif
    EXPECT_THAT(output, testing::HasSubstr("fn1"));
}

TEST(Exception, print_w_rank)
{
    testing::internal::CaptureStderr();

    Exception e(2, "Something bad happend");
    print(e);

    auto output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("[2] Something bad happend"));
    EXPECT_THAT(output, testing::HasSubstr("print_w_rank_Test::TestBody()"));
}

//

TEST(InternalError, test)
{
    InternalError e("text");
    EXPECT_STREQ(e.what(), "text");
}

TEST(NotImplementedException, empty_msg)
{
    NotImplementedException e;
    EXPECT_STREQ(e.what(), "");
}

TEST(NotImplementedException, empty_with_message)
{
    NotImplementedException e("text");
    EXPECT_STREQ(e.what(), "text");
}
