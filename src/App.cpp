// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/App.h"
#include "godzilla/PerfLog.h"
#include "godzilla/Problem.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"
#include "godzilla/Exception.h"
#include "godzilla/Utils.h"
#include "godzilla/Logger.h"
#include "godzilla/Assert.h"
#include "yaml-cpp/yaml.h"
#include "fmt/chrono.h"

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

App::App(const mpi::Communicator & comm, const std::string & name) :
    PrintInterface(comm, this, this->verbosity_level, name),
    name(name),
    mpi_comm(comm),
    registry(godzilla::registry),
    logger(Qtr<Logger>::alloc()),
    verbosity_level(1),
    cout_buf_(nullptr),
    cerr_buf_(nullptr),
    problem(nullptr),
    factory(registry)
{
    CALL_STACK_MSG();
}

App::App(const mpi::Communicator & comm, Registry & registry, const std::string & name) :
    PrintInterface(comm, this, this->verbosity_level, name),
    name(name),
    mpi_comm(comm),
    registry(registry),
    logger(Qtr<Logger>::alloc()),
    verbosity_level(1),
    cout_buf_(nullptr),
    cerr_buf_(nullptr),
    problem(nullptr),
    factory(registry)
{
    CALL_STACK_MSG();
}

App::~App()
{
    CALL_STACK_MSG();
    this->factory.destroy();

    if (this->cout_buf_ != nullptr) {
        std::cout.rdbuf(this->cout_buf_);
        this->stdout_file_.close();
    }
    if (this->cerr_buf_ != nullptr) {
        std::cerr.rdbuf(this->cerr_buf_);
        this->stderr_file_.close();
    }
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
App::get_restart_file_name() const
{
    CALL_STACK_MSG();
    return this->restart_file_name;
}

void
App::set_restart_file_name(const std::string & file_name)
{
    CALL_STACK_MSG();
    this->restart_file_name = file_name;
}

void
App::set_perf_log_file_name(const std::string & file_name)
{
    CALL_STACK_MSG();
    this->perf_log_file_name = file_name;
}

const mpi::Communicator &
App::get_comm() const
{
    CALL_STACK_MSG();
    return this->mpi_comm;
}

Parameters *
App::get_parameters(const std::string & class_name)
{
    return this->factory.get_parameters(class_name);
}

int
App::run()
{
    CALL_STACK_MSG();

    if (!check_integrity()) {
        // NOTE: logger could return a string containing the number of errors and warnirngs and that
        // can be thorwn as the text of the exception
        this->logger->print();
        throw Exception("");
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    run_problem();
    auto end_time = std::chrono::high_resolution_clock::now();

    if (!this->perf_log_file_name.empty()) {
        std::chrono::duration<double> duration = end_time - start_time;
        write_perf_log(this->perf_log_file_name, duration);
        lprintln(9, "Performance log written into: {}", this->perf_log_file_name);
    }

    return 0;
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

bool
App::check_integrity()
{
    CALL_STACK_MSG();
    lprintln(9, "Checking integrity");
    if (this->logger->get_num_entries() > 0)
        return false;
    else
        return true;
}

void
App::run_problem()
{
    CALL_STACK_MSG();
    lprintln(9, "Running");
    assert_true(this->problem != nullptr, "Problem is null");
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

void
App::redirect_stdout(const std::string & file_name)
{
    CALL_STACK_MSG();
    this->stdout_file_.open(file_name);
    this->cout_buf_ = std::cout.rdbuf();
    std::cout.rdbuf(this->stdout_file_.rdbuf());
}

void
App::redirect_stderr(const std::string & file_name)
{
    CALL_STACK_MSG();
    this->stderr_file_.open(file_name);
    this->cerr_buf_ = std::cerr.rdbuf();
    std::cerr.rdbuf(this->stderr_file_.rdbuf());
}

} // namespace godzilla
