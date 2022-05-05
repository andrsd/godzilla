#include "gtest/gtest.h"
#include "App.h"
#include "Mesh.h"
#include "FENonlinearProblem.h"
#include "AuxiliaryField.h"
#include "FENonlinearProblem_test.h"

using namespace godzilla;

TEST(AuxiliaryFieldTest, non_existent_id)
{
    testing::internal::CaptureStderr();

    TestApp app;

    Mesh * mesh = nullptr;
    {
        const std::string class_name = "LineMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 2;
        mesh = app.buildObject<Mesh>(class_name, "mesh", params);
    }
    mesh->create();
    GTestFENonlinearProblem * prob = nullptr;
    {
        const std::string class_name = "GTestFENonlinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Mesh *>("_mesh") = mesh;
        prob = app.buildObject<GTestFENonlinearProblem>(class_name, "prob", params);
    }
    prob->addAuxFE(0, "aux1", 1, 1);

    class TestAuxFld : public AuxiliaryField {
    public:
        TestAuxFld(const InputParameters & params) : AuxiliaryField(params) {}
        virtual PetscInt
        getFieldId() const
        {
            return 1;
        }
        virtual void
        setUp(DM dm, DM dm_aux)
        {
        }
        virtual PetscInt
        getNumComponents() const
        {
            return 2;
        }
    };

    InputParameters params = AuxiliaryField::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "aux";
    params.set<FEProblemInterface *>("_fepi") = prob;
    auto aux = TestAuxFld(params);
    prob->addAuxiliaryField(&aux);
    prob->create();

    app.checkIntegrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Auxiliary field 'aux' is set on auxiliary field with ID '1', "
                                   "but such ID does not exist."));
}

TEST(AuxiliaryFieldTest, inconsistent_comp_number)
{
    testing::internal::CaptureStderr();

    TestApp app;

    Mesh * mesh = nullptr;
    {
        const std::string class_name = "LineMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 2;
        mesh = app.buildObject<Mesh>(class_name, "mesh", params);
    }
    mesh->create();
    GTestFENonlinearProblem * prob = nullptr;
    {
        const std::string class_name = "GTestFENonlinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Mesh *>("_mesh") = mesh;
        prob = app.buildObject<GTestFENonlinearProblem>(class_name, "prob", params);
    }
    prob->addAuxFE(0, "aux1", 1, 1);

    class TestAuxFld : public AuxiliaryField {
    public:
        TestAuxFld(const InputParameters & params) : AuxiliaryField(params) {}
        virtual PetscInt
        getFieldId() const
        {
            return 0;
        }
        virtual void
        setUp(DM dm, DM dm_aux)
        {
        }
        virtual PetscInt
        getNumComponents() const
        {
            return 2;
        }
    };

    InputParameters params = AuxiliaryField::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "aux";
    params.set<FEProblemInterface *>("_fepi") = prob;
    auto aux = TestAuxFld(params);
    prob->addAuxiliaryField(&aux);
    prob->create();

    app.checkIntegrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Auxiliary field 'aux' has 2 component(s), but is set on a "
                                   "field with 1 component(s)."));
}
