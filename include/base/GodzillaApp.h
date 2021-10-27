#pragma once

#include "MooseApp.h"

class GodzillaApp : public MooseApp
{
public:
    GodzillaApp(InputParameters parameters);

    virtual void run();

protected:
    virtual std::string getApplicationName() const;
    virtual std::string getApplicationVersion() const;
    virtual void processCommandLine();
    virtual void executeInputFile();

    enum {
        None,
        PrintHelp,
        PrintVersion,
        Execute
    } _command;

    std::string _input_file_name;

public:
    static InputParameters validParams();

    static void registerApps();
    static void registerObjects(Factory & factory);
    static void registerObjectDepends(Factory & factory);
    static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
    static void associateSyntaxDepends(Syntax & syntax, ActionFactory & action_factory);
    static void registerExecFlags(Factory & factory);
};
