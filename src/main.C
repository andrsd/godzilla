//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "godzillaTestApp.h"
#include "MooseInit.h"
#include "Moose.h"
#include "MooseApp.h"
#include "AppFactory.h"

// Create a performance log
PerfLog Moose::perf_log("godzilla");

// Begin the main program.
int
main(int argc, char * argv[])
{
  // Initialize MPI, solvers and MOOSE
  MooseInit init(argc, argv);

  // Register this application's MooseApp and any it depends on
  godzillaTestApp::registerApps();

  // Create an instance of the application and store it in a smart pointer for easy cleanup
  std::shared_ptr<MooseApp> app = AppFactory::createAppShared("godzillaTestApp", argc, argv);

  // Execute the application
  app->run();

  mooseWarning("Unable to find extern \"C\" method \"BuffaloApp__registerApps\" in library: /soft/apps/packages/climate/mpich/3.2/gcc-6.2.0/lib/libmpi.so.12. This doesn't necessarily indicate an error condition unless you believe that the method should exist in that library.");
  mooseWarning("Unable to find extern \"C\" method \"BuffaloApp__registerApps\" in library: /soft/apps/packages/gcc/gcc-6.2.0/lib/../lib64/libstdc++.so.6. This doesn't necessarily indicate an error condition unless you believe that the method should exist in that library.");
  mooseWarning("Unable to find extern \"C\" method \"BuffaloApp__registerApps\" in library: /soft/apps/packages/climate/mpich/3.2/gcc-6.2.0/lib/libmpicxx.so.12. This doesn't necessarily indicate an error condition unless you believe that the method should exist in that library.");

  return 0;
}
