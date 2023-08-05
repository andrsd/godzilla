#include "gmock/gmock.h"
#include "TestApp.h"
#include "GTestFENonlinearProblem.h"
#include "GTest2FieldsFENonlinearProblem.h"
#include "LineMesh.h"
#include "NaturalBC.h"
#include "WeakForm.h"
#include "BndResidualFunc.h"
#include "BndJacobianFunc.h"

using namespace godzilla;
using namespace testing;

TEST(NaturalBCTest, api)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);

    Parameters prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_params);
    app.problem = &prob;

    class MockNaturalBC : public NaturalBC {
    public:
        explicit MockNaturalBC(const Parameters & params) : NaturalBC(params), comps({ 3, 5 }) {}

        virtual const std::vector<Int> &
        get_components() const
        {
            return this->comps;
        }

        virtual void
        set_up_weak_form()
        {
        }

    protected:
        std::vector<Int> comps;
    };

    Parameters params = NaturalBC::parameters();
    params.set<const App *>("_app") = &app;
    params.set<DiscreteProblemInterface *>("_dpi") = &prob;
    params.set<std::string>("boundary") = "left";
    MockNaturalBC bc(params);

    mesh.create();
    prob.create();
    bc.create();

    std::vector<Int> comps = bc.get_components();
    EXPECT_THAT(comps, testing::ElementsAre(3, 5));
    EXPECT_EQ(bc.get_field_id(), 0);
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
    explicit TestNaturalBC(const Parameters & params) : NaturalBC(params), comps({ 0 }) {}

    virtual const std::vector<Int> &
    get_components() const
    {
        return this->comps;
    }

    virtual void
    set_up_weak_form()
    {
        set_residual_block(new TestNatF0(this), nullptr);
        set_jacobian_block(this->fid, new TestNatG0(this), nullptr, nullptr, nullptr);
    }

    WeakForm *
    get_wf() const
    {
        return this->wf;
    }

protected:
    std::vector<Int> comps;
};

} // namespace

TEST(NaturalBCTest, fe)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);

    Parameters prob_params = GTest2FieldsFENonlinearProblem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<Mesh *>("_mesh") = &mesh;
    GTest2FieldsFENonlinearProblem prob(prob_params);
    app.problem = &prob;
    prob.set_aux_fe(0, "aux1", 1, 1);

    Parameters bc_params = TestNaturalBC::parameters();
    bc_params.set<const App *>("_app") = &app;
    bc_params.set<DiscreteProblemInterface *>("_dpi") = &prob;
    bc_params.set<std::string>("_name") = "bc1";
    bc_params.set<std::string>("boundary") = "left";
    bc_params.set<std::string>("field") = "u";
    TestNaturalBC bc(bc_params);
    prob.add_boundary_condition(&bc);

    mesh.create();
    prob.create();

    PetscDS ds = prob.getDS();
    //
    Int num_bd;
    PetscDSGetNumBoundary(ds, &num_bd);
    EXPECT_EQ(num_bd, 1);
    //
    Int field = bc.get_field_id();
    WeakForm * wf = bc.get_wf();
    Int id = bc.get_ids()[0];
    Int part = 0;
    const auto & f0 = wf->get(PETSC_WF_BDF0, bc.get_label(), id, field, part);
    EXPECT_EQ(f0.size(), 1);
    EXPECT_NE(dynamic_cast<TestNatF0 *>(f0[0]), nullptr);

    const auto & g0 = wf->get(PETSC_WF_BDG0, bc.get_label(), id, field, field, part);
    EXPECT_EQ(g0.size(), 1);
    EXPECT_NE(dynamic_cast<TestNatG0 *>(g0[0]), nullptr);
}

TEST(NaturalBCTest, non_existing_field)
{
    testing::internal::CaptureStderr();

    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTest2FieldsFENonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<Mesh *>("_mesh") = &mesh;
    GTest2FieldsFENonlinearProblem problem(prob_pars);
    app.problem = &problem;

    Parameters params = TestNaturalBC::parameters();
    params.set<const App *>("_app") = &app;
    params.set<DiscreteProblemInterface *>("_dpi") = &problem;
    params.set<std::string>("field") = "asdf";
    TestNaturalBC bc(params);

    mesh.create();
    problem.add_boundary_condition(&bc);
    problem.create();

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                HasSubstr("Field 'asdf' does not exists. Typo?"));
}

TEST(NaturalBCTest, field_param_not_specified)
{
    testing::internal::CaptureStderr();

    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTest2FieldsFENonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<Mesh *>("_mesh") = &mesh;
    GTest2FieldsFENonlinearProblem problem(prob_pars);
    app.problem = &problem;

    Parameters params = TestNaturalBC::parameters();
    params.set<const App *>("_app") = &app;
    params.set<DiscreteProblemInterface *>("_dpi") = &problem;
    TestNaturalBC bc(params);

    mesh.create();
    problem.add_boundary_condition(&bc);
    problem.create();

    app.check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        HasSubstr(
            "Use the 'field' parameter to assign this boundary condition to an existing field."));
}
