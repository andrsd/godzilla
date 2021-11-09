#include "GodzillaApp_test.h"
#include "PrintInterface_test.h"
#include "CallStack.h"


using namespace godzilla;

TEST_F(PrintInterfaceTest, print)
{
    testing::internal::CaptureStdout();
    this->app->godzillaPrint(0, "Message");

    EXPECT_EQ(
        testing::internal::GetCapturedStdout(),
        "Message\n"
    );
}

TEST_F(PrintInterfaceTest, error)
{
    EXPECT_DEATH(
        this->app->godzillaError("Error"),
        "ERROR: Error"
    );
}

TEST_F(PrintInterfaceTest, error_with_callstack)
{
    _F_;
    EXPECT_DEATH(
        this->app->godzillaErrorWithCallStack("Error"),
        "ERROR: Error"
    );
}

TEST_F(PrintInterfaceTest, check_petsc_error)
{
    EXPECT_DEATH(
        this->app->checkPetscError(123),
        "ERROR: PETSc error: 123"
    );
}
