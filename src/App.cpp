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
    PrintInterface(*this),
    comm(comm),
    args(app_name),
    input_file_arg("i", "input-file", "Input file to execute", false, "", "string"),
    verbose_arg("", "verbose", "Verbosity level", false, 1, "number"),
    no_colors_switch("", "no-colors", "Do not use terminal colors", false),
    verbosity_level(1),
    gyml(new GYMLFile(*this))
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
    delete this->gyml;
    Factory::destroy();
}

const Logger &
App::getLogger() const
{
    _F_;
    return this->log;
}

Problem *
App::getProblem() const
{
    _F_;
    assert(this->gyml != nullptr);
    return this->gyml->getProblem();
}

void
App::create()
{
    _F_;
    this->gyml->create();
}

void
App::parseCommandLine(int argc, char * argv[])
{
    _F_;
    this->args.parse(argc, argv);
}

const unsigned int &
App::getVerbosityLevel() const
{
    _F_;
    return this->verbosity_level;
}

const MPI_Comm &
App::getComm() const
{
    _F_;
    return this->comm;
}

const PetscMPIInt &
App::getCommRank() const
{
    _F_;
    return this->comm_rank;
}

const PetscMPIInt &
App::getCommSize() const
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
        runInputFile(file_name);
    }
}

void
App::runInputFile(const std::string & file_name)
{
    _F_;
    if (utils::pathExists(file_name)) {
        godzillaPrint(9, "Reading '", file_name, "'...");
        buildFromGYML(file_name);
        create();

        godzillaPrint(9, "Checking integrity...");
        checkIntegrity();

        godzillaPrint(9, "Running '", file_name, "'...");
        runProblem();
    }
    else
        error("Unable to open '",
              file_name,
              "' for reading. Make sure it exists and you have read permissions.");
}

void
App::buildFromGYML(const std::string & file_name)
{
    _F_;
    this->gyml->parse(file_name);
    this->gyml->build();
}

void
App::checkIntegrity()
{
    _F_;
    this->gyml->check();
    if (this->log.getNumEntries() > 0) {
        this->log.print();
        godzilla::internal::terminate();
    }
}

void
App::runProblem()
{
    _F_;
    Problem * p = this->gyml->getProblem();
    assert(p != nullptr);
    p->run();
}

} // namespace godzilla
