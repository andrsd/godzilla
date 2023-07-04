#include "gmock/gmock.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "FENonlinearProblem.h"
#include "AuxiliaryField.h"
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
    auto aux = TestAuxFld(params);
    prob->add_auxiliary_field(&aux);

    mesh->create();

    EXPECT_EQ(aux.get_label(), nullptr);
    EXPECT_EQ(aux.get_region(), "rgn");
    EXPECT_EQ(aux.get_block_id(), -1);
    EXPECT_EQ(aux.get_field_id(), 0);
    EXPECT_EQ(aux.get_func(), nullptr);
    EXPECT_EQ(aux.get_context(), &aux);
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
    auto aux = TestAuxFld(params);
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
    auto aux = TestAuxFld(params);
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
    auto aux = TestAuxFld(params);
    prob->add_auxiliary_field(&aux);

    mesh->create();
    prob->create();

    app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Region 'asdf' does not exists. Typo?"));
}
