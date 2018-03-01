#include "godzillaApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

template <>
InputParameters
validParams<godzillaApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

godzillaApp::godzillaApp(InputParameters parameters) : MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  godzillaApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  godzillaApp::associateSyntax(_syntax, _action_factory);

  Moose::registerExecFlags(_factory);
  ModulesApp::registerExecFlags(_factory);
  godzillaApp::registerExecFlags(_factory);
}

godzillaApp::~godzillaApp() {}

void
godzillaApp::registerApps()
{
  registerApp(godzillaApp);
}

void
godzillaApp::registerObjects(Factory & /*factory*/)
{
  /* Uncomment Factory parameter and register your new production objects here! */
}

void
godzillaApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
  /* Uncomment Syntax and ActionFactory parameters and register your new production objects here! */
}

void
godzillaApp::registerObjectDepends(Factory & /*factory*/)
{
}

void
godzillaApp::associateSyntaxDepends(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}

void
godzillaApp::registerExecFlags(Factory & /*factory*/)
{
  /* Uncomment Factory parameter and register your new execution flags here! */
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
godzillaApp__registerApps()
{
  godzillaApp::registerApps();
}

extern "C" void
godzillaApp__registerObjects(Factory & factory)
{
  godzillaApp::registerObjects(factory);
}

extern "C" void
godzillaApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  godzillaApp::associateSyntax(syntax, action_factory);
}

extern "C" void
godzillaApp__registerExecFlags(Factory & factory)
{
  godzillaApp::registerExecFlags(factory);
}
