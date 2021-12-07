#include "GodzillaApp_test.h"
#include "PrintInterface.h"
#include "CallStack.h"

using namespace godzilla;

TEST(PrintInterfaceTest, print)
{
    testing::internal::CaptureStdout();

    class PrintTestApp : public TestApp {
    public:
        void
        test()
        {
            godzillaPrint(0, "Message");
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

TEST(PrintInterfaceTest, check_petsc_error)
{
    EXPECT_DEATH(checkPetscError(123), "error: PETSc error: 123");
}
