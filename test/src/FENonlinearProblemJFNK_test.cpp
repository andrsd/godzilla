#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/FENonlinearProblem.h"
#include "godzilla/Preconditioner.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/PCFactor.h"
#include "godzilla/Types.h"

using namespace godzilla;

namespace {

/// Test problem for simple FE solver using JFNK
class GTestFENonlinearProblemJFNK : public FENonlinearProblem {
public:
    explicit GTestFENonlinearProblemJFNK(const Parameters & pars);

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;
    void set_up_solve_type() override;
    Preconditioner create_preconditioner(PC pc) override;

    const FieldID iu;

    PCFactor p;
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
    const Dimension & dim;
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
    const Dimension & dim;
};

GTestFENonlinearProblemJFNK::GTestFENonlinearProblemJFNK(const Parameters & pars) :
    FENonlinearProblem(pars),
    iu(0)
{
}

void
GTestFENonlinearProblemJFNK::set_up_fields()
{
    set_field(this->iu, "u", 1, Order(1));
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

Preconditioner
GTestFENonlinearProblemJFNK::create_preconditioner(PC pc)
{
    this->p = PCFactor(pc);
    this->p.set_type(PCFactor::ILU);
    return this->p;
}

class DirichletBC : public EssentialBC {
public:
    explicit DirichletBC(const Parameters & pars) : EssentialBC(pars) {}

    void
    evaluate(Real time, const Real x[], Scalar u[]) override
    {
        u[0] = x[0] * x[0];
    }
};

} // namespace

TEST(FENonlinearProblemJFNKTest, solve)
{
    TestApp app;

    auto mesh_params = LineMesh::parameters();
    mesh_params.set<godzilla::App *>("_app", &app);
    mesh_params.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_params);

    auto prob_params = GTestFENonlinearProblemJFNK::parameters();
    prob_params.set<godzilla::App *>("_app", &app);
    prob_params.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblemJFNK prob(prob_params);
    app.set_problem(&prob);

    auto ic_params = ConstantInitialCondition::parameters();
    ic_params.set<godzilla::App *>("_app", &app);
    ic_params.set<DiscreteProblemInterface *>("_dpi", &prob);
    ic_params.set<std::vector<Real>>("value", { 0.1 });
    ConstantInitialCondition ic(ic_params);
    prob.add_initial_condition(&ic);

    auto bc_params = DirichletBC::parameters();
    bc_params.set<godzilla::App *>("_app", &app)
        .set<App *>("_app", &app)
        .set<std::vector<std::string>>("boundary", { "left", "right" });
    prob.add_boundary_condition<DirichletBC>(bc_params);

    prob.create();

    prob.run();

    bool conv = prob.converged();
    EXPECT_EQ(conv, true);

    auto x = prob.get_solution_vector();
    std::vector<Scalar> vals(1);
    x.get_values({ 0 }, vals);
    EXPECT_NEAR(vals[0], 0.25, 1e-9);
}
