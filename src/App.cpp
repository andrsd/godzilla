#include "App.h"
#include "GYMLFile.h"
#include "Mesh.h"
#include "Problem.h"
#include "CallStack.h"
#include "Error.h"
#include "Utils.h"
#include "Terminal.h"
#include "Logger.h"
#include <cassert>

namespace godzilla {

App::App(const std::string & app_name, const mpi::Communicator & comm) :
    PrintInterface(comm, this->verbosity_level, app_name),
    name(app_name),
    comm(comm),
    args(app_name),
    input_file_arg("i", "input-file", "Input file to execute", false, "", "string"),
    verbose_arg("", "verbose", "Verbosity level", false, 1, "number"),
    no_colors_switch("", "no-colors", "Do not use terminal colors", false),
    verbosity_level(1),
    yml(nullptr)
{
    _F_;
    this->log = new Logger();

    this->args.add(this->input_file_arg);
    this->args.add(this->verbose_arg);
    this->args.add(this->no_colors_switch);
}

App::~App()
{
    _F_;
    delete this->yml;
    delete this->log;
    Factory::destroy();
}

const std::string &
App::get_name() const
{
    _F_;
    return this->name;
}

const std::string &
App::get_version() const
{
    _F_;
    static const std::string ver(GODZILLA_VERSION);
    return ver;
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
    assert(this->yml != nullptr);
    return this->yml->get_problem();
}

void
App::create()
{
    _F_;
    this->yml->create();
}

void
App::parse_command_line(int argc, const char * const * argv)
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
    assert(this->yml != nullptr);
    return this->yml->get_file_name();
}

const mpi::Communicator &
App::get_comm() const
{
    _F_;
    return this->comm;
}

InputFile *
App::allocate_input_file()
{
    _F_;
    return new GYMLFile(this);
}

void
App::process_command_line()
{
    _F_;
    if (this->no_colors_switch.getValue())
        Terminal::num_colors = 1;

    if (this->verbose_arg.isSet())
        this->verbosity_level = this->verbose_arg.getValue();
}

void
App::run()
{
    _F_;
    process_command_line();

    if (this->input_file_arg.isSet())
        run_input_file();
}

void
App::run_input_file()
{
    _F_;
    auto input_file_name = this->input_file_arg.getValue();
    if (utils::path_exists(input_file_name)) {
        this->yml = allocate_input_file();
        build_from_yml(input_file_name);
        if (this->log->get_num_errors() == 0)
            create();
        check_integrity();
        run_problem();
    }
    else
        error("Unable to open '{}' for reading. Make sure it exists and you have read permissions.",
              input_file_name);
}

void
App::build_from_yml(const std::string & file_name)
{
    _F_;
    if (this->yml->parse(file_name))
        this->yml->build();
}

void
App::check_integrity()
{
    _F_;
    lprintf(9, "Checking integrity");
    this->yml->check();
    if (this->log->get_num_entries() > 0) {
        this->log->print();
        godzilla::internal::terminate();
    }
}

void
App::run_problem()
{
    _F_;
    lprintf(9, "Running");
    Problem * p = this->yml->get_problem();
    assert(p != nullptr);
    p->run();
}

} // namespace godzilla
