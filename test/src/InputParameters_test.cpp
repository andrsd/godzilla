#include "GodzillaConfig.h"
#include "gtest/gtest.h"
#include "InputParameters.h"
#include "Factory.h"

using namespace godzilla;

TEST(InputParametersTest, get)
{
    InputParameters params = Factory::get_valid_params("Object");
    EXPECT_DEATH(params.get<int>("i"), "No parameter 'i' found.");
}

TEST(InputParametersTest, param_value)
{
    InputParameters params = InputParameters::empty();
    params.add_param<Real>("param", 12.34, "doco");
    EXPECT_EQ(params.get<Real>("param"), 12.34);
    EXPECT_EQ(params.get_doc_string("param"), std::string("doco"));
}

TEST(InputParametersTest, has_value)
{
    InputParameters params = InputParameters::empty();
    params.add_param<Real>("param", 12.34, "doco");
    EXPECT_EQ(params.has<Real>("param"), true);
    params.clear();
    EXPECT_EQ(params.has<Real>("param"), false);
}

TEST(InputParametersTest, assign)
{
    InputParameters params1 = InputParameters::empty();
    params1.add_param<Real>("param", 12.34, "doco");

    InputParameters params2 = params1;
    EXPECT_EQ(params2.has<Real>("param"), true);
    EXPECT_EQ(params2.get<Real>("param"), 12.34);
    EXPECT_EQ(params2.get_doc_string("param"), std::string("doco"));
}

TEST(InputParametersTest, add_params)
{
    InputParameters params1 = InputParameters::empty();
    params1.add_param<Real>("p1", 12.34, "doco1");

    InputParameters params2 = InputParameters::empty();
    params1.add_param<Real>("p2", "doco2");

    params1 += params2;
    EXPECT_EQ(params1.has<Real>("p1"), true);
    EXPECT_EQ(params1.get<Real>("p1"), 12.34);
    EXPECT_EQ(params1.get_doc_string("p1"), std::string("doco1"));
    EXPECT_EQ(params1.has<Real>("p2"), true);
    EXPECT_EQ(params1.get_doc_string("p2"), std::string("doco2"));
}

InputParameters
validParams1()
{
    InputParameters params = InputParameters::empty();
    params.add_param<Real>("p", 78.56, "doco p");
    return params;
}

TEST(InputParametersTest, valid_params)
{
    InputParameters params1 = validParams1();
    EXPECT_EQ(params1.get<Real>("p"), 78.56);
    EXPECT_EQ(params1.get_doc_string("p"), std::string("doco p"));
}

TEST(InputParametersTest, empty_doc_str)
{
    InputParameters & params = Factory::get_valid_params("Object");

    EXPECT_EQ(params.get_doc_string("i"), std::string(""));
}

TEST(InputParametersTest, set_non_existing_param)
{
    InputParameters & params = Factory::get_valid_params("Object");
    params.set<double>("d") = 1.23;

    EXPECT_EQ(params.get<double>("d"), 1.23);
}

TEST(InputParametersTest, iterators)
{
    InputParameters params = InputParameters::empty();
    params.add_param<Real>("a", "doco");
    for (auto & p : params) {
    }
}

TEST(InputParametersTest, param)
{
    InputParameters params = InputParameters::empty();
    params.add_required_param<Real>("a", "doco");

    EXPECT_EQ(params.is_param_required("a"), true);
    EXPECT_EQ(params.type("a"), std::string("d"));
    EXPECT_EQ(params.is_private("a"), false);
}
