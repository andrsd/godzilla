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
        explicit TestEssentialBC(const Parameters & pars) : EssentialBC(pars), components({ 0 }) {}

        MOCK_METHOD(void, evaluate, (Int dim, Real time, const Real x[], Int nc, Scalar u[]));
        MOCK_METHOD(void, evaluate_t, (Int dim, Real time, const Real x[], Int nc, Scalar u[]));

        virtual const std::vector<Int> &
        get_components() const
        {
            return this->components;
        }

    protected:
        virtual void
        set_up_callback()
        {
        }

        std::vector<Int> components;
    };

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem problem(prob_pars);
    app.problem = &problem;

    Parameters params = TestEssentialBC::parameters();
    params.set<const App *>("_app") = &app;
    params.set<DiscreteProblemInterface *>("_dpi") = &problem;
    TestEssentialBC bc(params);

    mesh.create();
    problem.create();
    bc.create();

    const auto & comps = bc.get_components();
    EXPECT_EQ(comps.size(), 1);
    EXPECT_EQ(comps[0], 0);

    Real x[] = { 3 };
    Scalar u[] = { 0 };

    EXPECT_CALL(bc, evaluate);
    PetscFunc * fn = bc.get_function();
    (*fn)(1, 0, x, 1, u, &bc);

    EXPECT_CALL(bc, evaluate_t);
    PetscFunc * fn_t = bc.get_function_t();
    (*fn_t)(1, 0, x, 1, u, &bc);

    EXPECT_EQ(bc.get_context(), &bc);
    EXPECT_EQ(bc.get_field_id(), 0);
}
