#include "GodzillaTestApp.h"
#include "GodzillaApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

InputParameters
GodzillaTestApp::validParams()
{
    InputParameters params = GodzillaApp::validParams();
    return params;
}

GodzillaTestApp::GodzillaTestApp(InputParameters parameters) : MooseApp(parameters)
{
    Moose::registerObjects(_factory);
    GodzillaApp::registerObjectDepends(_factory);
    GodzillaApp::registerObjects(_factory);

    Moose::associateSyntax(_syntax, _action_factory);
    GodzillaApp::associateSyntaxDepends(_syntax, _action_factory);
    GodzillaApp::associateSyntax(_syntax, _action_factory);

    Moose::registerExecFlags(_factory);
    GodzillaApp::registerExecFlags(_factory);

    bool use_test_objs = getParam<bool>("allow_test_objects");
    if (use_test_objs)
    {
        GodzillaTestApp::registerObjects(_factory);
        GodzillaTestApp::associateSyntax(_syntax, _action_factory);
        GodzillaTestApp::registerExecFlags(_factory);
    }
}

void
GodzillaTestApp::registerApps()
{
    registerApp(GodzillaApp);
    registerApp(GodzillaTestApp);
}

void
GodzillaTestApp::registerObjects(Factory & /*factory*/)
{
    /* Uncomment Factory parameter and register your new test objects here! */
}

void
GodzillaTestApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
    /* Uncomment Syntax and ActionFactory parameters and register your new test objects here! */
}

void
GodzillaTestApp::registerExecFlags(Factory & /*factory*/)
{
    /* Uncomment Factory parameter and register your new execute flags here! */
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
GodzillaTestApp__registerApps()
{
    GodzillaTestApp::registerApps();
}

// External entry point for dynamic object registration
extern "C" void
GodzillaTestApp__registerObjects(Factory & factory)
{
    GodzillaTestApp::registerObjects(factory);
}

// External entry point for dynamic syntax association
extern "C" void
GodzillaTestApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
    GodzillaTestApp::associateSyntax(syntax, action_factory);
}

// External entry point for dynamic execute flag loading
extern "C" void
GodzillaTestApp__registerExecFlags(Factory & factory)
{
    GodzillaTestApp::registerExecFlags(factory);
}
