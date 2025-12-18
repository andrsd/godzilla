#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "godzilla/Enums.h"
#include "godzilla/EssentialBC.h"
#include "godzilla/ExodusIIOutput.h"
#include "godzilla/Init.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/ConstantAuxiliaryField.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/Parameters.h"
#include "godzilla/Types.h"
#include "HeatEquationProblem.h"
#include "ConvectiveHeatFluxBC.h"

using namespace godzilla;

class DirichletBC : public EssentialBC {
public:
    DirichletBC(const Parameters & pars) : EssentialBC(pars) {}

    void
    evaluate(Real t, const Real coord[], Scalar u[]) override
    {
        u[0] = 300;
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

        auto mesh_pars = RectangleMesh::parameters();
        mesh_pars.set<godzilla::App *>("app", &app);
        mesh_pars.set<Int>("nx", 2);
        mesh_pars.set<Int>("ny", 2);
        auto mesh = MeshFactory::create<RectangleMesh>(mesh_pars);

        auto prob_pars = HeatEquationProblem::parameters();
        prob_pars.set<godzilla::App *>("app", &app)
            .set<Mesh *>("mesh", mesh.get())
            .set<Real>("start_time", 0.)
            .set<Real>("end_time", 0.1)
            .set<Real>("dt", 0.01);
        HeatEquationProblem prob(prob_pars);
        app.set_problem(&prob);

        auto aux_htc_pars = ConstantAuxiliaryField::parameters();
        aux_htc_pars.set<godzilla::App *>("app", &app)
            .set<std::string>("name", "htc")
            .set<std::vector<Real>>("value", { 100. });
        prob.add_auxiliary_field<ConstantAuxiliaryField>(aux_htc_pars);

        auto aux_Tamb_pars = ConstantAuxiliaryField::parameters();
        aux_Tamb_pars.set<godzilla::App *>("app", &app)
            .set<std::string>("name", "T_ambient")
            .set<std::vector<Real>>("value", { 400. });
        prob.add_auxiliary_field<ConstantAuxiliaryField>(aux_Tamb_pars);

        auto ic_pars = ConstantInitialCondition::parameters();
        ic_pars.set<godzilla::App *>("app", &app)
            .set<std::string>("name", "all")
            .set<std::string>("field", "temp")
            .set<std::vector<Real>>("value", { 300 });
        prob.add_initial_condition<ConstantInitialCondition>(ic_pars);

        auto bc_left_pars = DirichletBC::parameters();
        bc_left_pars.set<godzilla::App *>("app", &app)
            .set<std::vector<std::string>>("boundary", { "left" });
        prob.add_boundary_condition<DirichletBC>(bc_left_pars);

        auto bc_right_pars = ConvectiveHeatFluxBC::parameters();
        bc_right_pars.set<godzilla::App *>("app", &app)
            .set<std::vector<std::string>>("boundary", { "right" });
        prob.add_boundary_condition<ConvectiveHeatFluxBC>(bc_right_pars);

        auto out_pars = ExodusIIOutput::parameters();
        out_pars.set<godzilla::App *>("app", &app)
            .set<fs::path>("file", "2d")
            .set<ExecuteOnFlags>("on", ExecuteOn::INITIAL | ExecuteOn::FINAL)
            .set<std::vector<std::string>>("variables", { "temp" });
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
