#include "base/App.h"
#include "GodzillaConfig.h"
#include "input/GYMLFile.h"
#include "grids/GGrid.h"
#include "problems/GProblem.h"
#include "executioners/GExecutioner.h"
#include "base/CallStack.h"
#include "utils/Utils.h"


namespace godzilla {

App::App(const std::string & app_name, MPI_Comm comm) :
    GPrintInterface(*this),
    comm(comm),
    args(app_name),
    input_file_arg("i", "input-file", "Input file to execute", false, "", "string"),
    verbose_arg("", "verbose", "Verbosity level", false, 1, "number"),
    verbosity_level(1),
    grid(nullptr),
    problem(nullptr),
    executioner(nullptr)
{
    _F_;
    this->args.add(this->input_file_arg);
    this->args.add(this->verbose_arg);
}

void
App::create()
{
}

void
App::parseCommandLine(int argc, char *argv[])
{
    this->args.parse(argc, argv);
}

const unsigned int &
App::getVerbosityLevel() const
{
    _F_;
    return this->verbosity_level;
}

void
App::run()
{
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

        godzillaPrint(9, "Running '", file_name, "'...");
        startExecutioner();
    }
    else
        godzillaError("Unable to open '", file_name, "' for reading. Make sure it exists and you have read permissions.");
}

void
App::buildFromGYML(const std::string & file_name)
{
    _F_;
    GYMLFile file(*this);
    file.parse(file_name);
    file.build();
    this->grid = file.getGrid();
    this->problem = file.getProblem();
    this->executioner = file.getExecutioner();

    this->grid->create();
    this->problem->create();
    this->executioner->create();
}

void
App::startExecutioner()
{
    _F_;
    if (this->executioner)
        this->executioner->execute();
}

}
