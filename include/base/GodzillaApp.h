#pragma once

#include "MooseApp.h"
#include "GPrintInterface.h"


class GExecutioner;


class GodzillaApp : public MooseApp,
                    public GPrintInterface
{
public:
    GodzillaApp(InputParameters parameters);

    virtual void run();

    virtual const unsigned int & getVerbosityLevel() const;

protected:
    virtual std::string getApplicationName() const;
    virtual std::string getApplicationVersion() const;
    /// Process command line
    virtual void processCommandLine();
    /// Run the execute command
    virtual void execute();
    /// Run the input file
    virtual void executeInputFile();
    ///
    virtual void buildFromGYML();

    enum {
        None,
        PrintHelp,
        PrintVersion,
        Execute
    } _command;

    /// Input file to run
    std::string _input_file_name;
    /// verbosity level
    const unsigned int _verbosity_level;

    /// Executioner
    std::shared_ptr<GExecutioner> _executioner;

public:
    static InputParameters validParams();

    static void registerApps();
    static void registerObjects(Factory & factory);
    static void registerObjectDepends(Factory & factory);
    static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
    static void associateSyntaxDepends(Syntax & syntax, ActionFactory & action_factory);
    static void registerExecFlags(Factory & factory);
};
