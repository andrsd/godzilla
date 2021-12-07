#include "GodzillaApp_test.h"
#include "StructuredGrid2D.h"
#include "InputParameters.h"
#include "petsc.h"

using namespace godzilla;

TEST(StructuredGrid2DTest, ctor)
{
    TestApp app;

    InputParameters params = StructuredGrid2D::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<PetscInt>("nx") = 3;
    params.set<PetscInt>("ny") = 4;
    StructuredGrid2D obj(params);

    EXPECT_EQ(obj.getNx(), 3);
    EXPECT_EQ(obj.getNy(), 4);

    obj.create();
    DM dm = obj.getDM();

    PetscInt dim;
    DMGetDimension(dm, &dim);
    EXPECT_EQ(dim, 2);
}
