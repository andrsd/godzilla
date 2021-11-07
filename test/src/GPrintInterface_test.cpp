#include "GodzillaApp_test.h"
#include "GPrintInterface_test.h"
#include "CallStack.h"


using namespace godzilla;

TEST_F(GPrintInterfaceTest, print)
{
    testing::internal::CaptureStdout();
    this->app->godzillaPrint(0, "Message");

    EXPECT_EQ(
        testing::internal::GetCapturedStdout(),
        "Message\n"
    );
}

TEST_F(GPrintInterfaceTest, error)
{
    EXPECT_DEATH(
        this->app->godzillaError("Error"),
        "ERROR: Error"
    );
}

TEST_F(GPrintInterfaceTest, error_with_callstack)
{
    _F_;
    EXPECT_DEATH(
        this->app->godzillaErrorWithCallStack("Error"),
        "ERROR: Error"
    );
}

TEST_F(GPrintInterfaceTest, check_petsc_error)
{
    EXPECT_DEATH(
        this->app->checkPetscError(123),
        "ERROR: PETSc error: 123"
    );
}
