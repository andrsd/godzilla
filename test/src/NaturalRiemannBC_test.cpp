#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "LineMesh.h"
#include "NaturalRiemannBC.h"
#include "ExplicitFVLinearProblem.h"
#include "TestApp.h"

using namespace godzilla;

namespace {

class TestBC : public NaturalRiemannBC {
public:
    explicit TestBC(const Parameters & params) : NaturalRiemannBC(params) {}

    virtual PetscInt
    get_num_components() const override
    {
        return 1;
    }

    virtual std::vector<PetscInt>
    get_components() const override
    {
        return { 0 };
    }

    virtual void
    evaluate(PetscReal time,
             const PetscReal * c,
             const PetscReal * n,
             const PetscScalar * xI,
             PetscScalar * xG) override
    {
        xG[0] = xI[0];
    }

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
    virtual void
    set_up_fields() override
    {
        add_field(0, "u", 1);
    }

    virtual void
    compute_flux(PetscInt dim,
                 PetscInt nf,
                 const PetscReal x[],
                 const PetscReal n[],
                 const PetscScalar uL[],
                 const PetscScalar uR[],
                 PetscInt n_consts,
                 const PetscScalar constants[],
                 PetscScalar flux[]) override
    {
    }
};

} // namespace

TEST(NaturalRiemannBCTest, api)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = TestExplicitFVLinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<PetscReal>("start_time") = 0.;
    prob_pars.set<PetscReal>("end_time") = 1e-3;
    prob_pars.set<PetscReal>("dt") = 1e-3;
    TestExplicitFVLinearProblem prob(prob_pars);
    app.problem = &prob;

    Parameters bc_pars = TestBC::parameters();
    bc_pars.set<const App *>("_app") = &app;
    bc_pars.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    bc_pars.set<std::string>("boundary") = "left";
    TestBC bc(bc_pars);
    prob.add_boundary_condition(&bc);

    mesh.create();
    prob.create();

    EXPECT_EQ(bc.get_num_components(), 1);
    EXPECT_THAT(bc.get_components(), testing::ElementsAre(0));
}
