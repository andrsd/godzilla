#include "base/GodzillaApp_test.h"
#include "executioners/GExecutioner_test.h"
#include "utils/InputParameters.h"
#include "petsc.h"


registerObject(MockGExecutioner);
registerObject(MockGProblem);

TEST_F(GExecutionerTest, create)
{
    auto prob = gProblem();
    auto exec = gExecutioner(prob);
    exec->create();
}

TEST_F(GExecutionerTest, execute)
{
    auto prob = gProblem();
    auto exec = gExecutioner(prob);
    EXPECT_CALL(*prob, solve());
    EXPECT_CALL(*prob, out());
    exec->execute();

    delete prob;
    delete exec;
}
