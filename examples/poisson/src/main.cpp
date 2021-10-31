#include "base/GodzillaApp.h"
#include "MooseInit.h"
#include "Moose.h"
#include "MooseApp.h"
#include "AppFactory.h"

PerfLog Moose::perf_log("poisson");

int
main(int argc, char * argv[])
{
    MooseInit init(argc, argv);

    GodzillaApp::registerApps();

    std::shared_ptr<MooseApp> app = AppFactory::createAppShared("GodzillaApp", argc, argv);

    app->run();

    return 0;
}
