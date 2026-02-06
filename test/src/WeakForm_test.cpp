#include "gmock/gmock.h"
#include "TestApp.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
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
    explicit TestBC(const Parameters & pars) : NaturalBC(pars) {}

private:
    std::vector<Int>
    create_components() override
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
    explicit TestF(Ref<GTestFENonlinearProblem> prob) : ResidualFunc(prob) {}

    void
    evaluate(Scalar f[]) const override
    {
        f[0] = 0.;
    }
};

class BndTestF : public BndResidualFunc {
public:
    explicit BndTestF(Ref<TestBC> bc) : BndResidualFunc(bc) {}

    void
    evaluate(Scalar f[]) const override
    {
        f[0] = 0.;
    }
};

class TestJ : public JacobianFunc {
public:
    explicit TestJ(Ref<GTestFENonlinearProblem> prob) : JacobianFunc(prob) {}

    void
    evaluate(Scalar g[]) const override
    {
        g[0] = 1.;
    }
};

class BndTestJ : public BndJacobianFunc {
public:
    explicit BndTestJ(Ref<TestBC> bc) : BndJacobianFunc(bc) {}

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

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<Ref<App>>("app", ref(app));
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    auto bc_pars = TestBC::parameters();
    bc_pars.set<Ref<App>>("app", ref(app));
    bc_pars.set<std::vector<String>>("boundary", { "left" });
    auto bc = prob.add_boundary_condition<TestBC>(bc_pars);

    FieldID fid(0);
    WeakForm wf;
    Label label;
    wf.add(WeakForm::F0, label, 0, fid, 0, new TestF(ref(prob)));
    wf.add(WeakForm::G0, label, 0, fid, fid, 0, new TestJ(ref(prob)));
    // bnd
    wf.add(WeakForm::BND_F0, label, 0, fid, 0, new BndTestF(bc));
    wf.add(WeakForm::BND_G0, label, 0, fid, fid, 0, new BndTestJ(bc));

    const auto & f0 = wf.get(WeakForm::F0, label, 0, fid, 0);
    EXPECT_EQ(f0.size(), 1);

    const auto & f1 = wf.get(WeakForm::F1, label, 0, fid, 0);
    EXPECT_EQ(f1.size(), 0);

    const auto & f0_bnd = wf.get(WeakForm::BND_F0, label, 0, fid, 0);
    EXPECT_EQ(f0_bnd.size(), 1);

    const auto & f1_bnd = wf.get(WeakForm::BND_F1, label, 0, fid, 0);
    EXPECT_EQ(f1_bnd.size(), 0);

    const auto & g0 = wf.get(WeakForm::G0, label, 0, fid, fid, 0);
    EXPECT_EQ(g0.size(), 1);

    const auto & g1 = wf.get(WeakForm::G1, label, 0, fid, fid, 0);
    EXPECT_EQ(g1.size(), 0);

    const auto & g0_bnd = wf.get(WeakForm::BND_G0, label, 0, fid, fid, 0);
    EXPECT_EQ(g0_bnd.size(), 1);

    const auto & g1_bnd = wf.get(WeakForm::BND_G1, label, 0, fid, fid, 0);
    EXPECT_EQ(g1_bnd.size(), 0);
}
