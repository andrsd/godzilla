#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/LineMesh.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/NaturalBC.h"
#include "godzilla/BndResidualFunc.h"
#include "godzilla/BndJacobianFunc.h"
#include "godzilla/WeakForm.h"

using namespace godzilla;

namespace {

class TestBC : public NaturalBC {
public:
    explicit TestBC(const Parameters & params) : NaturalBC(params), comps({ 1 }) {}

    const std::vector<Int> &
    get_components() const override
    {
        return this->comps;
    }
    void
    set_up_weak_form() override
    {
    }

protected:
    std::vector<Int> comps;
};

class TestF : public ResidualFunc {
public:
    explicit TestF(GTestFENonlinearProblem * prob) : ResidualFunc(prob) {}

    void
    evaluate(Scalar f[]) const override
    {
        f[0] = 0.;
    }
};

class BndTestF : public BndResidualFunc {
public:
    explicit BndTestF(TestBC * bc) : BndResidualFunc(bc) {}

    void
    evaluate(Scalar f[]) const override
    {
        f[0] = 0.;
    }
};

class TestJ : public JacobianFunc {
public:
    explicit TestJ(GTestFENonlinearProblem * prob) : JacobianFunc(prob) {}

    void
    evaluate(Scalar g[]) const override
    {
        g[0] = 1.;
    }
};

class BndTestJ : public BndJacobianFunc {
public:
    explicit BndTestJ(TestBC * bc) : BndJacobianFunc(bc) {}

    void
    evaluate(Scalar g[]) const override
    {
        g[0] = 1.;
    }
};

} // namespace

TEST(WeakFormTest, test)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    Parameters bc_pars = NaturalBC::parameters();
    bc_pars.set<App *>("_app") = &app;
    bc_pars.set<DiscreteProblemInterface *>("_dpi") = &prob;
    bc_pars.set<std::vector<std::string>>("boundary") = { "left" };
    TestBC bc(bc_pars);

    Int fid = 0;
    WeakForm wf;
    Label label;
    wf.add(WeakForm::F0, label, 0, fid, 0, new TestF(&prob));
    wf.add(PETSC_WF_G0, label, 0, fid, fid, 0, new TestJ(&prob));
    // bnd
    wf.add(WeakForm::BND_F0, label, 0, fid, 0, new BndTestF(&bc));
    wf.add(PETSC_WF_BDG0, label, 0, fid, fid, 0, new BndTestJ(&bc));

    const auto & f0 = wf.get(WeakForm::F0, label, 0, fid, 0);
    EXPECT_EQ(f0.size(), 1);

    const auto & f1 = wf.get(WeakForm::F1, label, 0, fid, 0);
    EXPECT_EQ(f1.size(), 0);

    const auto & f0_bnd = wf.get(WeakForm::BND_F0, label, 0, fid, 0);
    EXPECT_EQ(f0_bnd.size(), 1);

    const auto & f1_bnd = wf.get(WeakForm::BND_F1, label, 0, fid, 0);
    EXPECT_EQ(f1_bnd.size(), 0);

    const auto & g0 = wf.get(PETSC_WF_G0, label, 0, fid, fid, 0);
    EXPECT_EQ(g0.size(), 1);

    const auto & g1 = wf.get(PETSC_WF_G1, label, 0, fid, fid, 0);
    EXPECT_EQ(g1.size(), 0);

    const auto & g0_bnd = wf.get(PETSC_WF_BDG0, label, 0, fid, fid, 0);
    EXPECT_EQ(g0_bnd.size(), 1);

    const auto & g1_bnd = wf.get(PETSC_WF_BDG1, label, 0, fid, fid, 0);
    EXPECT_EQ(g1_bnd.size(), 0);
}
