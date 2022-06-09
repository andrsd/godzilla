#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "CallStack.h"
#include "Factory.h"
#include "LineMesh.h"
#include "ExplicitFELinearProblem.h"
#include "InputParameters.h"
#include "Output.h"
#include "TestApp.h"
#include "petsc.h"
#include "petscvec.h"

using namespace godzilla;

class TestExplicitFELinearProblem : public ExplicitFELinearProblem {
public:
    TestExplicitFELinearProblem(const InputParameters & params) : ExplicitFELinearProblem(params) {}

protected:
    virtual void
    set_up_fields()
    {
        add_fe(0, "u", 1, 1);
    }
    virtual void
    set_up_weak_form()
    {
    }
};

registerObject(TestExplicitFELinearProblem);

TEST(ExplicitFELinearProblemTest, wrong_scheme)
{
    testing::internal::CaptureStderr();

    TestApp app;
    LineMesh * mesh;
    TestExplicitFELinearProblem * prob;

    {
        const std::string class_name = "LineMesh";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<PetscInt>("nx") = 2;
        mesh = app.build_object<LineMesh>(class_name, "mesh", params);
    }

    {
        const std::string class_name = "TestExplicitFELinearProblem";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        params->set<PetscReal>("start_time") = 0.;
        params->set<PetscReal>("end_time") = 20;
        params->set<PetscReal>("dt") = 5;
        params->set<std::string>("scheme") = "asdf";
        prob = app.build_object<TestExplicitFELinearProblem>(class_name, "prob", params);
    }

    mesh->create();
    prob->create();
    prob->check();

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'scheme' parameter can be either 'euler', 'ssp' or 'rk'."));
}
