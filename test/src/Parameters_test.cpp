#include "gtest/gtest.h"
#include "Parameters.h"
#include "Factory.h"

using namespace godzilla;

TEST(ParametersTest, get)
{
    Parameters params = Object::valid_params();
    EXPECT_DEATH(params.get<int>("i"), "No parameter 'i' found.");
}

TEST(ParametersTest, param_value)
{
    Parameters params;
    params.add_param<PetscReal>("param", 12.34, "doco");
    EXPECT_EQ(params.get<PetscReal>("param"), 12.34);
    EXPECT_EQ(params.get_doc_string("param"), std::string("doco"));
}

TEST(ParametersTest, has_value)
{
    Parameters params;
    params.add_param<PetscReal>("param", 12.34, "doco");
    EXPECT_EQ(params.has<PetscReal>("param"), true);
    params.clear();
    EXPECT_EQ(params.has<PetscReal>("param"), false);
}

TEST(ParametersTest, assign)
{
    Parameters params1;
    params1.add_param<PetscReal>("param", 12.34, "doco");

    Parameters params2 = params1;
    EXPECT_EQ(params2.has<PetscReal>("param"), true);
    EXPECT_EQ(params2.get<PetscReal>("param"), 12.34);
    EXPECT_EQ(params2.get_doc_string("param"), std::string("doco"));
}

TEST(ParametersTest, add_params)
{
    Parameters params1;
    params1.add_param<PetscReal>("p1", 12.34, "doco1");

    Parameters params2;
    params1.add_param<PetscReal>("p2", "doco2");

    params1 += params2;
    EXPECT_EQ(params1.has<PetscReal>("p1"), true);
    EXPECT_EQ(params1.get<PetscReal>("p1"), 12.34);
    EXPECT_EQ(params1.get_doc_string("p1"), std::string("doco1"));
    EXPECT_EQ(params1.has<PetscReal>("p2"), true);
    EXPECT_EQ(params1.get_doc_string("p2"), std::string("doco2"));
}

Parameters
validParams1()
{
    Parameters params;
    params.add_param<PetscReal>("p", 78.56, "doco p");
    return params;
}

TEST(ParametersTest, valid_params)
{
    Parameters params1 = validParams1();
    EXPECT_EQ(params1.get<PetscReal>("p"), 78.56);
    EXPECT_EQ(params1.get_doc_string("p"), std::string("doco p"));
}

TEST(ParametersTest, empty_doc_str)
{
    Parameters params = Object::valid_params();

    EXPECT_EQ(params.get_doc_string("i"), std::string(""));
}

TEST(ParametersTest, set_non_existing_param)
{
    Parameters params = Object::valid_params();
    params.set<double>("d") = 1.23;

    EXPECT_EQ(params.get<double>("d"), 1.23);
}
