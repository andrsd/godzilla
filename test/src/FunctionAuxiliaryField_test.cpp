#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/Parameters.h"
#include "godzilla/LineMesh.h"
#include "godzilla/FunctionAuxiliaryField.h"

using namespace godzilla;

TEST(FunctionAuxiliaryFieldTest, create)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);

    Parameters prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    GTestFENonlinearProblem prob(prob_params);
    app.set_problem(&prob);

    Parameters aux_params = FunctionAuxiliaryField::parameters();
    aux_params.set<App *>("_app") = &app;
    aux_params.set<std::string>("_name") = "aux1";
    aux_params.set<DiscreteProblemInterface *>("_dpi") = &prob;
    aux_params.set<std::vector<std::string>>("value") = { "1234" };
    FunctionAuxiliaryField aux(aux_params);

    mesh.create();
    prob.set_aux_field(0, "aux1", 1, 1);
    prob.add_auxiliary_field(&aux);
    prob.create();

    EXPECT_EQ(aux.get_field_id(), 0);

    EXPECT_EQ(aux.get_num_components(), 1);

    Real time = 0;
    Real x[1] = { 1. };
    Real u[1] = { 0 };
    aux.evaluate(time, x, u);
    EXPECT_EQ(u[0], 1234.);
}
