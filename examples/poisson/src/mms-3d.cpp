#include "PoissonEquation.h"
#include "godzilla/App.h"
#include "godzilla/Init.h"
#include "godzilla/BoxMesh.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/DirichletBC.h"
#include "godzilla/FunctionAuxiliaryField.h"
#include "godzilla/ExodusIIOutput.h"

int
main(int argc, char * argv[])
{
    try {
        mpi::Communicator comm(MPI_COMM_WORLD);
        godzilla::Init init(argc, argv);

        godzilla::App app(comm, "poisson-3d");
        app.set_verbosity_level(9);

        auto mesh_pars = BoxMesh::parameters();
        mesh_pars.set<godzilla::App *>("_app", &app)
            .set<Int>("nx", 2)
            .set<Int>("ny", 2)
            .set<Int>("nz", 2)
            .set<Real>("xmin", 0)
            .set<Real>("xmax", 2)
            .set<Real>("ymin", 0)
            .set<Real>("ymax", 2)
            .set<Real>("zmin", 0)
            .set<Real>("zmax", 2);
        BoxMesh bm(mesh_pars);

        auto prob_pars = PoissonEquation::parameters();
        // clang-format off
        prob_pars
            .set<godzilla::App *>("_app", &app)
            .set<MeshObject *>("_mesh_obj", &bm);
        // clang-format on
        PoissonEquation prob(prob_pars);
        app.set_problem(&prob);

        auto aux_pars = FunctionAuxiliaryField::parameters();
        aux_pars.set<godzilla::App *>("_app", &app)
            .set<DiscreteProblemInterface *>("_dpi", &prob)
            .set<std::vector<std::string>>("value", { "-6" });
        FunctionAuxiliaryField aux(aux_pars);
        prob.add_auxiliary_field(&aux);

        auto ic_pars = ConstantInitialCondition::parameters();
        ic_pars.set<godzilla::App *>("_app", &app)
            .set<DiscreteProblemInterface *>("_dpi", &prob)
            .set<std::string>("field", "u")
            .set<std::vector<Real>>("value", { 0 });
        ConstantInitialCondition ic(ic_pars);
        prob.add_initial_condition(&ic);

        auto bc_pars = DirichletBC::parameters();
        bc_pars.set<godzilla::App *>("_app", &app)
            .set<DiscreteProblemInterface *>("_dpi", &prob)
            .set<std::vector<std::string>>("boundary",
                                           { "left", "right", "top", "bottom", "front", "back" })
            .set<std::vector<std::string>>("value", { "x*x + y*y + z*z" });
        DirichletBC bc(bc_pars);
        prob.add_boundary_condition(&bc);

        auto out_pars = ExodusIIOutput::parameters();
        out_pars.set<godzilla::App *>("_app", &app)
            .set<Problem *>("_problem", &prob)
            .set<MeshObject *>("_mesh_obj", &bm)
            .set<std::string>("file", "mms-3d")
            .set<std::vector<std::string>>("variables", { "u" });
        ExodusIIOutput out(out_pars);
        prob.add_output(&out);

        bm.create();
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
