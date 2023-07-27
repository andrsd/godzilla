#include "gmock/gmock.h"
#include "TestApp.h"
#include "FENonlinearProblem.h"
#include "AuxiliaryField.h"
#include "ConstantAuxiliaryField.h"
#include "FENonlinearProblem_test.h"

using namespace godzilla;

namespace {

class AuxiliaryFieldTest : public FENonlinearProblemTest {
public:
};

} // namespace

TEST_F(AuxiliaryFieldTest, api)
{
    class TestAuxFld : public AuxiliaryField {
    public:
        explicit TestAuxFld(const Parameters & params) : AuxiliaryField(params) {}
        void
        create() override
        {
        }
        Int
        get_num_components() const override
        {
            return 1;
        }
        PetscFunc *
        get_func() const override
        {
            return nullptr;
        }
        void
        evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override
        {
        }
    };

    prob->set_aux_fe(0, "fld", 1, 1);

    Parameters params = AuxiliaryField::parameters();
    params.set<const App *>("_app") = app;
    params.set<std::string>("_name") = "aux";
    params.set<DiscreteProblemInterface *>("_dpi") = prob;
    params.set<std::string>("field") = "fld";
    params.set<std::string>("region") = "rgn";
    TestAuxFld aux(params);
    prob->add_auxiliary_field(&aux);

    mesh->create();

    EXPECT_EQ(aux.get_label(), nullptr);
    EXPECT_EQ(aux.get_region(), "rgn");
    EXPECT_EQ(aux.get_block_id(), -1);
    EXPECT_EQ(aux.get_field_id(), 0);
    EXPECT_EQ(aux.get_func(), nullptr);
    EXPECT_EQ(aux.get_context(), &aux);

    EXPECT_TRUE(prob->has_aux("aux"));
    EXPECT_FALSE(prob->has_aux("no-aux"));
    EXPECT_EQ(prob->get_aux("aux"), &aux);
    EXPECT_EQ(prob->get_aux("no-aux"), nullptr);
}

TEST_F(AuxiliaryFieldTest, non_existent_id)
{
    class TestAuxFld : public AuxiliaryField {
    public:
        explicit TestAuxFld(const Parameters & params) : AuxiliaryField(params) {}
        Int
        get_field_id() const override
        {
            return 1;
        }
        Int
        get_num_components() const override
        {
            return 2;
        }
        PetscFunc *
        get_func() const override
        {
            return nullptr;
        }
        void
        evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override
        {
        }
    };

    testing::internal::CaptureStderr();

    prob->set_aux_fe(0, "aux1", 1, 1);

    Parameters params = AuxiliaryField::parameters();
    params.set<const App *>("_app") = app;
    params.set<std::string>("_name") = "aux";
    params.set<DiscreteProblemInterface *>("_dpi") = prob;
    TestAuxFld aux(params);
    prob->add_auxiliary_field(&aux);

    mesh->create();
    prob->create();

    app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Auxiliary field 'aux' is set on auxiliary field with ID '1', "
                                   "but such ID does not exist."));
}

TEST_F(AuxiliaryFieldTest, inconsistent_comp_number)
{
    class TestAuxFld : public AuxiliaryField {
    public:
        explicit TestAuxFld(const Parameters & params) : AuxiliaryField(params) {}
        Int
        get_field_id() const override
        {
            return 0;
        }
        Int
        get_num_components() const override
        {
            return 2;
        }
        PetscFunc *
        get_func() const override
        {
            return nullptr;
        }
        void
        evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override
        {
        }
    };

    testing::internal::CaptureStderr();

    prob->set_aux_fe(0, "aux1", 1, 1);

    Parameters params = AuxiliaryField::parameters();
    params.set<const App *>("_app") = app;
    params.set<std::string>("_name") = "aux";
    params.set<DiscreteProblemInterface *>("_dpi") = prob;
    TestAuxFld aux(params);
    prob->add_auxiliary_field(&aux);

    mesh->create();
    prob->create();

    app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Auxiliary field 'aux' has 2 component(s), but is set on a "
                                   "field with 1 component(s)."));
}

TEST_F(AuxiliaryFieldTest, non_existent_region)
{
    class TestAuxFld : public AuxiliaryField {
    public:
        explicit TestAuxFld(const Parameters & params) : AuxiliaryField(params) {}
        Int
        get_field_id() const override
        {
            return 0;
        }
        Int
        get_num_components() const override
        {
            return 1;
        }
        PetscFunc *
        get_func() const override
        {
            return nullptr;
        }
        void
        evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override
        {
        }
    };

    testing::internal::CaptureStderr();

    prob->set_aux_fe(0, "aux1", 1, 1);

    Parameters params = AuxiliaryField::parameters();
    params.set<const App *>("_app") = app;
    params.set<std::string>("_name") = "aux";
    params.set<DiscreteProblemInterface *>("_dpi") = prob;
    params.set<std::string>("region") = "asdf";
    TestAuxFld aux(params);
    prob->add_auxiliary_field(&aux);

    mesh->create();
    prob->create();

    app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Region 'asdf' does not exists. Typo?"));
}

TEST_F(AuxiliaryFieldTest, name_already_taken)
{
    prob->set_aux_fe(0, "fld", 1, 1);

    Parameters params = ConstantAuxiliaryField::parameters();
    params.set<const App *>("_app") = app;
    params.set<std::string>("_name") = "aux";
    params.set<DiscreteProblemInterface *>("_dpi") = prob;
    params.set<std::vector<Real>>("value") = { 1 };
    ConstantAuxiliaryField aux(params);
    prob->add_auxiliary_field(&aux);

    EXPECT_DEATH(prob->add_auxiliary_field(&aux),
                 "Cannot add auxiliary object 'aux'. Name already taken.");
}

TEST_F(AuxiliaryFieldTest, get_value)
{
    class TestAuxFld : public AuxiliaryField {
    public:
        explicit TestAuxFld(const Parameters & params) : AuxiliaryField(params) {}
        Int
        get_field_id() const override
        {
            return 0;
        }
        Int
        get_num_components() const override
        {
            return 1;
        }
        PetscFunc *
        get_func() const override
        {
            return nullptr;
        }
        void
        evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override
        {
            u[0] = time * (x[0] + 1234);
        }
    };

    prob->set_aux_fe(0, "aux1", 1, 1);

    Parameters params = AuxiliaryField::parameters();
    params.set<const App *>("_app") = app;
    params.set<std::string>("_name") = "aux";
    params.set<DiscreteProblemInterface *>("_dpi") = prob;
    params.set<std::string>("region") = "asdf";
    TestAuxFld aux(params);
    prob->add_auxiliary_field(&aux);

    mesh->create();
    prob->create();

    DenseVector<Real, 1> coord({ 2. });
    auto val = aux.get_value(3., coord);
    EXPECT_DOUBLE_EQ(val, 3708.);
}

TEST_F(AuxiliaryFieldTest, get_vector_value)
{
    class TestAuxFld : public AuxiliaryField {
    public:
        explicit TestAuxFld(const Parameters & params) : AuxiliaryField(params) {}
        Int
        get_field_id() const override
        {
            return 0;
        }
        Int
        get_num_components() const override
        {
            return 3;
        }
        PetscFunc *
        get_func() const override
        {
            return nullptr;
        }
        void
        evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override
        {
            u[0] = x[0] + 2;
            u[1] = time;
            u[2] = 42.;
        }
    };

    prob->set_aux_fe(0, "aux1", 1, 1);

    Parameters params = AuxiliaryField::parameters();
    params.set<const App *>("_app") = app;
    params.set<std::string>("_name") = "aux";
    params.set<DiscreteProblemInterface *>("_dpi") = prob;
    params.set<std::string>("region") = "asdf";
    TestAuxFld aux(params);
    prob->add_auxiliary_field(&aux);

    mesh->create();
    prob->create();

    DenseVector<Real, 1> coord({ 3. });
    auto val = aux.get_vector_value<3>(2., coord);
    EXPECT_DOUBLE_EQ(val(0), 5.);
    EXPECT_DOUBLE_EQ(val(1), 2.);
    EXPECT_DOUBLE_EQ(val(2), 42.);
}
