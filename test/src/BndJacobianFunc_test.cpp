#include "gmock/gmock.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "ImplicitFENonlinearProblem.h"
#include "NaturalBC.h"
#include "BndJacobianFunc.h"

using namespace godzilla;
using namespace testing;

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
        return { 0 };
    }

protected:
    void
    set_up_weak_form() override
    {
    }
};

class GTestProblem : public ImplicitFENonlinearProblem {
public:
    explicit GTestProblem(const Parameters & params) : ImplicitFENonlinearProblem(params) {}

    MOCK_METHOD(const PetscInt &, get_spatial_dimension, (), (const));
    MOCK_METHOD(const PetscScalar *, get_field_value, (const std::string & field_name), (const));
    MOCK_METHOD(const PetscScalar *, get_field_gradient, (const std::string & field_name), (const));
    MOCK_METHOD(const PetscScalar *, get_field_dot, (const std::string & field_name), (const));
    MOCK_METHOD(const PetscReal &, get_time_shift, (), (const));
    MOCK_METHOD(const PetscReal &, get_time, (), (const));
    MOCK_METHOD(PetscReal * const &, get_normal, (), (const));
    MOCK_METHOD(PetscReal * const &, get_xyz, (), (const));

protected:
    void
    set_up_fields() override
    {
        add_fe(0, "u", 1, 1);
    }

    void
    set_up_weak_form() override
    {
    }
};

class TestJ : public BndJacobianFunc {
public:
    explicit TestJ(const TestBC * bc) :
        BndJacobianFunc(bc),
        dim(get_spatial_dimension()),
        u(get_field_value("u")),
        u_x(get_field_gradient("u")),
        t(get_time()),
        t_shift(get_time_shift()),
        normal(get_normal()),
        xyz(get_xyz())
    {
    }

    void
    evaluate(PetscScalar f[]) override
    {
        f[0] = 0.;
    }

protected:
    const PetscInt & dim;
    const PetscScalar * u;
    const PetscScalar * u_x;
    const PetscReal & t;
    const PetscReal & t_shift;
    PetscReal * const & normal;
    PetscReal * const & xyz;
};

} // namespace

TEST(BndJacobianFuncTest, test)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<PetscReal>("start_time") = 0.;
    prob_pars.set<PetscReal>("end_time") = 20;
    prob_pars.set<PetscReal>("dt") = 5;
    GTestProblem prob(prob_pars);
    app.problem = &prob;

    Parameters bc_pars = NaturalBC::parameters();
    bc_pars.set<const App *>("_app") = &app;
    bc_pars.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    bc_pars.set<std::string>("field") = "u";
    bc_pars.set<std::string>("boundary") = "marker";
    TestBC bc(bc_pars);
    prob.add_boundary_condition(&bc);

    mesh.create();
    prob.create();
    bc.create();

    PetscInt dim;
    EXPECT_CALL(prob, get_spatial_dimension()).Times(1).WillOnce(ReturnRef(dim));
    EXPECT_CALL(prob, get_field_value(_)).Times(1).WillOnce(ReturnNull());
    EXPECT_CALL(prob, get_field_gradient(_)).Times(1).WillOnce(ReturnNull());
    PetscReal time;
    EXPECT_CALL(prob, get_time()).Times(1).WillOnce(ReturnRef(time));
    PetscReal time_shift;
    EXPECT_CALL(prob, get_time_shift()).Times(1).WillOnce(ReturnRef(time_shift));
    EXPECT_CALL(prob, get_normal()).Times(1).WillOnce(ReturnNull());
    EXPECT_CALL(prob, get_xyz()).Times(1).WillOnce(ReturnNull());

    TestJ jac(&bc);
}