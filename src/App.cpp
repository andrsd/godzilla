// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/App.h"
#include "godzilla/GYMLFile.h"
#include "godzilla/Mesh.h"
#include "godzilla/Problem.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"
#include "godzilla/Exception.h"
#include "godzilla/Utils.h"
#include "godzilla/Terminal.h"
#include "godzilla/Logger.h"
#include "godzilla/Config.h"
#include <cassert>

namespace godzilla {

Registry App::registry;

App::App(const mpi::Communicator & comm,
         const std::string & name,
         int argc,
         const char * const * argv) :
    PrintInterface(comm, this->verbosity_level, name),
    name(name),
    mpi_comm(comm),
    logger(new Logger()),
    cmdln_opts(name),
    verbosity_level(1),
    yml(nullptr),
    problem(nullptr),
    factory(App::get_registry())
{
    CALL_STACK_MSG();
    this->args.resize(argc);
    for (int i = 0; i < argc; i++)
        this->args.emplace_back(argv[i]);
}

App::App(const mpi::Communicator & comm,
         const std::string & name,
         const std::vector<std::string> & args) :
    PrintInterface(comm, this->verbosity_level, name),
    name(name),
    mpi_comm(comm),
    logger(new Logger()),
    args(args),
    cmdln_opts(name),
    verbosity_level(1),
    yml(nullptr),
    problem(nullptr),
    factory(App::get_registry())
{
}

App::App(const mpi::Communicator & comm,
         const Registry & registry,
         const std::string & name,
         int argc,
         const char * const * argv) :
    PrintInterface(comm, this->verbosity_level, name),
    name(name),
    mpi_comm(comm),
    logger(new Logger()),
    cmdln_opts(name),
    verbosity_level(1),
    yml(nullptr),
    problem(nullptr),
    factory(registry)
{
    CALL_STACK_MSG();
    this->args.resize(argc);
    for (int i = 0; i < argc; i++)
        this->args.emplace_back(argv[i]);
}

App::App(const mpi::Communicator & comm,
         const Registry & registry,
         const std::string & name,
         const std::vector<std::string> & args) :
    PrintInterface(comm, this->verbosity_level, name),
    name(name),
    mpi_comm(comm),
    logger(new Logger()),
    args(args),
    cmdln_opts(name),
    verbosity_level(1),
    yml(nullptr),
    problem(nullptr),
    factory(registry)
{
    CALL_STACK_MSG();
}

App::~App()
{
    CALL_STACK_MSG();
    delete this->yml;
    delete this->logger;
    this->factory.destroy();
}

const std::string &
App::get_name() const
{
    CALL_STACK_MSG();
    return this->name;
}

const std::string &
App::get_version() const
{
    CALL_STACK_MSG();
    static const std::string ver(GODZILLA_VERSION);
    return ver;
}

Logger *
App::get_logger() const
{
    CALL_STACK_MSG();
    return this->logger;
}

Factory &
App::get_factory()
{
    return this->factory;
}

Problem *
App::get_problem() const
{
    CALL_STACK_MSG();
    return this->problem;
}

void
App::set_problem(Problem * problem)
{
    CALL_STACK_MSG();
    this->problem = problem;
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

cxxopts::ParseResult
App::parse_command_line()
{
    CALL_STACK_MSG();
    try {
        auto argc = this->args.size();
        std::vector<const char *> argv;
        argv.reserve(argc + 2);
        argv.push_back(get_name().c_str());
        for (auto & a : this->args)
            argv.push_back(a.c_str());
        argv.push_back(nullptr);
        return this->cmdln_opts.parse(argc + 1, argv.data());
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
    CALL_STACK_MSG();
    return this->verbosity_level;
}

void
App::set_verbosity_level(unsigned int level)
{
    CALL_STACK_MSG();
    this->verbosity_level = level;
}

const std::string &
App::get_input_file_name() const
{
    CALL_STACK_MSG();
    static std::string empty_file_name;
    if (this->yml != nullptr)
        return this->yml->get_file_name();
    else
        return empty_file_name;
}

const mpi::Communicator &
App::get_comm() const
{
    CALL_STACK_MSG();
    return this->mpi_comm;
}

cxxopts::Options &
App::get_command_line_opts()
{
    return this->cmdln_opts;
}

void
App::set_input_file(InputFile * input_file)
{
    CALL_STACK_MSG();
    this->yml = input_file;
}

Parameters *
App::get_parameters(const std::string & class_name)
{
    return this->factory.get_parameters(class_name);
}

void
App::process_command_line(const cxxopts::ParseResult & result)
{
    CALL_STACK_MSG();
    if (result.count("help")) {
        fmt::print("{}", this->cmdln_opts.help());
    }
    else if (result.count("version"))
        fmt::print("{}, version {}\n", get_name(), get_version());
    else {
        if (result.count("no-colors"))
            Terminal::set_colors(false);

        if (result.count("verbose"))
            set_verbosity_level(result["verbose"].as<unsigned int>());

        if (result.count("input-file")) {
            auto input_file_name = result["input-file"].as<std::string>();
            run_input_file(input_file_name);
        }
    }
}

InputFile *
App::create_input_file()
{
    CALL_STACK_MSG();
    return new GYMLFile(this);
}

void
App::run()
{
    CALL_STACK_MSG();
    create_command_line_options();
    auto result = parse_command_line();
    process_command_line(result);
}

void
App::run_input_file(const std::string & input_file_name)
{
    CALL_STACK_MSG();
    if (utils::path_exists(input_file_name)) {
        auto input_file = create_input_file();
        set_input_file(input_file);
        build_from_yml(input_file_name);
        if (this->logger->get_num_errors() == 0)
            this->yml->create_objects();
        if (check_integrity())
            run_problem();
    }
    else
        throw Exception(
            "Unable to open '{}' for reading. Make sure it exists and you have read permissions.",
            input_file_name);
}

void
App::build_from_yml(const std::string & file_name)
{
    CALL_STACK_MSG();
    if (this->yml->parse(file_name)) {
        this->yml->build();
        this->problem = this->yml->get_problem();
    }
}

bool
App::check_integrity()
{
    CALL_STACK_MSG();
    lprint(9, "Checking integrity");
    if (this->yml)
        this->yml->check();
    if (this->logger->get_num_entries() > 0) {
        this->logger->print();
        return false;
    }
    else
        return true;
}

void
App::run_problem()
{
    CALL_STACK_MSG();
    lprint(9, "Running");
    assert(this->problem != nullptr);
    this->problem->run();
}

Registry &
App::get_registry()
{
    return registry;
}

} // namespace godzilla
