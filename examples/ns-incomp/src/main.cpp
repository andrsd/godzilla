#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "godzilla/Init.h"
#include "godzilla/InitialCondition.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/Parameters.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/ExodusIIOutput.h"
#include "NSIncompressibleProblem.h"

using namespace godzilla;

// Reynolds number
constexpr Real Re = 400;

// initial condition

class VelocityIC : public InitialCondition {
public:
    VelocityIC(const Parameters & pars) : InitialCondition(pars) {}

    void
    evaluate(Real t, const Real coord[], Scalar u[]) override
    {
        auto x = coord[0];
        auto y = coord[1];
        u[0] = t + x * x + y * y;
        u[1] = t + 2.0 * x * x - 2.0 * x * y;
    }
};

class PressureIC : public InitialCondition {
public:
    PressureIC(const Parameters & pars) : InitialCondition(pars) {}

    void
    evaluate(Real t, const Real coord[], Scalar u[]) override
    {
        auto x = coord[0];
        auto y = coord[1];
        u[0] = x + y - 1;
    }
};

// auxiliary field

class ForcingFnAux : public AuxiliaryField {
public:
    explicit ForcingFnAux(const Parameters & pars) : AuxiliaryField(pars) {}

    Int
    get_num_components() const override
    {
        return 2;
    }

    void
    evaluate(Real t, const Real coord[], Scalar u[]) override
    {
        auto x = coord[0];
        auto y = coord[1];
        // clang-format off
        u[0] = 2*t*(x + y) - 2*x*y*y + 4*x*x*y + 2*x*x*x - 4.0/Re + 1.0;
        u[1] = 2*t*x       - 2*y*y*y + 4*x*y*y + 2*x*x*y - 4.0/Re + 1.0;
        // clang-format on
    }
};

// boundary condition

class VelocityBC : public EssentialBC {
public:
    explicit VelocityBC(const Parameters & pars) : EssentialBC(pars) {}

    void
    evaluate(Real t, const Real coord[], Scalar u[]) override
    {
        auto x = coord[0];
        auto y = coord[1];
        u[0] = t + x * x + y * y;
        u[1] = t + 2.0 * x * x - 2.0 * x * y;
    }

    void
    evaluate_t(Real t, const Real coord[], Scalar u[]) override
    {
        u[0] = 1;
        u[1] = 1;
    }
};

// main

int
main(int argc, char * argv[])
{
    try {
        mpi::Communicator comm;
        Init init(argc, argv);

        App app(comm, "ns-incomp");
        app.set_verbosity_level(9);

        auto mesh_pars = RectangleMesh::parameters();
        mesh_pars.set<godzilla::App *>("app", &app)
            .set<Int>("nx", 2)
            .set<Int>("ny", 2)
            .set<Real>("xmin", -1)
            .set<Real>("xmax", 1)
            .set<Real>("ymin", -1)
            .set<Real>("ymax", 1);
        auto mesh = MeshFactory::create<RectangleMesh>(mesh_pars);

        auto prob_pars = NSIncompressibleProblem::parameters();
        prob_pars.set<godzilla::App *>("app", &app)
            .set<Mesh *>("mesh", mesh.get())
            .set<Real>("start_time", 0)
            .set<Real>("end_time", 1.)
            .set<Real>("dt", 0.2)
            .set<Real>("Re", Re);
        NSIncompressibleProblem prob(prob_pars);
        app.set_problem(&prob);

        auto ic_vel_pars = VelocityIC::parameters();
        ic_vel_pars.set<godzilla::App *>("app", &app)
            .set<std::string>("name", "velocity")
            .set<std::string>("field", "velocity");
        prob.add_initial_condition<VelocityIC>(ic_vel_pars);

        auto ic_p_pars = PressureIC::parameters();
        ic_p_pars.set<godzilla::App *>("app", &app)
            .set<std::string>("name", "pressure")
            .set<std::string>("field", "pressure");
        prob.add_initial_condition<PressureIC>(ic_p_pars);

        auto aux_pars = ForcingFnAux::parameters();
        aux_pars.set<godzilla::App *>("app", &app);
        aux_pars.set<std::string>("name", "ffn");
        prob.add_auxiliary_field<ForcingFnAux>(aux_pars);

        auto bc_pars = VelocityBC::parameters();
        bc_pars.set<godzilla::App *>("app", &app)
            .set<std::string>("name", "all")
            .set<std::string>("field", "velocity")
            .set<std::vector<std::string>>("boundary", { "left", "right", "top", "bottom" });
        prob.add_boundary_condition<VelocityBC>(bc_pars);

        auto out_pars = ExodusIIOutput::parameters();
        out_pars.set<godzilla::App *>("app", &app)
            .set<std::vector<std::string>>("variables", { "pressure", "velocity" })
            .set<std::string>("file", "mms-2d");
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
