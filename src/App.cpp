#include "App.h"
#include "GodzillaConfig.h"
#include "GYMLFile.h"
#include "CallStack.h"
#include "Error.h"
#include "Utils.h"
#include "Terminal.h"
#include <assert.h>

namespace godzilla {

App::App(const std::string & app_name, MPI_Comm comm) :
    PrintInterface(*this),
    comm(comm),
    args(app_name),
    input_file_arg("i", "input-file", "Input file to execute", false, "", "string"),
    verbose_arg("", "verbose", "Verbosity level", false, 1, "number"),
    no_colors_switch("", "no-colors", "Do not use terminal colors", false),
    verbosity_level(1)
{
    _F_;
    MPI_Comm_size(comm, &this->comm_size);
    MPI_Comm_rank(comm, &this->comm_rank);

    this->args.add(this->input_file_arg);
    this->args.add(this->verbose_arg);
    this->args.add(this->no_colors_switch);
}

App::~App()
{
    _F_;
    Factory::destroy();
}

const Logger &
App::get_logger() const
{
    _F_;
    return this->log;
}

void
App::create()
{
    _F_;
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
        std::string file_name = this->input_file_arg.getValue();
        run_input_file(file_name);
    }
}

void
App::run_input_file(const std::string & file_name)
{
    _F_;
    if (utils::path_exists(file_name)) {
        godzilla_print(9, "Reading '", file_name, "'...");
        build_from_gyml(file_name);

        godzilla_print(9, "Checking integrity...");
        check_integrity();

        godzilla_print(9, "Running '", file_name, "'...");
        run_problem();
    }
    else
        error("Unable to open '",
              file_name,
              "' for reading. Make sure it exists and you have read permissions.");
}

void
App::build_from_gyml(const std::string & file_name)
{
    _F_;
    GYMLFile file(*this);
    file.parse(file_name);
    file.build();
}

void
App::check_integrity()
{
    _F_;
    if (this->log.get_num_entries() > 0) {
        this->log.print();
        godzilla::internal::terminate();
    }
}

void
App::run_problem()
{
    _F_;
}

} // namespace godzilla
