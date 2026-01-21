#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "godzilla/Enums.h"
#include "godzilla/EssentialBC.h"
#include "godzilla/ExodusIIOutput.h"
#include "godzilla/Init.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ConstantAuxiliaryField.h"
#include "HeatEquationProblem.h"
#include "godzilla/InitialCondition.h"
#include "godzilla/Parameters.h"
#include "godzilla/Types.h"

using namespace godzilla;

Real
exact_fn(Real t, Real x)
{
    return t + x * x;
}

class TempIC : public InitialCondition {
public:
    TempIC(const Parameters & pars) : InitialCondition(pars) {}

    void
    evaluate(Real t, const Real coord[], Scalar u[]) override
    {
        u[0] = exact_fn(t, coord[0]);
    }
};

class DirichletBC : public EssentialBC {
public:
    DirichletBC(const Parameters & pars) : EssentialBC(pars) {}

    void
    evaluate(Real t, const Real coord[], Scalar u[]) override
    {
        u[0] = exact_fn(t, coord[0]);
    }

    void
    evaluate_t(Real t, const Real coord[], Scalar u[]) override
    {
        u[0] = 1;
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

        auto mesh_pars = app.make_parameters<LineMesh>();
        mesh_pars.set<Int>("nx", 2);
        mesh_pars.set<Real>("xmax", 2);
        auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

        auto prob_pars = app.make_parameters<HeatEquationProblem>();
        prob_pars.set<Mesh *>("mesh", mesh.get())
            .set<Real>("start_time", 0.)
            .set<Real>("end_time", 1)
            .set<Real>("dt", 0.5)
            .set<Int>("p_order", 2);
        auto prob = app.make_problem<HeatEquationProblem>(prob_pars);

        auto aux_pars = app.make_parameters<ConstantAuxiliaryField>();
        aux_pars.set<String>("name", "q_ppp");
        aux_pars.set<std::vector<Real>>("value", { -1. });
        prob->add_auxiliary_field<ConstantAuxiliaryField>(aux_pars);

        auto ic_pars = app.make_parameters<TempIC>();
        ic_pars.set<String>("name", "all");
        ic_pars.set<String>("field", "temp");
        prob->add_initial_condition<TempIC>(ic_pars);

        auto bc_pars = app.make_parameters<DirichletBC>();
        bc_pars.set<std::vector<String>>("boundary", { "left", "right" });
        prob->add_boundary_condition<DirichletBC>(bc_pars);

        auto out_pars = app.make_parameters<ExodusIIOutput>();
        out_pars.set<fs::path>("file", "mms-1d")
            .set<ExecuteOnFlags>("on", ExecuteOn::INITIAL | ExecuteOn::FINAL)
            .set<std::vector<String>>("variables", { "temp" });
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
