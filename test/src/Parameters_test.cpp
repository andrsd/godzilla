#include "gtest/gtest.h"
#include "godzilla/Types.h"
#include "godzilla/Parameters.h"
#include "godzilla/Factory.h"
#include "ExceptionTestMacros.h"

using namespace godzilla;

TEST(ParametersTest, get)
{
    Parameters params = Object::parameters();
    EXPECT_THROW_MSG(params.get<int>("i"), "No parameter 'i' found.");
}

TEST(ParametersTest, param_value)
{
    Parameters params;
    params.add_param<Real>("param", 12.34, "doco");
    EXPECT_EQ(params.get<Real>("param"), 12.34);
    EXPECT_EQ(params.get_doc_string("param"), std::string("doco"));
}

TEST(ParametersTest, has_value)
{
    Parameters params;
    params.add_param<Real>("param", 12.34, "doco");
    EXPECT_TRUE(params.has<Real>("param"));
    params.clear();
    EXPECT_EQ(params.has<Real>("param"), false);
}

TEST(ParametersTest, assign)
{
    Parameters params1;
    params1.add_param<Real>("param", 12.34, "doco");

    Parameters params2 = params1;
    EXPECT_TRUE(params2.has<Real>("param"));
    EXPECT_EQ(params2.get<Real>("param"), 12.34);
    EXPECT_EQ(params2.get_doc_string("param"), std::string("doco"));
}

TEST(ParametersTest, add_params)
{
    Parameters params1;
    params1.add_param<Real>("p1", 12.34, "doco1");

    Parameters params2;
    params1.add_param<Real>("p2", "doco2");

    params1 += params2;
    EXPECT_EQ(params1.has<Real>("p1"), true);
    EXPECT_EQ(params1.get<Real>("p1"), 12.34);
    EXPECT_EQ(params1.get_doc_string("p1"), std::string("doco1"));
    EXPECT_EQ(params1.has<Real>("p2"), true);
    EXPECT_EQ(params1.get_doc_string("p2"), std::string("doco2"));
}

Parameters
validParams1()
{
    Parameters params;
    params.add_param<Real>("p", 78.56, "doco p");
    return params;
}

TEST(ParametersTest, valid_params)
{
    Parameters params1 = validParams1();
    EXPECT_EQ(params1.get<Real>("p"), 78.56);
    EXPECT_EQ(params1.get_doc_string("p"), std::string("doco p"));
}

TEST(ParametersTest, empty_doc_str)
{
    Parameters params = Object::parameters();

    EXPECT_EQ(params.get_doc_string("i"), std::string(""));
}

TEST(ParametersTest, set_non_existing_param)
{
    auto params = Object::parameters();
    params.set<double>("d", 1.23);

    EXPECT_EQ(params.get<double>("d"), 1.23);
}

TEST(ParametersTest, get_non_existing_param_with_default)
{
    auto params = Object::parameters();
    auto val = params.get<double>("d", 1.23);

    EXPECT_EQ(val, 1.23);
}

TEST(ParametersTest, chained_set)
{
    Parameters params;
    params.set<Int>("num", 1234)
        .set<double>("float", 12.34)
        .set<std::string>("text", "some long text");
}

TEST(ParametersTest, move_oper)
{
    Parameters params1;
    params1.set<Int>("i", 1);

    Parameters params2 = std::move(params1);
    EXPECT_TRUE(params2.has<Int>("i"));

    EXPECT_FALSE(params1.has<Int>("i"));
}
