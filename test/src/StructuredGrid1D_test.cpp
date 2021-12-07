#include "GodzillaApp_test.h"
#include "StructuredGrid1D.h"
#include "InputParameters.h"
#include "petsc.h"

using namespace godzilla;

TEST(StructuredGrid1DTest, api)
{
    TestApp app;

    InputParameters params = StructuredGrid1D::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<PetscInt>("nx") = 3;
    StructuredGrid1D obj(params);

    EXPECT_EQ(obj.getNx(), 3);

    obj.create();
    DM dm = obj.getDM();

    PetscInt dim;
    DMGetDimension(dm, &dim);
    EXPECT_EQ(dim, 1);
}
