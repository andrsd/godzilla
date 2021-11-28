#include "App.h"
#include "GodzillaConfig.h"
#include "GYMLFile.h"
#include "Function.h"
#include "Grid.h"
#include "Problem.h"
#include "CallStack.h"
#include "Utils.h"
#include <assert.h>

namespace godzilla {

namespace internal {

[[noreturn]] void
terminate()
{
    MPI_Finalize();
    exit(1);
}

} // namespace internal

App::App(const std::string & app_name, MPI_Comm comm) :
    PrintInterface(*this),
    comm(comm),
    args(app_name),
    input_file_arg("i", "input-file", "Input file to execute", false, "", "string"),
    verbose_arg("", "verbose", "Verbosity level", false, 1, "number"),
    verbosity_level(1),
    grid(nullptr),
    problem(nullptr)
{
    _F_;
    this->args.add(this->input_file_arg);
    this->args.add(this->verbose_arg);
}

App::~App()
{
    _F_;
    Factory::destroy();
}

const Logger &
App::getLogger() const
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

void
App::run()
{
    _F_;
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

        godzillaPrint(9, "Checking integrity...");
        checkIntegrity();

        godzillaPrint(9, "Running '", file_name, "'...");
        runProblem();
    }
    else
        godzillaError("Unable to open '",
                      file_name,
                      "' for reading. Make sure it exists and you have read permissions.");
}

void
App::buildFromGYML(const std::string & file_name)
{
    _F_;
    GYMLFile file(*this);
    file.parse(file_name);
    file.build();
    this->grid = file.getGrid();
    assert(this->grid != nullptr);
    this->problem = file.getProblem();
    assert(this->problem != nullptr);

    this->functions = file.getFunctions();
    for (auto & f : this->functions)
        f->create();
    this->grid->create();
    this->problem->create();
}

void
App::checkIntegrity()
{
    _F_;
    this->grid->check();
    this->problem->check();

    if (this->log.getNumEntries() > 0) {
        this->log.print();
        godzilla::internal::terminate();
    }
}

void
App::runProblem()
{
    _F_;
    this->problem->run();
}

} // namespace godzilla
