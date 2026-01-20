#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "godzilla/Enums.h"
#include "godzilla/EssentialBC.h"
#include "godzilla/ExodusIIOutput.h"
#include "godzilla/Init.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/ConstantAuxiliaryField.h"
#include "godzilla/Parameters.h"
#include "godzilla/Types.h"
#include "HeatEquationExplicit.h"

using namespace godzilla;

class TempIC : public InitialCondition {
public:
    TempIC(const Parameters & pars) : InitialCondition(pars) {}

    void
    evaluate(Real t, const Real coord[], Scalar u[]) override
    {
        auto x = coord[0];
        auto y = coord[1];
        // clang-format off
        u[0] = 2*t + x*x + y*y;
        // clang-format on
    }
};

class DirichletBC : public EssentialBC {
public:
    DirichletBC(const Parameters & pars) : EssentialBC(pars) {}

    void
    evaluate(Real t, const Real coord[], Scalar u[]) override
    {
        auto x = coord[0];
        auto y = coord[1];
        // clang-format off
        u[0] = 2*t + x*x + y*y;
        // clang-format on
    }
};

int
main(int argc, char * argv[])
{
    try {
        mpi::Communicator comm;
        Init init(argc, argv);

        App app(comm, "heat-eqn");
        app.set_verbosity_level(9);

        auto mesh_pars = app.make_parameters<RectangleMesh>();
        mesh_pars.set<Int>("nx", 4);
        mesh_pars.set<Int>("ny", 4);
        auto mesh = MeshFactory::create<RectangleMesh>(mesh_pars);

        auto prob_pars = app.make_parameters<HeatEquationExplicit>();
        prob_pars.set<Mesh *>("mesh", mesh.get())
            .set<Real>("start_time", 0.)
            .set<Real>("end_time", 5e-3)
            .set<Real>("dt", 1e-3)
            .set<Int>("order", 2);
        HeatEquationExplicit prob(prob_pars);
        app.set_problem(&prob);

        auto aux_ffn_pars = app.make_parameters<ConstantAuxiliaryField>();
        aux_ffn_pars.set<String>("name", "forcing_fn");
        aux_ffn_pars.set<std::vector<Real>>("value", { 2. });
        prob.add_auxiliary_field<ConstantAuxiliaryField>(aux_ffn_pars);

        auto ic_pars = app.make_parameters<TempIC>();
        ic_pars.set<String>("name", "all")
            .set<String>("field", "temp")
            .set<std::vector<Real>>("value", { 300 });
        prob.add_initial_condition<TempIC>(ic_pars);

        auto bc_all_pars = app.make_parameters<DirichletBC>();
        bc_all_pars.set<String>("name", "all")
            .set<std::vector<String>>("boundary", { "left", "right", "top", "bottom" });
        prob.add_boundary_condition<DirichletBC>(bc_all_pars);

        auto out_pars = app.make_parameters<ExodusIIOutput>();
        out_pars.set<fs::path>("file", "2d-explicit")
            .set<ExecuteOnFlags>("on", ExecuteOn::INITIAL | ExecuteOn::FINAL)
            .set<std::vector<String>>("variables", { "temp" });
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
