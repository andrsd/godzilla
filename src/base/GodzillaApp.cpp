#include "GodzillaApp.h"
#include "GodzillaRevision.h"
#include "CommandLine.h"
#include "AppFactory.h"
#include "GodzillaError.h"
#include "GYMLParser.h"

InputParameters
GodzillaApp::validParams()
{
    InputParameters params = emptyInputParameters();
    params.addCommandLineParam<bool>(
        "display_version", "-v --version", false, "Print application version");
    params.addCommandLineParam<bool>("help", "-h --help", false, "Displays CLI usage statement.");

    params.addCommandLineParam<std::string>("input_file", "-i file", "Input file to run.");


    // for the MooseApp::ctor
    params.addParam<bool>("perf_graph_live_all", false, "Forces printing of ALL progress messages.");
    params.addParam<bool>("disable_perf_graph_live", false, "Disables PerfGraph Live Printing.");
    params.addParam<bool>("use_split", false, "use split distributed mesh files");
    params.addParam<bool>("check_input", false, "Check the input file (i.e. requires -i <filename>) and quit.");
    params.addParam<bool>("automatic_automatic_scaling", false, "Whether to turn on automatic scaling by default.");
    params.addParam<unsigned int>("stop_for_debugger", 30, "Pauses the application during startup for the specified time to allow for connection of debuggers.");
    // the name passed to AppFactory::create
    params.addPrivateParam<std::string>("_app_name");

    return params;
}

GodzillaApp::GodzillaApp(InputParameters parameters) :
    MooseApp(parameters),
    _command(None)
{
    GodzillaApp::registerObjects(_factory);
    GodzillaApp::associateSyntax(_syntax, _action_factory);
    GodzillaApp::registerExecFlags(_factory);
}

std::string
GodzillaApp::getApplicationName() const
{
  return "godzilla";
}

std::string
GodzillaApp::getApplicationVersion() const
{
  return GODZILLA_VERSION;
}

void
GodzillaApp::processCommandLine()
{
    std::shared_ptr<CommandLine> cmd_line = commandLine();

    if ((cmd_line->getArguments().size() <= 1) || getParam<bool>("help"))
    {
        _command = PrintHelp;
        return;
    }
    else if (getParam<bool>("display_version"))
    {
        _command = PrintHelp;
        return;
    }

    if (isParamValid("input_file"))
    {
        _command = Execute;
        _input_file_name = getParam<std::string>("input_file");
    }
}

void
GodzillaApp::run()
{
    processCommandLine();

    if (_command == Execute)
        executeInputFile();
    else if (_command == PrintHelp)
        _command_line->printUsage();
    else if (_command == PrintVersion)
        Moose::out << getApplicationName() << " version " << getVersion() << std::endl;
}

void
GodzillaApp::executeInputFile()
{
    if (MooseUtils::pathExists(_input_file_name))
    {
        GYMLParser parser(*this);
        parser.load(_input_file_name);
    }
    else
        godzillaError("Unable to open '", _input_file_name, "' for reading. Make it exists and you have read permissions.");
}

void
GodzillaApp::registerApps()
{
    registerApp(GodzillaApp);
}

void
GodzillaApp::registerObjects(Factory & /*factory*/)
{
}

void
GodzillaApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}

void
GodzillaApp::registerObjectDepends(Factory & /*factory*/)
{
}

void
GodzillaApp::associateSyntaxDepends(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}

void
GodzillaApp::registerExecFlags(Factory & /*factory*/)
{
}

//  Dynamic Library Entry Points - DO NOT MODIFY
extern "C" void
GodzillaApp__registerApps()
{
    GodzillaApp::registerApps();
}

extern "C" void
GodzillaApp__registerObjects(Factory & factory)
{
    GodzillaApp::registerObjects(factory);
}

extern "C" void
GodzillaApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
    GodzillaApp::associateSyntax(syntax, action_factory);
}

extern "C" void
GodzillaApp__registerExecFlags(Factory & factory)
{
    GodzillaApp::registerExecFlags(factory);
}
