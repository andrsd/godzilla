#include "gmock/gmock.h"
#include "godzilla/Expected.h"

using namespace godzilla;

namespace {

enum class Error {
    //
    Error1,
    Error2,
    Failed,
    Unknown
};

Expected<int, Error>
function(int arg)
{
    if (arg == 0)
        return Unexpected(Error::Unknown);

    return 100 + arg;
}

} // namespace

TEST(ExpectedTest, exected)
{
    auto res = function(1);
    EXPECT_TRUE(res);
    EXPECT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 101);
}

TEST(ExpectedTest, error)
{
    auto res = function(0);
    EXPECT_FALSE(res);
    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), Error::Unknown);
}
