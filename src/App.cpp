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

App::App(const mpi::Communicator & comm,
         const std::string & app_name,
         int argc,
         const char * const * argv) :
    PrintInterface(comm, this->verbosity_level, app_name),
    name(app_name),
    comm(comm),
    log(nullptr),
    argc(argc),
    argv(argv),
    cmdln_opts(app_name),
    verbosity_level(1),
    yml(nullptr)
{
    _F_;
    this->log = new Logger();
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
App::create_command_line_options()
{
    this->cmdln_opts.add_option("", "h", "help", "Show this help page", cxxopts::value<bool>(), "");
    this->cmdln_opts.add_option("",
                                "i",
                                "input-file",
                                "Input file to execute",
                                cxxopts::value<std::string>(),
                                "");
    this->cmdln_opts.add_option("", "v", "version", "Show the version", cxxopts::value<bool>(), "");
    this->cmdln_opts
        .add_option("", "", "verbose", "Verbosity level", cxxopts::value<unsigned int>(), "");
    this->cmdln_opts
        .add_option("", "", "no-colors", "Do not use terminal colors", cxxopts::value<bool>(), "");
}

void
App::create()
{
    _F_;
    this->yml->create();
}

cxxopts::ParseResult
App::parse_command_line()
{
    _F_;
    try {
        return this->cmdln_opts.parse(this->argc, this->argv);
    }
    catch (const cxxopts::exceptions::exception & e) {
        fmt::print(stderr, "Error: {}\n", e.what());
        fmt::print(stdout, "{}", this->cmdln_opts.help());
        internal::terminate(1);
    }
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
App::process_command_line(cxxopts::ParseResult & result)
{
    _F_;
    if (result.count("help")) {
        fmt::print("{}", this->cmdln_opts.help());
    }
    else if (result.count("version"))
        fmt::print("{}, version {}\n", get_name(), get_version());
    else {
        if (result.count("no-colors"))
            Terminal::num_colors = 1;

        if (result.count("verbose"))
            this->verbosity_level = result["verbose"].as<unsigned int>();

        if (result.count("input-file")) {
            auto input_file_name = result["input-file"].as<std::string>();
            run_input_file(input_file_name);
        }
    }
}

void
App::run()
{
    _F_;
    this->create_command_line_options();
    auto result = parse_command_line();
    process_command_line(result);
}

void
App::run_input_file(const std::string & input_file_name)
{
    _F_;
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
