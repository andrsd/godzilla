#include "base/GodzillaTestApp.h"
#include "base/GodzillaApp.h"
// #include "Moose.h"
#include "base/AppFactory.h"
// #include "MooseSyntax.h"
// #include "ModulesApp.h"

InputParameters
GodzillaTestApp::validParams()
{
    InputParameters params = GodzillaApp::validParams();
    return params;
}

GodzillaTestApp::GodzillaTestApp(InputParameters parameters) : GodzillaApp(parameters)
{
    _F_
    GodzillaApp::registerObjects(_factory);
    GodzillaTestApp::registerObjects(_factory);
}

void
GodzillaTestApp::registerApps()
{
    _F_
    registerApp(GodzillaTestApp);
}

void
GodzillaTestApp::registerObjects(Factory & /*factory*/)
{
    _F_
}

void
GodzillaTestApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
    _F_
}

void
GodzillaTestApp::registerExecFlags(Factory & /*factory*/)
{
    _F_
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
