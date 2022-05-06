#include "GodzillaConfig.h"
#include "Factory.h"
#include "FENonlinearProblem_test.h"
#include "InputParameters.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include "petsc.h"
#include "petscvec.h"

using namespace ::testing;
using namespace godzilla;

registerObject(GTestFENonlinearProblem);
registerObject(GTest2FieldsFENonlinearProblem);
registerObject(GTest2CompIC);

static void
f0_u(PetscInt dim,
     PetscInt Nf,
     PetscInt NfAux,
     const PetscInt uOff[],
     const PetscInt uOff_x[],
     const PetscScalar u[],
     const PetscScalar u_t[],
     const PetscScalar u_x[],
     const PetscInt aOff[],
     const PetscInt aOff_x[],
     const PetscScalar a[],
     const PetscScalar a_t[],
     const PetscScalar a_x[],
     PetscReal t,
     const PetscReal x[],
     PetscInt numConstants,
     const PetscScalar constants[],
     PetscScalar f0[])
{
    f0[0] = 2.0;
}

/* gradU[comp*dim+d] = {u_x, u_y} or {u_x, u_y, u_z} */
static void
f1_u(PetscInt dim,
     PetscInt Nf,
     PetscInt NfAux,
     const PetscInt uOff[],
     const PetscInt uOff_x[],
     const PetscScalar u[],
     const PetscScalar u_t[],
     const PetscScalar u_x[],
     const PetscInt aOff[],
     const PetscInt aOff_x[],
     const PetscScalar a[],
     const PetscScalar a_t[],
     const PetscScalar a_x[],
     PetscReal t,
     const PetscReal x[],
     PetscInt numConstants,
     const PetscScalar constants[],
     PetscScalar f1[])
{
    PetscInt d;
    for (d = 0; d < dim; ++d)
        f1[d] = u_x[d];
}

/* < \nabla v, \nabla u + {\nabla u}^T >
   This just gives \nabla u, give the perdiagonal for the transpose */
static void
g3_uu(PetscInt dim,
      PetscInt Nf,
      PetscInt NfAux,
      const PetscInt uOff[],
      const PetscInt uOff_x[],
      const PetscScalar u[],
      const PetscScalar u_t[],
      const PetscScalar u_x[],
      const PetscInt aOff[],
      const PetscInt aOff_x[],
      const PetscScalar a[],
      const PetscScalar a_t[],
      const PetscScalar a_x[],
      PetscReal t,
      PetscReal u_tShift,
      const PetscReal x[],
      PetscInt numConstants,
      const PetscScalar constants[],
      PetscScalar g3[])
{
    PetscInt d;
    for (d = 0; d < dim; ++d)
        g3[d * dim + d] = 1.0;
}

TEST_F(FENonlinearProblemTest, fields)
{
    mesh->create();
    prob->create();

    EXPECT_EQ(prob->getFieldName(0), "u");
    EXPECT_EQ(prob->getFieldId("u"), 0);
    EXPECT_EQ(prob->hasFieldById(0), true);
    EXPECT_EQ(prob->hasFieldByName("u"), true);

    EXPECT_DEATH(prob->getFieldName(65536), "error: Field with ID = '65536' does not exist.");
    EXPECT_DEATH(prob->getFieldId("nonexistent"),
                 "error: Field 'nonexistent' does not exist\\. Typo\\?");
    EXPECT_EQ(prob->hasFieldById(65536), false);
    EXPECT_EQ(prob->hasFieldByName("nonexistent"), false);
}

TEST_F(FENonlinearProblemTest, add_duplicate_field_id)
{
    prob->addFE(0, "first", 1, 1);
    EXPECT_DEATH(prob->addFE(0, "second", 1, 1),
                 "error: Cannot add field 'second' with ID = 0. ID already exists.");
}

TEST_F(FENonlinearProblemTest, get_aux_fields)
{
    mesh->create();
    prob->addAuxFE(0, "aux_one", 1, 1);
    prob->create();

    EXPECT_EQ(prob->getAuxFieldName(0), "aux_one");
    EXPECT_EQ(prob->getAuxFieldId("aux_one"), 0);
    EXPECT_EQ(prob->hasAuxFieldById(0), true);
    EXPECT_EQ(prob->hasAuxFieldByName("aux_one"), true);

    EXPECT_DEATH(prob->getAuxFieldName(1), "error: Auxiliary field with ID = '1' does not exist.");
    EXPECT_DEATH(prob->getAuxFieldId("aux_two"),
                 "error: Auxiliary field 'aux_two' does not exist\\. Typo\\?");
    EXPECT_EQ(prob->hasAuxFieldById(1), false);
    EXPECT_EQ(prob->hasAuxFieldByName("aux_two"), false);
}

TEST_F(FENonlinearProblemTest, add_duplicate_aux_field_id)
{
    prob->addAuxFE(0, "first", 1, 1);
    EXPECT_DEATH(prob->addAuxFE(0, "second", 1, 1),
                 "error: Cannot add auxiliary field 'second' with ID = 0. ID is already taken.");
}

TEST_F(FENonlinearProblemTest, solve)
{
    {
        const std::string class_name = "ConstantIC";
        InputParameters * params = Factory::getValidParams(class_name);
        params->set<std::vector<PetscReal>>("value") = { 0.1 };
        auto ic = this->app->buildObject<InitialCondition>(class_name, "ic", params);
        prob->addInitialCondition(ic);
    }

    {
        const std::string class_name = "DirichletBC";
        InputParameters * params = Factory::getValidParams(class_name);
        params->set<const App *>("_app") = this->app;
        params->set<std::string>("boundary") = "marker";
        params->set<std::vector<std::string>>("value") = { "x*x" };
        auto bc = this->app->buildObject<BoundaryCondition>(class_name, "bc", params);
        prob->addBoundaryCondition(bc);
    }

    mesh->create();
    prob->create();

    prob->solve();

    bool conv = prob->converged();
    EXPECT_EQ(conv, true);

    const Vec x = prob->getSolutionVector();
    PetscInt ni = 1;
    PetscInt ix[1] = { 0 };
    PetscScalar xx[1];
    VecGetValues(x, ni, ix, xx);
    EXPECT_DOUBLE_EQ(xx[0], 0.25);
}

TEST_F(FENonlinearProblemTest, solve_no_ic)
{
    {
        const std::string class_name = "DirichletBC";
        InputParameters * params = Factory::getValidParams(class_name);
        params->set<std::string>("boundary") = "marker";
        params->set<std::vector<std::string>>("value") = { "x*x" };
        auto bc = this->app->buildObject<BoundaryCondition>(class_name, "bc", params);
        prob->addBoundaryCondition(bc);
    }

    mesh->create();
    prob->create();

    const Vec x = prob->getSolutionVector();
    PetscInt ni = 1;
    PetscInt ix[1] = { 0 };
    PetscScalar xx[1];
    VecGetValues(x, ni, ix, xx);
    EXPECT_DOUBLE_EQ(xx[0], 0.);
}

TEST_F(FENonlinearProblemTest, err_ic_comp_mismatch)
{
    testing::internal::CaptureStderr();

    {
        const std::string class_name = "GTest2CompIC";
        InputParameters * params = Factory::getValidParams(class_name);
        auto ic = this->app->buildObject<InitialCondition>(class_name, "ic", params);
        prob->addInitialCondition(ic);
    }
    mesh->create();
    prob->create();
    this->app->checkIntegrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("Initial condition 'ic' operates on 2 components, but is set on a field "
                           "with 1 components."));
}

TEST_F(FENonlinearProblemTest, err_duplicate_ics)
{
    testing::internal::CaptureStderr();

    {
        const std::string class_name = "ConstantIC";
        InputParameters * params = Factory::getValidParams(class_name);
        params->set<std::vector<PetscReal>>("value") = { 0.1 };
        auto ic = this->app->buildObject<InitialCondition>(class_name, "ic1", params);
        prob->addInitialCondition(ic);
    }
    const std::string class_name = "ConstantIC";
    InputParameters * params = Factory::getValidParams(class_name);
    params->set<std::vector<PetscReal>>("value") = { 0.2 };
    auto ic = this->app->buildObject<InitialCondition>(class_name, "ic2", params);
    prob->addInitialCondition(ic);
    this->app->checkIntegrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Initial condition 'ic2' is being applied to a field that already has an "
                    "initial condition."));
}

TEST(TwoFieldFENonlinearProblemTest, err_not_enough_ics)
{
    testing::internal::CaptureStderr();

    class TestApp : public App {
    public:
        TestApp() : App("godzilla", MPI_COMM_WORLD) {}

        virtual void
        checkIntegrity()
        {
            if (this->log.getNumEntries() > 0)
                this->log.print();
        }
    } app;
    Mesh * mesh;
    FENonlinearProblem * prob;

    {
        const std::string class_name = "LineMesh";
        InputParameters * params = Factory::getValidParams(class_name);
        params->set<PetscInt>("nx") = 2;
        mesh = app.buildObject<Mesh>(class_name, "mesh", params);
    }
    {
        const std::string class_name = "GTest2FieldsFENonlinearProblem";
        InputParameters * params = Factory::getValidParams(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        prob = app.buildObject<FENonlinearProblem>(class_name, "prob", params);
    }

    {
        const std::string class_name = "ConstantIC";
        InputParameters * params = Factory::getValidParams(class_name);
        params->set<std::vector<PetscReal>>("value") = { 0.1 };
        auto ic = app.buildObject<InitialCondition>(class_name, "ic1", params);
        prob->addInitialCondition(ic);
    }

    mesh->create();
    prob->create();
    app.checkIntegrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Provided 2 field(s), but 1 initial condition(s)."));
}

TEST_F(FENonlinearProblemTest, err_nonexisting_bc_bnd)
{
    testing::internal::CaptureStderr();

    {
        const std::string class_name = "DirichletBC";
        InputParameters * params = Factory::getValidParams(class_name);
        params->set<std::string>("boundary") = "asdf";
        params->set<std::vector<std::string>>("value") = { "0.1" };
        auto bc = this->app->buildObject<BoundaryCondition>(class_name, "bc1", params);
        prob->addBoundaryCondition(bc);
    }

    mesh->create();
    prob->create();
    this->app->checkIntegrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Boundary condition 'bc1' is set on boundary 'asdf' which does not exist "
                    "in the mesh."));
}

TEST_F(FENonlinearProblemTest, compute_residual_callback)
{
    Vec x = nullptr;
    Vec f = nullptr;
    EXPECT_EQ(this->prob->computeResidualCallback(x, f), 0);
}

TEST_F(FENonlinearProblemTest, compute_jacobian_callback)
{
    Vec x = nullptr;
    Mat J = nullptr;
    EXPECT_EQ(this->prob->computeJacobianCallback(x, J, J), 0);
}

TEST_F(FENonlinearProblemTest, set_constant)
{
    std::vector<PetscReal> consts = { 5, 3, 1 };
    this->prob->setConstants(consts);

    auto & k = this->prob->getConstants();
    EXPECT_EQ(k[0], 5);
    EXPECT_EQ(k[1], 3);
    EXPECT_EQ(k[2], 1);
}

TEST_F(FENonlinearProblemTest, set_constant_2)
{
    this->mesh->create();
    this->prob->create();
    std::vector<PetscReal> consts = { 5, 3, 1 };
    this->prob->setConstants(consts);
    this->prob->setUpConstants();
    PetscDS ds = this->prob->getDS();
    PetscInt n_consts;
    const PetscReal * cs;
    PetscDSGetConstants(ds, &n_consts, &cs);
    EXPECT_EQ(n_consts, 3);
    EXPECT_EQ(cs[0], 5);
    EXPECT_EQ(cs[1], 3);
    EXPECT_EQ(cs[2], 1);
}

// GTestFENonlinearProblem

GTestFENonlinearProblem::GTestFENonlinearProblem(const InputParameters & params) :
    FENonlinearProblem(params),
    iu(0)
{
}

GTestFENonlinearProblem::~GTestFENonlinearProblem() {}

void
GTestFENonlinearProblem::onSetFields()
{
    _F_;
    PetscInt order = 1;
    addFE(this->iu, "u", 1, order);
}

void
GTestFENonlinearProblem::onSetWeakForm()
{
    setResidualBlock(this->iu, f0_u, f1_u);
    setJacobianBlock(this->iu, this->iu, NULL, NULL, NULL, g3_uu);
}

PetscErrorCode
GTestFENonlinearProblem::computeResidualCallback(Vec x, Vec f)
{
    return FENonlinearProblem::computeResidualCallback(x, f);
}

PetscErrorCode
GTestFENonlinearProblem::computeJacobianCallback(Vec x, Mat J, Mat Jp)
{
    return FENonlinearProblem::computeJacobianCallback(x, J, Jp);
}

//

GTest2FieldsFENonlinearProblem::GTest2FieldsFENonlinearProblem(const InputParameters & params) :
    GTestFENonlinearProblem(params),
    iv(1)
{
}

void
GTest2FieldsFENonlinearProblem::onSetFields()
{
    GTestFENonlinearProblem::onSetFields();
    addFE(this->iv, "v", 1, 1);
}
