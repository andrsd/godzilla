//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "godzillaTestApp.h"
#include "godzillaApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

template <>
InputParameters
validParams<godzillaTestApp>()
{
  InputParameters params = validParams<godzillaApp>();
  return params;
}

godzillaTestApp::godzillaTestApp(InputParameters parameters) : MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  godzillaApp::registerObjectDepends(_factory);
  godzillaApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  godzillaApp::associateSyntaxDepends(_syntax, _action_factory);
  godzillaApp::associateSyntax(_syntax, _action_factory);

  Moose::registerExecFlags(_factory);
  ModulesApp::registerExecFlags(_factory);
  godzillaApp::registerExecFlags(_factory);

  bool use_test_objs = getParam<bool>("allow_test_objects");
  if (use_test_objs)
  {
    godzillaTestApp::registerObjects(_factory);
    godzillaTestApp::associateSyntax(_syntax, _action_factory);
    godzillaTestApp::registerExecFlags(_factory);
  }
}

godzillaTestApp::~godzillaTestApp() {}

void
godzillaTestApp::registerApps()
{
  registerApp(godzillaApp);
  registerApp(godzillaTestApp);
}

void
godzillaTestApp::registerObjects(Factory & /*factory*/)
{
  /* Uncomment Factory parameter and register your new test objects here! */
}

void
godzillaTestApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
  /* Uncomment Syntax and ActionFactory parameters and register your new test objects here! */
}

void
godzillaTestApp::registerExecFlags(Factory & /*factory*/)
{
  /* Uncomment Factory parameter and register your new execute flags here! */
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
godzillaTestApp__registerApps()
{
  godzillaTestApp::registerApps();
}

// External entry point for dynamic object registration
extern "C" void
godzillaTestApp__registerObjects(Factory & factory)
{
  godzillaTestApp::registerObjects(factory);
}

// External entry point for dynamic syntax association
extern "C" void
godzillaTestApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  godzillaTestApp::associateSyntax(syntax, action_factory);
}

// External entry point for dynamic execute flag loading
extern "C" void
godzillaTestApp__registerExecFlags(Factory & factory)
{
  godzillaTestApp::registerExecFlags(factory);
}
