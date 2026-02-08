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

App::App(mpi::Communicator comm, String name) :
    PrintInterface(comm, cref(*this), this->verbosity_level, name),
    name(name),
    mpi_comm(comm),
    registry(godzilla::registry),
    logger(Qtr<Logger>::alloc()),
    verbosity_level(1),
    cout_buf_(nullptr),
    cerr_buf_(nullptr),
    problem(nullptr)
{
    CALL_STACK_MSG();
}

App::App(mpi::Communicator comm, Registry & registry, String name) :
    PrintInterface(comm, cref(*this), this->verbosity_level, name),
    name(name),
    mpi_comm(comm),
    registry(registry),
    logger(Qtr<Logger>::alloc()),
    verbosity_level(1),
    cout_buf_(nullptr),
    cerr_buf_(nullptr),
    problem(nullptr)
{
    CALL_STACK_MSG();
}

App::~App()
{
    CALL_STACK_MSG();

    if (this->cout_buf_ != nullptr) {
        std::cout.rdbuf(this->cout_buf_);
        this->stdout_file_.close();
    }
    if (this->cerr_buf_ != nullptr) {
        std::cerr.rdbuf(this->cerr_buf_);
        this->stderr_file_.close();
    }
}

String
App::get_name() const
{
    CALL_STACK_MSG();
    return this->name;
}

String
App::get_version() const
{
    CALL_STACK_MSG();
    static const String ver(GODZILLA_VERSION);
    return ver;
}

Ref<Logger>
App::get_logger()
{
    CALL_STACK_MSG();
    expect_true(this->logger != nullptr, "Logger is null");
    return ref(*this->logger);
}

Ref<Problem>
App::get_problem() const
{
    CALL_STACK_MSG();
    expect_true(this->problem != nullptr, "Problem is null");
    return ref(*this->problem);
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

fs::path
App::get_restart_file_name() const
{
    CALL_STACK_MSG();
    return this->restart_file_name;
}

void
App::set_restart_file_name(fs::path file_name)
{
    CALL_STACK_MSG();
    this->restart_file_name = std::move(file_name);
}

void
App::set_perf_log_file_name(fs::path file_name)
{
    CALL_STACK_MSG();
    this->perf_log_file_name = std::move(file_name);
}

mpi::Communicator
App::get_comm() const
{
    CALL_STACK_MSG();
    return this->mpi_comm;
}

int
App::run()
{
    CALL_STACK_MSG();

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

void
App::run_problem()
{
    CALL_STACK_MSG();
    expect_true(this->problem != nullptr, "Problem is null");
    this->problem->create();

    lprintln(9, "Running");
    this->problem->run();
}

Registry &
App::get_registry()
{
    return this->registry;
}

void
App::write_perf_log(const fs::path & file_name, std::chrono::duration<double> run_time) const
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
        String datetime = fmt::format("{:%d %b %Y, %H:%M:%S}", *std::localtime(&now));

        YAML::Node yinfo;
        yinfo["petsc-version"] = String(version);
        yinfo["arch"] = String(arch);
        yinfo["hostname"] = String(hostname);
        yinfo["username"] = String(username);
        yinfo["program-name"] = String(pname);
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

        std::ofstream fout(file_name.c_str());
        fout << yaml.c_str();
        fout << std::endl;
        fout.close();
    }
}

void
App::redirect_stdout(fs::path file_name)
{
    CALL_STACK_MSG();
    this->stdout_file_.open(file_name.c_str());
    this->cout_buf_ = std::cout.rdbuf();
    std::cout.rdbuf(this->stdout_file_.rdbuf());
}

void
App::redirect_stderr(fs::path file_name)
{
    CALL_STACK_MSG();
    this->stderr_file_.open(file_name.c_str());
    this->cerr_buf_ = std::cerr.rdbuf();
    std::cerr.rdbuf(this->stderr_file_.rdbuf());
}

} // namespace godzilla
