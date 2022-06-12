#include "App.h"
#include "GodzillaConfig.h"
#include "GYMLFile.h"
#include "Mesh.h"
#include "Problem.h"
#include "CallStack.h"
#include "Error.h"
#include "Utils.h"
#include "Terminal.h"
#include <assert.h>

namespace godzilla {

App::App(const std::string & app_name, MPI_Comm comm) :
    comm(comm),
    args(app_name),
    input_file_arg("i", "input-file", "Input file to execute", false, "", "string"),
    verbose_arg("", "verbose", "Verbosity level", false, 1, "number"),
    no_colors_switch("", "no-colors", "Do not use terminal colors", false),
    verbosity_level(1),
    gyml(nullptr)
{
    _F_;
    this->log = new Logger();
    MPI_Comm_size(comm, &this->comm_size);
    MPI_Comm_rank(comm, &this->comm_rank);

    this->args.add(this->input_file_arg);
    this->args.add(this->verbose_arg);
    this->args.add(this->no_colors_switch);
}

App::~App()
{
    _F_;
    delete this->gyml;
    delete this->log;
    Factory::destroy();
}

Logger *
App::get_logger() const
{
    _F_;
    return this->log;
}

Problem *
App::get_problem() const
{
    _F_;
    assert(this->gyml != nullptr);
    return this->gyml->get_problem();
}

void
App::create()
{
    _F_;
    this->gyml->create();
}

void
App::parse_command_line(int argc, char * argv[])
{
    _F_;
    this->args.parse(argc, argv);
}

const unsigned int &
App::get_verbosity_level() const
{
    _F_;
    return this->verbosity_level;
}

const std::string &
App::get_input_file_name() const
{
    _F_;
    return this->input_file_name;
}

const MPI_Comm &
App::get_comm() const
{
    _F_;
    return this->comm;
}

const PetscMPIInt &
App::get_comm_rank() const
{
    _F_;
    return this->comm_rank;
}

const PetscMPIInt &
App::get_comm_size() const
{
    _F_;
    return this->comm_size;
}

GYMLFile *
App::allocate_gyml()
{
    _F_;
    return new GYMLFile(this);
}

void
App::run()
{
    _F_;
    if (this->no_colors_switch.getValue())
        Terminal::num_colors = 1;

    if (this->verbose_arg.isSet()) {
        this->verbosity_level = this->verbose_arg.getValue();
    }

    if (this->input_file_arg.isSet()) {
        this->gyml = allocate_gyml();
        this->input_file_name = this->input_file_arg.getValue();
        run_input_file();
    }
}

void
App::run_input_file()
{
    _F_;
    if (utils::path_exists(this->input_file_name)) {
        build_from_gyml();
        if (this->log->get_num_errors() == 0)
            create();
        check_integrity();
        run_problem();
    }
    else
        error("Unable to open '%s' for reading. Make sure it exists and you have read permissions.",
              this->input_file_name);
}

void
App::build_from_gyml()
{
    _F_;
    if (this->gyml->parse(this->input_file_name))
        this->gyml->build();
}

void
App::check_integrity()
{
    _F_;
    this->gyml->check();
    if (this->log->get_num_entries() > 0) {
        this->log->print();
        godzilla::internal::terminate();
    }
}

void
App::run_problem()
{
    _F_;
    Problem * p = this->gyml->get_problem();
    assert(p != nullptr);
    p->run();
}

} // namespace godzilla
