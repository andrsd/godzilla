#include "gmock/gmock.h"
#include "FENonlinearProblem_test.h"
#include "godzilla/InitialCondition.h"
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

    Int
    get_num_components() const override
    {
        return 1.;
    }
    MOCK_METHOD(void, evaluate, (Real, const Real x[], Scalar u[]), ());
};

class TestInitialCondition : public InitialCondition {
public:
    explicit TestInitialCondition(const Parameters & pars) : InitialCondition(pars) {}

    Int
    get_num_components() const override
    {
        return 1.;
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

    Int
    get_num_components() const override
    {
        return 2.;
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
    params.set<App *>("_app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", this->prob);
    params.set<std::string>("_name", "obj");
    MockInitialCondition ic(params);

    EXPECT_EQ(ic.get_field_id(), FieldID::INVALID);
}

TEST_F(InitialConditionTest, test)
{
    this->mesh->create();

    this->prob->add_aux_field("a", 1, Order(1));

    auto params = InitialCondition::parameters();
    params.set<App *>("_app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", this->prob);
    params.set<std::string>("_name", "obj");
    params.set<std::string>("field", "u");
    MockInitialCondition ic(params);

    auto aux_ic_pars = InitialCondition::parameters();
    aux_ic_pars.set<App *>("_app", this->app);
    aux_ic_pars.set<DiscreteProblemInterface *>("_dpi", this->prob);
    aux_ic_pars.set<std::string>("_name", "a_ic");
    aux_ic_pars.set<std::string>("field", "a");
    MockInitialCondition aux_ic(aux_ic_pars);

    this->prob->add_initial_condition(&ic);
    this->prob->add_initial_condition(&aux_ic);
    this->prob->create();

    EXPECT_TRUE(this->app->check_integrity());
    this->app->get_logger()->print();

    EXPECT_EQ(ic.get_field_id(), FieldID(0));
    EXPECT_EQ(ic.get_dimension(), 1);

    EXPECT_TRUE(this->prob->has_initial_condition("obj"));
    EXPECT_EQ(this->prob->get_initial_condition("obj"), &ic);

    EXPECT_FALSE(this->prob->has_initial_condition("non-existent-ic"));
    EXPECT_EQ(this->prob->get_initial_condition("non-existent-ic"), nullptr);

    auto ics = this->prob->get_initial_conditions();
    EXPECT_THAT(ics, ElementsAre(&ic));

    // aux ICs

    EXPECT_EQ(aux_ic.get_field_id(), FieldID(0));

    EXPECT_TRUE(this->prob->has_initial_condition("a_ic"));
    EXPECT_EQ(this->prob->get_initial_condition("a_ic"), &aux_ic);

    auto aux_ics = this->prob->get_aux_initial_conditions();
    EXPECT_THAT(aux_ics, ElementsAre(&aux_ic));
}

TEST_F(InitialConditionTest, get_value)
{
    auto params = TestInitialCondition::parameters();
    params.set<App *>("_app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", this->prob);
    params.set<std::string>("_name", "obj");
    TestInitialCondition ic(params);
}

TEST_F(InitialConditionTest, get_vector_value)
{
    auto params = TestVectorInitialCondition::parameters();
    params.set<App *>("_app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", this->prob);
    params.set<std::string>("_name", "obj");
    TestVectorInitialCondition ic(params);
}

TEST_F(InitialConditionTest, duplicate_ic_name)
{
    auto params = TestInitialCondition::parameters();
    params.set<App *>("_app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", this->prob);
    params.set<std::string>("_name", "obj");
    TestInitialCondition ic(params);

    this->prob->add_initial_condition(&ic);

    EXPECT_THROW_MSG(this->prob->add_initial_condition(&ic),
                     "Cannot add initial condition object 'obj'. Name already taken.");
}

TEST_F(InitialCondition2FieldTest, no_field_param)
{
    auto params = InitialCondition::parameters();
    params.set<App *>("_app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", this->prob);
    params.set<std::string>("_name", "obj");
    MockInitialCondition ic(params);

    this->mesh->create();

    this->prob->add_initial_condition(&ic);
    EXPECT_THROW(this->prob->create(), Exception);
}

TEST_F(InitialCondition2FieldTest, non_existing_field)
{
    testing::internal::CaptureStderr();

    auto params = InitialCondition::parameters();
    params.set<App *>("_app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", this->prob);
    params.set<std::string>("_name", "obj");
    params.set<std::string>("field", "asdf");
    MockInitialCondition ic(params);

    this->mesh->create();

    this->prob->add_initial_condition(&ic);
    this->prob->create();

    EXPECT_FALSE(this->app->check_integrity());
    this->app->get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Field 'asdf' does not exists. Typo?"));
}
