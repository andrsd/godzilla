#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/FENonlinearProblem.h"
#include "godzilla/LineMesh.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/NaturalBC.h"
#include "godzilla/WeakForm.h"
#include "godzilla/BndResidualFunc.h"
#include "godzilla/BndJacobianFunc.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestNeumannProblem : public FENonlinearProblem {
public:
    explicit TestNeumannProblem(const Parameters & params);
    void solve() override;

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    const Int iu;
};

class F0 : public ResidualFunc {
public:
    explicit F0(TestNeumannProblem * prob) :
        ResidualFunc(prob),
        u(get_field_value("u")),
        x(get_xyz())
    {
    }

    void
    evaluate(Scalar f[]) const override
    {
        f[0] = this->u(0) + 2.0 - this->x(0) * this->x(0);
    }

protected:
    const FieldValue & u;
    const Point & x;
};

class F1 : public ResidualFunc {
public:
    explicit F1(TestNeumannProblem * prob) :
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

class G0 : public JacobianFunc {
public:
    explicit G0(TestNeumannProblem * prob) : JacobianFunc(prob) {}

    void
    evaluate(Scalar g[]) const override
    {
        g[0] = 1.;
    }
};

class G3 : public JacobianFunc {
public:
    explicit G3(TestNeumannProblem * prob) : JacobianFunc(prob), dim(get_spatial_dimension()) {}

    void
    evaluate(Scalar g[]) const override
    {
        for (Int d = 0; d < this->dim; ++d)
            g[d * this->dim + d] = 1.;
    }

protected:
    const Int & dim;
};

TestNeumannProblem::TestNeumannProblem(const Parameters & params) :
    FENonlinearProblem(params),
    iu(0)
{
}

void
TestNeumannProblem::solve()
{
    FENonlinearProblem::solve();
}

void
TestNeumannProblem::set_up_fields()
{
    Int order = 2;
    set_fe(this->iu, "u", 1, order);
}

void
TestNeumannProblem::set_up_weak_form()
{
    add_residual_block(this->iu, new F0(this), new F1(this));
    add_jacobian_block(this->iu, this->iu, new G0(this), nullptr, nullptr, new G3(this));
}

class BndF0 : public BndResidualFunc {
public:
    explicit BndF0(const NaturalBC * bc) : BndResidualFunc(bc), n(get_normal()), x(get_xyz()) {}

    void
    evaluate(Scalar f[]) const override
    {
        f[0] = -2. * this->x(0) * n(0);
    }

protected:
    const Normal & n;
    const Point & x;
};

class BndG0 : public BndJacobianFunc {
public:
    explicit BndG0(const NaturalBC * bc) : BndJacobianFunc(bc) {}

    void
    evaluate(Scalar g[]) const override
    {
        g[0] = 0.;
    }
};

class TestNeumannBC : public NaturalBC {
public:
    explicit TestNeumannBC(const Parameters & params) : NaturalBC(params), comps({ 0 }) {}

    const std::vector<Int> &
    get_components() const override
    {
        return this->comps;
    }

    void
    set_up_weak_form() override
    {
        add_residual_block(new BndF0(this), nullptr);
        add_jacobian_block(get_field_id(), new BndG0(this), nullptr, nullptr, nullptr);
    }

protected:
    std::vector<Int> comps;
};

} // namespace

TEST(NeumannProblemTest, solve)
{
    TestApp app;

    Parameters mesh_params = RectangleMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    mesh_params.set<Int>("ny") = 1;
    RectangleMesh mesh(mesh_params);

    Parameters prob_params = TestNeumannProblem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<Mesh *>("_mesh") = &mesh;
    TestNeumannProblem prob(prob_params);
    app.set_problem(&prob);

    Parameters bc_left_pars = TestNeumannBC::parameters();
    bc_left_pars.set<App *>("_app") = &app;
    bc_left_pars.set<DiscreteProblemInterface *>("_dpi") = &prob;
    bc_left_pars.set<std::string>("_name") = "bc1";
    bc_left_pars.set<std::vector<std::string>>("boundary") = { "left" };
    TestNeumannBC bc_left(bc_left_pars);
    prob.add_boundary_condition(&bc_left);

    Parameters bc_right_pars = TestNeumannBC::parameters();
    bc_right_pars.set<App *>("_app") = &app;
    bc_right_pars.set<DiscreteProblemInterface *>("_dpi") = &prob;
    bc_right_pars.set<std::string>("_name") = "bc2";
    bc_right_pars.set<std::vector<std::string>>("boundary") = { "right" };
    TestNeumannBC bc_right(bc_right_pars);
    prob.add_boundary_condition(&bc_right);

    mesh.create();
    prob.create();

    prob.solve();

    bool conv = prob.converged();
    EXPECT_EQ(conv, true);

    std::vector<Scalar> sln = { 0.0625, 0.5625, 0.,     0.25,   1., 0.,   0.25, 1,
                                0.0625, 0.5625, 0.0625, 0.5625, 0., 0.25, 1. };
    auto x = prob.get_solution_vector();
    EXPECT_EQ(x.get_size(), sln.size());
    auto * xx = x.get_array();
    for (Int i = 0; i < x.get_size(); i++)
        EXPECT_NEAR(xx[i], sln[i], 1e-14);
    x.restore_array(xx);
}
