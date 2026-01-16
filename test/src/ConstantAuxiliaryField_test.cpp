#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/Parameters.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ConstantAuxiliaryField.h"
#include "godzilla/Types.h"

using namespace godzilla;

TEST(ConstantAuxiliaryFieldTest, create)
{
    TestApp app;

    auto mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("app", &app);
    mesh_params.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_params);

    auto prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<App *>("app", &app);
    prob_params.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_params);

    prob.set_aux_field(FieldID(0), "aux1", 1, Order(1));
    auto aux_params = ConstantAuxiliaryField::parameters();
    aux_params.set<App *>("app", &app)
        .set<String>("name", "aux1")
        .set<DiscreteProblemInterface *>("_dpi", &prob)
        .set<std::vector<Real>>("value", { 1234 });
    auto aux = prob.add_auxiliary_field<ConstantAuxiliaryField>(aux_params);

    prob.create();

    EXPECT_EQ(aux->get_field_id(), FieldID(0));
    EXPECT_EQ(aux->get_num_components(), 1);

    Real time = 0;
    Real x[1] = { 1. };
    Real u[1] = { 0 };
    aux->evaluate(time, x, u);
    EXPECT_EQ(u[0], 1234.);
}
