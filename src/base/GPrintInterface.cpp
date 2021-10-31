#include "base/GPrintInterface.h"
#include "utils/MooseUtils.h"
#include "base/MooseObject.h"
#include "base/GodzillaApp.h"
#include "base/CallStack.h"


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
godzillaErrorRaw(std::string msg, bool call_stack)
{
    msg = godzillaMsgFmt(msg, "ERROR", COLOR_RED);
    Moose::err << msg << std::flush;

    Threads::spin_mutex::scoped_lock lock(godzilla_err_lock);

    if (call_stack) {
        Moose::err << std::endl;
        getCallstack().dump();
    }

    // we should call MPI_Finalize, but it throws "yaksa: 7 leaked handle pool objects"
    // MPI_Finalize();
    exit(-1);
}


} // namespace internal
} // namespace godzilla


GPrintInterface::GPrintInterface(const GodzillaApp & app) :
    verbosity_level(app.getVerbosityLevel()),
    prefix("")
{
    _F_;
}

GPrintInterface::GPrintInterface(const MooseObject * obj) :
    verbosity_level(dynamic_cast<GodzillaApp &>(obj->getMooseApp()).getVerbosityLevel()),
    prefix(obj->name() + ": ")
{
    _F_;
}
