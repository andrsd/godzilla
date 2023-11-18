#include "godzilla/Problem.h"
#include "godzilla/CallStack.h"
#include "godzilla/Mesh.h"
#include "godzilla/Function.h"
#include "godzilla/Postprocessor.h"
#include "godzilla/Output.h"
#include "godzilla/Section.h"

namespace godzilla {

Parameters
Problem::parameters()
{
    Parameters params = Object::parameters();
    params.add_private_param<Mesh *>("_mesh", nullptr);
    return params;
}

Problem::Problem(const Parameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    mesh(get_param<Mesh *>("_mesh")),
    default_output_on()
{
}

DM
Problem::get_dm() const
{
    _F_;
    return this->mesh->get_dm();
}

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

Mesh *
Problem::get_mesh() const
{
    _F_;
    return this->mesh;
}

Int
Problem::get_dimension() const
{
    _F_;
    return this->mesh->get_dimension();
}

Real
Problem::get_time() const
{
    _F_;
    return 0.;
}

Int
Problem::get_step_num() const
{
    _F_;
    return 0;
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
Problem::output(ExecuteOn::ExecuteOnFlag flag)
{
    _F_;
    for (auto & o : this->outputs)
        if (o->should_output(flag))
            o->output_step();
}

void
Problem::on_initial()
{
    _F_;
    compute_postprocessors();
    output(ExecuteOn::INITIAL);
}

void
Problem::on_final()
{
    _F_;
    compute_postprocessors();
    output(ExecuteOn::FINAL);
}

Vector
Problem::create_local_vector() const
{
    _F_;
    Vec v;
    PETSC_CHECK(DMCreateLocalVector(get_dm(), &v));
    return { v };
}

Vector
Problem::get_local_vector() const
{
    _F_;
    Vec v;
    PETSC_CHECK(DMGetLocalVector(get_dm(), &v));
    return { v };
}

void
Problem::restore_local_vector(const Vector & vec) const
{
    _F_;
    Vec v = vec;
    PETSC_CHECK(DMRestoreLocalVector(get_dm(), &v));
}

Vector
Problem::create_global_vector() const
{
    _F_;
    Vec v;
    PETSC_CHECK(DMCreateGlobalVector(get_dm(), &v));
    return { v };
}

Vector
Problem::get_global_vector() const
{
    _F_;
    Vec v;
    PETSC_CHECK(DMGetGlobalVector(get_dm(), &v));
    return { v };
}

void
Problem::restore_global_vector(const Vector & vec) const
{
    _F_;
    Vec v = vec;
    PETSC_CHECK(DMRestoreGlobalVector(get_dm(), &v));
}

Matrix
Problem::create_matrix() const
{
    _F_;
    Mat m;
    PETSC_CHECK(DMCreateMatrix(get_dm(), &m));
    return { m };
}

Section
Problem::get_local_section() const
{
    PetscSection section = nullptr;
    PETSC_CHECK(DMGetLocalSection(get_dm(), &section));
    return { section };
}

void
Problem::set_local_section(const Section & section) const
{
    _F_;
    PETSC_CHECK(DMSetLocalSection(get_dm(), section));
}

Section
Problem::get_global_section() const
{
    PetscSection section = nullptr;
    PETSC_CHECK(DMGetGlobalSection(get_dm(), &section));
    return { section };
}

void
Problem::set_global_section(const Section & section) const
{
    _F_;
    PETSC_CHECK(DMSetGlobalSection(get_dm(), section));
}

void
Problem::set_default_output_on(ExecuteOn flags)
{
    this->default_output_on = flags;
}

} // namespace godzilla
