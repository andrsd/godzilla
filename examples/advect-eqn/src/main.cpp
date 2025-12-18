#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "godzilla/Init.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/Registry.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ExodusIIOutput.h"
#include "AdvectionEquation.h"
#include "InflowBC.h"
#include "OutflowBC.h"

int
main(int argc, char * argv[])
{
    try {
        mpi::Communicator comm;
        godzilla::Init init(argc, argv);

        godzilla::App app(comm, "advect-eqn");

        auto mesh_pars = LineMesh::parameters();
        mesh_pars.set<godzilla::App *>("app", &app)
            .set<Int>("nx", 5)
            .set<Real>("xmin", 0)
            .set<Real>("xmax", 1);
        auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

        auto prob_pars = AdvectionEquation::parameters();
        prob_pars.set<godzilla::App *>("app", &app)
            .set<Mesh *>("mesh", mesh.get())
            .set<Real>("dt", 1e-3)
            .set<Real>("end_time", 5e-3)
            .set<std::string>("scheme", "ssp-rk-2");
        AdvectionEquation prob(prob_pars);
        app.set_problem(&prob);

        auto bc_left_pars = InflowBC::parameters();
        bc_left_pars.set<godzilla::App *>("app", &app)
            .set<std::vector<std::string>>("boundary", { "left" })
            .set<Real>("vel", 1.);
        prob.add_boundary_condition<InflowBC>(bc_left_pars);

        auto bc_right_pars = OutflowBC::parameters();
        bc_right_pars.set<godzilla::App *>("app", &app)
            .set<std::vector<std::string>>("boundary", { "right" });
        prob.add_boundary_condition<OutflowBC>(bc_right_pars);

        auto out_pars = ExodusIIOutput::parameters();
        out_pars.set<App *>("app", &app);
        out_pars.set<fs::path>("file", "out");
        prob.add_output<ExodusIIOutput>(out_pars);

        prob.create();

        app.run();

        return 0;
    }
    catch (Exception & e) {
        fmt::println("{}", e.what());
        print_call_stack(e.get_call_stack());
        return -1;
    }
    catch (...) {
        return -1;
    }
}
