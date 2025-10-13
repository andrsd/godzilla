#include "BurgersEquation.h"
#include "godzilla/App.h"
#include "godzilla/Init.h"
#include "godzilla/MeshObject.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/DirichletBC.h"
#include "godzilla/ExodusIIOutput.h"

godzilla::Registry registry;

void
register_objects(godzilla::Registry & r)
{
    godzilla::App::register_objects(r);
    REGISTER_OBJECT(r, BurgersEquation);
}

int
main(int argc, char * argv[])
{
    try {
        mpi::Communicator comm;
        godzilla::Init init(argc, argv);
        register_objects(registry);

        godzilla::App app(comm, registry, "burgers-eqn");
        app.set_verbosity_level(9);

        auto mesh_pars = LineMesh::parameters();
        mesh_pars.set<godzilla::App *>("_app", &app)
            .set<Int>("nx", 100)
            .set<Real>("xmin", 0)
            .set<Real>("xmax", 1);
        LineMesh lm(mesh_pars);

        auto prob_pars = BurgersEquation::parameters();
        prob_pars.set<godzilla::App *>("_app", &app)
            .set<MeshObject *>("_mesh_obj", &lm)
            .set<Real>("dt", 0.002)
            .set<Real>("start_time", 0)
            .set<Real>("end_time", 0.01)
            .set<Real>("viscosity", 0.005);
        BurgersEquation prob(prob_pars);
        app.set_problem(&prob);

        auto ic_pars = ConstantInitialCondition::parameters();
        ic_pars.set<godzilla::App *>("_app", &app)
            .set<DiscreteProblemInterface *>("_dpi", &prob)
            .set<std::vector<Real>>("value", { 0 });
        ConstantInitialCondition ic(ic_pars);
        prob.add_initial_condition(&ic);

        auto bc_left_pars = DirichletBC::parameters();
        bc_left_pars.set<godzilla::App *>("_app", &app)
            .set<DiscreteProblemInterface *>("_dpi", &prob)
            .set<std::vector<std::string>>("boundary", { "left" })
            .set<std::vector<std::string>>("value", { "1" });
        DirichletBC bc_left(bc_left_pars);
        prob.add_boundary_condition(&bc_left);

        auto bc_right_pars = DirichletBC::parameters();
        bc_right_pars.set<godzilla::App *>("_app", &app)
            .set<DiscreteProblemInterface *>("_dpi", &prob)
            .set<std::vector<std::string>>("boundary", { "right" })
            .set<std::vector<std::string>>("value", { "-1" });
        DirichletBC bc_right(bc_right_pars);
        prob.add_boundary_condition(&bc_right);

        auto out_pars = ExodusIIOutput::parameters();
        out_pars.set<godzilla::App *>("_app", &app)
            .set<Problem *>("_problem", &prob)
            .set<std::string>("file", "burgers")
            .set<std::vector<std::string>>("on", { "initial", "final" });
        ExodusIIOutput out(out_pars);
        prob.add_output(&out);

        lm.create();
        prob.create();
        prob.run();

        return 0;
    }
    catch (Exception & e) {
        fmt::println("{}", e.what());
        fmt::println("");
        print_call_stack(e.get_call_stack());
        return -1;
    }
    catch (...) {
        return -1;
    }
}
