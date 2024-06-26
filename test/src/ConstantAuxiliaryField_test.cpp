#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/Parameters.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ConstantAuxiliaryField.h"

using namespace godzilla;

TEST(ConstantAuxiliaryFieldTest, create)
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

    Parameters aux_params = ConstantAuxiliaryField::parameters();
    aux_params.set<App *>("_app") = &app;
    aux_params.set<std::string>("_name") = "aux1";
    aux_params.set<DiscreteProblemInterface *>("_dpi") = &prob;
    aux_params.set<std::vector<Real>>("value") = { 1234 };
    ConstantAuxiliaryField aux(aux_params);

    mesh.create();
    prob.create();
    prob.set_aux_field(0, "aux1", 1, 1);
    prob.add_auxiliary_field(&aux);

    EXPECT_EQ(aux.get_field_id(), 0);

    EXPECT_EQ(aux.get_num_components(), 1);

    Real time = 0;
    Real x[1] = { 1. };
    Real u[1] = { 0 };
    aux.evaluate(time, x, u);
    EXPECT_EQ(u[0], 1234.);
}
