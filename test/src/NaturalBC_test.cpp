#include "gmock/gmock.h"
#include "TestApp.h"
#include "GTestFENonlinearProblem.h"
#include "GTest2FieldsFENonlinearProblem.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/NaturalBC.h"
#include "godzilla/WeakForm.h"
#include "godzilla/BndResidualFunc.h"
#include "godzilla/BndJacobianFunc.h"

using namespace godzilla;
using namespace testing;

TEST(NaturalBCTest, api)
{
    TestApp app;

    auto mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("app", &app);
    mesh_params.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_params);

    auto prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<App *>("app", &app);
    prob_params.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_params);
    app.set_problem(&prob);

    class MockNaturalBC : public NaturalBC {
    public:
        explicit MockNaturalBC(const Parameters & pars) : NaturalBC(pars) {}

        std::vector<Int>
        create_components() override
        {
            return { 3, 5 };
        }

        void
        set_up_weak_form() override
        {
        }
    };

    auto params = NaturalBC::parameters();
    params.set<App *>("app", &app);
    params.set<DiscreteProblemInterface *>("_dpi", &prob);
    params.set<std::vector<String>>("boundary", { "left" });
    MockNaturalBC bc(params);

    prob.create();
    bc.create();

    std::vector<Int> comps = bc.get_components();
    EXPECT_THAT(comps, testing::ElementsAre(3, 5));
    EXPECT_EQ(bc.get_field_id(), FieldID(0));
}

namespace {

class TestNatF0 : public BndResidualFunc {
public:
    explicit TestNatF0(const NaturalBC * bc) : BndResidualFunc(bc), u(get_field_value("u")) {}

    void
    evaluate(Scalar f[]) const override
    {
        f[0] = 100 * this->u(0);
    }

protected:
    const FieldValue & u;
};

class TestNatG0 : public BndJacobianFunc {
public:
    explicit TestNatG0(const NaturalBC * bc) : BndJacobianFunc(bc) {}

    void
    evaluate(Scalar g[]) const override
    {
        g[0] = 100;
    }
};

class TestNaturalBC : public NaturalBC {
public:
    explicit TestNaturalBC(const Parameters & pars) : NaturalBC(pars) {}

    std::vector<Int>
    create_components() override
    {
        return { 0 };
    }

    void
    set_up_weak_form() override
    {
        add_residual_block(new TestNatF0(this), nullptr);
        add_jacobian_block(get_field_id(), new TestNatG0(this), nullptr, nullptr, nullptr);
    }
};

} // namespace

TEST(NaturalBCTest, fe)
{
    TestApp app;

    auto mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("app", &app);
    mesh_params.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_params);

    auto prob_params = GTest2FieldsFENonlinearProblem::parameters();
    prob_params.set<App *>("app", &app);
    prob_params.set<Mesh *>("mesh", mesh.get());
    GTest2FieldsFENonlinearProblem prob(prob_params);
    app.set_problem(&prob);

    auto bc_params = TestNaturalBC::parameters();
    bc_params.set<App *>("app", &app);
    bc_params.set<String>("name", "bc1");
    bc_params.set<std::vector<String>>("boundary", { "left" });
    bc_params.set<String>("field", "u");
    auto bc = prob.add_boundary_condition<TestNaturalBC>(bc_params);

    prob.set_aux_field(FieldID(0), "aux1", 1, Order(1));
    prob.create();

    PetscDS ds = prob.get_ds();
    //
    Int num_bd;
    PetscDSGetNumBoundary(ds, &num_bd);
    EXPECT_EQ(num_bd, 1);
    //
    auto field = bc->get_field_id();
    auto & wf = prob.get_weak_form();
    auto label = mesh->get_label("left");
    auto ids = label.get_values();

    const auto & f0 = wf.get(WeakForm::BND_F0, label, ids[0], field, 0);
    EXPECT_EQ(f0.size(), 1);
    EXPECT_NE(dynamic_cast<TestNatF0 *>(f0[0]), nullptr);

    const auto & g0 = wf.get(WeakForm::BND_G0, label, ids[0], field, field, 0);
    EXPECT_EQ(g0.size(), 1);
    EXPECT_NE(dynamic_cast<TestNatG0 *>(g0[0]), nullptr);
}

TEST(NaturalBCTest, non_existing_field)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTest2FieldsFENonlinearProblem::parameters();
    prob_pars.set<App *>("app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTest2FieldsFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    auto params = TestNaturalBC::parameters();
    params.set<App *>("app", &app);
    params.set<std::vector<String>>("boundary", { "left" });
    params.set<String>("field", "asdf");
    problem.add_boundary_condition<TestNaturalBC>(params);

    EXPECT_DEATH(problem.create(), "Field 'asdf' does not exists. Typo?");
}

TEST(NaturalBCTest, field_param_not_specified)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTest2FieldsFENonlinearProblem::parameters();
    prob_pars.set<App *>("app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTest2FieldsFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    auto params = TestNaturalBC::parameters();
    params.set<App *>("app", &app);
    params.set<std::vector<String>>("boundary", { "left" });
    problem.add_boundary_condition<TestNaturalBC>(params);

    EXPECT_DEATH(
        problem.create(),
        "Use the 'field' parameter to assign this boundary condition to an existing field.");
}
