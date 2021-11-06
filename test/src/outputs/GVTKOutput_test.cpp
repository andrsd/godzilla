#include "base/GodzillaApp_test.h"
#include "outputs/GVTKOutput_test.h"
#include "petsc.h"


TEST_F(GVTKOutputTest, output)
{
    testing::internal::CaptureStderr();

    auto prob = gProblem();
    prob->create();
    auto exec = gExecutioner(prob);

    auto out1 = gOutput(prob, "out.vtk");
    // TODO: when we have simple FE testing problem, we need to use it here
    // FE problem uses DMPLEX and the output will work. GTestPetscLinearProblem
    // uses DA and the VTK ouputer does not work with that. That's why we see
    // PETSc ERROR
    out1->output();
}
