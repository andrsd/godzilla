#pragma once

#include "base/GodzillaApp.h"

class GodzillaTestApp : public GodzillaApp
{
public:
    GodzillaTestApp(InputParameters parameters);

public:
    static InputParameters validParams();

    static void registerApps();
    static void registerObjects(Factory & factory);
    static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
    static void registerExecFlags(Factory & factory);
};
