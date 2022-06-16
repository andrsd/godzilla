#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "Factory.h"
#include "LineMesh.h"
#include "EssentialBC.h"
#include "PiecewiseLinear.h"

using namespace godzilla;

TEST(EssentialBCTest, api)
{
    TestApp app;

    class TestEssentialBC : public EssentialBC {
    public:
        TestEssentialBC(const InputParameters & pars) : EssentialBC(pars) {}

        MOCK_METHOD(
            void,
            evaluate,
            (PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscScalar u[]));
        MOCK_METHOD(
            void,
            evaluate_t,
            (PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscScalar u[]));

        virtual PetscInt
        get_num_components() const
        {
            return 1.;
        }
        virtual std::vector<PetscInt>
        get_components() const
        {
            return { 0 };
        }

    protected:
        virtual void
        set_up_callback()
        {
        }
    };

    InputParameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    InputParameters prob_pars = GTestFENonlinearProblem::valid_params();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem problem(prob_pars);
    app.problem = &problem;

    InputParameters params = TestEssentialBC::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<const FEProblemInterface *>("_fepi") = &problem;
    TestEssentialBC bc(params);

    mesh.create();
    problem.create();
    bc.create();

    EXPECT_EQ(bc.get_num_components(), 1);
    auto comps = bc.get_components();
    EXPECT_EQ(comps.size(), 1);
    EXPECT_EQ(comps[0], 0);
    EXPECT_EQ(bc.get_bc_type(), DM_BC_ESSENTIAL);

    PetscReal x[] = { 3 };
    PetscScalar u[] = { 0 };

    EXPECT_CALL(bc, evaluate);
    PetscFunc * fn = bc.get_function();
    (*fn)(1, 0, x, 1, u, &bc);

    EXPECT_CALL(bc, evaluate_t);
    PetscFunc * fn_t = bc.get_function_t();
    (*fn_t)(1, 0, x, 1, u, &bc);
}
