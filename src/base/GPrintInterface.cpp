#include "GPrintInterface.h"
#include "MooseUtils.h"
#include "GodzillaApp.h"


namespace godzilla
{

namespace internal
{

void
godzillaMsgRaw(const std::string & msg)
{
    Moose::out << msg << std::endl;
}

std::string
godzillaMsgFmt(const std::string & msg, const std::string & title, const std::string & color)
{
    std::ostringstream oss;
    oss << color << title << ": " << msg << COLOR_DEFAULT << std::endl;
    return oss.str();
}

void
godzillaStreamAll(std::ostringstream &)
{
}

static Threads::spin_mutex godzilla_err_lock;

[[noreturn]] void
godzillaErrorRaw(std::string msg)
{
    msg = godzillaMsgFmt(msg, "ERROR", COLOR_RED);

    Threads::spin_mutex::scoped_lock lock(godzilla_err_lock);
    Moose::err << msg << std::flush;

    // we should call MPI_Finalize, but it throws "yaksa: 7 leaked handle pool objects"
    // MPI_Finalize();
    exit(-1);
}


} // namespace internal
} // namespace godzilla


GPrintInterface::GPrintInterface(GodzillaApp & app) :
    _verbosity_level(app.getVerbosityLevel())
{
}
