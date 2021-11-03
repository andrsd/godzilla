#include "base/GodzillaApp_test.h"
#include "base/GPrintInterface_test.h"
#include "base/CallStack.h"


TEST_F(GPrintInterfaceTest, print)
{
    testing::internal::CaptureStdout();
    auto app = std::dynamic_pointer_cast<GodzillaApp>(this->app);
    app->godzillaPrint(0, "Message");

    EXPECT_EQ(
        testing::internal::GetCapturedStdout(),
        "Message\n"
    );
}

TEST_F(GPrintInterfaceTest, error)
{
    auto app = std::dynamic_pointer_cast<GodzillaApp>(this->app);
    EXPECT_DEATH(
        app->godzillaError("Error"),
        "ERROR: Error"
    );
}

TEST_F(GPrintInterfaceTest, error_with_callstack)
{
    _F_;
    auto app = std::dynamic_pointer_cast<GodzillaApp>(this->app);
    EXPECT_DEATH(
        app->godzillaErrorWithCallStack("Error"),
        "ERROR: Error"
    );
}
