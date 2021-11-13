#include "GodzillaApp_test.h"
#include "Executioner_test.h"
#include "InputParameters.h"
#include "petsc.h"

registerObject(MockExecutioner);
registerObject(MockProblem);
registerObject(MockOutput);

TEST_F(ExecutionerTest, create)
{
    auto prob = gProblem();
    auto exec = gExecutioner(prob);
    exec->create();
}

TEST_F(ExecutionerTest, output)
{
    auto prob = gProblem();
    auto exec = gExecutioner(prob);

    auto out1 = gOutput(prob, "out1");
    exec->addOutput(out1);

    auto out2 = gOutput(prob, "out2");
    exec->addOutput(out2);

    EXPECT_CALL(*out1, output()).Times(1);
    EXPECT_CALL(*out2, output()).Times(1);
    exec->output();
}
