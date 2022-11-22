#include "gmock/gmock.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "GTestImplicitFENonlinearProblem.h"
#include "JacobianFunc.h"

using namespace godzilla;

namespace {

class TestJ : public JacobianFunc {
public:
    explicit TestJ(const GTestImplicitFENonlinearProblem * prob) : JacobianFunc(prob) {}

    void
    evaluate(PetscScalar g[]) override
    {
        g[0] = 1.;
    }

    const FEProblemInterface *
    get_fe_problem() const
    {
        return JacobianFunc::get_fe_problem();
    }

    const PetscInt &
    get_spatial_dimension() const
    {
        return JacobianFunc::get_spatial_dimension();
    }

    const PetscScalar *
    get_field_value(const std::string & field_name) const
    {
        return JacobianFunc::get_field_value(field_name);
    }

    const PetscScalar *
    get_field_gradient(const std::string & field_name) const
    {
        return JacobianFunc::get_field_gradient(field_name);
    }

    const PetscReal &
    get_time_shift() const
    {
        return JacobianFunc::get_time_shift();
    }

    const PetscReal &
    get_time() const
    {
        return JacobianFunc::get_time();
    }
};

} // namespace

TEST(JacobianFuncTest, test)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestImplicitFENonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<PetscReal>("start_time") = 0.;
    prob_pars.set<PetscReal>("end_time") = 20;
    prob_pars.set<PetscReal>("dt") = 5;
    GTestImplicitFENonlinearProblem prob(prob_pars);

    mesh.create();
    prob.create();
    prob.set_up_initial_guess();
    prob.solve();

    TestJ jac(&prob);
    EXPECT_EQ(jac.get_fe_problem(), &prob);
    EXPECT_EQ(jac.get_spatial_dimension(), 1);
    const PetscScalar * val = jac.get_field_value("u");
    EXPECT_EQ(val[0], 0);
    const PetscScalar * grad = jac.get_field_gradient("u");
    EXPECT_EQ(grad[0], 0);
    const PetscReal & time_shift = jac.get_time_shift();
    EXPECT_EQ(time_shift, 0.);
    EXPECT_EQ(jac.get_time(), 20.);
}
