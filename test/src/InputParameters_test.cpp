#include "gtest/gtest.h"
#include "InputParameters.h"
#include "Factory.h"

using namespace godzilla;

TEST(InputParametersTest, get)
{
    InputParameters params = Factory::getValidParams("Object");
    EXPECT_DEATH(params.get<int>("i"), "No parameter 'i' found.");
}

TEST(InputParametersTest, param_value)
{
    InputParameters params = emptyInputParameters();
    params.addParam<PetscReal>("param", 12.34, "doco");
    EXPECT_EQ(params.get<PetscReal>("param"), 12.34);
    EXPECT_EQ(params.getDocString("param"), std::string("doco"));
}

TEST(InputParametersTest, has_value)
{
    InputParameters params = emptyInputParameters();
    params.addParam<PetscReal>("param", 12.34, "doco");
    EXPECT_EQ(params.has<PetscReal>("param"), true);
    params.clear();
    EXPECT_EQ(params.has<PetscReal>("param"), false);
}

TEST(InputParametersTest, assign)
{
    InputParameters params1 = emptyInputParameters();
    params1.addParam<PetscReal>("param", 12.34, "doco");

    InputParameters params2 = params1;
    EXPECT_EQ(params2.has<PetscReal>("param"), true);
    EXPECT_EQ(params2.get<PetscReal>("param"), 12.34);
    EXPECT_EQ(params2.getDocString("param"), std::string("doco"));
}

TEST(InputParametersTest, add_params)
{
    InputParameters params1 = emptyInputParameters();
    params1.addParam<PetscReal>("p1", 12.34, "doco1");

    InputParameters params2 = emptyInputParameters();
    params1.addParam<PetscReal>("p2", "doco2");

    params1 += params2;
    EXPECT_EQ(params1.has<PetscReal>("p1"), true);
    EXPECT_EQ(params1.get<PetscReal>("p1"), 12.34);
    EXPECT_EQ(params1.getDocString("p1"), std::string("doco1"));
    EXPECT_EQ(params1.has<PetscReal>("p2"), true);
    EXPECT_EQ(params1.getDocString("p2"), std::string("doco2"));
}

InputParameters
validParams1()
{
    InputParameters params = emptyInputParameters();
    params.addParam<PetscReal>("p", 78.56, "doco p");
    return params;
}

TEST(InputParametersTest, valid_params)
{
    InputParameters params1 = validParams1();
    EXPECT_EQ(params1.get<PetscReal>("p"), 78.56);
    EXPECT_EQ(params1.getDocString("p"), std::string("doco p"));
}

TEST(InputParametersTest, empty_doc_str)
{
    InputParameters & params = Factory::getValidParams("Object");

    EXPECT_EQ(params.getDocString("i"), std::string(""));
}

TEST(InputParametersTest, set_non_existing_param)
{
    InputParameters & params = Factory::getValidParams("Object");
    params.set<double>("d") = 1.23;

    EXPECT_EQ(params.get<double>("d"), 1.23);
}
