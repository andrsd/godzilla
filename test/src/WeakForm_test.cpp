#include "gmock/gmock.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "GTestFENonlinearProblem.h"
#include "ResidualFunc.h"
#include "JacobianFunc.h"
#include "NaturalBC.h"
#include "BndResidualFunc.h"
#include "BndJacobianFunc.h"
#include "WeakForm.h"

using namespace godzilla;

namespace {

class TestBC : public NaturalBC {
public:
    explicit TestBC(const Parameters & params) : NaturalBC(params) {}

    PetscInt
    get_num_components() const override
    {
        return 1;
    }
    std::vector<PetscInt>
    get_components() const override
    {
        return { 1 };
    }
    void
    set_up_weak_form() override
    {
    }
};

class TestF : public ResidualFunc {
public:
    explicit TestF(const GTestFENonlinearProblem * prob) : ResidualFunc(prob) {}

    void
    evaluate(PetscScalar f[]) override
    {
        f[0] = 0.;
    }
};

class BndTestF : public BndResidualFunc {
public:
    explicit BndTestF(const TestBC * bc) : BndResidualFunc(bc) {}

    void
    evaluate(PetscScalar f[]) override
    {
        f[0] = 0.;
    }
};

class TestJ : public JacobianFunc {
public:
    explicit TestJ(const GTestFENonlinearProblem * prob) : JacobianFunc(prob) {}

    void
    evaluate(PetscScalar g[]) override
    {
        g[0] = 1.;
    }
};

class BndTestJ : public BndJacobianFunc {
public:
    explicit BndTestJ(const TestBC * bc) : BndJacobianFunc(bc) {}

    void
    evaluate(PetscScalar g[]) override
    {
        g[0] = 1.;
    }
};

} // namespace

TEST(WeakFormTest, test)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    Parameters bc_pars = NaturalBC::parameters();
    bc_pars.set<const App *>("_app") = &app;
    bc_pars.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    bc_pars.set<std::string>("boundary") = "left";
    TestBC bc(bc_pars);

    PetscInt fid = 0;
    WeakForm wf;
    wf.add(PETSC_WF_F0, nullptr, 0, fid, 0, new TestF(&prob));
    wf.add(PETSC_WF_G0, nullptr, 0, fid, fid, 0, new TestJ(&prob));
    // bnd
    wf.add(PETSC_WF_BDF0, nullptr, 0, fid, 0, new BndTestF(&bc));
    wf.add(PETSC_WF_BDG0, nullptr, 0, fid, fid, 0, new BndTestJ(&bc));

    const auto & f0 = wf.get(PETSC_WF_F0, nullptr, 0, fid, 0);
    EXPECT_EQ(f0.size(), 1);

    const auto & f1 = wf.get(PETSC_WF_F1, nullptr, 0, fid, 0);
    EXPECT_EQ(f1.size(), 0);

    const auto & f0_bnd = wf.get(PETSC_WF_BDF0, nullptr, 0, fid, 0);
    EXPECT_EQ(f0_bnd.size(), 1);

    const auto & f1_bnd = wf.get(PETSC_WF_BDF1, nullptr, 0, fid, 0);
    EXPECT_EQ(f1_bnd.size(), 0);

    const auto & g0 = wf.get(PETSC_WF_G0, nullptr, 0, fid, fid, 0);
    EXPECT_EQ(g0.size(), 1);

    const auto & g1 = wf.get(PETSC_WF_G1, nullptr, 0, fid, fid, 0);
    EXPECT_EQ(g1.size(), 0);

    const auto & g0_bnd = wf.get(PETSC_WF_BDG0, nullptr, 0, fid, fid, 0);
    EXPECT_EQ(g0_bnd.size(), 1);

    const auto & g1_bnd = wf.get(PETSC_WF_BDG1, nullptr, 0, fid, fid, 0);
    EXPECT_EQ(g1_bnd.size(), 0);
}
