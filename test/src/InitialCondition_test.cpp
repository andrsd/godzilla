#include "gmock/gmock.h"
#include "FENonlinearProblem_test.h"
#include "InitialCondition.h"
#include "Parameters.h"

using namespace godzilla;
using namespace testing;

namespace {

class InitialConditionTest : public FENonlinearProblemTest {
public:
    //    void
    //    SetUp() override
    //    {
    //        FENonlinearProblemTest::SetUp();
    //    }
    //
    //    const Int ia;
};

class InitialCondition2FieldTest : public FENonlinear2FieldProblemTest {
public:
};

class MockInitialCondition : public InitialCondition {
public:
    explicit MockInitialCondition(const Parameters & params) : InitialCondition(params) {}

    Int
    get_num_components() const override
    {
        return 1.;
    }
    MOCK_METHOD(void, evaluate, (Int, Real, const Real x[], Int Nc, Scalar u[]), ());
};

class TestInitialCondition : public InitialCondition {
public:
    explicit TestInitialCondition(const Parameters & params) : InitialCondition(params) {}

    Int
    get_num_components() const override
    {
        return 1.;
    }

    void
    evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override
    {
        u[0] = time * (x[0] + 10);
    }
};

class TestVectorInitialCondition : public InitialCondition {
public:
    explicit TestVectorInitialCondition(const Parameters & params) : InitialCondition(params) {}

    Int
    get_num_components() const override
    {
        return 2.;
    }

    void
    evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override
    {
        u[0] = time * (x[0] + 5);
        u[1] = (x[0] + time);
    }
};

} // namespace

TEST_F(InitialConditionTest, api)
{
    Parameters params = InitialCondition::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<DiscreteProblemInterface *>("_dpi") = this->prob;
    params.set<std::string>("_name") = "obj";
    MockInitialCondition ic(params);

    EXPECT_EQ(ic.get_field_id(), -1);

    Real x[1] = { 0. };
    Real u[1] = { 0. };
    EXPECT_CALL(ic, evaluate);
    PetscFunc * fn = ic.get_function();
    EXPECT_EQ((*fn)(1, 0., x, 1, u, &ic), 0);
}

TEST_F(InitialConditionTest, test)
{
    this->prob->add_aux_fe("a", 1, 1);

    Parameters params = InitialCondition::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<DiscreteProblemInterface *>("_dpi") = this->prob;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("field") = "u";
    MockInitialCondition ic(params);

    Parameters aux_ic_pars = InitialCondition::parameters();
    aux_ic_pars.set<const App *>("_app") = this->app;
    aux_ic_pars.set<DiscreteProblemInterface *>("_dpi") = this->prob;
    aux_ic_pars.set<std::string>("_name") = "a_ic";
    aux_ic_pars.set<std::string>("field") = "a";
    MockInitialCondition aux_ic(aux_ic_pars);

    this->prob->add_initial_condition(&ic);
    this->prob->add_initial_condition(&aux_ic);

    this->mesh->create();
    this->prob->create();

    this->app->check_integrity();

    EXPECT_EQ(ic.get_field_id(), 0);

    EXPECT_TRUE(this->prob->has_initial_condition("obj"));
    EXPECT_EQ(this->prob->get_initial_condition("obj"), &ic);

    EXPECT_FALSE(this->prob->has_initial_condition("non-existent-ic"));
    EXPECT_EQ(this->prob->get_initial_condition("non-existent-ic"), nullptr);

    auto ics = this->prob->get_initial_conditions();
    EXPECT_THAT(ics, ElementsAre(&ic));

    // aux ICs

    EXPECT_EQ(aux_ic.get_field_id(), 0);

    EXPECT_TRUE(this->prob->has_initial_condition("a_ic"));
    EXPECT_EQ(this->prob->get_initial_condition("a_ic"), &aux_ic);

    auto aux_ics = this->prob->get_aux_initial_conditions();
    EXPECT_THAT(aux_ics, ElementsAre(&aux_ic));
}

TEST_F(InitialConditionTest, get_value)
{
    Parameters params = TestInitialCondition::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<DiscreteProblemInterface *>("_dpi") = this->prob;
    params.set<std::string>("_name") = "obj";
    TestInitialCondition ic(params);
}

TEST_F(InitialConditionTest, get_vector_value)
{
    Parameters params = TestVectorInitialCondition::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<DiscreteProblemInterface *>("_dpi") = this->prob;
    params.set<std::string>("_name") = "obj";
    TestVectorInitialCondition ic(params);
}

TEST_F(InitialConditionTest, duplicate_ic_name)
{
    Parameters params = TestInitialCondition::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<DiscreteProblemInterface *>("_dpi") = this->prob;
    params.set<std::string>("_name") = "obj";
    TestInitialCondition ic(params);

    this->prob->add_initial_condition(&ic);

    EXPECT_DEATH(this->prob->add_initial_condition(&ic),
                 "Cannot add initial condition object 'obj'. Name already taken.");
}

TEST_F(InitialCondition2FieldTest, no_field_param)
{
    testing::internal::CaptureStderr();

    Parameters params = InitialCondition::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<DiscreteProblemInterface *>("_dpi") = this->prob;
    params.set<std::string>("_name") = "obj";
    MockInitialCondition ic(params);

    this->prob->add_initial_condition(&ic);

    this->mesh->create();
    this->prob->create();

    this->app->check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr(
            "Use the 'field' parameter to assign this initial condition to an existing field"));
}

TEST_F(InitialCondition2FieldTest, non_existing_field)
{
    testing::internal::CaptureStderr();

    Parameters params = InitialCondition::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<DiscreteProblemInterface *>("_dpi") = this->prob;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("field") = "asdf";
    MockInitialCondition ic(params);

    this->prob->add_initial_condition(&ic);

    this->mesh->create();
    this->prob->create();

    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Field 'asdf' does not exists. Typo?"));
}
