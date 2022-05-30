#include "gmock/gmock.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "FENonlinearProblem.h"
#include "AuxiliaryField.h"
#include "FENonlinearProblem_test.h"

using namespace godzilla;

class AuxiliaryFieldTest : public FENonlinearProblemTest {
public:
};

TEST_F(AuxiliaryFieldTest, non_existent_id)
{
    class TestAuxFld : public AuxiliaryField {
    public:
        explicit TestAuxFld(const InputParameters & params) : AuxiliaryField(params) {}
        virtual PetscInt
        get_field_id() const
        {
            return 1;
        }
        virtual PetscInt
        get_num_components() const
        {
            return 2;
        }
        virtual PetscFunc *
        get_func() const
        {
            return nullptr;
        }
    };

    testing::internal::CaptureStderr();

    prob->add_aux_fe(0, "aux1", 1, 1);

    InputParameters params = AuxiliaryField::valid_params();
    params.set<const App *>("_app") = app;
    params.set<std::string>("_name") = "aux";
    params.set<FEProblemInterface *>("_fepi") = prob;
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
        explicit TestAuxFld(const InputParameters & params) : AuxiliaryField(params) {}
        virtual PetscInt
        get_field_id() const
        {
            return 0;
        }
        virtual PetscInt
        get_num_components() const
        {
            return 2;
        }
        virtual PetscFunc *
        get_func() const
        {
            return nullptr;
        }
    };

    testing::internal::CaptureStderr();

    prob->add_aux_fe(0, "aux1", 1, 1);

    InputParameters params = AuxiliaryField::valid_params();
    params.set<const App *>("_app") = app;
    params.set<std::string>("_name") = "aux";
    params.set<FEProblemInterface *>("_fepi") = prob;
    auto aux = TestAuxFld(params);
    prob->add_auxiliary_field(&aux);

    mesh->create();
    prob->create();

    app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Auxiliary field 'aux' has 2 component(s), but is set on a "
                                   "field with 1 component(s)."));
}
