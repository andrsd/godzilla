#include "base/GodzillaTestApp.h"
#include "MooseInit.h"
#include "Moose.h"
#include "MooseApp.h"
#include "AppFactory.h"

PerfLog Moose::perf_log("godzilla");

int
main(int argc, char * argv[])
{
    MooseInit init(argc, argv);

    GodzillaTestApp::registerApps();

    std::shared_ptr<MooseApp> app = AppFactory::createAppShared("GodzillaTestApp", argc, argv);

    app->run();

    return 0;
}
