#include "gmock/gmock.h"
#include "godzilla/Godzilla.h"
#include "TestApp.h"
#include "godzilla/LineMesh.h"
#include "godzilla/FENonlinearProblem.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/DirichletBC.h"

using namespace godzilla;

namespace {

/// Test problem for simple FE solver using JFNK
class GTestFENonlinearProblemJFNK : public FENonlinearProblem {
public:
    explicit GTestFENonlinearProblemJFNK(const Parameters & params);

    void solve() override;

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;
    void set_up_solve_type() override;
    void set_up_preconditioning() override;

    const Int iu;
};

class F0 : public ResidualFunc {
public:
    explicit F0(GTestFENonlinearProblemJFNK * prob) : ResidualFunc(prob) {}

    void
    evaluate(Scalar f[]) const override
    {
        f[0] = 2.0;
    }
};

class F1 : public ResidualFunc {
public:
    explicit F1(GTestFENonlinearProblemJFNK * prob) :
        ResidualFunc(prob),
        dim(get_spatial_dimension()),
        u_x(get_field_gradient("u"))
    {
    }

    void
    evaluate(Scalar f[]) const override
    {
        for (Int d = 0; d < this->dim; ++d)
            f[d] = this->u_x(d);
    }

protected:
    const Int & dim;
    const FieldGradient & u_x;
};

class G3 : public JacobianFunc {
public:
    explicit G3(GTestFENonlinearProblemJFNK * prob) :
        JacobianFunc(prob),
        dim(get_spatial_dimension())
    {
    }

    void
    evaluate(Scalar g[]) const override
    {
        for (Int d = 0; d < this->dim; ++d)
            g[d * this->dim + d] = 1.;
    }

protected:
    const Int & dim;
};

REGISTER_OBJECT(GTestFENonlinearProblemJFNK);

GTestFENonlinearProblemJFNK::GTestFENonlinearProblemJFNK(const Parameters & params) :
    FENonlinearProblem(params),
    iu(0)
{
}

void
GTestFENonlinearProblemJFNK::set_up_fields()
{
    set_fe(this->iu, "u", 1, 1);
}

void
GTestFENonlinearProblemJFNK::set_up_weak_form()
{
    add_residual_block(this->iu, new F0(this), new F1(this));
    add_jacobian_preconditioner_block(this->iu, this->iu, nullptr, nullptr, nullptr, new G3(this));
}

void
GTestFENonlinearProblemJFNK::set_up_solve_type()
{
    set_use_matrix_free(true, false);
}

void
GTestFENonlinearProblemJFNK::set_up_preconditioning()
{
    PC pc;
    PETSC_CHECK(KSPGetPC(this->ksp, &pc));
    PETSC_CHECK(PCSetType(pc, PCILU));
}

void
GTestFENonlinearProblemJFNK::solve()
{
    FENonlinearProblem::solve();
}

} // namespace

TEST(FENonlinearProblemJFNKTest, solve)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<godzilla::App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);

    Parameters prob_params = GTestFENonlinearProblemJFNK::parameters();
    prob_params.set<godzilla::App *>("_app") = &app;
    prob_params.set<Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblemJFNK prob(prob_params);
    app.problem = &prob;

    Parameters ic_params = ConstantInitialCondition::parameters();
    ic_params.set<godzilla::App *>("_app") = &app;
    ic_params.set<DiscreteProblemInterface *>("_dpi") = &prob;
    ic_params.set<std::vector<Real>>("value") = { 0.1 };
    ConstantInitialCondition ic(ic_params);
    prob.add_initial_condition(&ic);

    Parameters bc_params = DirichletBC::parameters();
    bc_params.set<godzilla::App *>("_app") = &app;
    bc_params.set<App *>("_app") = &app;
    bc_params.set<DiscreteProblemInterface *>("_dpi") = &prob;
    bc_params.set<std::vector<std::string>>("boundary") = { "left", "right" };
    bc_params.set<std::vector<std::string>>("value") = { "x*x" };
    DirichletBC bc(bc_params);
    prob.add_boundary_condition(&bc);

    mesh.create();
    prob.create();

    prob.solve();

    bool conv = prob.converged();
    EXPECT_EQ(conv, true);

    auto x = prob.get_solution_vector();
    std::vector<Scalar> vals(1);
    x.get_values({ 0 }, vals);
    EXPECT_DOUBLE_EQ(vals[0], 0.25);
}
