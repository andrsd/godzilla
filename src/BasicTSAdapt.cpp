#include "Godzilla.h"
#include "BasicTSAdapt.h"
#include "CallStack.h"

namespace godzilla {

registerObject(BasicTSAdapt);

InputParameters
BasicTSAdapt::valid_params()
{
    InputParameters params = TimeSteppingAdaptor::valid_params();
    return params;
}

BasicTSAdapt::BasicTSAdapt(const InputParameters & params) : TimeSteppingAdaptor(params) {}

void
BasicTSAdapt::set_type()
{
    _F_;
    PETSC_CHECK(TSAdaptSetType(this->ts_adapt, TSADAPTBASIC));
}

} // namespace godzilla
