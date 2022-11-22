#include "gmock/gmock.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "GTestImplicitFENonlinearProblem.h"
#include "ResidualFunc.h"

using namespace godzilla;

namespace {

class TestF : public ResidualFunc {
public:
    explicit TestF(const GTestImplicitFENonlinearProblem * prob) : ResidualFunc(prob) {}

    void
    evaluate(PetscScalar f[]) override
    {
        f[0] = 0.;
    }

    const FEProblemInterface *
    get_fe_problem() const
    {
        return ResidualFunc::get_fe_problem();
    }

    const PetscInt &
    get_spatial_dimension() const
    {
        return ResidualFunc::get_spatial_dimension();
    }

    const PetscScalar *
    get_field_value(const std::string & field_name) const
    {
        return ResidualFunc::get_field_value(field_name);
    }

    const PetscScalar *
    get_field_gradient(const std::string & field_name) const
    {
        return ResidualFunc::get_field_gradient(field_name);
    }

    const PetscScalar *
    get_field_dot(const std::string & field_name) const
    {
        return ResidualFunc::get_field_dot(field_name);
    }

    const PetscReal &
    get_time() const
    {
        return ResidualFunc::get_time();
    }
};

} // namespace

TEST(ResidualFuncTest, test)
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

    TestF res(&prob);
    EXPECT_EQ(res.get_fe_problem(), &prob);
    EXPECT_EQ(res.get_spatial_dimension(), 1);
    const PetscScalar * val = res.get_field_value("u");
    EXPECT_EQ(val[0], 0);
    const PetscScalar * grad = res.get_field_gradient("u");
    EXPECT_EQ(grad[0], 0);
    const PetscScalar * dot = res.get_field_dot("u");
    EXPECT_EQ(dot[0], 0);
    EXPECT_EQ(res.get_time(), 20.);
}
