#include "gtest/gtest.h"
#include "App.h"
#include "LineMesh.h"
#include "FENonlinearProblem.h"
#include "AuxiliaryField.h"
#include "FENonlinearProblem_test.h"

using namespace godzilla;

TEST(AuxiliaryFieldTest, non_existent_id)
{
    testing::internal::CaptureStderr();

    TestApp app;

    InputParameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);
    mesh.create();

    InputParameters prob_pars = GTestFENonlinearProblem::valid_params();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);
    prob.add_aux_fe(0, "aux1", 1, 1);

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

    InputParameters params = AuxiliaryField::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "aux";
    params.set<FEProblemInterface *>("_fepi") = &prob;
    auto aux = TestAuxFld(params);
    prob.add_auxiliary_field(&aux);
    prob.create();

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Auxiliary field 'aux' is set on auxiliary field with ID '1', "
                                   "but such ID does not exist."));
}

TEST(AuxiliaryFieldTest, inconsistent_comp_number)
{
    testing::internal::CaptureStderr();

    TestApp app;

    InputParameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);
    mesh.create();

    InputParameters prob_pars = GTestFENonlinearProblem::valid_params();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);
    prob.add_aux_fe(0, "aux1", 1, 1);

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

    InputParameters params = AuxiliaryField::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "aux";
    params.set<FEProblemInterface *>("_fepi") = &prob;
    auto aux = TestAuxFld(params);
    prob.add_auxiliary_field(&aux);
    prob.create();

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Auxiliary field 'aux' has 2 component(s), but is set on a "
                                   "field with 1 component(s)."));
}
