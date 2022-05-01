#include "Problem.h"
#include "CallStack.h"
#include "Output.h"

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
    for (auto & out : this->outputs)
        out->create();
}

const Real &
Problem::get_time() const
{
    _F_;
    return this->time;
}

void
Problem::add_output(Output * output)
{
    _F_;
    this->outputs.push_back(output);
}

void
Problem::output()
{
    _F_;
    for (auto & out : this->outputs)
        out->output();
}

} // namespace godzilla
