#include "Problem.h"
#include "CallStack.h"

namespace godzilla {

InputParameters
Problem::valid_params()
{
    InputParameters params = Object::valid_params();
    return params;
}

Problem::Problem(const InputParameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    time(0.)
{
}

Problem::~Problem() {}

void
Problem::check()
{
    _F_;
}

void
Problem::create()
{
    _F_;
}

const Real &
Problem::get_time() const
{
    _F_;
    return this->time;
}

} // namespace godzilla
