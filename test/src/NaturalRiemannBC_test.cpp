#include "gmock/gmock.h"
#include <gtest/gtest.h>
#include "godzilla/LineMesh.h"
#include "godzilla/NaturalRiemannBC.h"
#include "godzilla/ExplicitFVLinearProblem.h"
#include "TestApp.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestBC : public NaturalRiemannBC {
public:
    explicit TestBC(const Parameters & params) : NaturalRiemannBC(params), comps({ 0 }) {}

    const std::vector<Int> &
    get_components() const override
    {
        return this->comps;
    }

    MOCK_METHOD(void,
                evaluate,
                (Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG));

protected:
    std::vector<Int> comps;

public:
    static Parameters
    parameters()
    {
        Parameters params = NaturalRiemannBC::parameters();
        return params;
    }
};

class TestExplicitFVLinearProblem : public ExplicitFVLinearProblem {
public:
    explicit TestExplicitFVLinearProblem(const Parameters & params) :
        ExplicitFVLinearProblem(params)
    {
    }

protected:
    void
    set_up_fields() override
    {
        add_field(0, "u", 1);
    }

    void
    set_up_weak_form() override
    {
        set_riemann_solver(0, this, &TestExplicitFVLinearProblem::compute_flux);
    }

    void
    compute_flux(const PetscReal x[],
                 const PetscReal n[],
                 const PetscScalar u_l[],
                 const PetscScalar u_r[],
                 PetscScalar flux[])
    {
    }
};

} // namespace

TEST(NaturalRiemannBCTest, api)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = TestExplicitFVLinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    prob_pars.set<Real>("start_time") = 0.;
    prob_pars.set<Real>("end_time") = 1e-3;
    prob_pars.set<Real>("dt") = 1e-3;
    TestExplicitFVLinearProblem prob(prob_pars);
    app.set_problem(&prob);

    Parameters bc_pars = TestBC::parameters();
    bc_pars.set<App *>("_app") = &app;
    bc_pars.set<DiscreteProblemInterface *>("_dpi") = &prob;
    bc_pars.set<std::vector<std::string>>("boundary") = { "left" };
    TestBC bc(bc_pars);
    prob.add_boundary_condition(&bc);

    mesh.create();

#if PETSC_VERSION_GE(3, 21, 0)
    EXPECT_THROW(prob.create(), Exception);
#else
    prob.create();

    EXPECT_THAT(bc.get_components(), ElementsAre(0));
    EXPECT_EQ(bc.get_field_id(), 0);
#endif
}
