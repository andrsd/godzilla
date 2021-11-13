#include "GodzillaApp_test.h"
#include "Steady_test.h"
#include "InputParameters.h"
#include "petsc.h"

registerObject(MockSteady);
registerObject(MockProblem);
registerObject(MockOutput);

TEST_F(SteadyTest, execute)
{
    auto prob = gProblem();
    auto exec = gExecutioner(prob);
    EXPECT_CALL(*prob, solve()).Times(1);
    exec->execute();
}
