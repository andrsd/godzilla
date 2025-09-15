// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/App.h"
#include "godzilla/GYMLFile.h"
#include "godzilla/Mesh.h"
#include "godzilla/PerfLog.h"
#include "godzilla/Problem.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"
#include "godzilla/Exception.h"
#include "godzilla/Utils.h"
#include "godzilla/Terminal.h"
#include "godzilla/Logger.h"
#include "yaml-cpp/yaml.h"
#include "fmt/chrono.h"
#include <cassert>
#include <fstream>

namespace YAML {

Emitter &
operator<<(Emitter & out, const godzilla::Registry::ObjectDescription::Parameter & param)
{
    out << BeginMap;
    out << Key << "name" << Value << param.name;
    out << Key << "type" << Value << param.type;
    out << Key << "description" << Value << param.description;
    out << Key << "required" << Value << param.required;
    out << EndMap;
    return out;
}

Emitter &
operator<<(Emitter & out, const godzilla::Registry::ObjectDescription & obj)
{
    out << BeginMap;
    out << Key << "name" << Value << obj.name;
    out << Key << "parameters" << Value << BeginSeq;
    for (const auto & param : obj.parameters)
        out << param;
    out << EndSeq;
    out << EndMap;
    return out;
}

} // namespace YAML

namespace godzilla {

Registry registry;

App::App(const mpi::Communicator & comm,
         const std::string & name,
         int argc,
         const char * const * argv) :
    PrintInterface(comm, this, this->verbosity_level, name),
    name(name),
    mpi_comm(comm),
    registry(godzilla::registry),
    logger(Qtr<Logger>::alloc()),
    cmdln_opts(name),
    verbosity_level(1),
    yml(nullptr),
    problem(nullptr),
    factory(registry)
{
    CALL_STACK_MSG();
    this->args.resize(argc);
    for (int i = 0; i < argc; ++i)
        this->args.emplace_back(argv[i]);
}

App::App(const mpi::Communicator & comm,
         const std::string & name,
         const std::vector<std::string> & args) :
    PrintInterface(comm, this, this->verbosity_level, name),
    name(name),
    mpi_comm(comm),
    registry(godzilla::registry),
    logger(Qtr<Logger>::alloc()),
    args(args),
    cmdln_opts(name),
    verbosity_level(1),
    yml(nullptr),
    problem(nullptr),
    factory(registry)
{
}

App::App(const mpi::Communicator & comm,
         Registry & registry,
         const std::string & name,
         int argc,
         const char * const * argv) :
    PrintInterface(comm, this, this->verbosity_level, name),
    name(name),
    mpi_comm(comm),
    registry(registry),
    logger(Qtr<Logger>::alloc()),
    cmdln_opts(name),
    verbosity_level(1),
    yml(nullptr),
    problem(nullptr),
    factory(registry)
{
    CALL_STACK_MSG();
    this->args.resize(argc);
    for (int i = 0; i < argc; ++i)
        this->args.emplace_back(argv[i]);
}

App::App(const mpi::Communicator & comm,
         Registry & registry,
         const std::string & name,
         const std::vector<std::string> & args) :
    PrintInterface(comm, this, this->verbosity_level, name),
    name(name),
    mpi_comm(comm),
    registry(registry),
    logger(Qtr<Logger>::alloc()),
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
App::get_logger()
{
    CALL_STACK_MSG();
    return this->logger.get();
}

Factory &
App::get_factory()
{
    return this->factory;
}

const InputFile *
App::get_input_file() const
{
    CALL_STACK_MSG();
    return this->yml.get();
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
    this->cmdln_opts
        .add_option("", "", "restart-from", "Restart file name", cxxopts::value<std::string>(), "");
    this->cmdln_opts.add_option("", "v", "version", "Show the version", cxxopts::value<bool>(), "");
    this->cmdln_opts
        .add_option("", "", "verbose", "Verbosity level", cxxopts::value<unsigned int>(), "");
    this->cmdln_opts
        .add_option("", "", "no-colors", "Do not use terminal colors", cxxopts::value<bool>(), "");
    this->cmdln_opts.add_option("",
                                "",
                                "export-parameters",
                                "Export parameters for all registered objects into a YAML file",
                                cxxopts::value<bool>(),
                                "");
    this->cmdln_opts.add_option("",
                                "",
                                "perf-log",
                                "Save performance log into a file",
                                cxxopts::value<std::string>(),
                                "");
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
        throw Exception("");
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
    if (this->yml == nullptr)
        return empty_file_name;
    else
        return this->yml->get_file_name();
}

const std::string &
App::get_restart_file_name() const
{
    CALL_STACK_MSG();
    return this->restart_file_name;
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

        if (result.count("restart-from"))
            this->restart_file_name = result["restart-from"].as<std::string>();

        if (result.count("perf-log"))
            this->perf_log_file_name = result["perf-log"].as<std::string>();

        if (result.count("input-file")) {
            auto input_file_name = result["input-file"].as<std::string>();
            run_input_file(input_file_name);
        }

        if (result.count("export-parameters"))
            export_parameters_yaml();
    }
}

Qtr<InputFile>
App::create_input_file()
{
    CALL_STACK_MSG();
    return Qtr<GYMLFile>::alloc(this);
}

void
App::run()
{
    CALL_STACK_MSG();

    auto start_time = std::chrono::high_resolution_clock::now();
    create_command_line_options();
    auto result = parse_command_line();
    process_command_line(result);
    auto end_time = std::chrono::high_resolution_clock::now();

    if (!this->perf_log_file_name.empty()) {
        std::chrono::duration<double> duration = end_time - start_time;
        write_perf_log(this->perf_log_file_name, duration);
        lprintln(9, "Performance log written into: {}", this->perf_log_file_name);
    }
}

void
App::run_input_file(const std::string & input_file_name)
{
    CALL_STACK_MSG();
    if (utils::path_exists(input_file_name)) {
        this->yml = create_input_file();
        if (this->yml == nullptr)
            throw InternalError("App::yaml is null");
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
App::export_parameters_yaml() const
{
    CALL_STACK_MSG();
    auto objs = this->registry.get_object_description();
    YAML::Emitter yaml;
    yaml << YAML::BeginMap;
    yaml << YAML::Key << "classes" << YAML::Value << YAML::BeginSeq;
    for (auto & o : objs)
        yaml << o;
    yaml << YAML::EndSeq;
    yaml << YAML::EndMap;
    std::cout << yaml.c_str() << std::endl;
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
    lprintln(9, "Checking integrity");
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
    lprintln(9, "Running");
    assert(this->problem != nullptr);
    this->problem->run();
}

Registry &
App::get_registry()
{
    return this->registry;
}

void
App::write_perf_log(const std::string file_name, std::chrono::duration<double> run_time) const
{
    CALL_STACK_MSG();
    auto comm = get_comm();

    auto build_stat_node = [&](double val) {
        double min, max, tot;
        comm.all_reduce(val, min, mpi::op::min<double>());
        comm.all_reduce(val, max, mpi::op::max<double>());
        comm.all_reduce(val, tot, mpi::op::sum<double>());
        auto ratio = utils::ratio(max, min);
        YAML::Node ynode;
        ynode["max"] = max;
        ynode["ratio"] = ratio;
        ynode["avg"] = tot / static_cast<double>(comm.size());
        return ynode;
    };

    YAML::Node yperflog;
    // info section
    {
        char version[256];
        PETSC_CHECK(PetscGetVersion(version, sizeof(version)));
        char arch[128];
        PETSC_CHECK(PetscGetArchType(arch, sizeof(arch)));
        char hostname[128];
        PETSC_CHECK(PetscGetHostName(hostname, sizeof(hostname)));
        char username[128];
        PETSC_CHECK(PetscGetUserName(username, sizeof(username)));
        char pname[PETSC_MAX_PATH_LEN];
        PETSC_CHECK(PetscGetProgramName(pname, sizeof(pname)));

        std::time_t now = std::time(nullptr);
        std::string datetime = fmt::format("{:%d %b %Y, %H:%M:%S}", *std::localtime(&now));

        YAML::Node yinfo;
        yinfo["petsc-version"] = std::string(version);
        yinfo["arch"] = std::string(arch);
        yinfo["hostname"] = std::string(hostname);
        yinfo["username"] = std::string(username);
        yinfo["program-name"] = std::string(pname);
        yinfo["date"] = datetime;

        yperflog["info"] = yinfo;
    }
    // global section
    {
        YAML::Node yglobal;

        YAML::Node ytime;
        yglobal["time"] = build_stat_node(run_time.count());

        yperflog["global"] = yglobal;
    }
    // events section
    {
        YAML::Node yevents(YAML::NodeType::Sequence);

        auto & event_ids = perf_log::registered_event_ids();
        for (auto & id : event_ids) {
            perf_log::Event evt(id);
            auto nfo = evt.info();
            YAML::Node yevent;
            yevent["name"] = evt.name();
            yevent["calls"] = build_stat_node(nfo.num_calls());
            yevent["time"] = build_stat_node(nfo.time());
            yevent["flops"] = build_stat_node(nfo.flops());

            perf_log::LogDouble n_msgs;
            comm.all_reduce(nfo.num_messages(), n_msgs, mpi::op::sum<perf_log::LogDouble>());
            perf_log::LogDouble msg_len;
            comm.all_reduce(nfo.messages_length(), msg_len, mpi::op::sum<perf_log::LogDouble>());
            perf_log::LogDouble n_reducts;
            comm.all_reduce(nfo.num_reductions(), n_reducts, mpi::op::sum<perf_log::LogDouble>());

            // see `PetscLogHandlerView_Default_Info` in PETSc
            n_msgs *= 0.5;
            msg_len *= 0.5;
            n_reducts /= comm.size();

            yevent["messages"] = n_msgs;
            yevent["avg-message-length"] = utils::ratio(msg_len, n_msgs);
            yevent["reductions"] = n_reducts;

            yevents.push_back(yevent);
        }

        yperflog["events"] = yevents;
    }

    if (comm.rank() == 0) {
        YAML::Node yroot;
        yroot["perf-log"] = yperflog;
        YAML::Emitter yaml;
        yaml << yroot;
        if (!yaml.good())
            throw Exception("YAML Emitter error: {}", yaml.GetLastError());

        std::ofstream fout(file_name);
        fout << yaml.c_str();
        fout << std::endl;
        fout.close();
    }
}

} // namespace godzilla
