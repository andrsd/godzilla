#include "gtest/gtest.h"
#include "TestApp.h"
#include "GTestFENonlinearProblem.h"
#include "LineMesh.h"
#include "NaturalBC.h"
#include "WeakForm.h"
#include "BndResidualFunc.h"
#include "BndJacobianFunc.h"

using namespace godzilla;

TEST(NaturalBCTest, api)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_params);

    Parameters prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_params);
    app.problem = &prob;

    class MockNaturalBC : public NaturalBC {
    public:
        explicit MockNaturalBC(const Parameters & params) : NaturalBC(params) {}

        virtual PetscInt
        get_num_components() const
        {
            return 2;
        }

        virtual std::vector<PetscInt>
        get_components() const
        {
            std::vector<PetscInt> comps = { 3, 5 };
            return comps;
        }

        virtual void
        set_up_weak_form()
        {
        }
    };

    Parameters params = NaturalBC::parameters();
    params.set<const App *>("_app") = &app;
    params.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    params.set<std::string>("boundary") = "left";
    MockNaturalBC bc(params);

    mesh.create();
    prob.create();
    bc.create();

    EXPECT_EQ(bc.get_num_components(), 2);

    std::vector<PetscInt> comps = bc.get_components();
    EXPECT_EQ(comps[0], 3);
    EXPECT_EQ(comps[1], 5);
}

namespace {

class TestNatF0 : public BndResidualFunc {
public:
    explicit TestNatF0(const NaturalBC * bc) : BndResidualFunc(bc), u(get_field_value("u")) {}

    void
    evaluate(PetscScalar f[]) override
    {
        f[0] = 100 * this->u[0];
    }

protected:
    const FieldValue & u;
};

class TestNatG0 : public BndJacobianFunc {
public:
    explicit TestNatG0(const NaturalBC * bc) : BndJacobianFunc(bc) {}

    void
    evaluate(PetscScalar g[]) override
    {
        g[0] = 100;
    }
};

} // namespace

TEST(NaturalBCTest, fe)
{
    class TestNaturalBC : public NaturalBC {
    public:
        explicit TestNaturalBC(const Parameters & params) : NaturalBC(params) {}

        virtual PetscInt
        get_num_components() const
        {
            return 1;
        }

        virtual std::vector<PetscInt>
        get_components() const
        {
            std::vector<PetscInt> comps = { 0 };
            return comps;
        }

        virtual void
        set_up_weak_form()
        {
            set_residual_block(new TestNatF0(this), nullptr);
            set_jacobian_block(this->fid, new TestNatG0(this), nullptr, nullptr, nullptr);
        }

        PetscInt
        get_bd() const
        {
            return this->bd;
        }

        WeakForm *
        get_wf() const
        {
            return this->wf;
        }

        DMLabel
        get_label() const
        {
            return this->label;
        }

        const PetscInt *
        get_ids() const
        {
            return this->ids;
        }
    };

    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_params);

    Parameters prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_params);
    app.problem = &prob;
    prob.add_aux_fe(0, "aux1", 1, 1);

    Parameters bc_params = TestNaturalBC::parameters();
    bc_params.set<const App *>("_app") = &app;
    bc_params.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    bc_params.set<std::string>("_name") = "bc1";
    bc_params.set<std::string>("boundary") = "left";
    TestNaturalBC bc(bc_params);
    prob.add_boundary_condition(&bc);

    mesh.create();
    prob.create();

    PetscDS ds = prob.getDS();
    //
    PetscInt num_bd;
    PetscDSGetNumBoundary(ds, &num_bd);
    EXPECT_EQ(num_bd, 1);
    //
    DMBoundaryConditionType type;
    const char * name;
    DMLabel label;
    PetscInt nv;
    const PetscInt * values;
    PetscInt field;
    PetscInt nc;
    const PetscInt * comps;
    PetscDSGetBoundary(ds,
                       bc.get_bd(),
                       nullptr,
                       &type,
                       &name,
                       &label,
                       &nv,
                       &values,
                       &field,
                       &nc,
                       &comps,
                       nullptr,
                       nullptr,
                       nullptr);
    EXPECT_STREQ(name, "bc1");
    EXPECT_EQ(nv, 1);
    EXPECT_EQ(values[0], 1);
    EXPECT_EQ(field, 0);
    EXPECT_EQ(nc, 1);
    EXPECT_EQ(comps[0], 0);
    //
    WeakForm * wf = bc.get_wf();
    PetscInt id = bc.get_ids()[0];
    PetscInt part = 0;
    const auto & f0 = wf->get(PETSC_WF_BDF0, bc.get_label(), id, field, part);
    EXPECT_EQ(f0.size(), 1);
    EXPECT_NE(dynamic_cast<TestNatF0 *>(f0[0]), nullptr);

    const auto & g0 = wf->get(PETSC_WF_BDG0, bc.get_label(), id, field, field, part);
    EXPECT_EQ(g0.size(), 1);
    EXPECT_NE(dynamic_cast<TestNatG0 *>(g0[0]), nullptr);
}
