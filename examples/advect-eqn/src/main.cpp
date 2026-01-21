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

        auto mesh_pars = app.make_parameters<LineMesh>();
        mesh_pars.set<Int>("nx", 5);
        mesh_pars.set<Real>("xmin", 0);
        mesh_pars.set<Real>("xmax", 1);
        auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

        auto prob_pars = app.make_parameters<AdvectionEquation>();
        prob_pars.set<Mesh *>("mesh", mesh.get());
        prob_pars.set<Real>("dt", 1e-3);
        prob_pars.set<Real>("end_time", 5e-3);
        auto prob = app.make_problem<AdvectionEquation>(prob_pars);

        auto bc_left_pars = app.make_parameters<InflowBC>();
        bc_left_pars.set<std::vector<String>>("boundary", { "left" });
        bc_left_pars.set<Real>("vel", 1.);
        prob->add_boundary_condition<InflowBC>(bc_left_pars);

        auto bc_right_pars = app.make_parameters<OutflowBC>();
        bc_right_pars.set<std::vector<String>>("boundary", { "right" });
        prob->add_boundary_condition<OutflowBC>(bc_right_pars);

        auto out_pars = app.make_parameters<ExodusIIOutput>();
        out_pars.set<fs::path>("file", "out");
        prob->add_output<ExodusIIOutput>(out_pars);

        prob->create();

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
