#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/LineMesh.h"
#include "godzilla/LinearProblem.h"
#include "godzilla/PCShell.h"
#include "godzilla/Preconditioner.h"
#include "godzilla/Problem.h"
#include "godzilla/Vector.h"

using namespace godzilla;

namespace {

class CustomLinearProblem : public LinearProblem {
public:
    explicit CustomLinearProblem(const Parameters & params) : LinearProblem(params) {}

    void
    create() override
    {
        DMSetNumFields(get_dm(), 1);
        Int nc[1] = { 1 };
        Int n_dofs[2] = { 1, 0 };
        this->s = Section::create(get_dm(), nc, n_dofs, 0, nullptr, nullptr, nullptr, nullptr);
        set_local_section(this->s);
        LinearProblem::create();
    }

    void
    compute_operators(Matrix & A, Matrix & B)
    {
        A.set_value(0, 0, 2.);
        A.set_value(1, 1, 3.);
        A.assemble();
    }

    void
    compute_rhs(Vector & b)
    {
        b.set_values(std::vector<Int>({ 0, 1 }), { 20, 33 });
        b.assemble();
    }

    void
    set_up_callbacks() override
    {
        set_compute_operators(this, &CustomLinearProblem::compute_operators);
        set_compute_rhs(this, &CustomLinearProblem::compute_rhs);
    }

    Preconditioner
    create_preconditioner(PC pc) override
    {
        this->pcshell = PCShell(pc);
        this->pcshell.set_apply(this, &CustomLinearProblem::apply_pc);
        this->pcshell.set_apply_transpose(this, &CustomLinearProblem::apply_transpose_pc);
        return this->pcshell;
    }

    void
    apply_pc(const Vector & x, Vector & y)
    {
        y.assign(x);
        pc_applied = true;
    }

    void
    apply_transpose_pc(const Vector & x, Vector & y)
    {
        y.assign(x);
    }

    Section s;
    PCShell pcshell;
    bool pc_applied = false;
};

} // namespace

TEST(PCShellTest, run)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 1);
    LineMesh mesh(mesh_pars);
    mesh.create();

    auto prob_pars = LinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    CustomLinearProblem prob(prob_pars);

    prob.create();
    prob.run();

    EXPECT_TRUE(prob.pc_applied);

    prob.pcshell.set_name("test_pc_shell");
    EXPECT_EQ(prob.pcshell.get_name(), "test_pc_shell");
}
