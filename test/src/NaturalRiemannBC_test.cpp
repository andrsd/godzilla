#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "LineMesh.h"
#include "NaturalRiemannBC.h"
#include "ExplicitFVLinearProblem.h"
#include "TestApp.h"

using namespace godzilla;

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
    valid_params()
    {
        Parameters params = NaturalRiemannBC::valid_params();
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

TEST(NaturalRiemannBCTest, api)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = TestExplicitFVLinearProblem::valid_params();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<PetscReal>("start_time") = 0.;
    prob_pars.set<PetscReal>("end_time") = 1e-3;
    prob_pars.set<PetscReal>("dt") = 1e-3;
    TestExplicitFVLinearProblem prob(prob_pars);
    app.problem = &prob;

    Parameters bc_pars = TestBC::valid_params();
    bc_pars.set<const App *>("_app") = &app;
    bc_pars.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    bc_pars.set<std::string>("boundary") = "left";
    TestBC bc(bc_pars);
    prob.add_boundary_condition(&bc);

    mesh.create();
    prob.create();

    EXPECT_EQ(bc.get_bc_type(), DM_BC_NATURAL_RIEMANN);
    EXPECT_EQ(bc.get_num_components(), 1);
    EXPECT_THAT(bc.get_components(), testing::ElementsAre(0));
}
