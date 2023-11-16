#include "godzilla/Error.h"
#include "godzilla/CallStack.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(ErrorTest, mem_check)
{
    _F_;
    void * var = nullptr;
    EXPECT_DEATH(MEM_CHECK(var), "Out of memory");
}

TEST(ErrorTest, petsc_check_macro)
{
    EXPECT_DEATH(PETSC_CHECK(123), "\\[ERROR\\] PETSc error: 123");
}
