#include "gmock/gmock.h"
#include "FENonlinearProblem_test.h"
#include "godzilla/InitialCondition.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/Parameters.h"
#include "ExceptionTestMacros.h"
#include "godzilla/Types.h"

using namespace godzilla;
using namespace testing;

namespace {

class InitialConditionTest : public FENonlinearProblemTest {
public:
};

class InitialCondition2FieldTest : public FENonlinear2FieldProblemTest {
public:
};

class MockInitialCondition : public InitialCondition {
public:
    explicit MockInitialCondition(const Parameters & pars) : InitialCondition(pars) {}

    MOCK_METHOD(void, evaluate, (Real, const Real x[], Scalar u[]), ());
};

class TestInitialCondition : public InitialCondition {
public:
    explicit TestInitialCondition(const Parameters & pars) : InitialCondition(pars) {}

    std::vector<Int>
    create_components() override
    {
        return { 0 };
    }

    void
    evaluate(Real time, const Real x[], Scalar u[]) override
    {
        u[0] = time * (x[0] + 10);
    }
};

class TestVectorInitialCondition : public InitialCondition {
public:
    explicit TestVectorInitialCondition(const Parameters & pars) : InitialCondition(pars) {}

    std::vector<Int>
    create_components() override
    {
        return { 0, 1 };
    }

    void
    evaluate(Real time, const Real x[], Scalar u[]) override
    {
        u[0] = time * (x[0] + 5);
        u[1] = (x[0] + time);
    }
};

} // namespace

TEST_F(InitialConditionTest, api)
{
    auto params = InitialCondition::parameters();
    params.set<App *>("app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", this->prob);
    params.set<String>("name", "obj");
    MockInitialCondition ic(params);

    EXPECT_EQ(ic.get_field_id(), FieldID::INVALID);
}

TEST_F(InitialConditionTest, test)
{
    this->prob->add_aux_field("a", 1, Order(1));

    auto params = InitialCondition::parameters();
    params.set<App *>("app", this->app);
    params.set<String>("name", "obj");
    params.set<String>("field", "u");
    auto ic = this->prob->add_initial_condition<MockInitialCondition>(params);

    auto aux_ic_pars = InitialCondition::parameters();
    aux_ic_pars.set<App *>("app", this->app);
    aux_ic_pars.set<String>("name", "a_ic");
    aux_ic_pars.set<String>("field", "a");
    auto aux_ic = this->prob->add_initial_condition<MockInitialCondition>(aux_ic_pars);

    this->prob->create();

    EXPECT_EQ(ic->get_field_id(), FieldID(0));
    EXPECT_EQ(ic->get_dimension(), 1);

    EXPECT_TRUE(this->prob->has_initial_condition("obj"));
    auto ic2 = this->prob->get_initial_condition("obj");
    EXPECT_TRUE(ic2.has_value());
    EXPECT_EQ(ic2.value(), ic);

    EXPECT_FALSE(this->prob->has_initial_condition("non-existent-ic"));
    auto ic3 = this->prob->get_initial_condition("non-existent-ic");
    EXPECT_FALSE(ic3.has_value());

    auto ics = this->prob->get_initial_conditions();
    EXPECT_THAT(ics, ElementsAre(ic));

    // aux ICs

    EXPECT_EQ(aux_ic->get_field_id(), FieldID(0));

    EXPECT_TRUE(this->prob->has_initial_condition("a_ic"));
    auto aic2 = this->prob->get_initial_condition("a_ic");
    EXPECT_TRUE(aic2.has_value());
    EXPECT_EQ(aic2.value(), aux_ic);

    auto aux_ics = this->prob->get_aux_initial_conditions();
    EXPECT_THAT(aux_ics, ElementsAre(aux_ic));
}

TEST_F(InitialConditionTest, get_value)
{
    auto params = TestInitialCondition::parameters();
    params.set<App *>("app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", this->prob);
    params.set<String>("name", "obj");
    TestInitialCondition ic(params);
}

TEST_F(InitialConditionTest, get_vector_value)
{
    auto params = TestVectorInitialCondition::parameters();
    params.set<App *>("app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", this->prob);
    params.set<String>("name", "obj");
    TestVectorInitialCondition ic(params);
}

TEST_F(InitialConditionTest, duplicate_ic_name)
{
    auto params = TestInitialCondition::parameters();
    params.set<App *>("app", this->app);
    params.set<String>("name", "obj");

    this->prob->add_initial_condition<TestInitialCondition>(params);

    EXPECT_DEATH(this->prob->add_initial_condition<TestInitialCondition>(params),
                 "Cannot add initial condition object 'obj'. Name already taken.");
}

TEST_F(InitialConditionTest, constant_ic)
{
    auto params = ConstantInitialCondition::parameters();
    params.set<App *>("app", this->app);
    params.set<std::vector<Real>>("value", { 5 });
    auto ic = this->prob->add_initial_condition<ConstantInitialCondition>(params);
    this->prob->create();

    EXPECT_EQ(ic->get_num_components(), 1);

    Real time = 0.;
    Real x[] = { 0 };
    Scalar u[] = { 0 };
    ic->evaluate(time, x, u);

    EXPECT_EQ(u[0], 5);
}

TEST_F(InitialCondition2FieldTest, no_field_param)
{
    auto params = InitialCondition::parameters();
    params.set<App *>("app", this->app);
    params.set<String>("name", "obj");
    this->prob->add_initial_condition<MockInitialCondition>(params);

    EXPECT_THROW(this->prob->create(), Exception);
}

TEST_F(InitialCondition2FieldTest, non_existing_field)
{
    auto params = InitialCondition::parameters();
    params.set<App *>("app", this->app);
    params.set<String>("name", "obj");
    params.set<String>("field", "asdf");
    this->prob->add_initial_condition<MockInitialCondition>(params);

    EXPECT_DEATH(this->prob->create(), "Field 'asdf' does not exist. Typo?");
}
