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
    EXPECT_DEATH(checkPetscError(123), "error: PETSc error: 123");
}
