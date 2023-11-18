#include "godzilla/Godzilla.h"
#include "godzilla/BasicTSAdapt.h"
#include "godzilla/CallStack.h"

namespace godzilla {

REGISTER_OBJECT(BasicTSAdapt);

Parameters
BasicTSAdapt::parameters()
{
    Parameters params = TimeSteppingAdaptor::parameters();
    return params;
}

BasicTSAdapt::BasicTSAdapt(const Parameters & params) : TimeSteppingAdaptor(params) {}

void
BasicTSAdapt::set_type_impl()
{
    _F_;
    set_type(TSADAPTBASIC);
}

} // namespace godzilla
