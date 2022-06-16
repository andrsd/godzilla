#include "Problem.h"
#include "CallStack.h"
#include "Mesh.h"
#include "Function.h"
#include "Postprocessor.h"
#include "Output.h"

namespace godzilla {

InputParameters
Problem::valid_params()
{
    InputParameters params = Object::valid_params();
    params.add_private_param<const Mesh *>("_mesh", nullptr);
    return params;
}

Problem::Problem(const InputParameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    mesh(get_param<const Mesh *>("_mesh")),
    default_output_on(Output::ON_NONE),
    time(0.),
    step_num(0)
{
}

Problem::~Problem() {}

void
Problem::check()
{
    _F_;
    for (auto & f : this->functions)
        f->check();
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
    for (auto & f : this->functions)
        f->create();
    for (auto & pp : this->pps)
        pp.second->create();
    for (auto & out : this->outputs)
        out->create();
}

const Mesh *
Problem::get_mesh() const
{
    _F_;
    return this->mesh;
}

PetscInt
Problem::get_dimension() const
{
    _F_;
    return this->mesh->get_dimension();
}

const PetscReal &
Problem::get_time() const
{
    _F_;
    return this->time;
}

const PetscInt &
Problem::get_step_num() const
{
    _F_;
    return this->step_num;
}

const std::vector<Function *> &
Problem::get_functions() const
{
    _F_;
    return this->functions;
}

void
Problem::add_function(Function * fn)
{
    _F_;
    this->functions.push_back(fn);
}

void
Problem::add_output(Output * output)
{
    _F_;
    if (!output->is_param_valid("on"))
        output->set_exec_mask(this->default_output_on);
    this->outputs.push_back(output);
}

void
Problem::add_postprocessor(Postprocessor * pp)
{
    _F_;
    this->pps_names.push_back(pp->get_name());
    this->pps[pp->get_name()] = pp;
}

void
Problem::compute_postprocessors()
{
    _F_;
    for (auto & pp : this->pps)
        pp.second->compute();
}

Postprocessor *
Problem::get_postprocessor(const std::string & name) const
{
    _F_;
    const auto & it = this->pps.find(name);
    if (it != this->pps.end())
        return it->second;
    else
        return nullptr;
}

const std::vector<std::string> &
Problem::get_postprocessor_names() const
{
    _F_;
    return this->pps_names;
}

void
Problem::output(unsigned int mask)
{
    _F_;
    for (auto & o : this->outputs)
        if (o->should_output(mask))
            o->output_step();
}

void
Problem::on_initial()
{
    _F_;
    compute_postprocessors();
    output(Output::ON_INITIAL);
}

void
Problem::on_final()
{
    _F_;
    compute_postprocessors();
    output(Output::ON_FINAL);
}

} // namespace godzilla
