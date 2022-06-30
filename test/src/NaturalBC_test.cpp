#include "gtest/gtest.h"
#include "TestApp.h"
#include "GTestFENonlinearProblem.h"
#include "LineMesh.h"
#include "NaturalBC.h"

using namespace godzilla;

TEST(NaturalBCTest, api)
{
    TestApp app;

    InputParameters mesh_params = LineMesh::valid_params();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_params);

    InputParameters prob_params = GTestFENonlinearProblem::valid_params();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_params);
    app.problem = &prob;

    class MockNaturalBC : public NaturalBC {
    public:
        explicit MockNaturalBC(const InputParameters & params) : NaturalBC(params) {}

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

    InputParameters params = NaturalBC::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    params.set<std::string>("boundary") = "left";
    MockNaturalBC bc(params);

    mesh.create();
    prob.create();
    bc.create();

    EXPECT_EQ(bc.get_num_components(), 2);
    EXPECT_EQ(bc.get_bc_type(), DM_BC_NATURAL);

    std::vector<PetscInt> comps = bc.get_components();
    EXPECT_EQ(comps[0], 3);
    EXPECT_EQ(comps[1], 5);
}

void
__f0_test_natural_bc(PetscInt dim,
                     PetscInt nf,
                     PetscInt nf_aux,
                     const PetscInt u_off[],
                     const PetscInt u_off_x[],
                     const PetscScalar u[],
                     const PetscScalar u_t[],
                     const PetscScalar u_x[],
                     const PetscInt a_off[],
                     const PetscInt a_off_x[],
                     const PetscScalar a[],
                     const PetscScalar a_t[],
                     const PetscScalar a_x[],
                     PetscReal t,
                     const PetscReal x[],
                     const PetscReal n[],
                     PetscInt num_constants,
                     const PetscScalar constants[],
                     PetscScalar f0[])
{
    f0[0] = 100 * u[0];
}

void
__g0_test_natural_bc(PetscInt dim,
                     PetscInt nf,
                     PetscInt nf_aux,
                     const PetscInt u_off[],
                     const PetscInt u_off_x[],
                     const PetscScalar u[],
                     const PetscScalar u_t[],
                     const PetscScalar u_x[],
                     const PetscInt a_off[],
                     const PetscInt a_off_x[],
                     const PetscScalar a[],
                     const PetscScalar a_t[],
                     const PetscScalar a_x[],
                     PetscReal t,
                     PetscReal u_t_shift,
                     const PetscReal x[],
                     const PetscReal n[],
                     PetscInt num_constants,
                     const PetscScalar constants[],
                     PetscScalar g0[])
{
    g0[0] = 100;
}

TEST(NaturalBCTest, fe)
{
    class TestNaturalBC : public NaturalBC {
    public:
        explicit TestNaturalBC(const InputParameters & params) : NaturalBC(params) {}

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
            set_residual_block(__f0_test_natural_bc, nullptr);
            set_jacobian_block(this->fid, __g0_test_natural_bc, nullptr, nullptr, nullptr);
        }

        PetscInt
        getBd()
        {
            return this->bd;
        }
    };

    TestApp app;

    InputParameters mesh_params = LineMesh::valid_params();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_params);

    InputParameters prob_params = GTestFENonlinearProblem::valid_params();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_params);
    app.problem = &prob;
    prob.add_aux_fe(0, "aux1", 1, 1);

    InputParameters bc_params = TestNaturalBC::valid_params();
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
    PetscWeakForm wf;
    DMBoundaryConditionType type;
    const char * name;
    DMLabel label;
    PetscInt nv;
    const PetscInt * values;
    PetscInt field;
    PetscInt nc;
    const PetscInt * comps;
    PetscDSGetBoundary(ds,
                       bc.getBd(),
                       &wf,
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
}
