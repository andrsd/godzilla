#include "Godzilla.h"
#include "BasicTSAdapt.h"
#include "CallStack.h"

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
BasicTSAdapt::set_type()
{
    _F_;
    PETSC_CHECK(TSAdaptSetType(this->ts_adapt, TSADAPTBASIC));
}

} // namespace godzilla
