//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#ifndef GODZILLATESTAPP_H
#define GODZILLATESTAPP_H

#include "MooseApp.h"

class godzillaTestApp;

template <>
InputParameters validParams<godzillaTestApp>();

class godzillaTestApp : public MooseApp
{
public:
  godzillaTestApp(InputParameters parameters);
  virtual ~godzillaTestApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
  static void registerExecFlags(Factory & factory);
};

#endif /* GODZILLATESTAPP_H */
