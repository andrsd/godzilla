#include "BurgersEquation.h"
#include "godzilla/App.h"
#include "godzilla/EssentialBC.h"
#include "godzilla/Init.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/ExodusIIOutput.h"
#include "godzilla/Parameters.h"

class VelocityBC : public EssentialBC {
public:
    explicit VelocityBC(const Parameters & pars) : EssentialBC(pars), value(pars.get<Real>("value"))
    {
    }

    void
    evaluate(Real time, const Real x[], Scalar u[]) override
    {
        u[0] = this->value;
    }

private:
    Real value;

public:
    static Parameters parameters();
};

Parameters
VelocityBC::parameters()
{
    auto pars = EssentialBC::parameters();
    pars.add_required_param<Real>("value", "Value to impose");
    return pars;
}

int
main(int argc, char * argv[])
{
    try {
        mpi::Communicator comm;
        godzilla::Init init(argc, argv);

        godzilla::App app(comm, "burgers-eqn");
        app.set_verbosity_level(9);

        auto mesh_pars = LineMesh::parameters();
        mesh_pars.set<godzilla::App *>("app", &app)
            .set<Int>("nx", 100)
            .set<Real>("xmin", 0)
            .set<Real>("xmax", 1);
        auto lm = MeshFactory::create<LineMesh>(mesh_pars);

        auto prob_pars = BurgersEquation::parameters();
        prob_pars.set<godzilla::App *>("app", &app)
            .set<Mesh *>("mesh", lm.get())
            .set<Real>("dt", 0.002)
            .set<Real>("start_time", 0)
            .set<Real>("end_time", 0.01)
            .set<Real>("viscosity", 0.005);
        BurgersEquation prob(prob_pars);
        app.set_problem(&prob);

        auto ic_pars = ConstantInitialCondition::parameters();
        ic_pars.set<godzilla::App *>("app", &app);
        ic_pars.set<std::vector<Real>>("value", { 0 });
        prob.add_initial_condition<ConstantInitialCondition>(ic_pars);

        auto bc_left_pars = VelocityBC::parameters();
        bc_left_pars.set<godzilla::App *>("app", &app)
            .set<std::vector<std::string>>("boundary", { "left" })
            .set<Real>("value", 1);
        prob.add_boundary_condition<VelocityBC>(bc_left_pars);

        auto bc_right_pars = VelocityBC::parameters();
        bc_right_pars.set<godzilla::App *>("app", &app)
            .set<std::vector<std::string>>("boundary", { "right" })
            .set<Real>("value", -1);
        prob.add_boundary_condition<VelocityBC>(bc_right_pars);

        auto out_pars = ExodusIIOutput::parameters();
        out_pars.set<godzilla::App *>("app", &app)
            .set<Problem *>("_problem", &prob)
            .set<fs::path>("file", "burgers")
            .set<std::vector<std::string>>("on", { "initial", "final" });
        prob.add_output<ExodusIIOutput>(out_pars);

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
