#include "gmock/gmock.h"
#include "godzilla/Exception.h"
#include "godzilla/CallStack.h"

using namespace godzilla;

TEST(Exception, ctor_simple)
{
    Exception e("text");
    EXPECT_STREQ(e.what(), "text");
}

TEST(Exception, ctor_fmt)
{
    Exception e("number={}, str={}", 1, "str");
    EXPECT_STREQ(e.what(), "number=1, str=str");
}

namespace {

void
raise()
{
    internal::CallStack::Msg msg2(__FILE__, __LINE__, "fn2");
    throw Exception("error");
}

} // namespace

TEST(Exception, call_stack)
{
    internal::CallStack::Msg msg1(__FILE__, __LINE__, "fn1");
    try {
        raise();
        FAIL();
    }
    catch (Exception & e) {
        auto & cs = e.get_call_stack();
        EXPECT_EQ(cs.size(), 2);
        EXPECT_THAT(cs.at(0), testing::HasSubstr("fn2"));
        EXPECT_THAT(cs.at(1), testing::HasSubstr("fn1"));
    }
}

//

TEST(InternalError, test)
{
    InternalError e("text");
    EXPECT_STREQ(e.what(), "Internal error: text");
}
