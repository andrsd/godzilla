#include "GodzillaApp_test.h"
#include "PrintInterface.h"

using namespace godzilla;

TEST(PrintInterfaceTest, print)
{
    testing::internal::CaptureStdout();

    class PrintTestApp : public TestApp {
    public:
        void
        test()
        {
            godzilla_print(0, "Message");
        }
    } app;

    app.test();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "Message\n");
}

TEST(PrintInterfaceTest, error)
{
    class PrintTestApp : public TestApp {
    public:
        void
        test()
        {
            error("Error");
        }
    } app;

    EXPECT_DEATH(app.test(), "error: Error");
}
