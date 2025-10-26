#include <gmock/gmock.h>
#include "TestApp.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/Parameters.h"
#include "godzilla/UnstructuredMesh.h"

using namespace godzilla;

TEST(MeshFactoryTest, create)
{
    TestApp app;

    auto pars = LineMesh::parameters();
    // clang-format off
    pars.set<godzilla::App *>("app", & app)
        .set<Int>("nx", 10)
        .set<Real>("xmin", 0.5)
        .set<Real>("xmax", 4);
    // clang-format on
    auto mesh = MeshFactory::create<LineMesh>(pars);

    ASSERT_TRUE(mesh != nullptr);
    EXPECT_EQ(mesh->get_num_cells(), 10);
}
