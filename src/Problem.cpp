#include "Problem.h"
#include "CallStack.h"
#include "Mesh.h"
#include "Postprocessor.h"
#include "Output.h"

namespace godzilla {

InputParameters
Problem::validParams()
{
    InputParameters params = Object::validParams();
    params.addPrivateParam<const Mesh *>("_mesh");
    return params;
}

Problem::Problem(const InputParameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    mesh(getParam<const Mesh *>("_mesh")),
    time(0.)
{
}

Problem::~Problem() {}

void
Problem::check()
{
    _F_;
    for (auto & pp : this->pps)
        pp.second->check();
    for (auto & out : this->outputs) {
        out->check();
    }
}

void
Problem::create()
{
    _F_;
    for (auto & pp : this->pps)
        pp.second->create();
    for (auto & out : this->outputs)
        out->create();
}

PetscInt
Problem::getDimension() const
{
    _F_;
    return this->mesh->getDimension();
}

const PetscReal &
Problem::getTime() const
{
    _F_;
    return this->time;
}

void
Problem::addOutput(Output * output)
{
    _F_;
    this->outputs.push_back(output);
}

void
Problem::addPostprocessor(Postprocessor * pp)
{
    _F_;
    this->pps[pp->getName()] = pp;
}

void
Problem::computePostprocessors()
{
    _F_;
    for (auto & pp : this->pps)
        pp.second->compute();
}

Postprocessor *
Problem::getPostprocessor(const std::string & name) const
{
    _F_;
    const auto & it = this->pps.find(name);
    if (it != this->pps.end())
        return it->second;
    else
        return nullptr;
}

} // namespace godzilla
