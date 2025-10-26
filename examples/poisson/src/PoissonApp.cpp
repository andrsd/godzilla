// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "PoissonApp.h"
#include "PoissonEquation.h"
#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "godzilla/CommandLineInterface.h"
#include "godzilla/Exception.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/BoxMesh.h"
#include "godzilla/Types.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/EssentialBC.h"
#include "godzilla/ConstantAuxiliaryField.h"
#include "godzilla/ExodusIIOutput.h"

using namespace godzilla;

class DirichletBC : public EssentialBC {
public:
    explicit DirichletBC(const Parameters & pars) : EssentialBC(pars), dim(pars.get<Int>("dim")) {}

    void
    evaluate(Real time, const Real x[], Scalar u[]) override
    {
        if (this->dim == 1)
            u[0] = x[0] * x[0];
        else if (this->dim == 2)
            u[0] = x[0] * x[0] + x[1] * x[1];
        else if (this->dim == 3)
            u[0] = x[0] * x[0] + x[1] * x[1] + x[2] * x[2];
    }

private:
    Int dim;

public:
    static Parameters
    parameters()
    {
        auto pars = EssentialBC::parameters();
        pars.add_required_param<Int>("dim", "Spatial dimension");
        return pars;
    };
};

PoissonApp::PoissonApp(mpi::Communicator comm, const std::string & name, int argc, char * argv[]) :
    App(comm, name),
    CommandLineInterface(*this, argc, argv)
{
    CALL_STACK_MSG();
}

cxxopts::Options
PoissonApp::create_command_line_options()
{
    CALL_STACK_MSG();
    cxxopts::Options opts(get_name());
    opts.add_option("", "h", "help", "Show this help page", cxxopts::value<bool>(), "");
    opts.add_option("", "d", "dimension", "Dimension of the problem", cxxopts::value<int>(), "");
    return opts;
}

void
PoissonApp::process_command_line(cxxopts::Options & opts, const cxxopts::ParseResult & result)
{
    CALL_STACK_MSG();
    if (result.count("help")) {
        fmt::print("{}", opts.help());
    }
    else if (result.count("dimension")) {
        auto dim = result["dimension"].as<int>();
        solve_problem(dim);
    }
}

int
PoissonApp::run()
{
    CALL_STACK_MSG();
    return CommandLineInterface::run();
}

Qtr<Mesh>
PoissonApp::create_mesh(Int dim)
{
    if (dim == 1) {
        auto mesh_pars = LineMesh::parameters();
        mesh_pars.set<godzilla::App *>("_app", this)
            .set<Int>("nx", 2)
            .set<Real>("xmin", 0)
            .set<Real>("xmax", 2);
        return MeshFactory::create<LineMesh>(mesh_pars);
    }
    else if (dim == 2) {
        auto mesh_pars = RectangleMesh::parameters();
        mesh_pars.set<godzilla::App *>("_app", this)
            .set<Int>("nx", 2)
            .set<Int>("ny", 2)
            .set<Real>("xmin", 0)
            .set<Real>("xmax", 2)
            .set<Real>("ymin", 0)
            .set<Real>("ymax", 2);
        return MeshFactory::create<RectangleMesh>(mesh_pars);
    }
    else if (dim == 3) {
        auto mesh_pars = BoxMesh::parameters();
        mesh_pars.set<godzilla::App *>("_app", this)
            .set<Int>("nx", 2)
            .set<Int>("ny", 2)
            .set<Int>("nz", 2)
            .set<Real>("xmin", 0)
            .set<Real>("xmax", 2)
            .set<Real>("ymin", 0)
            .set<Real>("ymax", 2)
            .set<Real>("zmin", 0)
            .set<Real>("zmax", 2);
        return MeshFactory::create<BoxMesh>(mesh_pars);
    }
    else
        throw Exception("Unsupported dimension {}", dim);
}

void
PoissonApp::create_auxs(godzilla::DiscreteProblemInterface & prob, godzilla::Int dim)
{
    CALL_STACK_MSG();
    Real value = 0;
    if (dim == 1)
        value = -2;
    else if (dim == 2)
        value = -4;
    else if (dim == 3)
        value = -6;
    else
        throw Exception("Unsupported dimension {}", dim);

    auto aux_pars = ConstantAuxiliaryField::parameters();
    aux_pars.set<godzilla::App *>("_app", this)
        .set<std::string>("name", "forcing_fn")
        .set<std::vector<Real>>("value", { value });
    prob.add_auxiliary_field<ConstantAuxiliaryField>(aux_pars);
}

void
PoissonApp::create_bcs(DiscreteProblemInterface & prob, Int dim)
{
    CALL_STACK_MSG();
    std::vector<std::string> boundaries;
    if (dim == 1)
        boundaries = { "left", "right" };
    else if (dim == 2)
        boundaries = { "left", "right", "top", "bottom" };
    else if (dim == 3)
        boundaries = { "left", "right", "top", "bottom", "front", "back" };
    else
        throw Exception("Unsupported dimension {}", dim);

    auto bc_pars = DirichletBC::parameters();
    bc_pars.set<godzilla::App *>("_app", this)
        .set<std::string>("name", "all")
        .set<Int>("dim", dim)
        .set<std::vector<std::string>>("boundary", boundaries);
    prob.add_boundary_condition<DirichletBC>(bc_pars);
}

void
PoissonApp::solve_problem(Int dim)
{
    CALL_STACK_MSG();
    set_verbosity_level(9);

    auto out_file_name = fmt::format("mms-{}d", dim);

    auto mo = create_mesh(dim);

    auto prob_pars = PoissonEquation::parameters();
    prob_pars.set<godzilla::App *>("_app", this);
    prob_pars.set<Mesh *>("mesh", mo.get());
    PoissonEquation prob(prob_pars);
    set_problem(&prob);

    auto ic_pars = ConstantInitialCondition::parameters();
    ic_pars.set<godzilla::App *>("_app", this)
        .set<std::string>("name", "all")
        .set<std::string>("field", "u")
        .set<std::vector<Real>>("value", { 0 });
    prob.add_initial_condition<ConstantInitialCondition>(ic_pars);

    create_auxs(prob, dim);
    create_bcs(prob, dim);

    auto out_pars = ExodusIIOutput::parameters();
    out_pars.set<godzilla::App *>("_app", this)
        .set<std::string>("file", out_file_name)
        .set<std::vector<std::string>>("variables", { "u" });
    prob.add_output<ExodusIIOutput>(out_pars);

    prob.create();
    prob.run();
}
