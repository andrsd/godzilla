#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/FENonlinearProblem.h"
#include "godzilla/Mesh.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/Types.h"
#include "godzilla/VTKOutput.h"
#include "petscdmda.h"

using namespace godzilla;

namespace {

class TestProblem : public FENonlinearProblem {
public:
    explicit TestProblem(const Parameters & pars) : FENonlinearProblem(pars), fid(FieldID::INVALID)
    {
    }

    void
    set_up_weak_form() override
    {
    }

    void
    set_up_fields() override
    {
        this->fid = add_field("fld", 1, Order(1));
    }

    FieldID fid;
};

} // namespace

TEST(VTKOutputTest, test)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 1);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = TestProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    TestProblem prob(prob_pars);

    auto pars = VTKOutput::parameters();
    pars.set<App *>("_app", &app);
    pars.set<Mesh *>("mesh", mesh.get());
    pars.set<std::string>("file", "file");
    auto out = prob.add_output<VTKOutput>(pars);

    prob.create();

    EXPECT_EQ(out->get_file_name(), "file.vtk");
    out->output_step();
}
