#include "godzilla/App.h"
#include "godzilla/Init.h"
#include "godzilla/Registry.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ExodusIIOutput.h"
#include "AdvectionEquation.h"
#include "InflowBC.h"
#include "OutflowBC.h"

godzilla::Registry registry;

void
register_objects(godzilla::Registry & r)
{
    godzilla::App::register_objects(r);
    REGISTER_OBJECT(r, AdvectionEquation);
    REGISTER_OBJECT(r, InflowBC);
    REGISTER_OBJECT(r, OutflowBC);
}

int
main(int argc, char * argv[])
{
    try {
        mpi::Communicator comm(MPI_COMM_WORLD);
        godzilla::Init init(argc, argv);
        register_objects(registry);

        godzilla::App app(comm, registry, "advect-eqn");

        auto mesh_pars = LineMesh::parameters();
        mesh_pars.set<godzilla::App *>("_app", &app)
            .set<Int>("nx", 5)
            .set<Real>("xmin", 0)
            .set<Real>("xmax", 1);
        LineMesh lm(mesh_pars);

        auto prob_pars = AdvectionEquation::parameters();
        prob_pars.set<godzilla::App *>("_app", &app)
            .set<Real>("dt", 1e-3)
            .set<Real>("end_time", 5e-3)
            .set<std::string>("scheme", "ssp-rk-2");
        AdvectionEquation prob(prob_pars);

        auto bc_left_pars = InflowBC::parameters();
        bc_left_pars.set<godzilla::App *>("_app", &app)
            .set<std::vector<std::string>>("boundary", { "left" })
            .set<Real>("vel", 1.);
        InflowBC bc_left(bc_left_pars);

        auto bc_right_pars = OutflowBC::parameters();
        bc_left_pars.set<godzilla::App *>("_app", &app)
            .set<std::vector<std::string>>("boundary", { "right" });
        OutflowBC bc_right(bc_right_pars);

        auto out_pars = ExodusIIOutput::parameters();
        out_pars.set<App *>("_app", &app).set<std::string>("file", "out");

        // prob.run();
        app.run();

        return 0;
    }
    catch (Exception & e) {
        fmt::println("{}", e.what());
        return -1;
    }
    catch (...) {
        return -1;
    }
}
