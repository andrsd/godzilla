#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/Interpolation.h"
#include "godzilla/LineMesh.h"
#include "petscdmplex.h"
#include "petscfe.h"

using namespace godzilla;

TEST(InterpolationTest, test_1d)
{
    TestApp app;

    auto comm = app.get_comm();

    Int dim = 1;
    std::array<Real, 1> lower = { 0 };
    std::array<Real, 1> upper = { 1 };
    std::array<Int, 1> faces = { 4 };
    std::array<DMBoundaryType, 1> periodicity = { DM_BOUNDARY_GHOSTED };
    DM dm;
    PETSC_CHECK(DMPlexCreateBoxMesh(comm,
                                    1,
                                    PETSC_TRUE,
                                    faces.data(),
                                    lower.data(),
                                    upper.data(),
                                    periodicity.data(),
                                    PETSC_FALSE,
                                    &dm));
    PetscFE fe;
    PETSC_CHECK(PetscFECreateLagrange(comm, dim, 1, PETSC_TRUE, 1, PETSC_DECIDE, &fe));
    PETSC_CHECK(DMSetField(dm, 0, nullptr, (PetscObject) fe));
    PETSC_CHECK(DMCreateDS(dm));

    Vec v_sln;
    PETSC_CHECK(DMCreateGlobalVector(dm, &v_sln));
    Vector sln(v_sln);
    sln.set_values({ 0, 1, 2, 3, 4 }, { 2., 2.5, 3., 3.5, 4. });
    sln.assemble();

    Interpolation interp;
    interp.create(app.get_comm());
    interp.set_dim(dim);
    interp.set_dof(1);
    interp.add_points({ 0.125, 0.875 });
    interp.set_up(dm, false, true);

    auto coord = interp.get_coordinates();
    auto * c = coord.get_array_read();
    EXPECT_NEAR(c[0], 0.125, 1.0e-12);
    EXPECT_NEAR(c[1], 0.875, 1.0e-12);
    coord.restore_array_read(c);

    EXPECT_EQ(interp.get_dim(), 1);
    EXPECT_EQ(interp.get_dof(), 1);

    auto vals = interp.get_vector();
    interp.evaluate(dm, sln, vals);
    auto * v = vals.get_array_read();
    EXPECT_NEAR(v[0], 2.25, 1e-10);
    EXPECT_NEAR(v[1], 3.75, 1e-10);
    vals.restore_array_read(v);

    interp.restore_vector(vals);

    interp.destroy();
    PetscFEDestroy(&fe);
    DMDestroy(&dm);
}
